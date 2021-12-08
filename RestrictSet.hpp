
#ifndef __RESTRICT_SET_H__
#define __RESTRICT_SET_H__

#include <string>
#include <set>

#include "TBLRecord.hpp"

struct stRestrictCondition {
    std::string     sst;
    std::string     sd;
    std::string     dnn;

    stRestrictCondition() = default;
    stRestrictCondition(const stRestrictCondition & _st) = default;
    stRestrictCondition(const stNsmfSessionInfoRecord & _row) {
        sst = std::to_string(_row.sst);
        sd  = _row.sd;
        dnn = _row.dnn;
    }
    ~stRestrictCondition() = default;
    stRestrictCondition & operator=(const stRestrictCondition & _st) = default;

    bool operator<(const stRestrictCondition & _right) const {
    
        if(this == &_right)
            return false;
    
        if(!(this->sst == _right.sst))
            return this->sst < _right.sst;
        if(!(this->sd == _right.sd))
            return this->sd < _right.sd;
        
        return this->dnn < _right.dnn;
    }
};


class RestrictSet {
public:
    explicit RestrictSet() = default;
    RestrictSet & operator=(const RestrictSet & _other) = default;

    ~RestrictSet() = default;

    void Clear() { set_.clear(); }
    bool Equal(const RestrictSet & _other);
    bool IsExist(const stRestrictCondition & _item);
    bool IsExist(const stRestrictCondition && _item);
    void Add(const stRestrictCondition & _item);
    void Del(const stRestrictCondition & _item);
    bool LoadFile(std::string & _fileName);
    bool WriteFile(std::string & _fileName);

private:
    void loadFile(std::string & _fileName);
    void writeFile(std::string & _fileName);

private:
    std::set<stRestrictCondition>   set_;    

};

#endif
