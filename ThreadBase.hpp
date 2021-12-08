#ifndef THREAD_BASE_HPP
#define THREAD_BASE_HPP

#include <thread>
#include <memory>
#include <atomic>

extern thread_local  char egwLogName[32];


class ThreadBase {
public:
    explicit ThreadBase()
        : bRunf_(true),
          pRefCounter_(std::make_shared<bool>()) {}
    virtual ~ThreadBase() = default;

    ThreadBase(const ThreadBase & _rhs) = delete;
    ThreadBase(const ThreadBase && _rhs) = delete;
    ThreadBase & operator=(ThreadBase & _rhs) = delete;
    ThreadBase & operator=(const ThreadBase & _rhs) = delete;

    bool IsRun() { return pRefCounter_.use_count() > 1; }

    void Run() {
        thr_ = std::thread(&ThreadBase::run, this, pRefCounter_);
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
    void Stop() { bRunf_.store(false); }

protected:
    void join() {
        bRunf_.store(false);

        if(thr_.joinable())
            thr_.join();
    }
    virtual void run(std::shared_ptr<bool> _pRefCounter) = 0;

protected:
    std::atomic_bool        bRunf_;
    std::shared_ptr<bool>   pRefCounter_;

    std::thread             thr_;
};


#endif // THREAD_BASE_HPP
