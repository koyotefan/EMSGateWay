#ifndef NOK_JOB_HPP
#define NOK_JOB_HPP

#include "EGWJob.hpp"

class NokJob : public EGWJob {
public:
    explicit NokJob() {}
    ~NokJob() {}

    void SetStatusCode(const int _code) { code_ = _code; }
    bool Do(HttpRequest & _req, HttpResponse & _resp) {
        _resp.SetStatusCode(code_);
        return true;
    }

private:
    int code_ = 404;
};


#endif // NOK_JOB_HPP
