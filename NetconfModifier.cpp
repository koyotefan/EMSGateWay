#include "NDFServiceLog.hpp"
#include "CNDFATOMAdaptor.hpp"

#include "NetconfModifier.hpp"
#include "rapidjson/document.h"

extern thread_local char egwLogName[32];

bool NetconfExceptionCFG::Save(std::string && _value) {

    std::lock_guard<std::mutex>     guard(mutex_);

    std::string     jData;
    makeJsonData(jData, _value);

    try {
        api_->EditConfigJsonReq("PCF_MP_CONFIG", jData, CNetconfAPI::SRC_RUNNING);
        api_->EditConfigJsonReq("PCF_MP_CONFIG", jData, CNetconfAPI::SRC_STARTUP);
    } catch (atom::CException & _e) {
        W_THD_LOG(egwLogName,"SaveConfig.Exception [%s] [%s]", _e.what(), jData.c_str());
        return false;
    } catch (...) {
        W_THD_LOG(egwLogName,"SaveConfig.Exception [%s]", jData.c_str());
        return false;
    }

    return true;
}

void NetconfExceptionCFG::makeJsonData(std::string & _jData, std::string & _value) {

    char buf[256];
    snprintf(buf, sizeof(buf),
        "{ \"mp\":{ \"smp\": { \"policy-exception-config\":\"%s\" } } }",
        _value.c_str());

    _jData = buf;
}

bool NetconfHolidayCfgMP::Save(std::string && _value) {

    std::lock_guard<std::mutex>     guard(mutex_);

    std::string     jData;
    makeJsonData(jData, _value);

    try {
        api_->EditConfigJsonReq("PCF_MP_CONFIG", jData, CNetconfAPI::SRC_RUNNING);
        api_->EditConfigJsonReq("PCF_MP_CONFIG", jData, CNetconfAPI::SRC_STARTUP);
    } catch (atom::CException & _e) {
        W_THD_LOG(egwLogName,"SaveConfig.Exception [%s] [%s]", _e.what(), jData.c_str());
        return false;
    } catch (...) {
        W_THD_LOG(egwLogName,"SaveConfig.Exception [%s]", jData.c_str());
        return false;
    }

    return true;
}

void NetconfHolidayCfgMP::makeJsonData(std::string & _jData, std::string & _value) {

    char buf[256];
    snprintf(buf, sizeof(buf),
        "{ \"mp\":{ \"smp\": { \"reload-holidays\":\"%s\" } } }",
        _value.c_str());

    _jData = buf;
}

bool NetconfHolidayCfgLB::Save(std::string && _value) {

    std::lock_guard<std::mutex>     guard(mutex_);

    std::string     jData;
    makeJsonData(jData, _value);

    try {
        api_->EditConfigJsonReq("PCF_LB_CONFIG", jData, CNetconfAPI::SRC_RUNNING);
        api_->EditConfigJsonReq("PCF_LB_CONFIG", jData, CNetconfAPI::SRC_STARTUP);
    } catch (atom::CException & _e) {
        W_THD_LOG(egwLogName,"SaveConfig.Exception [%s] [%s]", _e.what(), jData.c_str());
        return false;
    }

    return true;
}

void NetconfHolidayCfgLB::makeJsonData(std::string & _jData, std::string & _value) {

    char buf[256];
    snprintf(buf, sizeof(buf),
        "{ \"lb\":{ \"time-mgr\": { \"reload-holidays\":\"%s\" } } }",
        _value.c_str());

    _jData = buf;
}

bool NetconfHolidayGxMGR::Save(std::string && _value) {

    std::lock_guard<std::mutex>     guard(mutex_);

    std::string     jData;
    makeJsonData(jData, _value);

    try {
        api_->EditConfigJsonReq("PCF_MP_CONFIG", jData, CNetconfAPI::SRC_RUNNING);
        api_->EditConfigJsonReq("PCF_MP_CONFIG", jData, CNetconfAPI::SRC_STARTUP);
    } catch (atom::CException & _e) {
        W_THD_LOG(egwLogName,"SaveConfig.Exception [%s] [%s]", _e.what(), jData.c_str());
        return false;
    } catch (...) {
        W_THD_LOG(egwLogName,"SaveConfig.Exception [%s]", jData.c_str());
        return false;
    }

    return true;
}

void NetconfHolidayGxMGR::makeJsonData(std::string & _jData, std::string & _value) {

    char buf[256];
    snprintf(buf, sizeof(buf),
        "{ \"mp\":{ \"gxp\": { \"reload-holidays\":\"%s\" } } }",
        _value.c_str());

    _jData = buf;
}

bool NetconfReloadSessionRule::Save(std::string && _value) {

    std::lock_guard<std::mutex>     guard(mutex_);

    std::string     jData;
    makeJsonData(jData, _value);

    try {
        api_->EditConfigJsonReq("PCF_MP_CONFIG", jData, CNetconfAPI::SRC_RUNNING);
        api_->EditConfigJsonReq("PCF_MP_CONFIG", jData, CNetconfAPI::SRC_STARTUP);
    } catch (atom::CException & _e) {
        W_THD_LOG(egwLogName,"SaveConfig.Exception [%s] [%s]", _e.what(), jData.c_str());
        return false;
    } catch (...) {
        W_THD_LOG(egwLogName,"SaveConfig.Exception [%s]", jData.c_str());
        return false;
    }

    return true;
}

void NetconfReloadSessionRule::makeJsonData(std::string & _jData, std::string & _value) {

    char buf[256];
    snprintf(buf, sizeof(buf),
        "{ \"mp\":{ \"smp\": { \"reload-session-rule\":\"%s\" } } }",
        _value.c_str());

    _jData = buf;
}

bool NetconfReloadURSPData::Save(std::string && _value) {

    std::lock_guard<std::mutex>     guard(mutex_);

    std::string     jData;
    makeJsonData(jData, _value);

    try {
        api_->EditConfigJsonReq("PCF_MP_CONFIG", jData, CNetconfAPI::SRC_RUNNING);
        api_->EditConfigJsonReq("PCF_MP_CONFIG", jData, CNetconfAPI::SRC_STARTUP);
    } catch (atom::CException & _e) {
        W_THD_LOG(egwLogName,"SaveConfig.Exception [%s] [%s]", _e.what(), jData.c_str());
        return false;
    } catch (...) {
        W_THD_LOG(egwLogName,"SaveConfig.Exception [%s]", jData.c_str());
        return false;
    }

    return true;
}

void NetconfReloadURSPData::makeJsonData(std::string & _jData, std::string & _value) {

    char buf[256];
    snprintf(buf, sizeof(buf),
        "{ \"mp\":{ \"uep\": { \"reload-ursp-data\":\"%s\" } } }",
        _value.c_str());

    _jData = buf;
}

bool NetconfSliceDnn::Save(std::string && _value) {

    std::lock_guard<std::mutex>     guard(mutex_);

    std::string     jData;
    makeJsonData(jData, _value);

    try {
        api_->EditConfigJsonReq("PCF_MP_CONFIG", jData, CNetconfAPI::SRC_RUNNING);
        api_->EditConfigJsonReq("PCF_MP_CONFIG", jData, CNetconfAPI::SRC_STARTUP);
    } catch (atom::CException & _e) {
        W_THD_LOG(egwLogName,"SaveConfig.Exception [%s] [%s]", _e.what(), jData.c_str());
        return false;
    } catch (...) {
        W_THD_LOG(egwLogName,"SaveConfig.Exception [%s]", jData.c_str());
        return false;
    }

    return true;
}

void NetconfSliceDnn::makeJsonData(std::string & _jData, std::string & _value) {

    char buf[256];
    snprintf(buf, sizeof(buf),
        "{ \"mp\":{ \"smp\": { \"restriction-slice-dnn\":\"%s\" } } }",
        _value.c_str());

    _jData = buf;
}

bool NetconfGxMGRSubsException::Save(std::string && _value) {

    std::lock_guard<std::mutex>     guard(mutex_);

    std::string     jData;
    makeJsonData(jData, _value);

    try {
        api_->EditConfigJsonReq("PCF_MP_CONFIG", jData, CNetconfAPI::SRC_RUNNING);
        api_->EditConfigJsonReq("PCF_MP_CONFIG", jData, CNetconfAPI::SRC_STARTUP);
    } catch (atom::CException & _e) {
        W_THD_LOG(egwLogName,"SaveConfig.Exception [%s] [%s]", _e.what(), jData.c_str());
        return false;
    } catch (...) {
        W_THD_LOG(egwLogName,"SaveConfig.Exception [%s]", jData.c_str());
        return false;
    }

    return true;
}

void NetconfGxMGRSubsException::makeJsonData(std::string & _jData, std::string & _value) {

    char buf[256];
    snprintf(buf, sizeof(buf),
        "{ \"mp\":{ \"gxp\": { \"reload-subs-exception\":\"%s\" } } }",
        _value.c_str());

    _jData = buf;
}

bool NetconfGxMGRRARSkip::Save(std::string && _value) {

    std::lock_guard<std::mutex>     guard(mutex_);

    std::string     jData;
    makeJsonData(jData, _value);

    try {
        api_->EditConfigJsonReq("PCF_MP_CONFIG", jData, CNetconfAPI::SRC_RUNNING);
        api_->EditConfigJsonReq("PCF_MP_CONFIG", jData, CNetconfAPI::SRC_STARTUP);
    } catch (atom::CException & _e) {
        W_THD_LOG(egwLogName,"SaveConfig.Exception [%s] [%s]", _e.what(), jData.c_str());
        return false;
    } catch (...) {
        W_THD_LOG(egwLogName,"SaveConfig.Exception [%s]", jData.c_str());
        return false;
    }

    return true;
}

void NetconfGxMGRRARSkip::makeJsonData(std::string & _jData, std::string & _value) {

    char buf[256];
    snprintf(buf, sizeof(buf),
        "{ \"mp\":{ \"gxp\": { \"reload-rar-skip-exception\":\"%s\" } } }",
        _value.c_str());

    _jData = buf;
}

bool NetconfGxMGRDisasterRoaming::Save(std::string && _value) {

    std::lock_guard<std::mutex>     guard(mutex_);

    std::string     jData;
    makeJsonData(jData, _value);

    try {
        api_->EditConfigJsonReq("PCF_MP_CONFIG", jData, CNetconfAPI::SRC_RUNNING);
        api_->EditConfigJsonReq("PCF_MP_CONFIG", jData, CNetconfAPI::SRC_STARTUP);
    } catch (atom::CException & _e) {
        W_THD_LOG(egwLogName,"SaveConfig.Exception [%s] [%s]", _e.what(), jData.c_str());
        return false;
    } catch (...) {
        W_THD_LOG(egwLogName,"SaveConfig.Exception [%s]", jData.c_str());
        return false;
    }

    return true;
}

void NetconfGxMGRDisasterRoaming::makeJsonData(std::string & _jData, std::string & _value) {

    char buf[256];
    snprintf(buf, sizeof(buf),
        "{ \"mp\":{ \"gxp\": { \"reload-disaster-roaming\":\"%s\" } } }",
        _value.c_str());

    _jData = buf;
}

bool NetconfGxMGRFemtocell::Save(std::string && _value) {

    std::lock_guard<std::mutex>     guard(mutex_);

    std::string     jData;
    makeJsonData(jData, _value);

    try {
        api_->EditConfigJsonReq("PCF_MP_CONFIG", jData, CNetconfAPI::SRC_RUNNING);
        api_->EditConfigJsonReq("PCF_MP_CONFIG", jData, CNetconfAPI::SRC_STARTUP);
    } catch (atom::CException & _e) {
        W_THD_LOG(egwLogName,"SaveConfig.Exception [%s] [%s]", _e.what(), jData.c_str());
        return false;
    } catch (...) {
        W_THD_LOG(egwLogName,"SaveConfig.Exception [%s]", jData.c_str());
        return false;
    }

    return true;
}

void NetconfGxMGRFemtocell::makeJsonData(std::string & _jData, std::string & _value) {

    char buf[256];
    snprintf(buf, sizeof(buf),
        "{ \"mp\":{ \"gxp\": { \"reload-femtocell\":\"%s\" } } }",
        _value.c_str());

    _jData = buf;
}

