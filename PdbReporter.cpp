
#include "PdbReporter.hpp"


extern std::string  gSysName;

PdbReporter::PdbReporter() {
    className_ = typeid(*this).name();
}


bool PdbReporter::Init(int _port, bool _bIpv6) {

    if(pcm_.get() == nullptr) {
        W_THD_LOG(egwLogName,"PDB ConnectionManager is not registered");
        return false;
    }

    if(udp_.Init(_port, _bIpv6) == false) {
        W_THD_LOG(egwLogName,"EGW UDP Server Init() fail [%d:%d]", _port, _bIpv6);
        return false;
    }

    if(dNodeSql_.Bind() == false) {
        W_THD_LOG(egwLogName,"PDB Status DELETE Node Bind() fail");
        return false;
    }

    if(dProcSql_.Bind() == false) {
        W_THD_LOG(egwLogName,"PDB Status DELETE Proc Bind() fail");
        return false;
    }

    if(ssql_.Bind() == false) {
        W_THD_LOG(egwLogName,"PDB Status SELECT Bind() fail");
        return false;
    }

    if(usql_.Bind() == false) {
        W_THD_LOG(egwLogName,"PDB Status UPDATE Bind() fail");
        return false;
    }

    if(dsql_.Bind() == false) {
        W_THD_LOG(egwLogName,"PDB Status DELETE Bind() fail");
        return false;
    }

    if(isql_.Bind() == false) {
        W_THD_LOG(egwLogName,"PDB Status INSERT Bind() fail");
        return false;
    }

    if(asql_.Bind() == false) {
        W_THD_LOG(egwLogName,"PDB Status ALL SELECT Bind() fail");
        return false;
    }

    return true;
}

void PdbReporter::run(std::shared_ptr<bool>   _pRefCounter) {

    (void)_pRefCounter;

    memset(egwLogName, 0, sizeof(egwLogName));
    strncpy(egwLogName, "PDBReporter", sizeof("PDBReporter"));
    NDF_INIT_THD_LOG(egwLogName);

    I_THD_LOG(egwLogName,"========== START ==========");
    I_THD_LOG(egwLogName,"EmsGWServer in service");

    std::unique_ptr<PDB::Worker>     pdbW = std::make_unique<PDB::Worker>();
    pdbW->Assign(pcm_);

    if(pdbW->TurnOn(PDB::eDefDBType::PCF) == false) {
        E_THD_LOG(egwLogName, "PDB TurnOn() Fail [PCF]");
        bRunf_.store(false);
        return;
    }

    // millisecond
    udp_.SetWaitPeriod(100);

    //char    buf[4098];
    std::unique_ptr<char[]>     buf = std::make_unique<char[]>(12288);

    deleteNodeDB(pdbW.get(), gSysName); 

    pThreadMonitor_->Register(className_.c_str());

    while(bRunf_) {

        pThreadMonitor_->Refresh(className_.c_str());

        // Recv
        int ret = udp_.Recv(buf.get(), 12288-1);

        if(ret < 0) {
            E_THD_LOG(egwLogName,"UDP Server Recv() fail [%d]", ret);
            break;
        }

        if(ret == 0)
            continue;

        // D_THD_LOG(egwLogName,"[SERVER] RECV [%d]", ret);

        buf[ret] = '\0';
        //D_LOG("[SERVER] RECV [%d][%s]", ret, buf);
        
        update(pdbW.get(), buf.get());

        forceMemoryReset();
    }

    pThreadMonitor_->DeRegister(className_.c_str());

    deleteNodeDB(pdbW.get(), gSysName); 

    W_THD_LOG(egwLogName,"PdbReporter out of srvice");

}

void PdbReporter::forceMemoryReset() {
   
    time_t now = time(nullptr);

    // 30 sec 한번 씩, 메모리를 clear 할 꺼예요..
    if(now < lastResetT_ + 30)
        return ;

    I_THD_LOG(egwLogName,"PdbReporter.forceMemoryReset");
    lastResetT_ = now;

    mapItems_.clear(); 
}

bool PdbReporter::update(PDB::Worker * _pdbW, const char * _buf) {

    std::string     nodeId;
    std::string     procName;
    std::vector<stPdbStatusItem>    vecItems;

    if(parse(nodeId, procName, vecItems, _buf) == false)
        return false;

    if(vecItems.size() == 0) {
        deleteProcDB(_pdbW, nodeId, procName);
        mapItems_.clear(); // 특정 프로세스가 죽었어요.. 메모리를 모두 clear 할래요. 
        return false;
    }
   
    bool    bChanged = false;
    size_t  beforeMapSize = mapItems_.size();
 
    for(auto & item : vecItems) { 
        if(isPGLocalDB(item))
            continue;

        if(isEqual(item))
            continue;

        bChanged = true;
        if(updateDB(_pdbW, item) == false) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
            deleteDB(_pdbW, item);
            if(insertDB(_pdbW, item) == false)
                continue;
        }

        updateMemory(item); 
    }

    size_t  afterMapSize = mapItems_.size();

    // 각 프로세스가 모두 살아 있는 상태에서 변화 될 때만, 알람을 발생 시킵니다.
    // 그래서 Map 의 크기 변화가 없을 때에만 인지하는 거예요
    if(bChanged && (beforeMapSize == afterMapSize))
        alarmReport(_pdbW);

    return bChanged; 
}

bool PdbReporter::parse(std::string & _nodeId, std::string & _procName, std::vector<stPdbStatusItem> & _vec, const char * _buf) {

    try {

        rapidjson::Document     doc;

        if(doc.Parse(_buf).HasParseError()) {
            W_THD_LOG(egwLogName,"json Parse() Error [%s]", _buf);
            return false;
        }

        _nodeId = doc["nodeId"].GetString();
        _procName = doc["procName"].GetString();
        
        // 데이터를 Parsing 하여 DB 에 넣어야 해요.
        auto iter = doc.FindMember("reports");

        if(iter == doc.MemberEnd() || iter->value.IsArray() == false) {
            W_THD_LOG(egwLogName,"PdbReporter.parse.json unexpected [%s]", _buf);
            return false;
        }

        const rapidjson::Value & array = iter->value;

        // report 한 프로세스가 죽었다는 뜻이예요..
        // Process 가 죽거나, 살았던 시점으로 보기 때문에요.
        for(rapidjson::SizeType i=0; i < array.Size(); ++i) {
            const rapidjson::Value & obj = array[i];

            stPdbStatusItem         item;

            strncpy(item.nodeId,    _nodeId.c_str(), sizeof(item.nodeId)-1);
            strncpy(item.procName,  _procName.c_str(), sizeof(item.procName)-1); 

            std::string str = obj["pdbType"].GetString();
            strncpy(item.pdbType, str.c_str(), sizeof(item.pdbType)-1);

            item.shardingId = obj["shardingId"].GetInt();

            str = obj["dbName"].GetString();
            strncpy(item.dbName, str.c_str(), sizeof(item.dbName)-1);

            str = obj["dbIp"].GetString();
            strncpy(item.dbIp, str.c_str(), sizeof(item.dbIp)-1);

            item.dbPort = obj["dbPort"].GetInt();
            item.connCnt = obj["connCnt"].GetInt();
            item.actCnt = obj["actCnt"].GetInt();            

            _vec.emplace_back(item);
        }

    } catch (std::exception & _e) {

        W_THD_LOG(egwLogName,"PdbReporter.parse.json.Parse fail [%s:%s]", _buf, _e.what());
        return false;
    }

    return true;
}

bool PdbReporter::isPGLocalDB(const stPdbStatusItem & _item) {
    if(strstr(_item.dbName, PDB::ToString::Get(PDB::eDefDBType::PGLocal1)) != nullptr ||
       strstr(_item.dbName, PDB::ToString::Get(PDB::eDefDBType::PGLocal2)) != nullptr)
        return true;

    return false; 
}

bool PdbReporter::isEqual(stPdbStatusItem & _item) {

    std::tuple<std::string, std::string>    tup =
        make_tuple(_item.procName, _item.dbName);
   
    auto iter = mapItems_.find(tup);

    if(iter == mapItems_.end())
        return false;

    return (iter->second).IsEqual(_item);
}

void PdbReporter::updateMemory(stPdbStatusItem & _item) {

    std::tuple<std::string, std::string>    tup =
        make_tuple(_item.procName, _item.dbName);

    auto iter = mapItems_.find(tup);

    if(iter == mapItems_.end()) {
        mapItems_.emplace(tup, _item);
    } else {
        iter->second = _item;
    }
}

void PdbReporter::deleteNodeDB(PDB::Worker * _pdbW, std::string & _nodeId) {

    stPdbStatusItem     item;

    strncpy(item.nodeId, _nodeId.c_str(), sizeof(item.nodeId)-1);

    dNodeSql_.SetValue(item);

    if(_pdbW->Execute(dNodeSql_) == false) {
        W_THD_LOG(egwLogName,"DELETE Node fail [%d:%s] [%s]",
            dNodeSql_.GetSQLReturn(), dNodeSql_.GetClassName(), _nodeId.c_str());
    } else {
        I_THD_LOG(egwLogName,"DELETE Node success [%s]", _nodeId.c_str());
    }
}


void PdbReporter::deleteProcDB(PDB::Worker * _pdbW, std::string & _nodeId, std::string & _procName) {

    stPdbStatusItem     item;

    strncpy(item.nodeId, _nodeId.c_str(), sizeof(item.nodeId)-1);
    strncpy(item.procName, _procName.c_str(), sizeof(item.procName)-1);

    dProcSql_.SetValue(item);

    if(_pdbW->Execute(dProcSql_) == false) {
        W_THD_LOG(egwLogName,"DELETE PROC fail [%d:%s] [%s:%s]",
            dNodeSql_.GetSQLReturn(), dNodeSql_.GetClassName(), _nodeId.c_str(), _procName.c_str());
    } else {
        I_THD_LOG(egwLogName,"DELETE PROC [%s:%s]", _nodeId.c_str(), _procName.c_str());
    }
}

void PdbReporter::deleteDB(PDB::Worker * _pdbW, stPdbStatusItem & _item) {

    dsql_.SetValue(_item);

    if(_pdbW->Execute(dsql_) == false) {
        W_THD_LOG(egwLogName,"DELETE fail [%d:%s] [%s:%s:%s] [%d:%d]",
            dsql_.GetSQLReturn(), usql_.GetClassName(), 
            _item.nodeId, _item.procName, _item.dbName, _item.actCnt, _item.connCnt);
    } else {
        D_THD_LOG(egwLogName,"DELETE success [%s:%s:%s] [%d:%d]", _item.nodeId, _item.procName, _item.dbName, _item.actCnt, _item.connCnt);
    }
}

bool PdbReporter::updateDB(PDB::Worker * _pdbW, stPdbStatusItem & _item) {

    usql_.SetValue(_item);

    if(_pdbW->Execute(usql_) == false) {

        if(usql_.GetSQLReturn() == SQL_NO_DATA) {
            isql_.SetValue(_item);

            if(_pdbW->Execute(isql_) == true)
                return true;
        }

        W_THD_LOG(egwLogName,"UPDATE fail [%d:%s] [%s:%s:%s] [%d:%d]",
            usql_.GetSQLReturn(), usql_.GetClassName(), 
            _item.nodeId, _item.procName, _item.dbName, _item.actCnt, _item.connCnt);

        return false;
    }

    I_THD_LOG(egwLogName,"UPDATE [%s:%s:%s] [%d:%d]", _item.nodeId, _item.procName, _item.dbName, _item.actCnt, _item.connCnt);
    return true;
}

bool PdbReporter::insertDB(PDB::Worker * _pdbW, stPdbStatusItem & _item) {

    isql_.SetValue(_item);

    if(_pdbW->Execute(isql_) == false) {
        W_THD_LOG(egwLogName,"INSERT fail [%d:%s] [%s:%s:%s] [%d:%d]",
            isql_.GetSQLReturn(), isql_.GetClassName(), _item.nodeId, _item.procName, _item.dbName, _item.actCnt, _item.connCnt);

        return false;
    }

    I_THD_LOG(egwLogName,"INSERT success [%s:%s:%s] [%d:%d]", _item.nodeId, _item.procName, _item.dbName, _item.actCnt, _item.connCnt);
    return true;
}

void PdbReporter::alarmReport(PDB::Worker * _pdbW) {

    // 기존에 업데이트 된 상태에서 값이 바뀐거예요..
    time_t now = time(nullptr);
    if(now < (alarmGuardT_ + 10)) {
        // 너무 자주 알람이 울리긴 싫어요.
        return ;
    }
 
    std::map<std::string, std::string>   mV;
    selectDB(_pdbW, mV);

    I_THD_LOG(egwLogName,"alarm [%s] [%s]", gSysName.c_str(), "CHANGED"); 

    std::map<std::string, std::string>   mD;
    ATOM_TRAP_EXT(ATOM_TRAP_ACTION_NOTIFY,
             "db.connection.changed",
             const_cast<char *>(gSysName.c_str()),
             const_cast<char *>("CHANGED"),
             mD,
             mV);
    alarmGuardT_ = now;
}

void PdbReporter::selectDB(PDB::Worker * _pdbW, std::map<std::string, std::string> & _mV) {

    if(_pdbW->Execute(asql_) == false) {

        if(asql_.GetSQLReturn() != SQL_NO_DATA)
            W_THD_LOG(egwLogName,"SELECT fail [%d:%s]",
                asql_.GetSQLReturn(), asql_.GetClassName());

        if(_pdbW->Execute(asql_) == false) {
            W_THD_LOG(egwLogName,"SELECT fail [%d:%s]",
                asql_.GetSQLReturn(), asql_.GetClassName());
        }

        return ;
    }

    std::vector<stConnectCntByDBName>  & rows = asql_.GetRows();

    // make String
    std::map<std::string, stPSConnectCnt>  m;
    for(auto & iter : rows) {
        auto i = m.find(iter.dbName);

        if(i == m.end()) {
            stPSConnectCnt  stCnt;
            m.emplace(iter.dbName, stCnt); 

            i = m.find(iter.dbName);
        }
       
        stPSConnectCnt & stCnt = i->second;  


        if(strncasecmp(iter.PS, "P", 1)==0) {
            stCnt.PActive = iter.nACTIVE;
            stCnt.PConnect= iter.nCONN;
        } else {
            stCnt.SActive = iter.nACTIVE;
            stCnt.SConnect= iter.nCONN;
        }
    } 
   
    for(auto & iter : m) {
        stPSConnectCnt & stCnt = iter.second;

        char    buf[128];
        snprintf(buf, size_t(buf)-1,
            "P[%5d/%5d] S[%5d/%5d]", 
            stCnt.PActive, stCnt.PConnect,
            stCnt.SActive, stCnt.SConnect);

        _mV.emplace(iter.first, std::string(buf)); 

    }
}
