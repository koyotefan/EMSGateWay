#ifndef HOLIDAY_CFG_JOB_HPP
#define HOLIDAY_CFG_JOB_HPP

#include "EGWJob.hpp"
#include "NetconfModifier.hpp"

class HolidayCfgJob : public EGWJob {
public:
    explicit HolidayCfgJob() {}
    ~HolidayCfgJob() {}

    bool Init();
    bool Do(HttpRequest & _req, HttpResponse & _resp);

private:
    NetconfHolidayCfgMP   netconfMP_;
    NetconfHolidayCfgLB   netconfLB_;

    NetconfHolidayGxMGR   netconfGxMGR_;


};



#endif // HOLIDAY_CFG_JOB_HPP
