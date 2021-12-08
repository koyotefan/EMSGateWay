#ifndef RELOAD_SESSION_RULE_JOB_HPP
#define RELOAD_SESSION_RULE_JOB_HPP

#include "EGWJob.hpp"
#include "NetconfModifier.hpp"

class ReloadSessionRuleJob : public EGWJob {
public:
    explicit ReloadSessionRuleJob() {}
    ~ReloadSessionRuleJob() {}

    bool Init();
    bool Do(HttpRequest & _req, HttpResponse & _resp);

private:
    NetconfReloadSessionRule   netconf_;

};

#endif // RELOAD_SESSION_RULE_JOB_HPP
