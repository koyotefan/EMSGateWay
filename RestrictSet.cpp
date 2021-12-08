
#include <vector>
#include <algorithm>
#include <fstream>

#include "EGWDefine.hpp"
#include "RestrictSet.hpp" 

extern thread_local char egwLogName[32];

bool RestrictSet::LoadFile(std::string & _fileName) {

    try {
        loadFile(_fileName);
    } catch (...) {

        E_THD_LOG(egwLogName,"LoadFile is fail [%s]", _fileName.c_str());
        return false;
    }

    return true;
}

void RestrictSet::loadFile(std::string & _fileName) {

    std::ifstream   istream;
    istream.open(_fileName);

    if(istream.good() == false) {
        W_THD_LOG(egwLogName,"Restrict File load fail [%s]", _fileName.c_str());
        return ;
    }

    std::string rawline;
    while(std::getline(istream, rawline)) {

        auto t = EGWCommon::split(rawline, "#");
        std::string line = std::get<0>(t);

        if(line.length() == 0)
            continue;

        stRestrictCondition     item;
        auto firstTup = EGWCommon::split(line, ",");
        item.sst = std::get<0>(firstTup);

        auto secondTup = EGWCommon::split(std::get<1>(firstTup), ",");
        item.sd = std::get<0>(secondTup);
        item.dnn = std::get<1>(secondTup);

        set_.insert(item);
    }

    istream.close();
}

bool RestrictSet::IsExist(const stRestrictCondition & _item) {

    return set_.find(_item) != set_.end();    
}

bool RestrictSet::IsExist(const stRestrictCondition && _item) {

    return set_.find(_item) != set_.end();
}

void RestrictSet::Add(const stRestrictCondition & _item) {
    if(_item.sst.empty() || _item.sd.empty() || _item.dnn.empty() ) {
        W_THD_LOG(egwLogName,"Invalid item [%4s %10s %20s]",
            _item.sst.c_str(),
            _item.sd.c_str(),
            _item.dnn.c_str());

        return ;
    }

    set_.insert(_item);

    I_THD_LOG(egwLogName,"Restrict Add [%4s %10s %20s]", 
        _item.sst.c_str(), _item.sd.c_str(), _item.dnn.c_str());
}

void RestrictSet::Del(const stRestrictCondition & _item) {

    set_.erase(_item);

    I_THD_LOG(egwLogName,"Restrict Del [%4s %10s %20s]", 
        _item.sst.c_str(), _item.sd.c_str(), _item.dnn.c_str());
}

bool RestrictSet::WriteFile(std::string & _fileName) {

    try {
        writeFile(_fileName);
    } catch (...) {

        E_THD_LOG(egwLogName,"WriteFile is fail [%s]", _fileName.c_str());
        return false;
    }

    return true;
}

void RestrictSet::writeFile(std::string & _fileName) {

    std::ofstream   out;
    out.open(_fileName, std::ios::out|std::ios::trunc);

    if(out.good() == false) {
        W_THD_LOG(egwLogName,"Restrict File write fail [%s]", _fileName.c_str());
        return ;
    }

    D_THD_LOG(egwLogName,"Restrict file write [%s]", _fileName.c_str());
    for(const auto & item : set_) {
        out << item.sst << "," << item.sd << "," << item.dnn << "\n";
        I_THD_LOG(egwLogName, "file - [%4s %10s %20s]", 
            item.sst.c_str(), item.sd.c_str(), item.dnn.c_str());
    }
    out.close();
    return ;    
}


bool RestrictSet::Equal(const RestrictSet & _other) {

    std::vector<stRestrictCondition>    v;

    std::set_difference(set_.begin(), 
                        set_.end(), 
                        _other.set_.begin(), 
                        _other.set_.end(), 
                        std::back_inserter(v));

    if(v.size() > 0)
        return false;

    std::set_difference(_other.set_.begin(), 
                        _other.set_.end(), 
                        set_.begin(), 
                        set_.end(), 
                        std::back_inserter(v));

    return (v.size() == 0)?true:false;
}
