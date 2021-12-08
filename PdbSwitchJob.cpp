
#include "rapidjson/document.h"

#include "CNDFATOMAdaptor.hpp"
#include "PdbSwitchJob.hpp"
#include "PdbConsistInfo.hpp"

extern std::string  gSysName;

bool PdbSwitchJob::Init() {

    PdbConsistInfo  consist;
    if(consist.Load() == false) {
        W_THD_LOG(egwLogName,"PdbSwitchJob.consist.Load fail");
        return false;
    }

    if(netconfInfo_.Init() == false) {
        W_THD_LOG(egwLogName,"PdbSwitchJob.netconfInfo.Init fail");
        return false;
    }

    if(netconfInfo_.Load() == false) {
        W_THD_LOG(egwLogName,"PdbSwitchJob.netconfInfo.Load fail");
        return false;
    }

    if(netconfInfo_.AppendInfo(consist) == false) {
        W_THD_LOG(egwLogName,"PdbSwitchJob.AppendInfo fail");
        return false;
    }

    return true;

}

bool PdbSwitchJob::Do(HttpRequest & _req, HttpResponse & _resp) {

    std::string body = _req.GetBody();

    D_THD_LOG(egwLogName,"%s", body.c_str());

    std::vector<std::string>    activeNames;
    setActiveNames(activeNames, body);

    if(activeNames.empty()) {
        D_THD_LOG(egwLogName,"data is zero %s", body.c_str());
        return true;
    }

    bool bret = netconfInfo_.Reflect(activeNames);

    I_THD_LOG(egwLogName,"PDB Switching netconf reflect [%d]", bret);

    if(bret) {

        std::map<std::string, std::string> mD;
        for(auto & iter : activeNames) {
            mD.insert({iter, " "});
        }
        std::map<std::string, std::string> mV;

        ATOM_TRAP_EXT(ATOM_TRAP_ACTION_NOTIFY,
                        "db.connection.switch",
                        const_cast<char *>(gSysName.c_str()),
                        const_cast<char *>("SUCCESS"),
                        mD,
                        mV);
    }

    return true;

}

void PdbSwitchJob::setActiveNames(std::vector<std::string> & _vec, std::string & _body) {

    rapidjson::Document     doc;

    try {
        if(doc.Parse(_body.c_str()).HasParseError()) {
            W_THD_LOG(egwLogName,"json Parse() error [%s]", _body.c_str());
            return ;
        }

        std::string key("DB_NAME");
        if(doc.HasMember(key.c_str()) == false) {
            W_THD_LOG(egwLogName,"[%s] is not exist [%s]", key.c_str(), _body.c_str());
            return;
        }

        rapidjson::Value & value = doc[key.c_str()];

        if(value.IsArray() == false) {
            W_THD_LOG(egwLogName,"[%s] is not Array [%s]", key.c_str(), _body.c_str());
            return;
        }

        for(size_t i=0; i < value.Size(); ++i) {
            _vec.push_back(value[i].GetString());
        }

    } catch ( std::exception & e) {
        W_THD_LOG(egwLogName,"json Parse() exception [%s:%s]", e.what(), _body.c_str());
        return ;
    }
}
