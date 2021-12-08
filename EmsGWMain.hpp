#ifndef EMS_GW_MAIN_HPP
#define EMS_GW_MAIN_HPP

#include "CNDFApplication.hpp"
#include "CNetconfPDBActive.hpp"

#include "PDBConnectionManager.hpp"
#include "PDBWorker.hpp"

#include "CThreadMonitor.hpp"

#include "CLI_DbQuery.hpp"

class EmsGWMain : public CNDFApplication {

public:
    explicit EmsGWMain(int _argc, char * _argv[], int _initMode);
    ~EmsGWMain();
    EmsGWMain(const EmsGWMain & _rhs) = delete;

private:
    int     AppInitial(int _initResult);
    int     AppRun();
    int     AppFinal(int _runResult);

private:
    bool    doCommand(CNetconfPDBActive & _r);

private:
    bool    bRunf_;

    std::shared_ptr<PDB::ConnectionManager> pcm_;

    int                         nApp_HANGUP_SEC_ = -1;
    CThreadMonitor *            pThreadMonitor_ = nullptr;

public:
    string RtrvUptime(int argc, const char *const *argv, atom::EN_CLI_FORMAT format);
    string RtrvSubsInf(int argc, const char *const *argv, atom::EN_CLI_FORMAT format);
    string RtrvSubsCnt(int argc, const char *const *argv, atom::EN_CLI_FORMAT format);
    string RtrvSessInf(int argc, const char *const *argv, atom::EN_CLI_FORMAT format);
    string RtrvSessCnt(int argc, const char *const *argv, atom::EN_CLI_FORMAT format);
    string RtrvZoneList(int argc, const char *const *argv, atom::EN_CLI_FORMAT format);

private:
    bool InitCLI(void);
    PDB::Worker CliPdbWorker;

    CSessionInfo            m_cSessionInfo;
    CSessionCount           m_cSessionCount;
    CSubscriberCount        m_cSubscriberCount;
    CSubscriberProfile      m_cSubscriberProfile;
    CSubscriberService      m_cSubscriberService;
    CZoneInfo               m_cZoneInfo;
};


#endif // EMS_GW_MAIN_HPP
