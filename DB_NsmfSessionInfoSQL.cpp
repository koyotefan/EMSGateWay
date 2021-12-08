#include "DB_NsmfSessionInfoSQL.hpp"
#include "PDBOdbcErr.hpp"

extern std::string  gSysName;

Select_RestrictionListSQL::Select_RestrictionListSQL() {
    arrLen_ = new (std::nothrow) SQLLEN[Select_RestrictionListSQL::memberCnt];
    arrBc_  = new (std::nothrow) STBindColumn[Select_RestrictionListSQL::memberCnt];
}

Select_RestrictionListSQL::~Select_RestrictionListSQL() {
    if(arrLen_) {
        delete [] arrLen_;
        arrLen_ = nullptr;
    }

    if(arrBc_) {
        delete [] arrBc_;
        arrBc_ = nullptr;
    }
}

bool Select_RestrictionListSQL::Bind() {

    char buf[128];
    snprintf(buf,
        sizeof(buf),
        "SELECT "
            "MCC,"
            "MNC,"
            "SST,"
            "SD,"
            "DNN"
        " FROM T_NSMF_SESSION_INFO "
        " WHERE RESTRICTION=1 AND MONITOR_USED=1");

    if(SetSQL(PDB::eDefDBType::PCF, buf, strlen(buf)) == false) {
        E_THD_LOG(egwLogName, "SetSQL() fail [%s]", buf);
        return false;
    }

    for(int nLoop=0; nLoop < Select_RestrictionListSQL::memberCnt; ++nLoop)
        arrLen_[nLoop] = SQL_NTS;

    arrBc_[0] = {1, SQL_C_CHAR, mcc_, sizeof(mcc_), &arrLen_[0] };
    arrBc_[1] = {2, SQL_C_CHAR, mnc_, sizeof(mnc_), &arrLen_[1] };
    arrBc_[2] = {3, SQL_C_CHAR, sst_, sizeof(sst_), &arrLen_[2] };
    arrBc_[3] = {4, SQL_C_CHAR, sd_,  sizeof(sd_),  &arrLen_[3] };
    arrBc_[4] = {5, SQL_C_CHAR, dnn_, sizeof(dnn_), &arrLen_[4] };

    SetBindColumn(arrBc_, Select_RestrictionListSQL::memberCnt);

    return true;
}

bool Select_RestrictionListSQL::Execute(SQLHSTMT & _stmt) {

    vecRecords_.clear();

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

    while(true) {
        sret_ = SQLFetch(_stmt);

        if(sret_ == SQL_NO_DATA)
            break;

        if(sret_ != SQL_SUCCESS && sret_ != SQL_SUCCESS_WITH_INFO) {
            E_THD_LOG(egwLogName,"SQLFetch fail [%d] [%s]",
                sret_, PDB::ODBCErr::GetStringSTMT(_stmt).c_str());
            return false;
        }

        if(sret_ == SQL_SUCCESS_WITH_INFO) {
            W_THD_LOG(egwLogName,"SQLFetch [%d] [%s]",
                sret_, PDB::ODBCErr::GetStringSTMT(_stmt).c_str());
        }

        stNsmfSessionInfoRecord st;

        st.mcc      =   mcc_;
        st.mnc      =   mnc_;
        st.sst      =   std::stoi(std::string(sst_));
        st.sd       =   sd_;
        st.dnn      =   dnn_;

        vecRecords_.emplace_back(st);
    }

    return true;
}

Select_SliceDnnListSQL::Select_SliceDnnListSQL() {
    arrLen_ = new (std::nothrow) SQLLEN[Select_SliceDnnListSQL::columnCnt];
    arrBc_  = new (std::nothrow) STBindColumn[Select_SliceDnnListSQL::columnCnt];
}

Select_SliceDnnListSQL::~Select_SliceDnnListSQL() {
    if(arrLen_) {
        delete [] arrLen_;
        arrLen_ = nullptr;
    }

    if(arrBc_) {
        delete [] arrBc_;
        arrBc_ = nullptr;
    }
}

bool Select_SliceDnnListSQL::Bind() {

    char buf[128];
    snprintf(buf,
        sizeof(buf),
        "SELECT "
            "MCC,"
            "MNC,"
            "SST,"
            "SD,"
            "DNN "
        " FROM T_NSMF_SESSION_INFO "
        " WHERE MONITOR_USED=1");

    if(SetSQL(PDB::eDefDBType::PCF, buf, strlen(buf)) == false) {
        E_THD_LOG(egwLogName, "SetSQL() fail [%s]", buf);
        return false;
    }

    for(int nLoop=0; nLoop < Select_SliceDnnListSQL::columnCnt; ++nLoop)
        arrLen_[nLoop] = SQL_NTS;

    arrBc_[0] = {1, SQL_C_CHAR, mcc_, sizeof(mcc_), &arrLen_[0] };
    arrBc_[1] = {2, SQL_C_CHAR, mnc_, sizeof(mnc_), &arrLen_[1] };
    arrBc_[2] = {3, SQL_C_CHAR, sst_, sizeof(sst_), &arrLen_[2] };
    arrBc_[3] = {4, SQL_C_CHAR, sd_,  sizeof(sd_),  &arrLen_[3] };
    arrBc_[4] = {5, SQL_C_CHAR, dnn_, sizeof(dnn_), &arrLen_[4] };

    SetBindColumn(arrBc_, Select_SliceDnnListSQL::columnCnt);

    return true;
}

bool Select_SliceDnnListSQL::Execute(SQLHSTMT & _stmt) {

    vecRecords_.clear();

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

    while(true) {
        sret_ = SQLFetch(_stmt);

        if(sret_ == SQL_NO_DATA)
            break;

        if(sret_ != SQL_SUCCESS && sret_ != SQL_SUCCESS_WITH_INFO) {
            E_THD_LOG(egwLogName,"SQLFetch fail [%d] [%s]",
                sret_, PDB::ODBCErr::GetStringSTMT(_stmt).c_str());
            return false;
        }

        if(sret_ == SQL_SUCCESS_WITH_INFO) {
            W_THD_LOG(egwLogName,"SQLFetch [%d] [%s]",
                sret_, PDB::ODBCErr::GetStringSTMT(_stmt).c_str());
        }

        stNsmfSessionInfoRecord st;
   
        st.mcc      =   mcc_;
        st.mnc      =   mnc_;
        st.sst      =   std::stoi(std::string(sst_));
        st.sd       =   sd_;
        st.dnn      =   dnn_;

        vecRecords_.emplace_back(st);
    }

    return true;
}

Update_NsmfSessionInfoSQL::Update_NsmfSessionInfoSQL() {
    arrInd_ = new (std::nothrow) SQLLEN[Update_NsmfSessionInfoSQL::parameterCnt];
    arrBp_  = new (std::nothrow) STBindParameter[Update_NsmfSessionInfoSQL::parameterCnt];
}



Update_NsmfSessionInfoSQL::~Update_NsmfSessionInfoSQL() {
    if(arrInd_) {
        delete [] arrInd_;
        arrInd_ = nullptr;
    }

    if(arrBp_) {
        delete [] arrBp_;
        arrBp_ = nullptr;
    }
}

bool Update_NsmfSessionInfoSQL::Bind(int _nowCnt) {

    // 매번 쿼리문이 바뀌고 있으니, Parepared 를 못합니다.
    PreparedOff();

    char buf[512];
    snprintf(buf,
        sizeof(buf),
        "UPDATE T_NSMF_SESSION_INFO SET CNT=%d, RESTRICTION= "
            "(SELECT CASE "
                "WHEN ALLOW <= %d THEN 1 "
                "WHEN RELEASE < %d AND RESTRICTION=1 THEN 1 "
               // "WHEN REJECT_REL_CNT <= %d AND REJECTING=0 THEN 0 "
                "ELSE 0 "
                "END "
            " FROM T_NSMF_SESSION_INFO WHERE MCC=? AND MNC=? AND SST=? AND SD=? AND DNN=?), "
            " UPDATE_TIME=SYSDATE "
        " WHERE MCC=? AND MNC=? AND SST=? AND SD=? AND DNN=?", 
         _nowCnt, 
         _nowCnt, 
         _nowCnt);

    if(SetSQL(PDB::eDefDBType::PCF, buf, strlen(buf)) == false) {
        E_THD_LOG(egwLogName, "SetSQL() fail [%s]", buf);
        return false;
    }

    for(int nLoop=0; nLoop < Update_NsmfSessionInfoSQL::parameterCnt; ++nLoop)
        arrInd_[nLoop] = SQL_NTS;

    arrBp_[0] = {1, SQL_C_CHAR, SQL_CHAR, DB_NSMF_MCC_LEN,   0, mcc_, sizeof(mcc_), &arrInd_[0] };
    arrBp_[1] = {2, SQL_C_CHAR, SQL_CHAR, DB_NSMF_MNC_LEN,   0, mnc_, sizeof(mnc_), &arrInd_[1] };
    arrBp_[2] = {3, SQL_C_CHAR, SQL_CHAR, DB_S_NSSAI_SST_LEN,0, sst_, sizeof(sst_), &arrInd_[2] };
    arrBp_[3] = {4, SQL_C_CHAR, SQL_CHAR, DB_S_NSSAI_SD_LEN, 0, sd_,  sizeof(sd_),  &arrInd_[3] };
    arrBp_[4] = {5, SQL_C_CHAR, SQL_CHAR, DB_NSMF_DNN_LEN,   0, dnn_, sizeof(dnn_), &arrInd_[4] };
    arrBp_[5] = {6, SQL_C_CHAR, SQL_CHAR, DB_NSMF_MCC_LEN,   0, mcc_, sizeof(mcc_), &arrInd_[5] };
    arrBp_[6] = {7, SQL_C_CHAR, SQL_CHAR, DB_NSMF_MNC_LEN,   0, mnc_, sizeof(mnc_), &arrInd_[6] };
    arrBp_[7] = {8, SQL_C_CHAR, SQL_CHAR, DB_S_NSSAI_SST_LEN,0, sst_, sizeof(sst_), &arrInd_[7] };
    arrBp_[8] = {9, SQL_C_CHAR, SQL_CHAR, DB_S_NSSAI_SD_LEN, 0, sd_,  sizeof(sd_),  &arrInd_[8] };
    arrBp_[9] = {10,SQL_C_CHAR, SQL_CHAR, DB_NSMF_DNN_LEN,   0, dnn_, sizeof(dnn_), &arrInd_[9] };

    SetBindParameter(arrBp_, Update_NsmfSessionInfoSQL::parameterCnt);

    return true;
}

/*-
bool Update_NsmfSessionInfoSQL::Bind() {
    char buf[1024];
    snprintf(buf,
        sizeof(buf),
        "UPDATE T_SLICE_DNN_CNT SET NOW_CNT=?, REJECTING= "
            "(SELECT CASE "
                "WHEN REJECT_SET_CNT <= ? THEN 1 "
                "WHEN REJECT_REL_CNT < ? THEN REJECTING"
                "ELSE 0 "
                "END "
            " FROM T_SLICE_DNN_CNT WHERE SST=? AND SD=? AND DNN=?), "
            " UPDATE_TIME=SYSDATE "
        " WHERE SST=? AND SD=? AND DNN=?");

    if(SetSQL(PDB::eDefDBType::PCF, buf, strlen(buf)) == false) {
        E_THD_LOG(egwLogName, "SetSQL() fail [%s]", buf);
        return false;
    }

    for(int nLoop=0; nLoop < Update_NsmfSessionInfoSQL::parameterCnt; ++nLoop)
        arrInd_[nLoop] = SQL_NTS;

    arrBp_[0] = {1, SQL_C_SLONG, SQL_INTEGER, 0,                 0, &nowCnt_,    0,            &arrInd_[0] };
    arrBp_[1] = {2, SQL_C_SLONG, SQL_INTEGER, 0,                 0, &nowCnt_,    0,            &arrInd_[1] };
    arrBp_[2] = {3, SQL_C_SLONG, SQL_INTEGER, 0,                 0, &nowCnt_,    0,            &arrInd_[2] };
    arrBp_[3] = {4, SQL_C_SLONG, SQL_INTEGER, 0,                 0, &sst_,       0,            &arrInd_[3] };
    arrBp_[4] = {5, SQL_C_CHAR,  SQL_CHAR,    DB_S_NSSAI_SD_LEN, 0, sd_,         sizeof(sd_),  &arrInd_[4] };
    arrBp_[5] = {6, SQL_C_CHAR,  SQL_CHAR,    DB_DNN_LEN,        0, dnn_,        sizeof(dnn_), &arrInd_[5] };
    arrBp_[6] = {7, SQL_C_SLONG, SQL_INTEGER, 0,                 0, &sst_,       0,            &arrInd_[6] };
    arrBp_[7] = {8, SQL_C_CHAR,  SQL_CHAR,    DB_S_NSSAI_SD_LEN, 0, sd_,         sizeof(sd_),  &arrInd_[7] };
    arrBp_[8] = {9, SQL_C_CHAR,  SQL_CHAR,    DB_DNN_LEN,        0, dnn_,        sizeof(dnn_), &arrInd_[8] };

    SetBindParameter(arrBp_, Update_NsmfSessionInfoSQL::parameterCnt);

    return true;
}
-*/

bool Update_NsmfSessionInfoSQL::Execute(SQLHSTMT & _stmt) {

    sret_ = SQLExecDirect(_stmt, (SQLCHAR *)GetSQL(), (SQLINTEGER)GetSQLSize());

    // sret_ = SQLExecute(_stmt);

    if(sret_ == SQL_NO_DATA) {
        I_THD_LOG(egwLogName,"Update No DATA");
        return true;
    }

    if(sret_ != SQL_SUCCESS && sret_ != SQL_SUCCESS_WITH_INFO) {  
        E_THD_LOG(egwLogName,"SQLExecute fail [%d] [%s]",
            sret_, PDB::ODBCErr::GetStringSTMT(_stmt).c_str());
        return false;
    }

    if(sret_ == SQL_SUCCESS_WITH_INFO) {
        W_THD_LOG(egwLogName,"SQLExecute [%d] [%s]",
            sret_, PDB::ODBCErr::GetStringSTMT(_stmt).c_str());
    }

    D_THD_LOG(egwLogName,"update success [%s:%s:%s] [%s:%s] [%d]", sst_,sd_,dnn_, mcc_, mnc_, nowCnt_);

    return true;
}

void Update_NsmfSessionInfoSQL::SetValue(const stNsmfSessionInfoRecord & _record) {

    nowCnt_ = _record.nowCnt;

    memset(mcc_, 0, sizeof(mcc_));
    strncpy(mcc_, _record.mcc.c_str(), _record.mcc.length());

    memset(mnc_, 0, sizeof(mnc_));
    strncpy(mnc_, _record.mnc.c_str(), _record.mnc.length());

    memset(sst_, 0, sizeof(sst_));
    std::string     temp(std::to_string(_record.sst));
    strncpy(sst_, temp.c_str(), temp.length());

    memset(sd_, 0, sizeof(sd_));
    strncpy(sd_, _record.sd.c_str(), _record.sd.length());

    memset(dnn_, 0, sizeof(dnn_));
    strncpy(dnn_, _record.dnn.c_str(), _record.dnn.length());
}
