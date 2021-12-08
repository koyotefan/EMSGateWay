#ifndef PDB_REPORTER_HPP
#define PDB_REPORTER_HPP

#include <map>
#include <tuple>
#include <vector>

#include "PDBConnectionManager.hpp"
#include "PDBWorker.hpp"

#include "EGWUdpServer.hpp"
#include "ThreadBase.hpp"

#include "PdbStatusSQL.hpp"

#include "CThreadMonitor.hpp"

struct stPSConnectCnt {
    int          PActive  = 0;
    int          PConnect = 0;
    int          SActive  = 0;
    int          SConnect = 0;
};

class PdbReporter : public ThreadBase {
public:
    explicit PdbReporter();
    ~PdbReporter() { ThreadBase::join(); }

    bool Init(int _port, bool _bIpv6);
    void RegisterPdbManager(std::shared_ptr<PDB::ConnectionManager> _pcm, std::memory_order=std::memory_order_seq_cst) {
        pcm_ = atomic_load(&_pcm);
    }
    void SetHangupThreadMonitor(CThreadMonitor * _pThreadMonitor) {
        pThreadMonitor_ = _pThreadMonitor;
    }

private:
    void run(std::shared_ptr<bool> _pRefCounter);
    bool update(PDB::Worker * _pdbW, const char * _buf);
    bool parse(std::string & _nodeId, std::string & _procName, std::vector<stPdbStatusItem> & _vec, const char * _buf);
    void alarmReport(PDB::Worker * _pdbW);

    bool isPGLocalDB(const stPdbStatusItem & _item);
    bool isEqual(stPdbStatusItem & _item);
    void updateMemory(stPdbStatusItem & _item);
    void forceMemoryReset();

    void deleteNodeDB(PDB::Worker * _pdbW, std::string & _nodeId);
    void deleteProcDB(PDB::Worker * _pdbW, std::string & _nodeId, std::string & _procId);
    void deleteDB(PDB::Worker * _pdbW, stPdbStatusItem & _item);
    bool updateDB(PDB::Worker * _pdbW, stPdbStatusItem & _item);
    bool insertDB(PDB::Worker * _pdbW, stPdbStatusItem & _item);
    void selectDB(PDB::Worker * _pdbW, std::map<std::string, std::string> & _mV);

private:
    std::shared_ptr<PDB::ConnectionManager>     pcm_;
    EGWUdpServer    udp_;

    std::map<std::tuple<std::string, std::string>, stPdbStatusItem>    mapItems_;

    DeleteNodePdbStatusSQL  dNodeSql_;
    DeleteProcPdbStatusSQL  dProcSql_;
    DeletePdbStatusSQL      dsql_;
    SelectPdbStatusSQL      ssql_;
    UpdatePdbStatusSQL      usql_;
    InsertPdbStatusSQL      isql_;
    SelectPdbALLSQL         asql_;

    time_t          lastResetT_  = 0;
    time_t          alarmGuardT_ = 0;

    std::string      className_;
    CThreadMonitor * pThreadMonitor_ = nullptr;

};



#endif // PDB_REPORTER_HPP
