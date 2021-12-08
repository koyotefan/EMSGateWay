#ifndef NETSTAT_REPORTER_HPP
#define NETSTAT_REPORTER_HPP

#include <thread>
#include <memory>
#include <atomic>

#include "ThreadBase.hpp"

class NetstatReporter : public ThreadBase {
public:
    explicit NetstatReporter();
    ~NetstatReporter() { ThreadBase::join(); }

    bool Init();

private:
    void run(std::shared_ptr<bool> _pRefCounter);

};


#endif // NETSTAT_REPORTER_HPP
