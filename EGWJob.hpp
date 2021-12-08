#ifndef EGW_JOB_HPP
#define EGW_JOB_HPP

#include "EGWDefine.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

extern thread_local char egwLogName[32];

class EGWJob {
public:
    explicit EGWJob() {}
    virtual ~EGWJob() {}

    virtual bool Do(HttpRequest & _req, HttpResponse & _resp) = 0;

};

#endif // EGW_JOB_HPP
