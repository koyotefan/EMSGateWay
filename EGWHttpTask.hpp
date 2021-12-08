#ifndef EGW_HTTP_TASK_HPP
#define EGW_HTTP_TASK_HPP

#include "ThreadBase.hpp"
#include "EGWSocket.hpp"

#include "EGWJobFactory.hpp"

#include "CThreadMonitor.hpp"

class EGWHttpTask : public ThreadBase {
public:
    explicit EGWHttpTask(EGWJobFactory & _jobs);
    ~EGWHttpTask() { ThreadBase::join(); }

    bool Init(std::shared_ptr<EGWSocket>  _socket);
    void SetHangupThreadMonitor(CThreadMonitor * _pThreadMonitor) {
        pThreadMonitor_ = _pThreadMonitor;
    }

protected:
    void run(std::shared_ptr<bool> _pRefCounter);

private:
    EGWJobFactory &                jobs_;
    std::shared_ptr<EGWSocket>     socket_;

    std::string                     className_;
    CThreadMonitor *                pThreadMonitor_ = nullptr;
};


#endif // EGW_HTTP_TASK_HPP
