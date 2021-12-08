#ifndef __ExternalConnectionReporter_HPP__
#define __ExternalConnectionReporter_HPP__

#include "ThreadBase.hpp"
#include "CThreadMonitor.hpp"
#include "HttpClient.hpp"

class ExternalConnectionReporter : public ThreadBase
{
public:
    explicit ExternalConnectionReporter();
    ~ExternalConnectionReporter() { ThreadBase::join(); }

    bool Init();

    void SetHangupThreadMonitor(CThreadMonitor * _pThreadMonitor) {
        pThreadMonitor_ = _pThreadMonitor;
    }   

private:
    void run(std::shared_ptr<bool>  _pRefCounter);
    bool sendToAtomReportMessage(HttpClient * _p, 
                                 const std::string & _path,
                                 const std::string & _out);

private:

    std::string      className_;
    CThreadMonitor * pThreadMonitor_ = nullptr;    

    std::string     networkMonitorFile_;
    std::string     emsIp_;
    int             reportingPort_ = 5000;

};

#endif
