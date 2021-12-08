#ifndef NETCONF_MODIFIER_HPP
#define NETCONF_MODIFIER_HPP

#include "CNetconfBase.hpp"

class NetconfExceptionCFG : public CNetconfBase {
public:
    explicit NetconfExceptionCFG()
        : CNetconfBase("PCF_MP_CONFIG", "/mp/smp") {

        }

    virtual ~NetconfExceptionCFG() {}

    bool Init() { return CNetconfBase::Init(); }
    bool Save(std::string && _value);

private:
    void makeJsonData(std::string & _jData, std::string & _value);

};

class NetconfHolidayCfgMP : public CNetconfBase {
public:
    explicit NetconfHolidayCfgMP()
        : CNetconfBase("PCF_MP_CONFIG", "/mp/smp") {

        }

    virtual ~NetconfHolidayCfgMP() {}

    bool Init() { return CNetconfBase::Init(); }
    bool Save(std::string && _value);

private:
    void makeJsonData(std::string & _jData, std::string & _value);

};

class NetconfHolidayGxMGR: public CNetconfBase {
public:
    explicit NetconfHolidayGxMGR()
        : CNetconfBase("PCF_MP_CONFIG", "/mp/gxp") {

        }

    virtual ~NetconfHolidayGxMGR() {}

    bool Init() { return CNetconfBase::Init(); }
    bool Save(std::string && _value);

private:
    void makeJsonData(std::string & _jData, std::string & _value);

};

class NetconfHolidayCfgLB : public CNetconfBase {
public:
    explicit NetconfHolidayCfgLB()
        : CNetconfBase("PCF_LB_CONFIG", "/lb/time-mgr") {

        }

    virtual ~NetconfHolidayCfgLB() {}

    bool Init() { return CNetconfBase::Init(); }
    bool Save(std::string && _value);

private:
    void makeJsonData(std::string & _jData, std::string & _value);

};

class NetconfReloadSessionRule : public CNetconfBase {
public:
    explicit NetconfReloadSessionRule()
        : CNetconfBase("PCF_MP_CONFIG", "/mp/smp") {

        }

    virtual ~NetconfReloadSessionRule() {}

    bool Init() { return CNetconfBase::Init(); }
    bool Save(std::string && _value);

private:
    void makeJsonData(std::string & _jData, std::string & _value);

};

class NetconfReloadURSPData : public CNetconfBase {
public:
    explicit NetconfReloadURSPData()
        : CNetconfBase("PCF_MP_CONFIG", "/mp/uep") {

        }

    virtual ~NetconfReloadURSPData() {}

    bool Init() { return CNetconfBase::Init(); }
    bool Save(std::string && _value);

private:
    void makeJsonData(std::string & _jData, std::string & _value);

};

class NetconfSliceDnn: public CNetconfBase {
public:
    explicit NetconfSliceDnn()
        : CNetconfBase("PCF_MP_CONFIG", "/mp/smp") {

        }

    virtual ~NetconfSliceDnn() {}

    bool Init() { return CNetconfBase::Init(); }
    bool Save(std::string && _value);

private:
    void makeJsonData(std::string & _jData, std::string & _value);

};

class NetconfGxMGRSubsException: public CNetconfBase {
public:
    explicit NetconfGxMGRSubsException()
        : CNetconfBase("PCF_MP_CONFIG", "/mp/gxp") {

        } 

    virtual ~NetconfGxMGRSubsException() {}

    bool Init() { return CNetconfBase::Init(); }
    bool Save(std::string && _value);

private:
    void makeJsonData(std::string & _jData, std::string & _value);

};

class NetconfGxMGRRARSkip: public CNetconfBase {
public:
    explicit NetconfGxMGRRARSkip()
        : CNetconfBase("PCF_MP_CONFIG", "/mp/gxp") {

        } 

    virtual ~NetconfGxMGRRARSkip() {}

    bool Init() { return CNetconfBase::Init(); }
    bool Save(std::string && _value);

private:
    void makeJsonData(std::string & _jData, std::string & _value);

};

class NetconfGxMGRDisasterRoaming: public CNetconfBase {
public:
    explicit NetconfGxMGRDisasterRoaming()
        : CNetconfBase("PCF_MP_CONFIG", "/mp/gxp") {

        } 

    virtual ~NetconfGxMGRDisasterRoaming() {}

    bool Init() { return CNetconfBase::Init(); }
    bool Save(std::string && _value);

private:
    void makeJsonData(std::string & _jData, std::string & _value);

};

class NetconfGxMGRFemtocell: public CNetconfBase {
public:
    explicit NetconfGxMGRFemtocell()
        : CNetconfBase("PCF_MP_CONFIG", "/mp/gxp") {

        } 

    virtual ~NetconfGxMGRFemtocell() {}

    bool Init() { return CNetconfBase::Init(); }
    bool Save(std::string && _value);

private:
    void makeJsonData(std::string & _jData, std::string & _value);

};

#endif // NETCONF_MODIFIER_HPP
