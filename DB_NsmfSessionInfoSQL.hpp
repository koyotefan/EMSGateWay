#ifndef __DB_NsmfSessionInfoSQL_HPP__
#define __DB_NsmfSessionInfoSQL_HPP__

#include <vector>
#include "TBLRecord.hpp"
#include "PDBPreparedSQL.hpp"

class Select_RestrictionListSQL: public PDB::PreparedSQL
{
public:
    explicit Select_RestrictionListSQL();
    virtual ~Select_RestrictionListSQL();

    bool Bind();
    bool Execute(SQLHSTMT & _stmt);
    const std::vector<stNsmfSessionInfoRecord> & GetRows() { return vecRecords_; }

private:

    SQLLEN *        arrLen_;
    STBindColumn *  arrBc_;

    static const int memberCnt = 5; // 아래 3개의 개수를 뜻 합니다.

    char    mcc_[DB_NSMF_MCC_LEN+1] = {'\0',};
    char    mnc_[DB_NSMF_MNC_LEN+1] = {'\0',};
    char    sst_[DB_S_NSSAI_SST_LEN+1] = {'\0',};
    char    sd_[DB_S_NSSAI_SD_LEN +1] = {'\0',};
    char    dnn_[DB_NSMF_DNN_LEN+1] = {'\0', };

    std::vector<stNsmfSessionInfoRecord>    vecRecords_;

};

class Select_SliceDnnListSQL : public PDB::PreparedSQL 
{
public:
    explicit Select_SliceDnnListSQL();
    virtual ~Select_SliceDnnListSQL();

    bool Bind();
    bool Execute(SQLHSTMT & _stmt);
    std::vector<stNsmfSessionInfoRecord> & GetRows() { return vecRecords_; }

private:

    SQLLEN *        arrLen_;
    STBindColumn *  arrBc_;

    static const int    columnCnt = 5;

    char    mcc_[DB_NSMF_MCC_LEN+1] = {'\0',};
    char    mnc_[DB_NSMF_MNC_LEN+1] = {'\0',};
    char    sst_[DB_S_NSSAI_SST_LEN+1] = {'\0',};
    char    sd_[DB_S_NSSAI_SD_LEN +1] = {'\0',};
    char    dnn_[DB_NSMF_DNN_LEN+1] = {'\0', };

    std::vector<stNsmfSessionInfoRecord>    vecRecords_;
};

class Update_NsmfSessionInfoSQL : public PDB::PreparedSQL
{
public:
    explicit Update_NsmfSessionInfoSQL();
    virtual ~Update_NsmfSessionInfoSQL();

    bool Bind(int _nowCnt);
    bool Execute(SQLHSTMT & _stmt);
    void SetValue(const stNsmfSessionInfoRecord & _record);

private:
    SQLLEN *                arrInd_ = nullptr;
    STBindParameter *       arrBp_  = nullptr;

    static const int parameterCnt = 10;

    int     nowCnt_ = 0;
    char    mcc_[DB_NSMF_MCC_LEN+1] = {'\0',};
    char    mnc_[DB_NSMF_MNC_LEN+1] = {'\0',};
    char    sst_[DB_S_NSSAI_SST_LEN+1] = {'\0',};
    char    sd_[DB_S_NSSAI_SD_LEN +1] = {'\0',};
    char    dnn_[DB_NSMF_DNN_LEN+1] = {'\0', };

};


#endif
