#ifndef NETCONF_PDB_INFO_HPP
#define NETCONF_PDB_INFO_HPP

#include <unordered_map>

#include "CNetconfPDBActive.hpp"
#include "PdbConsistInfo.hpp"

extern thread_local char egwLogName[32];

class NetconfPdbInfo
{
public:
    explicit NetconfPdbInfo();
    ~NetconfPdbInfo();

    bool Init();
    bool Load();
    bool AppendInfo(PdbConsistInfo & _consist);
    bool Reflect(const std::vector<std::string> & _vecActive);

private:
    bool setInfo(std::string & _jData);

private:
    CNetconfPDBActive   netconf_;
    std::unordered_map<std::string, std::string>    m_;
};


#endif // NETCONF_PDB_INFO
