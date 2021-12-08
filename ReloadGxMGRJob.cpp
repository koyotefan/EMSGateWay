
#include <sstream> 

#include "EGWDefine.hpp"
#include "ReloadGxMGRJob.hpp"

extern std::string  gSysName;

bool GxMGRSubsExceptionJob::Init() {

    if(netconf_.Init() == false) {
        W_THD_LOG(egwLogName,"GxMGRSubsExceptionJob.netconf.Init fail");
        return false;
    }

    return true;
}

bool GxMGRSubsExceptionJob::Do(HttpRequest & _req, HttpResponse & _resp) {

    struct tm   stT;
    time_t t = time(nullptr);

    std::stringstream ss;
    ss << std::put_time(localtime_r(&t, &stT), "%Y%m%d%H%M%S");

    if(EGWCommon::IsLB(gSysName) == false) {
        netconf_.Save(ss.str());
    }

    _resp.SetStatusCode(200);
    return true;
}

bool GxMGRRARSkipJob ::Init() {

    if(netconf_.Init() == false) {
        W_THD_LOG(egwLogName,"GxMGRRARSkipJob.netconf.Init fail");
        return false;
    }

    return true;
}

bool GxMGRRARSkipJob::Do(HttpRequest & _req, HttpResponse & _resp) {

    struct tm   stT;
    time_t t = time(nullptr);

    std::stringstream ss;
    ss << std::put_time(localtime_r(&t, &stT), "%Y%m%d%H%M%S");

    if(EGWCommon::IsLB(gSysName) == false) {
        netconf_.Save(ss.str());
    }

    _resp.SetStatusCode(200);
    return true;
}

bool GxMGRDisasterRoamingJob::Init() {

    if(netconf_.Init() == false) {
        W_THD_LOG(egwLogName,"GxMGRDisasterRoamingJob.netconf.Init fail");
        return false;
    }

    return true;
}

bool GxMGRDisasterRoamingJob::Do(HttpRequest & _req, HttpResponse & _resp) {

    struct tm   stT;
    time_t t = time(nullptr);

    std::stringstream ss;
    ss << std::put_time(localtime_r(&t, &stT), "%Y%m%d%H%M%S");

    if(EGWCommon::IsLB(gSysName) == false) {
        netconf_.Save(ss.str());
    }

    _resp.SetStatusCode(200);
    return true;
}

bool GxMGRFemtocellJob::Init() {

    if(netconf_.Init() == false) {
        W_THD_LOG(egwLogName,"GxMGRFemtocellJob.netconf.Init fail");
        return false;
    }

    return true;
}

bool GxMGRFemtocellJob::Do(HttpRequest & _req, HttpResponse & _resp) {

    struct tm   stT;
    time_t t = time(nullptr);

    std::stringstream ss;
    ss << std::put_time(localtime_r(&t, &stT), "%Y%m%d%H%M%S");

    if(EGWCommon::IsLB(gSysName) == false) {
        netconf_.Save(ss.str());
    }

    _resp.SetStatusCode(200);
    return true;
}
