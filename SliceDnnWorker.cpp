#include "SliceDnnWorker.hpp"

#include "PDBWorker.hpp"
#include "PDBTimer.hpp"

#include "RestrictSet.hpp"
#include "FileMonitor.hpp"
#include "DB_NsmfSessionInfoSQL.hpp"
#include "DB_SliceDnnWorkSQL.hpp"
#include "DB_PcfBindingInfoSQL.hpp"

extern CNDFIniConfigContainer * gCfg;

SliceDnnWorker::SliceDnnWorker() {
    className_ = typeid(*this).name();
}
 
bool SliceDnnWorker::Init(bool _bLB) {
    if(pcm_.get() == nullptr) {
        E_THD_LOG(egwLogName,"PDB ConnectionManager is not registered");
        return false;
    }

    bLB_ = _bLB;

    char    buf[128]  = {'\0', };

    memset(buf, 0, sizeof(buf));
    if(gCfg->Get((char *)"EMS_GW", (char *)"UPDATE_CNT_TBL_PERIOD_SEC", buf) == false) {
        W_THD_LOG(egwLogName,"configuration (EMS_GW:UPDATE_CNT_TBL_PERIOD_SEC) not found!") ;       

    }

    if(strlen(buf))
        updateCntTBLPeriodSec_ = std::stoi(std::string(buf));

    I_THD_LOG(egwLogName,"Configuration (EMS_GW::UPDATE_CNT_TBL_PERIOD_SEC) [%d]", 
        updateCntTBLPeriodSec_) ;

    // For MP /////////////////////////////////////////////////////
    if(bLB_ == false) {

        if(gCfg->Get((char *)"SMP", (char *)"RESTRICTION_SLICE_DNN_FN", buf) == false) {
            E_THD_LOG(egwLogName,"configuration (SMP:RESTRICTION_SLICE_DNN_FN) not found!") ;       
            return false;
        }

        if(strlen(buf))
            restrictFileName_ = buf;

        I_THD_LOG(egwLogName,"Configuration (SMP::RESTRICTION_SLICE_DNN_FN) [%s]", 
            restrictFileName_.c_str()) ;

        if(netconf_.Init() == false) {
            W_THD_LOG(egwLogName,"SliceDnnWorker.netconf.Init fail");
            return false;
        }
    }
    /////////////////////////////////////////////////////////////////

    // For LB /////////////////////////////////////////////////////
    if(bLB_) {

        memset(buf, 0, sizeof(buf));
        if(gCfg->Get((char *)"EMS_GW", (char *)"SLAVE_SLEEP_PERIOD_SEC", buf) == false) {
            W_THD_LOG(egwLogName,"configuration (EMS_GW:SLAVE_SLEEP_PERIOD_SEC) not found!");
        }

        if(strlen(buf))
            slaveSleepPeriodSec_ = std::stoi(std::string(buf));

        I_THD_LOG(egwLogName,"Configuration (EMS_GW::SLAVE_SLEEP_PERIOD_SEC) [%d]", 
            slaveSleepPeriodSec_) ;

        if((slaveSleepPeriodSec_ * 2) >= updateCntTBLPeriodSec_) {
            E_THD_LOG(egwLogName,"Configuration invalid. SLAVE_SLEEP_PERIOD is too big then UPDATE_CNT_TBL_PERIOD_SEC");
            return false;
        }
    }

    /////////////////////////////////////////////////////////////////

    return true;
}

void SliceDnnWorker::run(std::shared_ptr<bool>   _pRefCounter) {

    (void)_pRefCounter;

    memset(egwLogName, 0, sizeof(egwLogName));
    strncpy(egwLogName, "SliceDnnWorker", sizeof("SliceDnnWorker"));
    NDF_INIT_THD_LOG(egwLogName);

    I_THD_LOG(egwLogName,"========== START=========="); 
    I_THD_LOG(egwLogName,"SliceDnnWorker in service");

    if(bLB_)
        runLB();
    else
        runMP();
}

void SliceDnnWorker::runLB() {

    /*-
        관련 TIMER 
         - MASTER 판정을 위하여 얼마나 오랫동안 업데이트가 안되었는지 확인 TIMER 필요.
         - T_PCF_BINDING_INFO 조회 주기 확인 TIMER 필요.
         - Loop Sleep 주기는 하드코딩해도 문제 없어요.

        1) T_SLICE_DNN_WORKING TBL 에 대한 업데이트를 시도 합니다.

        while(bRunf_) {
            2) SELECT 하여, 본인이 MASTER 인지 확인합니다.
               MASTER 가 아니라면, SLEEP 하고 CONTINUE 합니다.

            3) UPDATE 하여, 작업이 시작함을 갱신합니다.

            4) T_NSMF_SESSION_INFO 테이블을 읽고, 관심 LIST 를 얻습니다.

            5) T_PCF_BINDING_INFO 에서, 각각 LIST 별로 연결 COUNT 를 조회합니다.
               중간 중간에 시간이 오래되면, T_SLICE_DNN_WORKING 테이블을 갱신합니다.

            6) 획득한 COUNT 값에 대하여, T_NSMF_SESSION_INFO 에 업데이트 합니다.
               이 때, REJECTING 여부까지 업데이트 해야 합니다.
        }
    -*/

    // LB 자신의 이름으로 업데이트를 시도하여, Master 권한 가리는데 참여합니다.
    Update_SliceDnnWorkSQL      updateMasterSQL;

    if(updateMasterSQL.Bind() == false) {
        W_THD_LOG(egwLogName,"Update_SliceDnnWorkSQL Bind() fail");
        return ;
    }

    // 내가 Master 권한이 있는지 여부를 확인하기 위하여 조회 합니다.
    Select_SliceDnnWorkSQL      selectMasterSQL;

    if(selectMasterSQL.Bind() == false) {
        W_THD_LOG(egwLogName,"Select_SliceDnnWorkSQL Bind() fail");
        return ;
    }

    // T_NSMF_SESSION_INFO 테이블에서, 세션 수 확인이 필요한 목록을 조회합니다. 
    Select_SliceDnnListSQL       selectTargetSQL;

    if(selectTargetSQL.Bind() == false) {
        W_THD_LOG(egwLogName,"Select_SliceDnnListSQL Bind() fail");
        return ;
    }

    // T_PCF_BINDING_INFO 테이블에서, 조건별 세션 수를 조회합니다.
    Select_PcfBindingInfoSQL       selectCountSQL;

    if(selectCountSQL.Bind() == false) {
        W_THD_LOG(egwLogName,"Select_BindInfoSQL Bind() fail");
        return ;
    }

    // T_NSMF_SESSION_INFO 테이블에, 세션 수 및 제어 여부를 업데이트 합니다.
    Update_NsmfSessionInfoSQL       updateCntSQL;

    if(updateCntSQL.Bind(0) == false) {
        W_THD_LOG(egwLogName,"Update_NsmfSessionInfoSQL Bind() fail");
        return ;
    }

    std::unique_ptr<PDB::Worker>     pdbW = std::make_unique<PDB::Worker>();
    pdbW->Assign(pcm_);

    if(pdbW->TurnOn(PDB::eDefDBType::PCF) == false) {
        E_THD_LOG(egwLogName, "PDB TurnOn() Fail [PCF]");
        bRunf_.store(false);
        return;
    }


    PDB::Timer  masterCheckTimer(slaveSleepPeriodSec_);
    PDB::Timer  updateCntTimer(updateCntTBLPeriodSec_);

    pThreadMonitor_->Register(className_.c_str());

    while(bRunf_) {
        poll(nullptr, 0, 300);

        pThreadMonitor_->Refresh(className_.c_str());

        if(masterCheckTimer.IsTimeout()) {

            if(isMaster(pdbW.get(), updateMasterSQL, selectMasterSQL) == false)
                continue;

        } else {
            continue;
        }

        // 내가 MASTER 입니다.
        if(updateCntTimer.IsTimeout() == false)
            continue;

        // SST,SD,DNN LIST 를 얻습니다.
        if(pdbW->Execute(selectTargetSQL) == false) {
            W_THD_LOG(egwLogName,"Select T_NSMF_SESSION_INFO Execute fail. could not get the target list");
            continue;
        }

        auto & vec = selectTargetSQL.GetRows();
        I_THD_LOG(egwLogName, "sst-sd-dnn list [%d]", vec.size());

        for(auto & row : vec) {

            if(bRunf_ == false)
                break;
            
            selectCountSQL.SetValue(row);
            if(pdbW->Execute(selectCountSQL) == false) {
                W_THD_LOG(egwLogName,"Select T_PCF_BINDING_INFO Execute fail. could not get the number of sessions for the target");
                break;
            }

            row.nowCnt = selectCountSQL.GetSessionCount();
            updateCntSQL.Bind(row.nowCnt);
            updateCntSQL.SetValue(row);
            if(pdbW->Execute(updateCntSQL) == false) {
                W_THD_LOG(egwLogName,"Update T_NSMF_SESSION_INFO Execute fail. failed to update session count");
                W_THD_LOG(egwLogName, 
                    "T_NSMF_SESSION_INFO is updated failed [%4d %10s %20s] [%d]", 
                    row.sst, row.sd.c_str(), row.dnn.c_str(), row.nowCnt);
            } else {
                I_THD_LOG(egwLogName, 
                    "T_NSMF_SESSION_INFO is updated [%4d %10s %20s] [%d]", 
                    row.sst, row.sd.c_str(), row.dnn.c_str(), row.nowCnt);
            }

            if(masterCheckTimer.IsTimeout()) {
                updateMaster(pdbW.get(), updateMasterSQL);
                poll(nullptr, 0, 300);
            }
        }
    }

    pThreadMonitor_->DeRegister(className_.c_str());

    W_THD_LOG(egwLogName,"SliceDnnWorker out of srvice");

}

bool SliceDnnWorker::isMaster(PDB::Worker * _pdbW, 
                              Update_SliceDnnWorkSQL & _updateSQL,
                              Select_SliceDnnWorkSQL & _selectSQL) {

    if(_pdbW->Execute(_updateSQL) == false) {
        W_THD_LOG(egwLogName, "Update T_SLICE_DNN_WORK Execute fail");
        return false; 
    }

    if(_pdbW->Execute(_selectSQL) == false) {
        E_THD_LOG(egwLogName,"Select T_SLICE_DNN_WORK Execute fail");
        return false;
    }

    D_THD_LOG(egwLogName, "I am %s", (_selectSQL.IsMaster())?"master":"slace");

    return _selectSQL.IsMaster();
}

void SliceDnnWorker::updateMaster(PDB::Worker * _pdbW, Update_SliceDnnWorkSQL & _sql) {

    if(_pdbW->Execute(_sql) == false) {
        W_THD_LOG(egwLogName, "Update T_SLICE_DNN_WORK Execute fail");
    }

    D_THD_LOG(egwLogName, "update master work success");
}

void SliceDnnWorker::runMP() {

    /*-
        1) File 을 읽은 후, 제한 set 설정 합니다.

        while(bRunf_) {
            
            2) DB 조회를 하여 제한 LIST 를 확인합니다.
             제한 set 과 비교하여, 
                - not in 인 경우가 있다면... set 에 추가 합니다. File 편집대상 입니다.
                - in 경우라면.... File 미 편집 대상 입니다.

            3) 파일 편집 대상이라면, set 값으로 File 을 편집합니다.

            4) file 변경을 확인하여, 변경되었다면, netconf 를 편집합니다.
        }
    -*/

    RestrictSet     oset; // 이전 저장된 변수
    oset.LoadFile(restrictFileName_);

    RestrictSet     rset; // 새로운 값을 저장할 변수

    FileMonitor     rFileMonitor(restrictFileName_);

    Select_RestrictionListSQL restrictSQL;

    if(restrictSQL.Bind() == false) {
        W_THD_LOG(egwLogName,"Select_RestrictionListSQL Bind() fail");
        return ;
    }

    std::unique_ptr<PDB::Worker>     pdbW = std::make_unique<PDB::Worker>();
    pdbW->Assign(pcm_);

    if(pdbW->TurnOn(PDB::eDefDBType::PCF) == false) {
        E_THD_LOG(egwLogName, "PDB TurnOn() Fail [PCF]");
        bRunf_.store(false);
        return;
    }

    // PDB::Timer  updateTimer(updateCntTBLPeriodSec_);
    PDB::Timer  updateTimer(slaveSleepPeriodSec_);

    pThreadMonitor_->Register(className_.c_str());

    while(bRunf_) {
        pThreadMonitor_->Refresh(className_.c_str());

        if(updateTimer.IsTimeout()) {

            if(pdbW->Execute(restrictSQL) == false) {
                W_THD_LOG(egwLogName, "Select_RestrictionListSQL Execute fail [%d:%s]", 
                    restrictSQL.GetSQLReturn(), restrictSQL.GetClassName());
            }

            D_THD_LOG(egwLogName, "Select_RestrictionListSQL Execute success [%zd]", restrictSQL.GetRows().size());

            rset.Clear();
            auto & vecRjectRows = restrictSQL.GetRows();
            for(auto & item : vecRjectRows)
                rset.Add(item);

            if(oset.Equal(rset) == false) {
                rset.WriteFile(restrictFileName_);
                oset = rset;
            }
        }

        if(rFileMonitor.IsChanged()) {
            I_THD_LOG(egwLogName, "Restrict File Changed [%s]", restrictFileName_.c_str());

            struct tm   stT;
            time_t t = time(nullptr);

            std::stringstream ss;
            ss << std::put_time(localtime_r(&t, &stT), "%Y%m%d%H%M%S");

            netconf_.Save(ss.str());
        }

        poll(nullptr, 0, 300);
    }

    pThreadMonitor_->DeRegister(className_.c_str());

    W_THD_LOG(egwLogName,"SliceDnnWorker out of srvice");


}
