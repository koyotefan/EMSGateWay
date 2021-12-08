#ifndef __SliceDnnWorkSQL_HPP__
#define __SliceDnnWorkSQL_HPP__

#include "TBLRecord.hpp"
#include "PDBPreparedSQL.hpp"

class Update_SliceDnnWorkSQL : public PDB::PreparedSQL {
public:
    explicit Update_SliceDnnWorkSQL() = default;
    virtual ~Update_SliceDnnWorkSQL() = default;

    bool Bind();
    bool Execute(SQLHSTMT & _stmt);

private:

    int allowedMasterWorkingPeriod_ = DEFAULT_UPDATE_CNT_TBL_PERIOD_SEC;

};

class Select_SliceDnnWorkSQL : public PDB::PreparedSQL {
public:
    explicit Select_SliceDnnWorkSQL();
    virtual ~Select_SliceDnnWorkSQL();

    bool Bind();
    bool Execute(SQLHSTMT & _stmt);
    bool IsMaster() { return nMaster_ > 0; }

private:

    SQLLEN *        arrLen_;
    STBindColumn *  arrBc_;

    static const int memberCnt = 1; // 아래 1개의 개수를 뜻 합니다
    int              nMaster_  = 0;  // 1 이상이면 MASTER 입니다.
};

#endif