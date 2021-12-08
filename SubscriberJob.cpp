
#include "SubscriberJob.hpp"
#include "PgNotiListSQL.hpp"

extern std::string  gSysClusterName;

bool SubscriberJob::Do(HttpRequest & _req, HttpResponse & _resp) {

    InsertPgNotiListSQL iPg;
    if(iPg.Bind() == false) {
        E_THD_LOG(egwLogName, "InsertPgNotiListSQL Bind() fail");
        _resp.SetStatusCode(500);
        return false;
    }

    stPgNotiList    item;
    snprintf(item.mdn, sizeof(item.mdn)-1, "%s", mdn_.c_str());
    // snprintf(item.tid, sizeof(item.tid)-1, "0000000000000000");
    snprintf(item.tid, sizeof(item.tid)-1, "%016u", tid_);
    tid_++;
    snprintf(item.sys, sizeof(item.sys)-1, "%s", gSysClusterName.c_str());

    iPg.SetValue(item);

    if(pWorker_->Execute(iPg) == false) {
        W_THD_LOG(egwLogName,"InsertPgNotiListSQL Execute() fail [mdn:%s]", item.mdn);
    } else {
        I_THD_LOG(egwLogName,"InsertPgNotiListSQL.Execute.success [mdn:%s]", item.mdn);
    }

    _resp.SetStatusCode(200);
    return true;
}
