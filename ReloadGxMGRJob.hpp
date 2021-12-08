
#ifndef __ReloadGxMGRJob_HPP__
#define __ReloadGxMGRJob_HPP__

#include "EGWJob.hpp"
#include "NetconfModifier.hpp"

class GxMGRSubsExceptionJob: public EGWJob {
public:
    explicit GxMGRSubsExceptionJob() {}
    ~GxMGRSubsExceptionJob() {}

    bool Init();
    bool Do(HttpRequest & _req, HttpResponse & _resp);

private:
    NetconfGxMGRSubsException     netconf_;
};

class GxMGRRARSkipJob : public EGWJob {
public:
    explicit GxMGRRARSkipJob() {}
    ~GxMGRRARSkipJob() {}

    bool Init();
    bool Do(HttpRequest & _req, HttpResponse & _resp);

private:
    NetconfGxMGRRARSkip     netconf_; 
};

class GxMGRDisasterRoamingJob : public EGWJob {
public:
    explicit GxMGRDisasterRoamingJob() {}
    ~GxMGRDisasterRoamingJob() {}

    bool Init();
    bool Do(HttpRequest & _req, HttpResponse & _resp);

private:
    NetconfGxMGRDisasterRoaming    netconf_;
};

class GxMGRFemtocellJob : public EGWJob {
public:
    explicit GxMGRFemtocellJob() {}
    ~GxMGRFemtocellJob() {}

    bool Init();
    bool Do(HttpRequest & _req, HttpResponse & _resp);

private:
    NetconfGxMGRFemtocell   netconf_;
};

#endif