#ifndef PDU_SESSION_DETACH_JOB_HPP
#define PDU_SESSION_DETACH_JOB_HPP

#include <tuple>

#include "PDBConnectionManager.hpp"
#include "PDBWorker.hpp"
#include "EGWJob.hpp"

class PduSessionDetachJob : public EGWJob {
public:
    explicit PduSessionDetachJob() {}
    ~PduSessionDetachJob() {}

    void SetSmPolicyId(const std::string & _id) { smPolicyId_ = _id; }
    void SetPdbWorker(PDB::Worker * _pWorker) { pWorker_ = _pWorker; }
    bool Do(HttpRequest & _req, HttpResponse & _resp);

private:
    std::tuple<bool, std::string, std::string> getMdnAndDnn(const std::string & _body);

private:
    std::shared_ptr<PDB::ConnectionManager>     pcm_;
    std::string     smPolicyId_;

    PDB::Worker *   pWorker_ = nullptr;
};


#endif // PDU_SESSION_DETACH_JOB_HPP
