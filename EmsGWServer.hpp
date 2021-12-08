#ifndef EMS_GW_SERVER_HPP
#define EMS_GW_SERVER_HPP

#include <thread>
#include <memory>
#include <atomic>
#include <deque>

#include "EGWTcpServer.hpp"
#include "EGWHttpTask.hpp"
#include "EGWJobFactory.hpp"

#include "ThreadBase.hpp"
#include "PDBConnectionManager.hpp"

#include "CThreadMonitor.hpp"

class EmsGWServer : public ThreadBase {
public:
    explicit EmsGWServer();
    ~EmsGWServer();

    bool Init(int _port, bool _bIpv6);
    void RegisterPdbManager(std::shared_ptr<PDB::ConnectionManager> _pcm, std::memory_order=std::memory_order_seq_cst) {
        jobFactory_.RegisterPdbManager(_pcm);
    }
    void SetHangupThreadMonitor(CThreadMonitor * _pThreadMonitor) {
        pThreadMonitor_ = _pThreadMonitor;
    }

private:
    void run(std::shared_ptr<bool> _pRefCounter);
    std::shared_ptr<EGWHttpTask> makeTask(std::shared_ptr<EGWSocket> _client);

private:
    EGWTcpServer            server_;
    std::deque<std::shared_ptr<EGWHttpTask>>  deque_;

    EGWJobFactory           jobFactory_;

    std::string      className_;
    CThreadMonitor * pThreadMonitor_ = nullptr;
};


#endif // EMS_GW_SERVER_HPP
