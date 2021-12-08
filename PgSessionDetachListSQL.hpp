#ifndef PG_SESSION_DETACH_LIST_SQL_HPP
#define PG_SESSION_DETACH_LIST_SQL_HPP

#include "PDBPreparedSQL.hpp"

#define     LEN_MDN     16
#define     LEN_DNN     64
#define     LEN_PCF_ID  16

extern thread_local char egwLogName[32];

struct stPgSessionDetachList {
    char    mdn[LEN_MDN+1] = {'\0',};
    char    dnn[LEN_DNN+1] = {'\0',};
    char    pcfId[LEN_PCF_ID+1] = {'\0',};
    int     memberCnt = 3;

    stPgSessionDetachList & operator=(const stPgSessionDetachList & _st) {
        if(this != &_st) {
            memcpy(this, &_st, sizeof(_st));
        }

        return *this;
    }

};

class InsertPgSessionDetachListSQL : public PDB::PreparedSQL {
public:
    explicit InsertPgSessionDetachListSQL();
    ~InsertPgSessionDetachListSQL();

    bool Bind();
    void SetValue(const stPgSessionDetachList & _item);
    bool Execute(SQLHSTMT & _stmt);

private:
    stPgSessionDetachList      item_;

    SQLLEN *          arrLen_;
    STBindParameter * arrBp_;
};

#endif // PG_SESSION_DETACH_LIST_SQL_HPP
