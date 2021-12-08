#ifndef __SliceDnnWorker_HPP__
#define __SliceDnnWorker_HPP__


#include "ThreadBase.hpp"
#include "PDBConnectionManager.hpp"
#include "PDBWorker.hpp"

#include "CThreadMonitor.hpp"
#include "NetconfModifier.hpp"

#include "TBLRecord.hpp"
#include "DB_SliceDnnWorkSQL.hpp"

class SliceDnnWorker : public ThreadBase
{
public:
    explicit SliceDnnWorker();
    ~SliceDnnWorker() { ThreadBase::join(); }

    bool Init(bool _bLB);
    void RegisterPdbManager(std::shared_ptr<PDB::ConnectionManager> _pcm, std::memory_order=std::memory_order_seq_cst) {
        pcm_ = atomic_load(&_pcm);
    }
    void SetHangupThreadMonitor(CThreadMonitor * _pThreadMonitor) {
        pThreadMonitor_ = _pThreadMonitor;
    }    

private:
    void run(std::shared_ptr<bool> _pRefCounter);
    void runLB();
    void runMP();

    bool isMaster(PDB::Worker * pdbW, Update_SliceDnnWorkSQL & _updateSQL, Select_SliceDnnWorkSQL & _selectSQL);
    void updateMaster(PDB::Worker * pdbW, Update_SliceDnnWorkSQL & _sql);

private:
    std::shared_ptr<PDB::ConnectionManager>     pcm_;    

    std::string      className_;
    CThreadMonitor * pThreadMonitor_ = nullptr;    

    bool            bLB_ = false;

    // For MP ////////////////////////////
    std::string         restrictFileName_;
    NetconfSliceDnn     netconf_;

    // For LB ////////////////////////////
    int                 slaveSleepPeriodSec_   = 60;
    int                 updateCntTBLPeriodSec_ = DEFAULT_UPDATE_CNT_TBL_PERIOD_SEC; 

};

#endif
