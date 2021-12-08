#ifndef EGW_HTTP_TASK_CPP
#define EGW_HTTP_TASK_CPP

#include "EGWDefine.hpp"
#include "EGWHttpTask.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

EGWHttpTask::EGWHttpTask(EGWJobFactory & _jobs)
        : jobs_(_jobs) {

    className_ = typeid(*this).name();
}

bool EGWHttpTask::Init(std::shared_ptr<EGWSocket>  _socket) {
    if(_socket.get() == nullptr)
        return false;

    socket_ = _socket;
    return true;
}

void EGWHttpTask::run(std::shared_ptr<bool> _pRefCounter) {

    (void)_pRefCounter;

    pThreadMonitor_->Register(className_.c_str());

    HttpRequest     req;
    if(req.Receive(socket_.get()) == false) {

        pThreadMonitor_->DeRegister(className_.c_str());
        W_THD_LOG(egwLogName,"HTTP receive() fail");
        return ;
    }

    EGWJob & j = jobs_.GetJob(req.GetPath());

    HttpResponse    resp;
    if(j.Do(req, resp) == false) {
        resp.SetStatusCode(500);
    } else {
        resp.SetStatusCode(200);
    }

    pThreadMonitor_->Refresh(className_.c_str());

    if(resp.Send(socket_.get()) == false) {
        W_THD_LOG(egwLogName,"HTTP Response Send fail [%s]", resp.GetStatusLine());
    }

    char c;
    bool bret = socket_->Recv(&c, 1);
    D_THD_LOG(egwLogName,"End - [%d]", bret);

    pThreadMonitor_->DeRegister(className_.c_str());
    return;
}


#endif // EGW_HTTP_TASK_CPP
