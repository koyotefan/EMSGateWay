
#include <vector>
#include <sstream>

#include "EGWJobFactory.hpp"

EGWJobFactory::EGWJobFactory() {

}

EGWJobFactory::~EGWJobFactory() {

}

bool EGWJobFactory::Init() {


    if(exceptionRuleCfg_.Init() == false) {
        E_THD_LOG(egwLogName,"excpeiont Rule Config Job Init() fail");
        return false;
    }

    if(holidayCfg_.Init() == false) {
        E_THD_LOG(egwLogName,"holidy Config Job Init() fail");
        return false;
    }

    if(reloadSessionRule_.Init() == false) {
        E_THD_LOG(egwLogName,"reload Session Rule Job Init() fail");
        return false;
    }

    if(reloadURSPData_.Init() == false) {
        E_THD_LOG(egwLogName,"reload URSP Data Job Init() fail");
        return false;
    }

    if(pdbSwitching_.Init() == false) {
        E_THD_LOG(egwLogName,"pdb switching job Init() fail");
        return false;
    }

    if(gxMgrSubsException_.Init() == false) {
        E_THD_LOG(egwLogName,"Subscriber Exception Job For Gx.MGR Init() fail");
        return false;
    }

    if(gxMgrRARSkip_.Init() == false) {
        E_THD_LOG(egwLogName,"RAR Skip Job For Gx.MGR Init() fail");
        return false;
    }

    if(gxMgrDisasterRoaming_.Init() == false) {
        E_THD_LOG(egwLogName,"Disaster Roaming Job For Gx.MGR Init() fail");
        return false;
    }

    if(gxMgrFemtocell_.Init() == false) {
        E_THD_LOG(egwLogName,"Femtocell Job For Gx.MGR Init() fail");
        return false;
    }

    if(pcm_.get() == nullptr) {
        E_THD_LOG(egwLogName,"pdb connection manager is not ready");
        return false;
    }

    pdbPCFWorker_.Assign(pcm_);

    if(pdbPCFWorker_.TurnOn(PDB::eDefDBType::PCF) == false) {
        E_THD_LOG(egwLogName,"pdb connection manager is not ready");
        return false;
    }

    return true;

}

/*
  가입자 신규 생성 : PUT /emsgateway/subscriber/01011119999/create
  가입자 정보 변경 : PUT /emsgateway/subscriber/01011119999/modify
  Session Noti    : PUT /emsgateway/pdusession/{SM_POLICY_ID}/update
     - BODY 있음.
  PDU Session Detach: PUT /emsgateway/pdusession/{SM_POLICY_ID}/delete
  PDB Switch      : PUT /emsgateway/pdb/switch
     - BDDY 있음.
  내부 연결 해지   :  PUT /emsgateway/ipc/disconnect
     - BODY 있음. 추후 보강 예정
  예외 처리 등록 변경: PUT /emsgateway/config/exception-rule
  holiyday reloading : PUT /emsgateway/holiday
  세션 rule reloading : PUT /emsgateway/config/set-session-rule

-*/

EGWJob & EGWJobFactory::GetJob(const std::string _path) {
    D_THD_LOG(egwLogName,"Get path [%s]", _path.c_str());


    if(strcasestr(_path.c_str(), "/emsgateway") == nullptr)
        return nok_;

    if(strcasestr(_path.c_str(), "/subscriber") != nullptr) {
        D_THD_LOG(egwLogName,"selected subscriber job");
        std::string mdn = getWords(_path, 2, '/');
        subscriber_.SetPdbWorker(&pdbPCFWorker_);
        subscriber_.SetMdn(mdn);

        return subscriber_;
    }

    if(strcasestr(_path.c_str(), "/pdusession") != nullptr) {
        std::string smPolicyId = getWords(_path, 2, '/');
        if(strcasestr(_path.c_str(), "update") != nullptr) {
            D_THD_LOG(egwLogName,"selected pduSessionNoti job");
            pduSessionNoti_.SetSmPolicyId(smPolicyId);
            return pduSessionNoti_;
        }
        if(strcasestr(_path.c_str(), "delete") != nullptr) {
            D_THD_LOG(egwLogName,"selected pduSessionDetach job");
            pduSessionDetach_.SetSmPolicyId(smPolicyId);
            pduSessionDetach_.SetPdbWorker(&pdbPCFWorker_);
            return pduSessionDetach_;
        }
        return nok_;
    }

    if(strcasestr(_path.c_str(), "/pdb") != nullptr) {
        D_THD_LOG(egwLogName,"selected pdbSwitching job");
        return pdbSwitching_;
    }

    if(strcasestr(_path.c_str(), "/config") != nullptr) {
        if(strcasestr(_path.c_str(), "/exception-rule") != nullptr) {
            D_THD_LOG(egwLogName,"selected exceptionRuleCfg job");
            return exceptionRuleCfg_;
        }

        if(strcasestr(_path.c_str(), "/set-session-rule") != nullptr) {
            D_THD_LOG(egwLogName,"selected reloadSessionRule job");
            return reloadSessionRule_;
        }
        if(strcasestr(_path.c_str(), "/set-ursp-data") != nullptr) {
            D_THD_LOG(egwLogName,"selected reloadURSPData job");
            return reloadURSPData_;
        }
  
    }

    if(strcasestr(_path.c_str(), "/holiday") != nullptr) {
        D_THD_LOG(egwLogName,"selected holidayCfg job");
        return holidayCfg_;
    }

    if(strcasestr(_path.c_str(), "/gxp/reload-subs-exception") != nullptr) {
        D_THD_LOG(egwLogName,"selected reload-subs-exception job");
        return gxMgrSubsException_;
    }

    if(strcasestr(_path.c_str(), "gxp/reload-rar-skip-exception") != nullptr) {
        D_THD_LOG(egwLogName,"selected reload-rar-skip-exception job");
        return gxMgrRARSkip_;
    }

    if(strcasestr(_path.c_str(), "gxp/reload-disaster-roaming") != nullptr) {
        D_THD_LOG(egwLogName,"selected reload-disaster-roaming job");
        return gxMgrDisasterRoaming_;
    }

    if(strcasestr(_path.c_str(), "gxp/reload-femtocell") != nullptr) {
        D_THD_LOG(egwLogName,"selected job");
        return gxMgrFemtocell_;
    }

    return nok_;
}


std::string EGWJobFactory::getWords(const std::string & _org, off_t _offset, const char _delimiter) {
  std::vector<std::string> tokens;
  std::string   token;

  std::stringstream   ss(_org);
  while(std::getline(ss, token, _delimiter)) {
    if(token.size())
        tokens.push_back(token);
  }

  return tokens.at(_offset);
}

bool EGWJobFactory::isequals(std::string _src, std::string _dest) {
    return std::equal(_src.begin(), _src.end(),
                      _dest.begin(), _dest.begin(),
                      [](char a, char b) {
                        return tolower(a) == tolower(b);
                      });
}
