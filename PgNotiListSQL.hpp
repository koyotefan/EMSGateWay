#ifndef PG_NOTI_LIST_SQL_HPP  
#define PG_NOTI_LIST_SQL_HPP

#include "PDBPreparedSQL.hpp"

#define LEN_MDN     16
#define LEN_TID     16
#define LEN_SYS     16

extern thread_local char egwLogName[32];

struct stPgNotiList {
    char    mdn[LEN_MDN+1] = {0,};
    char    tid[LEN_TID+1] = {0,};
    char    sys[LEN_SYS+1] = {0,};
    int     memberCnt = 3;

    stPgNotiList & operator=(const stPgNotiList & _st) {
        if(this != &_st) {
            memcpy(this, &_st, sizeof(_st));
        }

        return *this;
    }
};


class InsertPgNotiListSQL : public PDB::PreparedSQL {
public:
    explicit InsertPgNotiListSQL();
    ~InsertPgNotiListSQL();

    bool Bind();
    void SetValue(const stPgNotiList & _item);
    bool Execute(SQLHSTMT & _stmt);

private:
    stPgNotiList      item_;

    SQLLEN *          arrLen_;
    STBindParameter * arrBp_;
};

#endif // PG_NOTI_LIST_SQL_HPP
