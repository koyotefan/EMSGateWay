
#include <sstream>

#include "EGWDefine.hpp"
#include "HolidayCfgJob.hpp"

extern std::string  gSysName;

bool HolidayCfgJob::Init() {

    if(netconfMP_.Init() == false) {
        W_THD_LOG(egwLogName,"HolidayCfgJob.netconfMP.Init fail");
        return false;
    }

    if(netconfLB_.Init() == false) {
        W_THD_LOG(egwLogName,"HolidayCfgJob.netconfLB.Init fail");
        return false;
    }

    return true;
}


bool HolidayCfgJob::Do(HttpRequest & _req, HttpResponse & _resp) {

    struct tm   stT;
    time_t t = time(nullptr);

    std::stringstream ss;
    ss << std::put_time(localtime_r(&t, &stT), "%Y%m%d%H%M%S");

    if(EGWCommon::IsLB(gSysName))
        netconfLB_.Save(ss.str());
    else {
        netconfMP_.Save(ss.str());
        netconfGxMGR_.Save(ss.str());
    }

    /*-
    if(gSysName.find("mp") != std::string::npos || gSysName.find("MP") != std::string::npos)
        netconfMP_.Save(ss.str());

    if(gSysName.find("lb") != std::string::npos || gSysName.find("LB") != std::string::npos)
        netconfLB_.Save(ss.str());
    -*/

    _resp.SetStatusCode(200);
    return true;
}
