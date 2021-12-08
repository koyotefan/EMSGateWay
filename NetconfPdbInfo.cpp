
#include "PDBConfig.hpp"
#include "NetconfPdbInfo.hpp"

NetconfPdbInfo::NetconfPdbInfo() {

}

NetconfPdbInfo::~NetconfPdbInfo() {

}

bool NetconfPdbInfo::Init() {
    return netconf_.Init();
}

bool NetconfPdbInfo::Load() {

    // unique 하게 netconf 에 접근해서.. 값을 읽어 올꺼예요.
    if(netconf_.LoadConfig() == false) {
        W_THD_LOG(egwLogName,"PDB Active netconf Load fail");
        return false;
    }

    std::string jData = netconf_.GetData();

    if(setInfo(jData) == false) {
        W_THD_LOG(egwLogName,"PDB Active Info is wrong [%s]", jData.c_str());
        return false;
    }

    return true;
}

bool NetconfPdbInfo::setInfo(std::string & _jData) {

    return  PDB::Config::Parsing_PCF_PDB_ACTIVE(m_, _jData);
}

bool NetconfPdbInfo::AppendInfo(PdbConsistInfo & _consist) {

    // consist 정보의 ClusterName 이 netconf 에 없으면, write 해야 해야 합니다.
    std::vector<std::pair<std::string, std::string>>   vec;
    size_t cnt = _consist.Difference(vec, m_);

    // D_THD_LOG(egwLogName,"----- Difference [%zu]", cnt);


    if(cnt == 0)
        return true;

    for(auto & iter : vec) {

        // D_THD_LOG(egwLogName,"----- AppendInfo [%s:%s]", std::get<0>(iter).c_str(), std::get<1>(iter).c_str());
        m_.emplace(std::get<0>(iter), std::get<1>(iter));
    }

    return netconf_.SaveConfig(m_);

}

bool NetconfPdbInfo::Reflect(const std::vector<std::string> & _vecActive) {

    std::vector<std::pair<std::string, std::string>>   vec;
    for(auto & activeName : _vecActive)
        vec.push_back({PDB::Config::GetClusterName(activeName), activeName});

    for(auto & ivec : vec) {

        std::string & clusterName = std::get<0>(ivec);
        std::string & activeName = std::get<1>(ivec);

        auto imap = m_.find(clusterName);

        if(imap == m_.end()) {
            m_.emplace(clusterName, activeName);
            //I_THD_LOG(egwLogName,"---------- Emplace cluster[%s] active[%s]", 
            //    clusterName.c_str(), ativeName.c_str()); 
            continue;
        }

        imap->second = activeName;

        /*- 무조건 적용하기 위하여 --
         if(activeName.compare(imap->second) != 0) {
            imap->second = activeName;
            //I_THD_LOG(egwLogName,"---------- Compare cluster[%s] active[%s]", 
            //    clusterName.c_str(), ativeName.c_str()); 
        }
        ---*/

    }

    bool bret = netconf_.SaveConfig(m_);

    I_THD_LOG(egwLogName,"SaveConfig [%s] map size[%zd]", bret?"success":"fail", m_.size()); 
    
    return bret;
}
