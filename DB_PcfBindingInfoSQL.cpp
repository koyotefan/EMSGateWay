
#include "DB_PcfBindingInfoSQL.hpp"
#include "PDBOdbcErr.hpp"

Select_PcfBindingInfoSQL::Select_PcfBindingInfoSQL() {
    arrInd_ = new (std::nothrow) SQLLEN[Select_PcfBindingInfoSQL::parameterCnt];
    arrBp_  = new (std::nothrow) STBindParameter[Select_PcfBindingInfoSQL::parameterCnt];

    arrLen_= new (std::nothrow) SQLLEN[Select_PcfBindingInfoSQL::columnCnt];
    arrBc_ = new (std::nothrow) STBindColumn[Select_PcfBindingInfoSQL::columnCnt];
}

Select_PcfBindingInfoSQL::~Select_PcfBindingInfoSQL() {

    if(arrInd_) {
        delete [] arrInd_;
        arrInd_ = nullptr;
    }

    if(arrBp_) {
        delete [] arrBp_;
        arrBp_ = nullptr;
    }

    if(arrLen_) {
        delete [] arrLen_;
        arrLen_ = nullptr;
    }

    if(arrBc_) {
        delete [] arrBc_;
        arrBc_ = nullptr;
    }    
}

bool Select_PcfBindingInfoSQL::Bind() {

    char buf[256];
    snprintf(buf,
            sizeof(buf),
            "SELECT count(*) "
            "FROM T_PCF_BINDING_INFO WHERE "
            " SNSSAI_SST=? AND SNSSAI_SD=? AND DNN=? AND MCC_MNC=?");

    if(SetSQL(PDB::eDefDBType::PCF, buf, strlen(buf)) == false) {
        E_THD_LOG(egwLogName, "SetSQL() fail [%s]", buf);
        return false;
    }

    for(int nLoop=0; nLoop < Select_PcfBindingInfoSQL::parameterCnt; ++nLoop)
        arrInd_[nLoop] = SQL_NTS;

    arrBp_[0] = {1, SQL_C_LONG, SQL_INTEGER, 0,                 0, &sst_, 0,           &arrInd_[0] };
    arrBp_[1] = {2, SQL_C_CHAR, SQL_CHAR,    DB_S_NSSAI_SD_LEN, 0, sd_,   sizeof(sd_), &arrInd_[1] };
    arrBp_[2] = {3, SQL_C_CHAR, SQL_CHAR,    DB_DNN_LEN,        0, dnn_,  sizeof(dnn_),&arrInd_[2] };
    arrBp_[3] = {4, SQL_C_CHAR, SQL_CHAR,    DB_PCF_MCC_MNC_LEN,0, mccmnc_,  sizeof(mccmnc_),&arrInd_[3] };


    SetBindParameter(arrBp_, Select_PcfBindingInfoSQL::parameterCnt);

    arrBc_[0] = {1, SQL_C_SLONG, &cnt_, sizeof(cnt_), &arrLen_[0] };

    SetBindColumn(arrBc_, Select_PcfBindingInfoSQL::columnCnt);

    return true;
}

bool Select_PcfBindingInfoSQL::Execute(SQLHSTMT & _stmt) {

    cnt_ = 0;

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

void Select_PcfBindingInfoSQL::SetValue(stNsmfSessionInfoRecord & _record) {

    sst_ = _record.sst;

    memset(sd_, 0, sizeof(sd_));
    strncpy(sd_, _record.sd.c_str(), _record.sd.length());

    memset(dnn_, 0, sizeof(dnn_));
    strncpy(dnn_, _record.dnn.c_str(), _record.dnn.length());

    snprintf(mccmnc_,
             sizeof(mccmnc_),
             "%s%s",
             _record.mcc.c_str(),
             _record.mnc.c_str());
}
