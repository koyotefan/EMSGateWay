
#include <sstream>

#include "EGWDefine.hpp"
#include "ExceptionRuleCfgJob.hpp"

extern std::string  gSysName;

bool ExceptionRuleCfgJob::Init() {

    if(netconfMP_.Init() == false) {
        W_THD_LOG(egwLogName,"ExceptionRuleCfgJob.netconfMP.Init fail");
        return false;
    }

    if(netconfGx_.Init() == false) {
        W_THD_LOG(egwLogName,"ExceptionRuleCfgJob.netconfGx.Init fail");
        return false;
    }

    return true;
}

bool ExceptionRuleCfgJob::Do(HttpRequest & _req, HttpResponse & _resp) {

    // gSysName 을 보고... MP 가 없으면.. 하면 안되요..
    /*-
    if(gSysName.find("mp") == std::string::npos && gSysName.find("MP") == std::string::npos) {
        W_THD_LOG(egwLogName,"I am not MP [%s]", gSysName.c_str());
        _resp.SetStatusCode(200);
        return true;
    }
    -*/
    if(EGWCommon::IsLB(gSysName) == true) {
        W_THD_LOG(egwLogName,"I am not MP [%s]", gSysName.c_str());
        _resp.SetStatusCode(200);
        return true;
    }

    struct tm   stT;
    time_t t = time(nullptr);

    std::stringstream ss;
    ss << std::put_time(localtime_r(&t, &stT), "%Y%m%d%H%M%S");

    netconfMP_.Save(ss.str());
    netconfGx_.Save(ss.str());

    _resp.SetStatusCode(200);
    return true;
}
