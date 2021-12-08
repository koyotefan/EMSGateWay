#ifndef PDU_SESSION_NOTI_JOB_HPP
#define PDU_SESSION_NOTI_JOB_HPP

#include "EGWJob.hpp"

class PduSessionNotiJob : public EGWJob {
public:
    explicit PduSessionNotiJob() {}
    ~PduSessionNotiJob() {}

    void SetSmPolicyId(const std::string & _id) { smPolicyId_ = _id; }
    bool Do(HttpRequest & _req, HttpResponse & _resp) {
        _resp.SetStatusCode(200);
        return true;
    }

private:
    std::string     smPolicyId_;
};


#endif // PDU_SESSION_NOTI_JOB_HPP
