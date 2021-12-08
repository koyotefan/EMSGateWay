#include <linux/unistd.h>       /* for _syscallX macros/related stuff */
#include <linux/kernel.h>       /* for struct sysinfo */
#include <sys/sysinfo.h>

#include "CNDFConv.hpp"

#include "BaseDefine.hpp"
#include "EGWDefine.hpp"
#include "EmsGWMain.hpp"
#include "EmsGWServer.hpp"
#include "PdbReporter.hpp"
#include "SliceDnnWorker.hpp"
#include "ExternalConnectionReporter.hpp"

#include "Verbose.hpp"
#define SIZE_STR_TEMP   1024

thread_local     char egwLogName[32] = {"MAIN"};

std::string             gSysClusterName;
std::string             gSysName;
std::string             gProcName;
CNDFIniConfigContainer * gCfg = nullptr;
//CEGW_Cli				gCli ;

auto strVERSION = std::string(PACKAGE_VERSION);

std::string extractBy(std::string & _org, std::string && _del, int _count=1) {
    
    std::size_t pos = 0;
    for(int n=0; n < _count; ++n, ++pos) {

        if((pos = _org.find(_del, pos)) == std::string::npos)
            return _org;
    }
 
    return _org.substr(0, pos-1);  
}

std::string GetSystemClusterName(std::string & _sysName) {

    // ex) PCF.01.LB.01 -> PCF.01
    // return extractBy(_sysName, ".", 2);

    // ex) pcf01-lb01-h01-f1 -> pcf01
    return extractBy(_sysName, "-", 1);

}


std::string MpRtrvUptime(int argc, const char* const* argv, void *user_data, atom::EN_CLI_FORMAT format)
{
    EmsGWMain *pclEmsGW = (EmsGWMain*)user_data;
    
    return  pclEmsGW->RtrvUptime(argc,argv,format);
}

std::string MpRtrvSubsInf(int argc, const char* const* argv, void *user_data, atom::EN_CLI_FORMAT format)
{
    EmsGWMain *pclEmsGW = (EmsGWMain*)user_data;
    
    return  pclEmsGW->RtrvSubsInf(argc,argv,format);
}

std::string MpRtrvSubsCnt(int argc, const char* const* argv, void *user_data, atom::EN_CLI_FORMAT format)
{
    EmsGWMain *pclEmsGW = (EmsGWMain*)user_data;
    
    return  pclEmsGW->RtrvSubsCnt(argc,argv,format);
}

std::string MpRtrvSessInf(int argc, const char* const* argv, void *user_data, atom::EN_CLI_FORMAT format)
{
    EmsGWMain *pclEmsGW = (EmsGWMain*)user_data;
    
    return  pclEmsGW->RtrvSessInf(argc,argv,format);
}

std::string MpRtrvSessCnt(int argc, const char* const* argv, void *user_data, atom::EN_CLI_FORMAT format)
{
    EmsGWMain *pclEmsGW = (EmsGWMain*)user_data;
    
    return  pclEmsGW->RtrvSessCnt(argc,argv,format);
}

std::string MpRtrvZoneList(int argc, const char* const* argv, void *user_data, atom::EN_CLI_FORMAT format)
{
    EmsGWMain *pclEmsGW = (EmsGWMain*)user_data;
    
    return  pclEmsGW->RtrvZoneList(argc,argv,format);
}



EmsGWMain::EmsGWMain(int _argc, char * _argv[], int _initMode)
    : CNDFApplication(_argc, _argv, _initMode),
      bRunf_(true) {


}

EmsGWMain::~EmsGWMain() {

}

int EmsGWMain::AppInitial(int _initResult) {

    switch(_initResult)
    {
        case NDF_APP_RUN_OK :
            break ;
        case NDF_APP_RUN_DUPLICATED :
        case NDF_APP_RUN_DAEMONIZE_FAILED :
        case NDF_APP_RUN_VNF_INIT_FAILED :
        case NDF_APP_RUN_GETENV_FAILED :
        case NDF_APP_RUN_CONFIG_FAILED :
        case NDF_APP_RUN_LOGGER_FAILED :
        case NDF_APP_RUN_LOGGER_PATH_NOT_FOUND :
        case NDF_APP_RUN_LOGGER_APPL_INDEX_INVALID :
        case NDF_APP_RUN_ATOM_FAILED : // environment variable($NDF_ATOM_IGNORE) undefined or defined with 'no'
        default :
            std::cout << "AppInitial Fail :" << _initResult << std::endl;
            return 0;
    }

    gSysName  = GetPkgNodeID();
    gProcName = GetProcName();
   
    gSysClusterName = GetSystemClusterName(gSysName); 

    // Config
    gCfg = GetConfigContainer();

    if(gCfg == nullptr) {
        std::cout << "GetConfigContainer Fail :" << std::endl;
        return 0;
    }
    
    // Log
    if(G_ATOM_ADAPTOR.SendVersion(strVERSION.c_str()) ==  false)
    {
        W_LOG("SendVersion Fail %s", strVERSION.c_str());
    }

    // //Cli
	// if ( gCli.Initialize(pcm_) == false )
	// {
	// 	E_LOG("gCliInitialize Failure");
	// 	return 0;
	// }
    
        InitCLI();
    return 1;
}

int EmsGWMain::AppFinal(int _runResult) {
    W_LOG("AppFinal() [arg:%d]", _runResult);

    return (_runResult == false)?NDF_APP_STOPPED_BY_ERROR:NDF_APP_STOPPED_BY_USER;
}

int EmsGWMain::AppRun() {

    char    buf[256];

    // pcm Init ///////////////////////////////////////
    if ( (gCfg->Get((char *)"PCF_PDB",(char *)"CONFIG",buf) == false ) &&
         (gCfg->Get((char *)"PDB_INFO", (char *)"CONFIG", buf) == false))
    {
        E_LOG("configuration (PCF_PDB:CONFIG), (PDB_INFO:CONFIG) not found!") ;
        return 0;
    }

    char    fname[256];
    CNDFConv::EnvReplace(fname,buf) ;

    pcm_ = std::make_shared<PDB::ConnectionManager>();

    if(pcm_->Init(gSysName, gProcName, fname) == false) {
        E_LOG("PDBConnectionManager Init() fail");
        return 0;
    }

    pThreadMonitor_ = ThdMonitorSTPtr;

    // PDB Hangup Time Setting ///////////////////
    if ( gCfg->Get((char *)"COMMON",(char *)"APP_HANGUP_SEC",buf) == false )
    {   
        W_LOG("configuration (COMMON:APP_HANGUP_SEC) not found!") ;
    } else {
        nApp_HANGUP_SEC_ = -1;

        try {  nApp_HANGUP_SEC_ = std::stoi(std::string(buf)); } catch (...) {}
        if(nApp_HANGUP_SEC_ > 0) {
            pcm_->SetHangupValidTime(nApp_HANGUP_SEC_);
            I_LOG("Configuration (COMMON:APP_HANGUP_SEC) [%d]", nApp_HANGUP_SEC_) ;
        }
    }

    CNetconfPDBActive   netconfPDBActive;

    if(netconfPDBActive.Init() == false) {
        E_LOG("Netconf for PDB Init() fail");
        return 0;
    }

    if(netconfPDBActive.LoadConfig() == false) {
        E_LOG("Netconf for PDB LoadConfig() fail");
        return 0;
    }

    I_LOG("LoadData [%s]", netconfPDBActive.GetData());
    pcm_->SwitchActiveDB(netconfPDBActive.GetData());

    ///////////////////////////////////////////////////

    std::unique_ptr<EmsGWServer>    server = std::make_unique<EmsGWServer>();
    server->SetHangupThreadMonitor(pThreadMonitor_);
    server->RegisterPdbManager(pcm_);
    if(server->Init(9030, false) == false) {
        E_LOG("EmsGWServer Init() fail");
        return 0;
    }

    std::unique_ptr<PdbReporter>    pdbReporter = std::make_unique<PdbReporter>();
    pdbReporter->SetHangupThreadMonitor(pThreadMonitor_);
    pdbReporter->RegisterPdbManager(pcm_);
    if(pdbReporter->Init(9030, false) == false) {
        E_LOG("PdbReporter Init() fail");
        return 0;
    }

    auto sliceDnnWorker = std::make_unique<SliceDnnWorker>(); 
    sliceDnnWorker->SetHangupThreadMonitor(pThreadMonitor_);
    sliceDnnWorker->RegisterPdbManager(pcm_);
    if(sliceDnnWorker->Init(EGWCommon::IsLB(gSysName)) == false) {
        E_LOG("SliceDnnWorker Init() fail");
        return 0;
    }

    auto externalReporter = std::make_unique<ExternalConnectionReporter>();

    if(EGWCommon::IsLB(gSysName)) {
        externalReporter->SetHangupThreadMonitor(pThreadMonitor_);
        if(externalReporter->Init() == false) {
            E_LOG("externalReporter Init() fail");
            return 0;
        }
    }

    server->Run();
    pdbReporter->Run();
    sliceDnnWorker->Run();

    if(EGWCommon::IsLB(gSysName))
    {
        externalReporter->Run();
    }

    CliPdbWorker.Assign(pcm_);
    CliPdbWorker.SetLogName(egwLogName);

    if (CliPdbWorker.TurnOn(PDB::eDefDBType::PCF) == false)
    {
        E_LOG("[PDB Worker] PCF TurnOn() fail");
        return 0;
    }
    if (CliPdbWorker.TurnOn(PDB::eDefDBType::Subscriber) == false)
    {
        E_LOG("[PDB Worker] Subscriber TurnOn() fail");
        return 0;
    }
    if (CliPdbWorker.TurnOn(PDB::eDefDBType::SessionMaster) == false)
    {
        E_LOG("[PDB Worker] SessionMaster TurnOn() fail");
        return 0;
    }

    if (CliPdbWorker.TurnOn(PDB::eDefDBType::SessionDetail) == false)
    {
        E_LOG("[PDB Worker] SessionDetail TurnOn() fail");
        return 0;
    }

    ATOM_SET_MANUAL_HANGUP_CONTROL();
    ATOM_NOTIFY_READY();
    while(bRunf_) {
        if(doCommand(netconfPDBActive) == false)
            break;

        if(server->IsRun() == false) {
            E_LOG("EmsGWServer Down");
            break;
        }

        if(pdbReporter->IsRun() == false) {
            E_LOG("PdbReporter Down");
            break;
        }

        if(sliceDnnWorker->IsRun() == false) {
            E_LOG("sliceDnnWorker Down");
            break;
        }

        if(EGWCommon::IsLB(gSysName) && externalReporter->IsRun() == false) {
            E_LOG("externalReporter Down");
            break;
        }

        if(netconfPDBActive.IsChanged()) {
            if(netconfPDBActive.LoadConfig()) {
                pcm_->SwitchActiveDB(netconfPDBActive.GetData());
            }
        }

        // doCommand() 에서 쉽니다. 걱정마세요.
        // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    W_LOG("EmsGW.Run.end");

    server->Stop();
    pdbReporter->Stop();

    return (bRunf_ == false)?1:0;

}

bool EmsGWMain::doCommand(CNetconfPDBActive & _netconfPDBActive) {
    int              cmd ;
    char             args[5120] ;
    bool             bShouldResponse = false ;
    int              ret ;

    if(IsExit())
    {
        I_LOG("EmsGWMain.doCommand.IsExit");
        bRunf_ = false;
        return true;
    }

    ret = GetCommand(&cmd,args) ;
    if ( ret == false )
    {
        W_LOG("EmsGWMain.doCommand.GetCommand fail");
        return false ;
    }

    if ( cmd == NDF_CMD_EMPTY ) {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        return true ;
    }

    D_LOG("EmsGWMain.doCommand.GetCommand [%d]", cmd) ;

    switch(cmd)
    {
        case NDF_CMD_APP_INIT  :
            bShouldResponse = true ;
            break ;
        case NDF_CMD_APP_STOP :
            I_LOG("EmsGWMain.doCommand.APP_STOP");
            bRunf_ = false;
            break ;
        case NDF_CMD_APP_SUSPEND :
        case NDF_CMD_APP_RESUME :
            // what to do?
            break;
        case NDF_CMD_APP_LOGLEVEL :
           break;
        case NDF_CMD_APP_STATISTICS :
            // gHist.BatchTransfer();
            break;
        case    NDF_CMD_UNKNOWN :
            // ATOM Returns Unknown command
            break;
        case    NDF_CMD_EMPTY :
            // no command
            break;
        case 110001 : // DM Signal Command
            bShouldResponse = true ;
            /* TO-DO cli command */
            break;
        case NDF_CMD_APP_PING:
            if(pThreadMonitor_->IsHangup()) {
                break;
            }

            if(pcm_->IsHangup()) {
                E_LOG ("[DETECTED THREAD HANG-UP] PDB Thread");
                break;
            }

            ATOM_SEND_HANGUP_RESPONSE();
            break;        
        case 12 :
        case 13 :
        case 14 :
            break;
        case 18 :
            if(_netconfPDBActive.Init() == false) {
                E_LOG("EmsGWMain.doCommand.GetCommand [%d] Init() fail", cmd);
            }

            ATOM_SET_MANUAL_HANGUP_CONTROL();
            ATOM_NOTIFY_READY();
            break; 
        default :
            E_LOG("EmsGWMain.doCommand.GetCommand.unknown [%d]", cmd);
            break ;
    }

    if ( bShouldResponse == true )
    {
        //PutResponse(true,0) ;
    }

    return true ;

}

bool EmsGWMain::InitCLI(void)
{
    bool bRtn = true;
    CNetconfAPI::ST_STATIC_CLI_INFO cliinfo[] =
        {
            {
                // CLI> RTRV-UPTIME
                MpRtrvUptime,                         // callback
                "C9001",                              // message number
                "CLI Callback Display system uptime", // description
                "RTRV-UPTIME",                        // command
                true,                                 // enable
                false,                                // password-yn
                5,
                (void *)this,
                .params = {}
             },
            {
                // CLI> RTRV-SUBS-INF
                MpRtrvSubsInf,                          // callback
                "C9002",                                // message number
                "CLI Callback Display Subscriber Info", // description
                "RTRV-SUBS-INF",                        // command
                true,                                   // enable
                false,                                  // password-yn
                5,
                (void *)this,
                .params =
                {
                    {
                        "MDN",                       // parameter name
                        "target MDN",                // parameter description
                        CCliParamType::string,       // value data type
                        1,                           // array count
                        {}
                    },                    
                }
            },
            { 
                // CLI> RTRV-SUBS-CNT
                MpRtrvSubsCnt,                           // callback
                "C9003",                                 // message number
                "CLI Callback Display Subscriber Count", // description
                "RTRV-SUBS-CNT",                         // command
                true,                                    // enable
                false,                                   // password-yn
                5,
                (void *)this,
                .params =
                {
                    {
                        "SERVICE ID",                // parameter name
                        "target service id",         // parameter description
                        CCliParamType::string,       // value data type
                        1,                           // array count
                        {}
                    },                    
                }
            },
            {
                // CLI> RTRV-SESS-INF
                MpRtrvSessInf,                       // callback
                "C9004",                             // message number
                "CLI Callback Display Session Info", // description
                "RTRV-SESS-INF",                     // command
                true,                                // enable
                false,                               // password-yn
                5,
                (void *)this,
                .params =
                {
                    {
                        "MDN",                       // parameter name
                        "target MDN",                // parameter description
                        CCliParamType::string,       // value data type
                        1,                           // array count
                        {}
                    },                    
                }
            },
            // {
            //     // CLI> RTRV-SESS-CNT
            //     MpRtrvSessCnt,                        // callback
            //     "C9005",                              // message number
            //     "CLI Callback Display Session Count", // description
            //     "RTRV-SESS-CNT",                      // command
            //     true,                                 // enable
            //     false,                                // password-yn
            //     5,
            //     (void *)this,
            //     .params =
            //     {
            //         {
            //             "COLUMN",                    // parameter name
            //             "target Column",             // parameter description
            //             CCliParamType::string,       // value data type
            //             1,                           // array count
            //             {}
            //         },                    
            //     }
            // },
            {
                // CLI> RTRV-ZONE-LIST
                MpRtrvZoneList,                   // callback
                "C9006",                          // message number
                "CLI Callback Display Zone List", // description
                "RTRV-ZONE-LIST",                 // command
                true,                             // enable
                false,                            // password-yn
                5,
                (void *)this,
                .params = {}
            }
        };

    try
    {
        CNetconfAPI *netConf_;
        int nMaxCnt = sizeof(cliinfo) / sizeof(CNetconfAPI::ST_STATIC_CLI_INFO);
        I_LOG("cli count[%d]", nMaxCnt);
        netConf_ = ATOM_GET_NETCONF_API();
        for (int i = 0; i < nMaxCnt; i++)
        {
            I_LOG("addCliForce[%d][%s]", i, cliinfo[i].name);
            netConf_->AddCliForce(&cliinfo[i]);
        }
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            W_LOG("[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        W_LOG("[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }

    return bRtn;
}

string EmsGWMain::RtrvUptime(int argc, const char *const *argv, atom::EN_CLI_FORMAT format)
{
    char strTemp[SIZE_STR_TEMP];
    string result;
    result.clear();

    snprintf(strTemp, SIZE_STR_TEMP, "System Uptime information\n : ");
    result += strTemp;

    struct sysinfo s_info;
    int error = sysinfo(&s_info);
    if (error == 0)
    {
        snprintf(strTemp, SIZE_STR_TEMP, "%u days, %u hours, %u minutes, %u seconds.", (unsigned int)s_info.uptime / 86400, (unsigned int)(s_info.uptime % 86400) / 3600, (unsigned int)(s_info.uptime % 3600) / 60, (unsigned int)s_info.uptime % 60);
        result += strTemp;
    }
    else
    {
        snprintf(strTemp, SIZE_STR_TEMP, "Fail to get sysinfo");
        result += strTemp;
    }

    return result;
}

string EmsGWMain::RtrvSubsInf(int argc, const char *const *argv, atom::EN_CLI_FORMAT format)
{
    char strTemp[SIZE_STR_TEMP];
    string result;
    result.clear();

    string MDN = argv[0];

    ST_CLI_SUBS_PROFILE      a_pstSubsProfile;

    bool bRtn = true;
    bRtn = m_cSubscriberProfile.SetValue((char *)MDN.c_str());
    if (bRtn == true)
    {
        if (CliPdbWorker.Execute(m_cSubscriberProfile) == true
            && SQL_NO_DATA != m_cSubscriberProfile.GetSQLReturn())
        {
            memcpy(&a_pstSubsProfile, m_cSubscriberProfile.GetValue(), sizeof(ST_CLI_SUBS_PROFILE));
            snprintf(strTemp, SIZE_STR_TEMP, "+------------------------------------------+\n");
            result += strTemp;
            snprintf(strTemp, SIZE_STR_TEMP, "| MDN              | %-15s       |\n",a_pstSubsProfile.szMDN);
            result += strTemp;
            snprintf(strTemp, SIZE_STR_TEMP, "+------------------+-----------------------+\n");
            result += strTemp;
            snprintf(strTemp, SIZE_STR_TEMP, "| MSISDN           | %-15s       |\n",a_pstSubsProfile.szMSISDN);
            result += strTemp;
            snprintf(strTemp, SIZE_STR_TEMP, "| PRODUCT ID       | %-15s       |\n",a_pstSubsProfile.szProductID);
            result += strTemp;
            snprintf(strTemp, SIZE_STR_TEMP, "| CREATE TIME      | %-15s       |\n",a_pstSubsProfile.szCreateTime);
            result += strTemp;
            snprintf(strTemp, SIZE_STR_TEMP, "| UPDATE TIME      | %-15s       |\n",a_pstSubsProfile.szUpdateTime);
            result += strTemp;
            snprintf(strTemp, SIZE_STR_TEMP, "| OCS              | %-15s       |\n",a_pstSubsProfile.szOCS);
            result += strTemp;
            snprintf(strTemp, SIZE_STR_TEMP, "| MVNO             | %-15s       |\n",a_pstSubsProfile.szMVNO);
            result += strTemp;
            snprintf(strTemp, SIZE_STR_TEMP, "| Category LTE     | %-15s       |\n",a_pstSubsProfile.szUECategoryLTE);
            result += strTemp;
            snprintf(strTemp, SIZE_STR_TEMP, "| Category 5G      | %-15s       |\n",a_pstSubsProfile.szUECategory5G);
            result += strTemp;
            snprintf(strTemp, SIZE_STR_TEMP, "| Device Type      | %-15s       |\n",a_pstSubsProfile.szUEDeviceType);
            result += strTemp;
            snprintf(strTemp, SIZE_STR_TEMP, "+------------------+-----------------------+\n");
            result += strTemp;

            ST_CLI_SUBS_SERVICE stSubsService;
            vector<ST_CLI_SUBS_SERVICE> *a_pstvecSubsService;

            bRtn = m_cSubscriberService.SetValue((char *)MDN.c_str());
            if (bRtn == true)
            {
                if (CliPdbWorker.Execute(m_cSubscriberService) == true)
                {
                    snprintf(strTemp, SIZE_STR_TEMP, "\n");
                    result += strTemp;

                    a_pstvecSubsService = m_cSubscriberService.GetValue();
                    if((int)a_pstvecSubsService->size()>0)
                    {
                        snprintf(strTemp, SIZE_STR_TEMP, "+----------------------------------------------------------------------------------------------------+\n");
                        result += strTemp;
                        snprintf(strTemp, SIZE_STR_TEMP, "| SVC ID              | Limit Value      | Active Time    | JOB CODE | CNUM         | Create Time    |\n");
                        result += strTemp;
                        snprintf(strTemp, SIZE_STR_TEMP, "|                     | Limit Valid Time | Expire Time    | Time Period ID          | Update Time    |\n");
                        result += strTemp;
                        snprintf(strTemp, SIZE_STR_TEMP, "+----------------------------------------------------------------------------------------------------+\n");
                        result += strTemp;

                        for(int i = 0 ; i < (int)a_pstvecSubsService->size();i++)
                        {
                            stSubsService = (ST_CLI_SUBS_SERVICE)a_pstvecSubsService->at(i);
                            snprintf(strTemp, SIZE_STR_TEMP, "| %-20s| %-17s| %-15s| %-9s| %-13s| %-15s|\n"
                                                           ,stSubsService.szSVCID
                                                           ,stSubsService.szLimit
                                                           ,stSubsService.szActiveTime
                                                           ,stSubsService.szJobCode
                                                           ,stSubsService.szCNUM
                                                           ,stSubsService.szCreateTime);
                            result += strTemp;
                            snprintf(strTemp, SIZE_STR_TEMP, "|                     | %-17s| %-15s| %-24s| %-15s|\n"
                                                           ,stSubsService.szLimitValidTime
                                                           ,stSubsService.szExpireTime
                                                           ,stSubsService.szTimePeroidId
                                                           ,stSubsService.szUpdateTime);
                            result += strTemp;
                            snprintf(strTemp, SIZE_STR_TEMP, "+----------------------------------------------------------------------------------------------------+\n");
                            result += strTemp;

                        }
                    }
                    else
                    {
                        snprintf(strTemp, SIZE_STR_TEMP, "+-----------------------------------------------+\n");
                        result += strTemp;
                        snprintf(strTemp, SIZE_STR_TEMP, "| No joined service.                            |\n");
                        result += strTemp;
                        snprintf(strTemp, SIZE_STR_TEMP, "+-----------------------------------------------+\n");
                        result += strTemp;
                    }
                }
                else
                {
                    snprintf(strTemp, SIZE_STR_TEMP, "\n FAIL TO SELECT 5G SUBSCRIBER SERVICE.\n");
                    result += strTemp;

                    bRtn = false;
                }
            }
        }
        else
        {
            snprintf(strTemp, SIZE_STR_TEMP, "\n FAIL TO SELECT 5G SUBSCRIBER PROFILE.\n");
            result += strTemp;

            bRtn = false;
        }
    }
    return result;
}
string EmsGWMain::RtrvSubsCnt(int argc, const char *const *argv, atom::EN_CLI_FORMAT format)
{
    char strTemp[SIZE_STR_TEMP];
    string result;
    result.clear();

    string SubsColumn = argv[0] ;
    
    ST_CLI_SUBS_CNT stSubsCnt;
    vector<ST_CLI_SUBS_CNT> *a_pstvecSubsCnt;

    bool bRtn = true;
    bRtn = m_cSubscriberCount.SetValue((char *)SubsColumn.c_str());
    if (bRtn == true)
    {
        if (CliPdbWorker.Execute(m_cSubscriberCount) == true)
        {
            snprintf(strTemp, SIZE_STR_TEMP, "+-----------------------------------------------+\n");
            result += strTemp;
            snprintf(strTemp, SIZE_STR_TEMP, "| Service ID : %-32s |\n",SubsColumn.c_str());
            result += strTemp;

            a_pstvecSubsCnt = m_cSubscriberCount.GetValue();
            if((int)a_pstvecSubsCnt->size()>0)
            {
                snprintf(strTemp, SIZE_STR_TEMP, "+------------------+----------------+-----------+\n");
                result += strTemp;
                snprintf(strTemp, SIZE_STR_TEMP, "|   Value          | Is valid       |     Count |\n");
                result += strTemp;
                snprintf(strTemp, SIZE_STR_TEMP, "+------------------+----------------+-----------+\n");
                result += strTemp;

                for(int i = 0 ; i < (int)a_pstvecSubsCnt->size();i++)
                {
                    stSubsCnt = (ST_CLI_SUBS_CNT)a_pstvecSubsCnt->at(i);
                    snprintf(strTemp, SIZE_STR_TEMP, "|   %-15s| %-15s|%10d |\n", stSubsCnt.VALUE, stSubsCnt.ISVALID, stSubsCnt.COUNT);
                    result += strTemp;
                }

                snprintf(strTemp, SIZE_STR_TEMP, "+------------------+----------------+-----------+\n");
                result += strTemp;
            }
            else
            {
                snprintf(strTemp, SIZE_STR_TEMP, "+-----------------------------------------------+\n");
                result += strTemp;
                snprintf(strTemp, SIZE_STR_TEMP, "| No Data.                                      |\n");
                result += strTemp;
                snprintf(strTemp, SIZE_STR_TEMP, "+-----------------------------------------------+\n");
                result += strTemp;

            }
        }
        else
        {
            snprintf(strTemp, SIZE_STR_TEMP, " FAIL TO SELECT SUBSCRIBER INFO.\n");
            result += strTemp;

            bRtn = false;
        }
    }



    return result;
}

string EmsGWMain::RtrvSessInf(int argc, const char *const *argv, atom::EN_CLI_FORMAT format)
{
    char strTemp[SIZE_STR_TEMP];
    string result;
    result.clear();

    string MDN = argv[0];

    ST_SMF_SESSION_INFO stSmfSessionInfo;
    vector<ST_SMF_SESSION_INFO> *a_pstvecSmfSessionInfo;

    bool bRtn = true;
    bRtn = m_cSessionInfo.SetValue((char *)MDN.c_str());
    if (bRtn == true)
    {
        if (CliPdbWorker.Execute(m_cSessionInfo,MDN.c_str()) == true)
        {
            a_pstvecSmfSessionInfo = m_cSessionInfo.GetValue();
            if((int)a_pstvecSmfSessionInfo->size()>0)
            {
                struct tm pTime;
                char strTime[64];
                
                snprintf(strTemp, SIZE_STR_TEMP, "+----------------+-----------------------+\n");
                result += strTemp;
                snprintf(strTemp, SIZE_STR_TEMP, "| MDN            | %-15s       |\n",MDN.c_str());
                result += strTemp;
                snprintf(strTemp, SIZE_STR_TEMP, "+----------------+-----------------------+\n\n");
                result += strTemp;

                for(int i = 0 ; i < (int)a_pstvecSmfSessionInfo->size();i++)
                {
                    stSmfSessionInfo = (ST_SMF_SESSION_INFO)a_pstvecSmfSessionInfo->at(i);

                    snprintf(strTemp, SIZE_STR_TEMP, "+----------------+-----------------------------------------------------------------+\n");
                    result += strTemp;
                    snprintf(strTemp, SIZE_STR_TEMP, "| POLICY ID      | %-64s|\n",stSmfSessionInfo.szSMPolicyID);
                    result += strTemp;
                    snprintf(strTemp, SIZE_STR_TEMP, "| SUPI           | %-64s|\n",stSmfSessionInfo.szSUPI);
                    result += strTemp;
                    snprintf(strTemp, SIZE_STR_TEMP, "| PDU_SESSION_ID | %-64d|\n",stSmfSessionInfo.nPDUSessionID);
                    result += strTemp;
                    snprintf(strTemp, SIZE_STR_TEMP, "| GPSI           | %-64s|\n",stSmfSessionInfo.szGPSI);
                    result += strTemp;
                    snprintf(strTemp, SIZE_STR_TEMP, "| IP_ADDR        | %-64s|\n",stSmfSessionInfo.szIpAddr);
                    result += strTemp;
                    snprintf(strTemp, SIZE_STR_TEMP, "| DNN            | %-64s|\n",stSmfSessionInfo.szDNN);
                    result += strTemp;
                    snprintf(strTemp, SIZE_STR_TEMP, "| S_NSSAI_SST    | %-64d|\n",stSmfSessionInfo.nSNssaiSst);
                    result += strTemp;
                    snprintf(strTemp, SIZE_STR_TEMP, "| S_NSSAI_SD     | %-64s|\n",stSmfSessionInfo.szSNssaiSd);
                    result += strTemp;
                    snprintf(strTemp, SIZE_STR_TEMP, "| LOC_ID         | %-64s|\n",stSmfSessionInfo.szLocId);
                    result += strTemp;
                    snprintf(strTemp, SIZE_STR_TEMP, "| MCC_MNC        | %-64s|\n",stSmfSessionInfo.szMCCMNC);
                    result += strTemp;
                    snprintf(strTemp, SIZE_STR_TEMP, "| RAT_TYPE       | %-64s|\n",stSmfSessionInfo.szRatType);
                    result += strTemp;
                    snprintf(strTemp, SIZE_STR_TEMP, "| NODE_ID        | %-64s|\n",stSmfSessionInfo.szNodeID);
                    result += strTemp;
                    snprintf(strTemp, SIZE_STR_TEMP, "| PROC_ID        | %-64s|\n",stSmfSessionInfo.szProcID);
                    result += strTemp;
                    snprintf(strTemp, SIZE_STR_TEMP, "| SMF_ID         | %-64s|\n",stSmfSessionInfo.szSMFId);
                    result += strTemp;
                    pTime = *localtime(&stSmfSessionInfo.tCreateTime);
                    snprintf(strTime,64,"%04d/%02d/%02d %02d:%02d:%02d",pTime.tm_year+1900,pTime.tm_mon,pTime.tm_mday,pTime.tm_hour,pTime.tm_min,pTime.tm_sec);
                    snprintf(strTemp, SIZE_STR_TEMP, "| CREATE_TIME    | %-64s|\n",strTime);
                    result += strTemp;
                    pTime = *localtime(&stSmfSessionInfo.tUpdateTime);
                    snprintf(strTime,64,"%04d/%02d/%02d %02d:%02d:%02d",pTime.tm_year+1900,pTime.tm_mon,pTime.tm_mday,pTime.tm_hour,pTime.tm_min,pTime.tm_sec);
                    snprintf(strTemp, SIZE_STR_TEMP, "| UPDATE_TIME    | %-64s|\n",strTime);
                    result += strTemp;
                    snprintf(strTemp, SIZE_STR_TEMP, "+----------------+-----------------------------------------------------------------+\n");
                    result += strTemp;

                    snprintf(strTemp, SIZE_STR_TEMP, "|OUT_HTTP_PAYLOAD| ");
                    result += strTemp;
                    snprintf(strTemp, SIZE_STR_TEMP, "%s\n\n\n",stSmfSessionInfo.szOutHttpPayload);
                    result += strTemp;
                }
            }
            else
            {
                snprintf(strTemp, SIZE_STR_TEMP, "+------------------------------------------------+\n");
                result += strTemp;
                snprintf(strTemp, SIZE_STR_TEMP, "| No Data.                                       |\n");
                result += strTemp;
                snprintf(strTemp, SIZE_STR_TEMP, "+------------------------------------------------+\n");
                result += strTemp;
            }
        }
        else
        {
            snprintf(strTemp, SIZE_STR_TEMP, " FAIL TO SELECT SESSION INFO.\n");
            result += strTemp;

            bRtn = false;
        }
    }


    return result;
}

string EmsGWMain::RtrvSessCnt(int argc, const char *const *argv, atom::EN_CLI_FORMAT format)
{
    char strTemp[SIZE_STR_TEMP];
    string result;
    result.clear();

    string SubsColumn = argv[0] ;
   
    ST_CLI_SESS_CNT stSessCount;
    vector<ST_CLI_SESS_CNT> *a_pstvecSessCount;

    bool bRtn = true;
    bRtn = m_cSessionCount.SetValue((char *)SubsColumn.c_str());
    if (bRtn == true)
    {
        if (CliPdbWorker.Execute(m_cSessionCount) == true)
        {
            snprintf(strTemp, SIZE_STR_TEMP, "+-------------------------------------------------+\n");
            result += strTemp;
            snprintf(strTemp, SIZE_STR_TEMP, "| Column : %-39s|\n",SubsColumn.c_str());
            result += strTemp;


            a_pstvecSessCount = m_cSessionCount.GetValue();
            if((int)a_pstvecSessCount->size()>0)
            {
                snprintf(strTemp, SIZE_STR_TEMP, "+-------------------------------+-----------------+\n");
                result += strTemp;
                snprintf(strTemp, SIZE_STR_TEMP, "| VALUE                         | COUNT           |\n");
                result += strTemp;
                snprintf(strTemp, SIZE_STR_TEMP, "+-------------------------------+-----------------+\n");
                result += strTemp;

                for(int i = 0 ; i < (int)a_pstvecSessCount->size();i++)
                {
                    stSessCount = (ST_CLI_SESS_CNT)a_pstvecSessCount->at(i);
                    snprintf(strTemp, SIZE_STR_TEMP, "| %-30s| %-16d|\n", stSessCount.VALUE, stSessCount.COUNT);
                    result += strTemp;
                }

                snprintf(strTemp, SIZE_STR_TEMP, "+-------------------------------+-----------------+\n");
                result += strTemp;
            }
            else
            {
                snprintf(strTemp, SIZE_STR_TEMP, "+------------------------------------------------+\n");
                result += strTemp;
                snprintf(strTemp, SIZE_STR_TEMP, "| No Data.                                       |\n");
                result += strTemp;
                snprintf(strTemp, SIZE_STR_TEMP, "+------------------------------------------------+\n");
                result += strTemp;

            }
        }
        else
        {
            snprintf(strTemp, SIZE_STR_TEMP, " FAIL TO SELECT SESSION INFO.\n");
            result += strTemp;

            bRtn = false;
        }
    }





    return result;
}

string EmsGWMain::RtrvZoneList(int argc, const char *const *argv, atom::EN_CLI_FORMAT format)
{
    char strTemp[SIZE_STR_TEMP];
    string result;
    result.clear();

    ST_5G_ZONE_INFO stZoneInfo;
    vector<ST_5G_ZONE_INFO> *a_pstvecZoneInfo;

    bool bRtn = true;
    if (bRtn == true)
    {
        if (CliPdbWorker.Execute(m_cZoneInfo) == true)
        {
            a_pstvecZoneInfo = m_cZoneInfo.GetValue();
            if((int)a_pstvecZoneInfo->size()>0)
            {
                snprintf(strTemp, SIZE_STR_TEMP, "+-------------------------------+---------------------+----------+\n");
                result += strTemp;
                snprintf(strTemp, SIZE_STR_TEMP, "| SVC_ID                        | ZONE CODE           | POSITION |\n");
                result += strTemp;
                snprintf(strTemp, SIZE_STR_TEMP, "+-------------------------------+---------------------+----------+\n");
                result += strTemp;

                for(int i = 0 ; i < (int)a_pstvecZoneInfo->size();i++)
                {
                    stZoneInfo = (ST_5G_ZONE_INFO)a_pstvecZoneInfo->at(i);
                    snprintf(strTemp, SIZE_STR_TEMP, "| %-30s| %-20s|%-9d |\n", stZoneInfo.szSVCID, stZoneInfo.szZoneCode, stZoneInfo.nPosition);
                    result += strTemp;
                }

                snprintf(strTemp, SIZE_STR_TEMP, "+-------------------------------+---------------------+----------+\n");
                result += strTemp;
            }
            else
            {
                snprintf(strTemp, SIZE_STR_TEMP, "+-----------------------------------------------+\n");
                result += strTemp;
                snprintf(strTemp, SIZE_STR_TEMP, "| No Data.                                      |\n");
                result += strTemp;
                snprintf(strTemp, SIZE_STR_TEMP, "+-----------------------------------------------+\n");
                result += strTemp;

            }
        }
        else
        {
            snprintf(strTemp, SIZE_STR_TEMP, " FAIL TO SELECT ZONE INFO.\n");
            result += strTemp;

            bRtn = false;
        }
    }


    return result;
}

int main(int argc, char * argv[]) {

    EmsGWMain gw(argc, argv, NDF_APPL_INIT_PKG|NDF_APPL_INIT_ATOM);
    //EmsGWMain gw(argc, argv, NDF_APPL_INIT_PKG);
    gw.Run();

    return 0;
}
