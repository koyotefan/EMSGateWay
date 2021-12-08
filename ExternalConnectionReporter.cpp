#include <poll.h>
#include "ExternalConnectionReporter.hpp"
#include "CNDFIniConfigContainer.hpp"
 
#include "Netstat.hpp"
#include "ServiceConfig.hpp"

extern CNDFIniConfigContainer * gCfg;

ExternalConnectionReporter::ExternalConnectionReporter() {
    className_ = typeid(*this).name();
}

bool ExternalConnectionReporter::Init() {

    char buf[128] = {'\0', };

    if(gCfg->Get((char *)"EMS_GW", (char *)"NETWORK_MONITOR_FILE", buf) == false) {
        W_THD_LOG(egwLogName,"configuration (EMS_GW:NETWORK_MONITOR_FILE) not found!") ;
        return false;
    } else {
        networkMonitorFile_ = buf;
        I_THD_LOG(egwLogName,
            "configuration (EMS_GW:NETWORK_MONITOR_FILE) [%s]", 
            networkMonitorFile_.c_str()) ;
    }
    
    if(gCfg->Get((char *)"EMS_GW", (char *)"EMS_IP", buf) == false) {
        W_THD_LOG(egwLogName,"configuration (EMS_GW:EMS_IP) not found!") ;
        return false;
    } else {
        emsIp_ = buf;
        I_THD_LOG(egwLogName,
            "configuration (EMS_GW:EMS_IP) [%s]", 
            emsIp_.c_str()) ;
    }

    if(gCfg->Get((char *)"EMS_GW", (char *)"EMS_REPORTING_PORT", buf) == false) {
        W_THD_LOG(egwLogName,
            "configuration (EMS_GW:REPORTING_PORT) not found! default [%d]",
            reportingPort_);
    } else {
        reportingPort_ = std::strtod(buf, nullptr);
        I_THD_LOG(egwLogName,
            "configuration (EMS_GW:EMS_REPORTING_PORT) [%d]", 
            reportingPort_) ;
    }

    return true;
}


void ExternalConnectionReporter::run(std::shared_ptr<bool>   _pRefCounter) {

    (void)_pRefCounter;

    memset(egwLogName, 0, sizeof(egwLogName));
    strncpy(egwLogName, "ExternalReporter", sizeof("ExternalReporter"));
    NDF_INIT_THD_LOG(egwLogName);

    I_THD_LOG(egwLogName,"========== START=========="); 

    auto netstatTCP4 = std::make_unique<NetstatTCP4>();
    auto serviceConfig = std::make_unique<ServiceConfig>(networkMonitorFile_);
    auto netstatSCTP = std::make_unique<NetstatSCTP>();

    auto httpClient = std::make_unique<HttpClient>();
    httpClient->Init(emsIp_, reportingPort_);

    pThreadMonitor_->Register(className_.c_str());

    std::string     outForPG;
    std::string     outForPGW;

    bool            bBeShouldSend = false;

    while(bRunf_) {
        poll(nullptr, 0, 300);

        //if(retryBarrier > 0)
        //    --retryBarrier;

        pThreadMonitor_->Refresh(className_.c_str());

        if(netstatTCP4->Load() == false) {
            break;
        }

        if(netstatSCTP->Load() == false) {
            break;
        }

        if(serviceConfig->Load() == false) {
            break;
        }

        if(netstatTCP4->IsChanged() == false &&
           netstatSCTP->IsChanged() == false &&
           serviceConfig->IsChanged() == false) {
            continue;
        }

        I_THD_LOG(egwLogName, "--------- tcp4 [%d] sctp [%d] ser [%d]", 
            netstatTCP4->IsChanged(),
            netstatSCTP->IsChanged(),
            serviceConfig->IsChanged());

        if(serviceConfig->UpdateConnection(netstatTCP4.get(), netstatSCTP.get())) {

        // I_THD_LOG(egwLogName, "--------- message making");

            outForPG.clear();
            serviceConfig->MakeAtomReportMessageForPG(outForPG);

            outForPGW.clear();
            serviceConfig->MakeAtomReportMessageForPGW(outForPGW);

            bBeShouldSend = true; 

        }

        if(bBeShouldSend) {

            if(sendToAtomReportMessage(httpClient.get(), "/node/status/pg", outForPG) &&
               sendToAtomReportMessage(httpClient.get(), "/node/status/ncn", outForPGW)) {

                bBeShouldSend = false;

                // 전송 실패시, 300 ms 마다 시도 되는건 아닌가? 하는 생각도 하지만,
                // IsChanged() 가 보통 2 sec 마다 true 를 내고 있어서...
                // 2 sec 마다 재시도 될꺼예요.

            }
        }

    }

    W_THD_LOG(egwLogName, "ExternalConnectionReporter out of service");
}

bool ExternalConnectionReporter::sendToAtomReportMessage(HttpClient *  _p, 
                                                         const std::string & _path,
                                                         const std::string & _out) {

    if(_p == nullptr) {
        E_THD_LOG(egwLogName, "HTTPClient is nullptr");
        return false;
    }

    if(_out.empty())
        return true;


    if(_p->Request(_path, _out)) {
        I_THD_LOG(egwLogName, "SEND");
        I_THD_LOG(egwLogName, "%s", _out.c_str());
        _p->Response();
        I_THD_LOG(egwLogName, "RECV RESP");

        return true;
        
    } else {
        W_THD_LOG(egwLogName, "SEND fail");
        W_THD_LOG(egwLogName, "%s", _out.c_str());
    }

    return false;
}

