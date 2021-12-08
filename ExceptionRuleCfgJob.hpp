#ifndef EXCEPTION_RULE_CFG_JOB_HPP
#define EXCEPTION_RULE_CFG_JOB_HPP

#include "EGWJob.hpp"
#include "NetconfModifier.hpp"

class ExceptionRuleCfgJob : public EGWJob {
public:
    explicit ExceptionRuleCfgJob() {}
    ~ExceptionRuleCfgJob() {}

    bool Init();
    bool Do(HttpRequest & _req, HttpResponse & _resp);

private:
    NetconfExceptionCFG         netconfMP_;
    NetconfGxMGRSubsException   netconfGx_;

};


#endif // EXCEPTION_RULE_CFG_JOB_HPP
