#ifndef SUBSCRIBER_JOB_HPP
#define SUBSCRIBER_JOB_HPP

#include "PDBConnectionManager.hpp"
#include "PDBWorker.hpp"
#include "EGWJob.hpp"

class SubscriberJob : public EGWJob {
public:
    explicit SubscriberJob() {}
    ~SubscriberJob() {}

    void SetMdn(const std::string & _mdn) { mdn_ = _mdn; }
    void SetPdbWorker(PDB::Worker * _pWorker) { pWorker_ = _pWorker; }
    bool Do(HttpRequest & _req, HttpResponse & _resp);

private:
    std::shared_ptr<PDB::ConnectionManager>     pcm_;
    std::string     mdn_;
    PDB::Worker *   pWorker_ = nullptr;

    unsigned int    tid_ = 0; 
};



#endif // SUBSCRIBER_JOB_HPP
