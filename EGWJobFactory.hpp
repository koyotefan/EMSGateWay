#ifndef EGW_JOB_FACTORY_HPP
#define EGW_JOB_FACTORY_HPP

#include "PDBConnectionManager.hpp"
#include "PDBWorker.hpp"

#include "OkJob.hpp"
#include "NokJob.hpp"
#include "PdbSwitchJob.hpp"
#include "SubscriberJob.hpp"
#include "PduSessionNotiJob.hpp"
#include "PduSessionDetachJob.hpp"
#include "ExceptionRuleCfgJob.hpp"
#include "HolidayCfgJob.hpp"
#include "ReloadSessionRuleJob.hpp"
#include "ReloadURSPDataJob.hpp"
#include "ReloadGxMGRJob.hpp"

class EGWJobFactory {
public:
    explicit EGWJobFactory();
    ~EGWJobFactory();

    bool Init();
    void RegisterPdbManager(std::shared_ptr<PDB::ConnectionManager> _pcm, std::memory_order=std::memory_order_seq_cst) {
        pcm_ = std::atomic_load(&_pcm);
    }
    EGWJob & GetJob(std::string _path);

private:
    std::string getWords(const std::string & _org, off_t _offset, const char _delimiter);
    bool isequals(std::string _src, std::string _dest);

private:

    std::shared_ptr<PDB::ConnectionManager>     pcm_;
    PDB::Worker             pdbPCFWorker_;

    OkJob                   ok_;
    NokJob                  nok_;
    PdbSwitchJob            pdbSwitching_;
    SubscriberJob           subscriber_;
    PduSessionNotiJob       pduSessionNoti_;
    PduSessionDetachJob     pduSessionDetach_;
    ExceptionRuleCfgJob     exceptionRuleCfg_;
    HolidayCfgJob           holidayCfg_;
    ReloadSessionRuleJob    reloadSessionRule_;
    ReloadURSPDataJob       reloadURSPData_;
    GxMGRSubsExceptionJob   gxMgrSubsException_;
    GxMGRRARSkipJob         gxMgrRARSkip_;
    GxMGRDisasterRoamingJob gxMgrDisasterRoaming_;
    GxMGRFemtocellJob       gxMgrFemtocell_;

};

#endif // EGW_JOB_FACTORY_HPP
