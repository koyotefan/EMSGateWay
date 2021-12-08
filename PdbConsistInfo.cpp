
#include "CNDFIniConfigContainer.hpp"

#include "PDBConfig.hpp"
#include "PdbConsistInfo.hpp"

extern CNDFIniConfigContainer * gCfg;

PdbConsistInfo::PdbConsistInfo() {

}

PdbConsistInfo::~PdbConsistInfo() {

}

bool PdbConsistInfo::Load() {

    std::string fileName = getPdbConfigFileName();

    if(fileName.length() == 0)
        return false;

    PDB::Config     config;

    if(config.Init(fileName) == false) {

        return false;
    }

    std::vector<std::string>     vecDBNames;
    config.GetDBNames(vecDBNames);

    for(auto & dbName : vecDBNames) {

        // name = DBType + shardingNumber
        std::string name = PDB::Config::GetClusterName(dbName);
        PDB::eDefClusterType type = PDB::Config::ExtractClusterType(dbName);

        setup(name, type, dbName);
    }

    return true;
}

std::string PdbConsistInfo::getPdbConfigFileName() {

    if(gCfg == nullptr) {
        E_THD_LOG(egwLogName,"Configuration is nullptr");
        return std::string("");
    }

    char buf[245];
    if((gCfg->Get((char *)"PCF_PDB", (char *)"CONFIG", buf) == false) &&
       (gCfg->Get((char *)"PDB_INFO", (char *)"CONFIG", buf) == false)) {
            E_THD_LOG(egwLogName,"Configuration (PCF_PDB:CONFIG),(PDB_INFO:CONFIG) not found");
            return std::string("");
    }

    return std::string(buf);
}

void PdbConsistInfo::setup(std::string & _name,
                           PDB::eDefClusterType  _type,
                           std::string & _dbName) {

    auto iter = mInfo_.find(_name);

    if(iter == mInfo_.end()) {
        DBNameArray     arr;
        mInfo_.emplace(_name, arr);

        iter = mInfo_.find(_name);
    }

    (iter->second).at(static_cast<size_t>(_type)) = _dbName;
}

size_t PdbConsistInfo::Difference(std::vector<std::pair<std::string, std::string>> & _vec,
                      std::unordered_map<std::string, std::string> & _map) {

    // D_THD_LOG(egwLogName,"-----Difference [%zu] [%zu]", _map.size(), mInfo_.size());

    for(auto & iter : mInfo_) {

        if(_map.find(iter.first) == _map.end())
            _vec.push_back({iter.first, iter.second[0]});
    }

    return _vec.size();

}
