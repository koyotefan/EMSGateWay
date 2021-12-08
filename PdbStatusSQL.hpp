#ifndef PDB_STATUS_SQL_HPP
#define PDB_STATUS_SQL_HPP

#include <vector>
#include "PDBPreparedSQL.hpp"

#define LEN_NODE_ID     64
#define LEN_PROC_ID     64
#define LEN_PDB_TYPE    32
#define LEN_DB_NAME     64
#define LEN_DB_IP       64

extern thread_local char egwLogName[32];

struct stPdbStatusItem {
    char     nodeId[LEN_NODE_ID+1] = {0,};
    char     procName[LEN_PROC_ID+1] = {0,};
    char     pdbType[LEN_PDB_TYPE+1] = {0,};
    int      shardingId = 0;
    char     dbName[LEN_DB_NAME+1] = {0,};
    char     dbIp[LEN_DB_IP+1] = {0,};
    int      dbPort  = 0;
    int      actCnt  = 0;
    int      connCnt = 0;
    int      memberCnt = 9;

    stPdbStatusItem & operator=(const stPdbStatusItem & _st) {
        if(this != &_st) {
            memcpy(this, &_st, sizeof(_st));
        }

        return *this;
    }

    bool IsEqual(const stPdbStatusItem & _st) {
        if(this == &_st)
            return true;

        if(actCnt != _st.actCnt)
            return false;

        if(connCnt != _st.connCnt)
            return false;

        if(strlen(nodeId) != strlen(_st.nodeId) || memcmp(nodeId, _st.nodeId, strlen(nodeId)) != 0)
            return false;

        if(strlen(procName) != strlen(_st.procName) || memcmp(procName, _st.procName, strlen(procName)) != 0)
            return false;

        if(strlen(pdbType) != strlen(_st.pdbType) || memcmp(pdbType, _st.pdbType, strlen(pdbType)) != 0)
            return false;

        if(shardingId != _st.shardingId)
            return false;

        if(strlen(dbName) != strlen(_st.dbName) || memcmp(dbName, _st.dbName, strlen(dbName)) != 0)
            return false;

        if(strlen(dbIp) != strlen(_st.dbIp) || memcmp(dbIp, _st.dbIp, strlen(dbIp)) != 0)
            return false;

        if(dbPort != _st.dbPort)
            return false;

        return true;
    }
};

class InsertPdbStatusSQL : public PDB::PreparedSQL {
public:
    explicit InsertPdbStatusSQL();
    ~InsertPdbStatusSQL();

    bool Bind();
    void SetValue(const stPdbStatusItem & _item);

    bool Execute(SQLHSTMT & _stmt);

private:
    stPdbStatusItem   item_;

    SQLLEN *          arrLen_;
    STBindParameter * arrBp_;
};


class DeleteNodePdbStatusSQL : public PDB::PreparedSQL {
public:
    explicit DeleteNodePdbStatusSQL();
    ~DeleteNodePdbStatusSQL();

    bool Bind();
    void SetValue(const stPdbStatusItem & _item);
    bool Execute(SQLHSTMT & _stmt);

private:
    stPdbStatusItem   item_;

    SQLLEN *          arrLen_;
    STBindParameter * arrBp_;
};

class DeleteProcPdbStatusSQL : public PDB::PreparedSQL {
public:
    explicit DeleteProcPdbStatusSQL();
    ~DeleteProcPdbStatusSQL();

    bool Bind();
    void SetValue(const stPdbStatusItem & _item);
    bool Execute(SQLHSTMT & _stmt);

private:
    stPdbStatusItem   item_;

    SQLLEN *          arrLen_;
    STBindParameter * arrBp_;
};

class DeletePdbStatusSQL : public PDB::PreparedSQL {
public:
    explicit DeletePdbStatusSQL();
    ~DeletePdbStatusSQL();

    bool Bind();
    void SetValue(const stPdbStatusItem & _item);
    bool Execute(SQLHSTMT & _stmt);

private:
    stPdbStatusItem   item_;

    SQLLEN *          arrLen_;
    STBindParameter * arrBp_;
};

class SelectPdbStatusSQL : public PDB::PreparedSQL {
public:
    explicit SelectPdbStatusSQL();
    ~SelectPdbStatusSQL();

    bool Bind();
    void SetValue(const stPdbStatusItem & _item);
    void GetItem(stPdbStatusItem & _item) { _item = item_; }

    // NO DATA 도 true Return 하니까 조심하세요.
    bool Execute(SQLHSTMT & _hstmt);

private:
    stPdbStatusItem   item_;

    SQLLEN *          arrLen_;
    STBindParameter * arrBp_;

    SQLLEN *          arrInd_;
    STBindColumn *    arrBc_;
};

// 자동 절체를 위한 SELECT 를 만들어야 해요.

class UpdatePdbStatusSQL : public PDB::PreparedSQL {
public:
    explicit UpdatePdbStatusSQL();
    ~UpdatePdbStatusSQL();

    bool Bind();
    void SetValue(const stPdbStatusItem & _item);
    bool Execute(SQLHSTMT & _stmt);

private:
    stPdbStatusItem   item_;

    SQLLEN *          arrLen_;
    STBindParameter * arrBp_;
};

#define     LEN_DB_CLUSTER_NAME     4

struct stConnectCntByDBName {
    char          dbName[LEN_PDB_TYPE+1] = {0,};
    char          PS[LEN_DB_CLUSTER_NAME+1] = {0,};
    int           nACTIVE = 0;
    int           nCONN   = 0;

    stConnectCntByDBName & operator=(const stConnectCntByDBName & _st) {

        if(this != &_st) {
            memcpy(this, &_st, sizeof(_st));
        }

        return *this;
    }

};


class SelectPdbALLSQL : public PDB::PreparedSQL {
public:
    explicit SelectPdbALLSQL();
    ~SelectPdbALLSQL();

    bool Bind();
    std::vector<stConnectCntByDBName> & GetRows() { return vRows_; }

    // NO DATA 도 true Return 하니까 조심하세요.
    bool Execute(SQLHSTMT & _hstmt);

private:

    stConnectCntByDBName                st_;
    std::vector<stConnectCntByDBName>   vRows_;

    SQLLEN *          arrLen_;
    STBindParameter * arrBp_;

    SQLLEN *          arrInd_;
    STBindColumn *    arrBc_;
  
};



#endif // PDB_STATUS_SQL_HPP
