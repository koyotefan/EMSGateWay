
#include <string>
#include <memory>
#include <poll.h>

#include "EGWDefine.hpp"
#include "EmsGWServer.hpp"

EmsGWServer::EmsGWServer() {
    className_ = typeid(*this).name();
}

EmsGWServer::~EmsGWServer() {
    while(deque_.empty() == false) {
        auto i = deque_.front();
        i->Stop();
        deque_.pop_front();
    }

    ThreadBase::join();
}

bool EmsGWServer::Init(int _port, bool _bIpv6) {

    if(jobFactory_.Init() == false) {
        E_THD_LOG(egwLogName,"Job Factory Init() fail");
        return false;
    }

    server_.SetIpv6(_bIpv6);
    server_.SetPort(_port);

    if(server_.Init() == false) {
        E_THD_LOG(egwLogName,"EGWTcpServer Init() fail");
        return false;
    }

    I_THD_LOG(egwLogName,"EmsGWServer Init() success");
    return true;
}


std::shared_ptr<EGWHttpTask> EmsGWServer::makeTask(
    std::shared_ptr<EGWSocket>     _client) {

    if(_client.get() == nullptr)
        return nullptr;

    std::shared_ptr<EGWHttpTask> http =
        std::make_shared<EGWHttpTask>(jobFactory_);

    if(http->Init(_client) == false) {
        W_THD_LOG(egwLogName,"EGWHttpTask Init() fail");
        return nullptr;
    }

    http->SetHangupThreadMonitor(pThreadMonitor_);

    http->Run();
    return http;
}

void EmsGWServer::run(std::shared_ptr<bool>   _pRefCounter) {

    memset(egwLogName, 0, sizeof(egwLogName));
    strncpy(egwLogName, "GWServer", sizeof("GWServer"));
    NDF_INIT_THD_LOG(egwLogName);

    I_THD_LOG(egwLogName,"========== START ==========");
    I_THD_LOG(egwLogName,"EmsGWServer in service");

    (void)_pRefCounter;

    // TO DO : 100 -> Config ? 100 ms
    server_.SetWaitPeriod(300);

    pThreadMonitor_->Register(className_.c_str());

    while(bRunf_) {
        pThreadMonitor_->Refresh(className_.c_str());

        std::shared_ptr<EGWHttpTask> http = makeTask(server_.GetNewClient());

        if(http.get() != nullptr) {
            deque_.emplace_back(std::move(http));
        }

        if(deque_.empty() == false) {
            auto i = deque_.front();

            if(i->IsRun() == false) {
                deque_.pop_front();
            }
        }
    }

    pThreadMonitor_->DeRegister(className_.c_str());

    W_THD_LOG(egwLogName,"EmsGWServer out of srvice");
}
