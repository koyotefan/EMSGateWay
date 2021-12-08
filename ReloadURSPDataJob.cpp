#include <sstream>

#include "ReloadURSPDataJob.hpp"

extern std::string  gSysName;

bool ReloadURSPDataJob::Init() {

    if(netconf_.Init() == false) {
        W_THD_LOG(egwLogName,"ReloadURSPData.netconf.Init fail");
        return false;
    }

    return true;
}


bool ReloadURSPDataJob::Do(HttpRequest & _req, HttpResponse & _resp) {

    struct tm   stT;
    time_t t = time(nullptr);

    std::stringstream ss;
    ss << std::put_time(localtime_r(&t, &stT), "%Y%m%d%H%M%S");

    D_THD_LOG(egwLogName,"--------- [%s]", gSysName.c_str());

    if(gSysName.find("mp") != std::string::npos || gSysName.find("MP") != std::string::npos)
        netconf_.Save(ss.str());

    _resp.SetStatusCode(200);
    return true;
}
