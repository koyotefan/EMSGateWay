#ifndef PDB_CONSIST_INFO_HPP
#define PDB_CONSIST_INFO_HPP

#include <unordered_map>
#include <array>

#include "PDBConfig.hpp"

extern thread_local char egwLogName[32];

using DBNameArray = std::array<std::string,
                               static_cast<size_t>(PDB::eDefClusterType::Cnt)>;

// Thread unsafe
class PdbConsistInfo {
public:
    explicit PdbConsistInfo();
    ~PdbConsistInfo();

    bool Load();

    size_t Difference(std::vector<std::pair<std::string, std::string>> & _vec,
                      std::unordered_map<std::string, std::string> & _map);

private:
    std::string getPdbConfigFileName();
    void setup(std::string & _name,
               PDB::eDefClusterType _type,
               std::string & _dbClusterName);


private:
    std::unordered_map<std::string, DBNameArray>    mInfo_;
};


#endif // PDB_CONSIST_INFO_HPP
