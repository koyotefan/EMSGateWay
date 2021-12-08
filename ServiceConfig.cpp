
#include <sstream>
#include <fstream>
#include <algorithm>
#include <map>

#include "CNDFATOMAdaptor.hpp"
#include "EGWDefine.hpp"
#include "ServiceConfig.hpp"

extern std::string  gSysName;
extern thread_local char egwLogName[32];

// ServiceConfig //////////////////////////////////////////////////////////////

ServiceConfig::ServiceConfig(const std::string & _fName)
    : fMonitor_(_fName) {

    setAlternativeNodeId();
}

void ServiceConfig::setAlternativeNodeId() {

    if(alternativeNodeId_.empty() == false)
        return ;

    // lb01 <-> lb02, app01 <-> app02, app03 <-> app04, app05 <-> app06, ...

    char  pcf[32] =  {'\0', };
    char  vnfc[32] = {'\0', };
    char  host[32] = {'\0', };

    sscanf(gSysName.c_str(), "%[^-]-%[^-]-%s", pcf, vnfc, host);

    char * p = nullptr;
    char * p1 = nullptr;

    // vnfc
    p = &vnfc[strlen(vnfc)-1];
    *p = (*p == '1')?'2':'1';

    // host
    p = &host[strlen(host)-1];
    p1 = p - 1;

    switch(*p)
    {
    case '0':
        *p1 = *p1 - 1;
        *p = '9';
        break;
    case '9':
        *p1 = *p1 + 1;
        *p = '0';
        break;
    default:
        *p = (*p % 2 == 0)?(*p-1):(*p+1);
        break;
    }

    alternativeNodeId_.append(pcf);
    alternativeNodeId_.append("-");
    alternativeNodeId_.append(vnfc);
    alternativeNodeId_.append("-");
    alternativeNodeId_.append(host);

} 

ServiceConfig::~ServiceConfig() {

}

bool ServiceConfig::Load() {

    bChanged_ = false;

    if(fMonitor_.IsChanged() == false)
        return true;

    bool bret = false;

    try {
        bret = load();
    } catch (...) {
        W_THD_LOG(egwLogName, 
            "File Loading fail [%s]", fMonitor_.GetFileName());
        return false;
    }

    if(bret)
        bChanged_ = true;

    prnItem();

    return bret;
}

void ServiceConfig::prnItem() {
    for(auto & item : vItems_) {
        D_THD_LOG(egwLogName, "item [%s:%s:%d:%d:%d:%d:%d]",
            item.name_.c_str(),
            item.targetSystem_.c_str(),
            item.bServer_,
            item.nPort_,
            static_cast<int>(item.proto_),
            item.nMinConnectionLimit_,
            item.nMinSystemCntLimit_);
        for(auto & st : item.vConnectedIpCnt_) {
            D_THD_LOG(egwLogName, "    - [%s:%s:%d]",
                st.name.c_str(),
                st.ip.c_str(),
                st.connCnt);
        }
    }
}

/*-
{
    "PG" : [
        {"NAME" : "PG01", "IP" : "192.168.10.256"},
        {"NAME" : "PG02", "IP" : "192.168.10.256"},
        {"NAME" : "PG03", "IP" : "192.168.10.256"},
        {"NAME" : "PG04", "IP" : "192.168.10.256"}
    ],
    "PGW" : [
        {"NAME" : "PGW01", "IP" : "192.168.10.256"},
        {"NAME" : "PGW02", "IP" : "192.168.10.256"},
        {"NAME" : "PGW03", "IP" : "192.168.10.256"},
        {"NAME" : "PGW04", "IP" : "192.168.10.256"}
    ],
    "SERVICE" : [
#       ["NAME","POLICY", "TARGET", "PROTOCOL", "PCF is SERVER/CLIENT",  "PORT", "MIN-CONNECTION-CNT", "MIN-IP-CNT"]
        ["5G-HDV-Routing", "BEST", "PG", "TCP", "CLIENT", 3868, 1, 1],
        ["LTE-HDV-Routing","BEST", "PG", "TCP", "CLIENT", 3868, 1, 1],
        ["5G-CN-USER-Noti","BEST", "PG", "TCP", "CLIENT", 3868, 1, 1],
        ["EPC-PG-Noti",    "BEST", "PG", "TCP", "CLIENT", 3868, 1, 1],
        ["Zone-InOut",     "BEST", "PG", "TCP", "CLIENT", 3868, 1, 1],
        ["PG-Local-DB",    "WORST","PG", "TCP", "CLIENT", 3868, 1, 1],
        ["Gx",             "WORST", "PGW","SCTP","SERVER", 3868, 1, -1]
    ]
}
-*/

bool ServiceConfig::load() {
    // JSON 파일을 읽습니다.
    std::ifstream    istream;
    istream.open(fMonitor_.GetFileName());

    if(istream.good() == false) {
        W_THD_LOG(egwLogName,"Config File load fail [%s]", fMonitor_.GetFileName());
        return false;
    }

    std::stringstream   buffer;
    buffer << istream.rdbuf();
    istream.close();

    return toParse(buffer.str());
}

bool ServiceConfig::toParse(std::string && _str) { 

    rapidjson::Document     doc;

    if(doc.Parse(_str.c_str()).HasParseError()) {
        W_THD_LOG(egwLogName, "Config File Json Parse fail [%s]", _str.c_str());
        return false;
    }

	// ServiceItem vector 를 채웁니다.
	std::vector<ServiceItem> 	vec;

    if(setService(vec, doc) == false) {
        W_THD_LOG(egwLogName, "Json Parse fail for Service [%s]", _str.c_str());
        return false;
    }

    if(setIp(vec, doc) == false) {
        W_THD_LOG(egwLogName, "Config File Json Parse fail for Ip [%s]", _str.c_str());
        return false;
    }

	// 모두 성공하면, vector 를 변경합니다.
	vItems_ = vec;
    return true;
}

bool ServiceConfig::setService(std::vector<ServiceItem> & _vec, rapidjson::Document & _doc) {
    std::string key("SERVICE");
    if(_doc.HasMember(key.c_str()) == false) {
        W_THD_LOG(egwLogName, "[%s] is not exit in Json", key.c_str());
        return false;
    }

    rapidjson::Value & value = _doc[key.c_str()];
    if(value.IsArray() == false) {
        W_THD_LOG(egwLogName, "[%s] is not array in Json", key.c_str());
        return false;
    }

    for(size_t i=0; i < value.Size(); ++i) {
        rapidjson::Value & line = value[i];
        if(line.IsArray() == false) {
            W_THD_LOG(egwLogName, "[%s] is not array in Json", key.c_str());
            return false;
        }

        //  ["NAME","POLICY","TARGET","SERVER/CLIENT","PORT","PROTOCOL","MIN-CONNECTION-CNT","MIN-IP-CNT"]
        if(line.Size() != 8) {
            W_THD_LOG(egwLogName, "[%s] Size is unexpected [%zd]", key.c_str(), line.Size());
            return false;
        }

        ServiceItem     item;
        item.name_          = line[0].GetString();
        item.svcPolicy_     = line[1].GetString();
        item.targetSystem_  = line[2].GetString();
        item.proto_ = [](std::string _str) -> ServiceProtocol { 
            if(strcasestr(_str.c_str(), "TCP") != nullptr)
                return ServiceProtocol::TCP;
            if(strcasestr(_str.c_str(), "UDP") != nullptr)
                return ServiceProtocol::UDP;
            if(strcasestr(_str.c_str(), "SCTP") != nullptr)
                return ServiceProtocol::SCTP;
            return ServiceProtocol::ERR;
        }(line[3].GetString());
        item.bServer_ = (strcasestr(line[4].GetString(), "S") != nullptr)?true:false;
        item.nPort_                 = line[5].GetInt();
        item.nMinConnectionLimit_   = line[6].GetInt();
        item.nMinSystemCntLimit_    = line[7].GetInt();

        _vec.emplace_back(item);
    }

    return true;
}

bool ServiceConfig::setIp(std::vector<ServiceItem> & _vec, rapidjson::Document & _doc) {
    for(auto & item : _vec) {
        std::string key(item.targetSystem_.c_str());
        if(_doc.HasMember(key.c_str()) == false) {
            W_THD_LOG(egwLogName, "[%s] is not exit in Json", key.c_str());
            return false;
        }

        rapidjson::Value & value = _doc[key.c_str()];
        if(value.IsArray() == false) {
            W_THD_LOG(egwLogName, "[%s] is not array in Json", key.c_str());
            return false;
        }

        for(size_t i=0; i < value.Size(); ++i) {
            rapidjson::Value & obj = value[i];
            if(obj.IsObject() == false) {
                W_THD_LOG(egwLogName, "[%s] is not Object in Json", key.c_str());
                return false;
            }

            if(obj.HasMember("NAME") == false || obj.HasMember("IP") == false) {
                W_THD_LOG(egwLogName, "[%s] is not exist NAME or IP", key.c_str());
                return false;
            }
        
            // {"NAME" : "PG01", "IP" : "192.168.10.256"},
            stConnectedCnt      st;
            st.name = obj["NAME"].GetString();
            st.ip   = obj["IP"].GetString();

            item.vConnectedIpCnt_.emplace_back(st);
        }
    }

    return true;
}

bool ServiceConfig::UpdateConnection(NetstatTCP4 * _tcp4, NetstatSCTP * _sctp){
    // 기존의 연결 값과 Update 되었다면, true 기존 값이 변경되지 않으면 false 입니다.
    bool bret = false;
    int  cnt  = 0;

    for(auto & item : vItems_) {
        for(auto & st : item.vConnectedIpCnt_) {

            cnt = 0;
            // st.ip , item.bServer_ , item.proto_
            switch(item.proto_) {
            case ServiceProtocol::TCP:
                cnt = getConnectedCnt(_tcp4, st.ip, item.nPort_, item.bServer_);

                if(!bret && st.connCnt != cnt)
                    bret = true;

                st.connCnt = cnt;
                break;
            case ServiceProtocol::SCTP:
                cnt = getConnectedCnt(_sctp, st.ip, item.nPort_, item.bServer_);

                if(!bret && st.connCnt != cnt)
                    bret = true;

                st.connCnt = cnt;
                break;
            case ServiceProtocol::UDP:
            case ServiceProtocol::ERR:
            default:
                W_THD_LOG(egwLogName, "Unsupported PROTOCOL [%d]", static_cast<int>(item.proto_));
                break;
            }
        }  
    }

    D_THD_LOG(egwLogName, "-------- [Changed:%d]", bret);

    if(bret)
        prnItem();

    return bret;

}

int ServiceConfig::getConnectedCnt(NetstatTCP4 * _tcp4, std::string & _ip, int _nPort, bool _bServer) {

    _tcp4->ClearResult();

    if(_bServer) {
        _tcp4->Filter(NetstatItem::REMOTE_IP, _ip)
             ->Filter(NetstatItem::LOCAL_PORT, _nPort)
             ->Filter(NetstatItem::STATE, NetstatValue::TCP_ESTABLISHED);
    } else {
        _tcp4->Filter(NetstatItem::REMOTE_IP,   _ip)
             ->Filter(NetstatItem::REMOTE_PORT, _nPort)
             ->Filter(NetstatItem::STATE, NetstatValue::TCP_ESTABLISHED);
    }

    return _tcp4->GetFilteredCnt();
}

int ServiceConfig::getConnectedCnt(NetstatSCTP * _sctp, std::string & _ip, int _nPort, bool _bServer) {

    _sctp->ClearResult();

    if(_bServer) {
        _sctp->Filter(NetstatItem::REMOTE_IP, _ip)
             ->Filter(NetstatItem::LOCAL_PORT, _nPort)
             ->Filter(NetstatItem::STATE, NetstatValue::TCP_ESTABLISHED);
    } else {
        _sctp->Filter(NetstatItem::REMOTE_IP,   _ip)
             ->Filter(NetstatItem::REMOTE_PORT, _nPort)
             ->Filter(NetstatItem::STATE, NetstatValue::TCP_ESTABLISHED);
    }

    return _sctp->GetFilteredCnt();
}

void ServiceConfig::MakeAtomReportMessageForPG(std::string & _out) {

/*-
    {
    "report-node-id":"pcf01-lb01-app01",
    "service-list":[{
        "service":"5G HDV Routing",
        "alternative-node-id":"pcf01-lb02-app02",
        "service-policy" : "WORST",
        "service-status" : "OK",
            "list":[{
            "name":"PG01",
            "Status":"OK",
                "IP":"172.1.1.12"
            },{
            "name":"PG02",
            "Status":"NOK",
            "IP":"172.1.1.13"
        }]
    }]
    }
-*/

    _out.clear();


    // PG 는 모든 LB 가 연동하니까요.. PGW 여부를 체크하지 않아요.
    if(vItems_.empty())
        return;

    char    buf[256];

    snprintf(buf, sizeof(buf), "{\"report-node-id\": \"%s\",", gSysName.c_str());
    _out.append(buf);

    snprintf(buf, sizeof(buf), "\"service-list\": [");
    _out.append(buf);

    for(auto & item : vItems_) {

        if(IsPGWItem(item.name_) == true)
            continue;

        makeServiceList(_out, item);
        _out.append(",");
        alarm_.Trap(item);
    }
    _out.pop_back();

    snprintf(buf, sizeof(buf), "]}");
    _out.append(buf);

}

void ServiceConfig::makeServiceList(std::string & _out, const ServiceItem & _item) {


    // PGW 는 makeIngressList 로 해서, 따로 만들 꺼예요.
    char    buf[256];

    snprintf(buf, sizeof(buf), "{\"service\": \"%s\",", _item.name_.c_str());
    _out.append(buf);

    snprintf(buf, sizeof(buf), "\"service-status\": \"%s\",", _item.IsOK()?"OK":"NOK");
    _out.append(buf);

    snprintf(buf, sizeof(buf), "\"alternative-node-id\": \"%s\",", alternativeNodeId_.c_str());
    _out.append(buf);

    snprintf(buf, sizeof(buf), "\"service-policy\": \"%s\",", _item.GetPolicy());
    _out.append(buf);

    snprintf(buf, sizeof(buf), "\"list\": [");
    _out.append(buf);

    for(auto & i : _item.vConnectedIpCnt_) {
        snprintf(buf, sizeof(buf), "{\"name\":\"%s\", \"Status\":\"%s\", \"ip\":\"%s\"}",
            i.name.c_str(),
            (i.connCnt > 0)?"OK":"NOK",
            i.ip.c_str());
        _out.append(buf);
        _out.append(",");
    }

    _out.pop_back();

    snprintf(buf, sizeof(buf), "]}");
    _out.append(buf);

}

bool ServiceConfig::IsPGWItem(std::string _name) {

    std::transform(_name.begin(), _name.end(), _name.begin(), ::toupper);

    return _name.compare("PGW") == 0;

}


void ServiceConfig::MakeAtomReportMessageForPGW(std::string & _out) {

/*-
    {
    "report-node-id":"pcf01-lb01-app01",
    "ingress-list":[{
        "service":"PGW",
        "node-list":[{
            "node-name":"PGW01",
            "tps" : 0,
            "connections":[{
                "IP":"172.1.1.12",
                "count": 0
            }]
            },{
            "node-name":"PGW01",
            "tps" : 0,
            "connections":[{
                "IP":"172.1.1.12",
                "count": 0
            }]

        }]
    }]
    }
-*/



    _out.clear();

    if(vItems_.empty() || doYouHavePGWType() == false) 
        return;

    char    buf[256];

    snprintf(buf, sizeof(buf), "{\"report-node-id\": \"%s\",", gSysName.c_str());
    _out.append(buf);

    snprintf(buf, sizeof(buf), "\"ingress-list\": [");
    _out.append(buf);

    for(auto & item : vItems_) {
        if(IsPGWItem(item.name_) == false)
            continue;

        makeIngressList(_out, item);
        _out.append(",");
        alarm_.Trap(item);
    }
    _out.pop_back();

    snprintf(buf, sizeof(buf), "]}");
    _out.append(buf);

}

bool ServiceConfig::doYouHavePGWType() {

    for(const auto & item : vItems_) {
        if(IsPGWItem(item.name_))
            return true;
    }

    return false;
}


void ServiceConfig::makeIngressList(std::string & _out, const ServiceItem & _item) {

    // 여기는 PGW 만 만들어요. 
    char    buf[256];

    snprintf(buf, sizeof(buf), "{\"service\": \"%s\",", _item.name_.c_str());
    _out.append(buf);


    snprintf(buf, sizeof(buf), "\"node-list\": [");
    _out.append(buf);

    for(auto & i : _item.vConnectedIpCnt_) {
        snprintf(buf, sizeof(buf), "{\"node-name\":\"%s\", \"tps\":0,", i.name.c_str());
        _out.append(buf);

        snprintf(buf, sizeof(buf), "\"connections\":[{\"ip\": \"%s\", \"count\": %d}]", i.ip.c_str(), i.connCnt);
        _out.append(buf);

        snprintf(buf, sizeof(buf), "}");
        _out.append(buf);

        _out.append(",");
    }
    _out.pop_back();

    snprintf(buf, sizeof(buf), "]}");
    _out.append(buf);

}


// ServiceItem ///////////////////////////////////////////////////////////////

ServiceItem::ServiceItem() {

}

ServiceItem::~ServiceItem() {

}

/*-
ServiceItem::ServiceItem(const ServiceItem & _inst) {
    name_           = _inst.name_;
    targetSystem_   = _inst.targetSystem_;
    vConnectedIpCnt_= _inst.vConnectedIpCnt_;
    bServer_        = _inst.bServer_;
    usPort_         = _inst.usPort_;
    proto_          = _inst.proto_;
    nMinConnectionLimit_ = _inst.nMinConnectionLimit_;
    nMinSystemCntLimit_  = _inst.nMinSystemCntLimit_;
}

ServiceItem & ServiceItem::operator=(const ServiceItem & _inst) {
    if(this != &st) {
        name_           = _inst.name_;
        targetSystem_   = _inst.targetSystem_;
        vConnectedIpCnt_= _inst.vConnectedIpCnt_;
        bServer_        = _inst.bServer_;
        usPort_         = _inst.usPort_;
        proto_          = _inst.proto_;
        nMinConnectionLimit_ = _inst.nMinConnectionLimit_;
        nMinSystemCntLimit_  = _inst.nMinSystemCntLimit_;
    }

    return *this;
}
-*/

bool ServiceItem::IsOK() const {

    // 5G-HDV-Routing
    if(strcasestr(name_.c_str(), "5G-HDV-Routing") != nullptr)
        return policy_5G_HDV_Routing();

    // LTE-HDV-Routing 
    if(strcasestr(name_.c_str(), "LTE-HDV-Routing") != nullptr)
        return policy_LTE_HDV_Routing();

    // 5G-CN-USER-Noti 
    if(strcasestr(name_.c_str(), "5G-CN-USER-Noti") != nullptr)
        return policy_5G_CN_USER_Noti();

    // EPC-PG-Noti
    if(strcasestr(name_.c_str(), "EPC-PG-Noti") != nullptr)
        return policy_EPC_PG_Noti();

    // Zone-InOut 
    if(strcasestr(name_.c_str(), "Zone-InOut") != nullptr)
        return policy_Zone_InOut();

    // PG-Local-DB
    if(strcasestr(name_.c_str(), "PG-Local-DB") != nullptr)
        return policy_PG_Local_DB();

    // Baro-D-InOut
    if(strcasestr(name_.c_str(), "Baro-D-InOut") != nullptr)
        return policy_Baro_D_InOut();

    W_THD_LOG(egwLogName, "[%s] is unknown service name", name_.c_str());
    return false;
}

bool ServiceItem::policy_default() const {
    int cnt = 0;
    for(auto & i : vConnectedIpCnt_)
        cnt += i.connCnt;

    return cnt > 0;
}

bool ServiceItem::policy_5G_HDV_Routing() const {

    // HDV Voice 는 PG 시스템 중 한개만 붙어도 되요.
    // 최소 연결은 1개만 있으면 되요.
    // SBI 연결이지요.
    // LB01 과 LB02 중 하나는 꼭 필요.
    return policy_default();    
}

bool ServiceItem::policy_LTE_HDV_Routing() const {

    // LTE HDV Voice 는 PG 시스템 중 한개만 붙어도 되요.
    // 최소 시스템 연결은 1개만 있으면 되요.
    // LB01 과 LB02 중 하나는 꼭 필요.
    return policy_default();    
}

bool ServiceItem::policy_5G_CN_USER_Noti() const {

    // 5G 가입자 NOTI 는 PG 시스템 중 한개만 붙어도 되요.
    // 최소 연결은 1개만 있으면 되요.
    // LB01 과 LB02 중 하나는 꼭 필요.
    return policy_default();    
}

bool ServiceItem::policy_EPC_PG_Noti() const {
    
    // RBUS NOTI 를 말해요..
    // 최소 연결은 1개만 있으면 되요.
    // LB01 과 LB02 중 하나는 꼭 필요.
    return policy_default();    
}

bool ServiceItem::policy_Zone_InOut() const {
    
    // 5G 가입자 NOTI 는 PG 시스템 중 한개만 붙어도 되요.
    // 최소 연결은 1개만 있으면 되요.
    // LB01 과 LB02 중 하나는 꼭 필요.
    return policy_default();    
}

bool ServiceItem::policy_PG_Local_DB() const {
    
    // 5G 가입자 NOTI 는 PG 시스템 중 두개 연결되어야 해요.
    // 최소 연결은 1개 있어야 해요. 

    int connectedSystemCnt = 0;
    int cnt = 0;
    for(auto & i : vConnectedIpCnt_) {
        cnt += i.connCnt;
        if(i.connCnt > 0)
            ++connectedSystemCnt;
    }

    return connectedSystemCnt >= 2 && cnt >= 2;
}

bool ServiceItem::policy_Baro_D_InOut() const {
    return policy_Zone_InOut();
}

// AlarmForExternalConnection

AlarmForExternalConnection::AlarmForExternalConnection() {

}

AlarmForExternalConnection::~AlarmForExternalConnection() {

}

void AlarmForExternalConnection::Trap(ServiceItem & _item) {

    bool    bIsOK = _item.IsOK();
    
    if(isChangedAlarmStatus(_item.name_, bIsOK) == true) {

        const char * p = (bIsOK)?"CONNECT":"DISCONNECT";

        // SendAlarm
        I_THD_LOG(egwLogName, 
            "exteranl.connection.status [%s] [%s]", 
            _item.name_.c_str(), 
            p);

        std::map<std::string, std::string>  mV;
        std::map<std::string, std::string>  mD;

        std::string metric = 
            (ServiceConfig::IsPGWItem(_item.name_))?
            "pgw.connection.status":"external.connection.status";

        mV.emplace("service", _item.name_);
        ATOM_TRAP_EXT(ATOM_TRAP_ACTION_NOTIFY,
                    metric.c_str(),
                    const_cast<char *>(gSysName.c_str()),
                    const_cast<char *>(p),
                    mD,
                    mV);
    }
}

bool AlarmForExternalConnection::isChangedAlarmStatus(const std::string & _sname, bool _bAlarm) {

    // map 에서 찾아봐요.

    auto iter = m_.find(_sname);

    if(iter == m_.end()) {
        m_.emplace(_sname, _bAlarm);
        return true;
    }

    if(iter->second != _bAlarm) {
        iter->second = _bAlarm;
        return true;
    }

    return false;
}
