#ifndef RELOAD_URSP_DATA_JOB_HPP
#define RELOAD_URSP_DATA_JOB_HPP

#include "EGWJob.hpp"
#include "NetconfModifier.hpp"

class ReloadURSPDataJob : public EGWJob {
public:
    explicit ReloadURSPDataJob() {}
    ~ReloadURSPDataJob() {}

    bool Init();
    bool Do(HttpRequest & _req, HttpResponse & _resp);

private:
    NetconfReloadURSPData netconf_;

};

#endif // RELOAD_URSP_DATA_JOB_HPP
