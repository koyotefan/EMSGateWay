
#include "PgSessionDetachListSQL.hpp"
#include "PDBOdbcErr.hpp"

InsertPgSessionDetachListSQL::InsertPgSessionDetachListSQL() {
    arrLen_ = new (std::nothrow) SQLLEN[item_.memberCnt];
    arrBp_  = new (std::nothrow) STBindParameter[item_.memberCnt];
}

InsertPgSessionDetachListSQL::~InsertPgSessionDetachListSQL() {
    if(arrLen_) {
        delete [] arrLen_;
        arrLen_ = nullptr;
    }

    if(arrBp_) {
        delete [] arrBp_;
        arrBp_ = nullptr;
    }
}

bool InsertPgSessionDetachListSQL::Bind() {
    char buf[512];
    snprintf(buf, sizeof(buf)-1,
        "INSERT INTO T_5G_PG_SESSION_DETACH_LIST ("
        "MDN,"
        "DNN,"
        "PCF_ID) "
        "VALUES (?,?,?)"
    );

    if(SetSQL(PDB::eDefDBType::PCF, buf, strlen(buf)) == false) {
        W_THD_LOG(egwLogName,"SetSQL() fail [%s]", buf);
        return false;
    }

    int parameterCnt = 3;
    for(int nLoop=0; nLoop < parameterCnt; ++nLoop)
        arrLen_[nLoop] = SQL_NTS;

    arrBp_[0] = {1, SQL_C_CHAR, SQL_CHAR, LEN_MDN, 0, item_.mdn, sizeof(item_.mdn), &arrLen_[0] };
    arrBp_[1] = {2, SQL_C_CHAR, SQL_CHAR, LEN_DNN, 0, item_.dnn, sizeof(item_.dnn), &arrLen_[1] };
    arrBp_[2] = {3, SQL_C_CHAR, SQL_CHAR, LEN_PCF_ID, 0, item_.pcfId,sizeof(item_.pcfId),&arrLen_[2] };

    SetBindParameter(arrBp_, parameterCnt);

    return true;
}

void InsertPgSessionDetachListSQL::SetValue(const stPgSessionDetachList & _item) {
    item_ = _item;
}

bool InsertPgSessionDetachListSQL::Execute(SQLHSTMT & _stmt) {

    sret_ = SQLExecute(_stmt);
    if(sret_ == SQL_SUCCESS || sret_ == SQL_SUCCESS_WITH_INFO) {
        D_THD_LOG(egwLogName,"InsertPgSessionDetachListSQL Success - SQLExecute");
        return true;
    }

    W_THD_LOG(egwLogName,"InsertPgSessionDetachListSQL fail [%d] [%s]",
        sret_, PDB::ODBCErr::GetStringSTMT(_stmt).c_str());
    return false;
}

