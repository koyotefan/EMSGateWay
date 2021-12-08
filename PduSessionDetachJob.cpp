
#include "rapidjson/document.h"

#include "PduSessionDetachJob.hpp"
#include "PgSessionDetachListSQL.hpp"

extern std::string  gSysClusterName;

std::tuple<bool, std::string, std::string>
PduSessionDetachJob::getMdnAndDnn(const std::string & _body) {

    std::string     mdn;
    std::string     dnn;

    rapidjson::Document     doc;

    try {
        if(doc.Parse(_body.c_str()).HasParseError()) {
            W_THD_LOG(egwLogName,"json Parse() error [%s]", _body.c_str());
            return std::make_tuple(false, "", "");
        }

        rapidjson::Value::ConstMemberIterator iter = doc.FindMember("MDN");

        if(iter == doc.MemberEnd() || iter->value.IsString() == false) {
            W_THD_LOG(egwLogName,"[MDN] is not exist [%s]", _body.c_str());
            return std::make_tuple(false, "", "");
        } else {
            mdn = iter->value.GetString();
        }

        iter = doc.FindMember("MDN");
        if(iter == doc.MemberEnd() || iter->value.IsString() == false) {
            W_THD_LOG(egwLogName,"[DNN] is not exist [%s]", _body.c_str());
            return std::make_tuple(false, "", "");
        } else {
            dnn = iter->value.GetString();
        }


    } catch ( std::exception & e) {
        W_THD_LOG(egwLogName,"json Parse() exception [%s:%s]", e.what(), _body.c_str());
        return std::make_tuple(false, "", "");
    }

    return std::make_tuple(true, mdn, dnn);
}

bool PduSessionDetachJob::Do(HttpRequest & _req, HttpResponse & _resp) {

    std::string body = _req.GetBody();
    D_THD_LOG(egwLogName,"%s", body.c_str());

    std::tuple<bool, std::string, std::string> tValues = getMdnAndDnn(body);

    if(std::get<0>(tValues) != true) {
        D_THD_LOG(egwLogName,"data is unexpected");
        return true;
    }

    InsertPgSessionDetachListSQL    iSD;

    if(iSD.Bind() == false) {
        E_THD_LOG(egwLogName, "InsertPgSessionDetachListSQL Bind() fail");
        _resp.SetStatusCode(500);
        return false;
    }

    stPgSessionDetachList   item;
    snprintf(item.mdn, sizeof(item.mdn)-1, "%s", std::get<1>(tValues).c_str());
    snprintf(item.dnn, sizeof(item.dnn)-1, "%s", std::get<2>(tValues).c_str());
    snprintf(item.pcfId, sizeof(item.pcfId)-1, "%s", gSysClusterName.c_str());

    iSD.SetValue(item);

    if(pWorker_->Execute(iSD) == false) {
        W_THD_LOG(egwLogName,"InsertPgSessionDetachListSQL.Execute() fail [mdn:%s] [dnn:%s]", item.mdn);
    } else {
        I_THD_LOG(egwLogName,"InsertPgSessionDetachListSQL.Execute.success [mdn:%s] [dnn:%s]", item.dnn);
    }

    _resp.SetStatusCode(200);
    return true;

}
