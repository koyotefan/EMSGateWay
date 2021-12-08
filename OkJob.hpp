#ifndef OK_JOB_HPP
#define OK_JOB_HPP

#include "EGWJob.hpp"

class OkJob : public EGWJob {
public:
    explicit OkJob() {}
    ~OkJob() {}

    bool Do(HttpRequest & _req, HttpResponse & _resp) {

        D_LOG("Method        : [%s]", _req.GetMethod());
        D_LOG("Path          : [%s]", _req.GetPath());
        D_LOG("Version       : [%s]", _req.GetVersion());
        D_LOG("Content-Type  : [%s]", _req.GetHeader("Content-Type"));
        D_LOG("Content-Length: [%s]", _req.GetHeader("Content-Length"));
        D_LOG("Body          : [%s]", _req.GetBody());

        _resp.SetStatusCode(200);
        D_LOG("Status Line   : [%s]", _resp.GetStatusLine());
        D_LOG("Content-Type  : [%s]", _resp.GetHeader("Content-Type"));
        D_LOG("Content-Length: [%s]", _resp.GetHeader("Content-Length"));
        D_LOG("Body          : [%s]", _resp.GetBody());

        return true;
    }

};


#endif // OK_JOB_HPP
