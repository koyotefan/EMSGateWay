#ifndef __DB_PcfBindingInfoSQL_HPP__
#define __DB_PcfBindingInfoSQL_HPP__

#include "PDBPreparedSQL.hpp"
#include "TBLRecord.hpp"

class Select_PcfBindingInfoSQL : public PDB::PreparedSQL
{
public:
    explicit Select_PcfBindingInfoSQL();
    ~Select_PcfBindingInfoSQL();

    bool Bind();
    bool Execute(SQLHSTMT & _stmt);
    void SetValue(stNsmfSessionInfoRecord & _record);
    int  GetSessionCount() { return cnt_; }

private:

    // 조건
    static const int    parameterCnt = 4;
    SQLLEN *            arrInd_ = nullptr;
    STBindParameter *   arrBp_  = nullptr; 

    int     sst_ = 0;
    char    sd_[DB_S_NSSAI_SD_LEN+1]    = {'\0',};
    char    dnn_[DB_DNN_LEN+1]          = {'\0',};
    char    mccmnc_[DB_PCF_MCC_MNC_LEN+1]  = {'\0',};

    // 결과
    static const int    columnCnt = 1; 
    SQLLEN *            arrLen_ = nullptr;
    STBindColumn *      arrBc_  = nullptr; 

    int     cnt_ = 0;

};

#endif
