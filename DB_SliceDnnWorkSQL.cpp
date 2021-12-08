
#include "DB_SliceDnnWorkSQL.hpp"
#include "PDBOdbcErr.hpp"

extern std::string gSysName;

bool Update_SliceDnnWorkSQL::Bind() {

    char buf[256];
    snprintf(buf,
            sizeof(buf),
            "UPDATE T_SLICE_DNN_WORK "
            "SET HOSTNAME='%s', UPDATE_TIME=SYSDATE "
            "WHERE HOSTNAME='%s' OR UPDATE_TIME + (%d / 24 / 60 / 60) < SYSDATE",
            gSysName.c_str(),
            gSysName.c_str(),
            allowedMasterWorkingPeriod_);

    if(SetSQL(PDB::eDefDBType::PCF, buf, strlen(buf)) == false) {
        E_THD_LOG(egwLogName, "SetSQL() fail [%s]", buf);
        return false;
    }

    return true;
}

bool Update_SliceDnnWorkSQL::Execute(SQLHSTMT & _stmt) {

    sret_ = SQLExecute(_stmt);

    if(sret_ == SQL_NO_DATA)
        return true;

    if(sret_ != SQL_SUCCESS &&
       sret_ != SQL_SUCCESS_WITH_INFO) {
        E_THD_LOG(egwLogName,"SQLExecute fail [%d] [%s]",
            sret_, PDB::ODBCErr::GetStringSTMT(_stmt).c_str());
        return false;
    }

    if(sret_ == SQL_SUCCESS_WITH_INFO) {
        W_THD_LOG(egwLogName,"SQLExecute [%d] [%s]",
            sret_, PDB::ODBCErr::GetStringSTMT(_stmt).c_str());
    }

    return true;
}

Select_SliceDnnWorkSQL::Select_SliceDnnWorkSQL() {
    arrLen_ = new (std::nothrow) SQLLEN[Select_SliceDnnWorkSQL::memberCnt];
    arrBc_  = new (std::nothrow) STBindColumn[Select_SliceDnnWorkSQL::memberCnt];
}

Select_SliceDnnWorkSQL::~Select_SliceDnnWorkSQL() {
    if(arrLen_) {
        delete [] arrLen_;
        arrLen_ = nullptr;
    }

    if(arrBc_) {
        delete [] arrBc_;
        arrBc_ = nullptr;
    }
}

bool Select_SliceDnnWorkSQL::Bind() {

    char buf[128];
    snprintf(buf,
        sizeof(buf),
        "SELECT "
            "COUNT(*)"
        " FROM T_SLICE_DNN_WORK "
        " WHERE HOSTNAME='%s'", gSysName.c_str());

    if(SetSQL(PDB::eDefDBType::PCF, buf, strlen(buf)) == false) {
        E_THD_LOG(egwLogName, "SetSQL() fail [%s]", buf);
        return false;
    }

    for(int nLoop=0; nLoop < Select_SliceDnnWorkSQL::memberCnt; ++nLoop)
        arrLen_[nLoop] = SQL_NTS;

    arrBc_[0] = {1, SQL_C_SLONG, &nMaster_, sizeof(nMaster_), &arrLen_[0] };

    SetBindColumn(arrBc_, Select_SliceDnnWorkSQL::memberCnt);

    return true;
}

bool Select_SliceDnnWorkSQL::Execute(SQLHSTMT & _stmt) {

    nMaster_ = 0;

    sret_ = SQLExecute(_stmt);

    if(sret_ == SQL_NO_DATA)
        return true;

    if(sret_ != SQL_SUCCESS && sret_ != SQL_SUCCESS_WITH_INFO) {  
        E_THD_LOG(egwLogName,"SQLExecute fail [%d] [%s]",
            sret_, PDB::ODBCErr::GetStringSTMT(_stmt).c_str());
        return false;
    }

    if(sret_ == SQL_SUCCESS_WITH_INFO) {
        W_THD_LOG(egwLogName,"SQLExecute [%d] [%s]",
            sret_, PDB::ODBCErr::GetStringSTMT(_stmt).c_str());
    }

    sret_ = SQLFetch(_stmt);

    if(sret_ == SQL_NO_DATA)
        return true;

    // 무조건 한개 이기 때문에, loop 를 돌 필요가 없어요.

    if(sret_ != SQL_SUCCESS && sret_ != SQL_SUCCESS_WITH_INFO) {  
        E_THD_LOG(egwLogName,"SQLFetch fail [%d] [%s]",
            sret_, PDB::ODBCErr::GetStringSTMT(_stmt).c_str());
        return false;
    }

    if(sret_ == SQL_SUCCESS_WITH_INFO) {
        W_THD_LOG(egwLogName,"SQLFetch [%d] [%s]",
            sret_, PDB::ODBCErr::GetStringSTMT(_stmt).c_str());
    }

    return true;
}
