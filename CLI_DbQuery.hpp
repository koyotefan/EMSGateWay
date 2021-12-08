#pragma once

#include "CNDFODBCAdaptor.hpp"
#include "CODBClib.hpp"

#include "define.hpp"
#include "BaseDefine.hpp"
#include "PDBPreparedSQL.hpp"

class CSessionInfo : public PDB::PreparedSQL, public CODBClib
{
private:
    char m_szMDN[LEN_MDN + 1];
    ST_SMF_SESSION_INFO m_stSmfSessionInfo;
    vector<ST_SMF_SESSION_INFO> vecSmfSessionInfo;
public:
    CSessionInfo();
    ~CSessionInfo();
    bool Initialize();
    bool SetValue(char *a_pszMDN);
    vector<ST_SMF_SESSION_INFO> *GetValue() { return &vecSmfSessionInfo; }
    bool Prepare();
    bool SetParameter();
    bool SetColumn();
    bool SetBindParameter();
    bool SetBindColumn();
    bool Execute(SQLHSTMT &a_sqlHStmt);
};


class CSessionCount : public PDB::PreparedSQL, public CODBClib
{
private:
    char m_szColumn[128+1];
    ST_CLI_SESS_CNT m_stSessCnt;
    vector<ST_CLI_SESS_CNT> vecSessCnt;

public:
    CSessionCount();
    ~CSessionCount();
    bool Initialize();
    bool SetValue(char *a_pszColumn);
    vector<ST_CLI_SESS_CNT> *GetValue() { return &vecSessCnt; }
    bool Prepare();
    bool SetParameter();
    bool SetColumn();
    bool SetBindParameter();
    bool SetBindColumn();
    bool Execute(SQLHSTMT &a_sqlHStmt);
};

class CSubscriberCount : public PDB::PreparedSQL, public CODBClib
{
private:
    char m_szServiceID[LEN_SVC_ID+1];
    ST_CLI_SUBS_CNT m_stSubsCnt;
    vector<ST_CLI_SUBS_CNT> vecSubsCnt;

public:
    CSubscriberCount();
    ~CSubscriberCount();
    bool Initialize();
    bool SetValue(char *a_pszSvcId);
    vector<ST_CLI_SUBS_CNT> *GetValue() { return &vecSubsCnt; }
    bool Prepare();
    bool SetParameter();
    bool SetColumn();
    bool SetBindParameter();
    bool SetBindColumn();
    bool Execute(SQLHSTMT &a_sqlHStmt);
};

class CSubscriberProfile : public PDB::PreparedSQL, public CODBClib
{
private:
    char m_szMDN[LEN_MDN + 1];
    ST_CLI_SUBS_PROFILE      m_stSubsProfile;

public:
    CSubscriberProfile();
    ~CSubscriberProfile();
    bool Initialize();
    bool SetValue(char *a_pszMDN);
    ST_CLI_SUBS_PROFILE*GetValue() { return &m_stSubsProfile; }
    bool Prepare();
    bool SetParameter();
    bool SetColumn();
    bool SetBindParameter();
    bool SetBindColumn();
    bool Execute(SQLHSTMT &a_sqlHStmt);
};

class CSubscriberService : public PDB::PreparedSQL, public CODBClib
{
private:
    char m_szMDN[LEN_MDN + 1];
    ST_CLI_SUBS_SERVICE m_stSubsService;
    vector<ST_CLI_SUBS_SERVICE> vecSubsService;

public:
    CSubscriberService();
    ~CSubscriberService();
    bool Initialize();
    bool SetValue(char *a_pszMDN);
    vector<ST_CLI_SUBS_SERVICE> *GetValue() { return &vecSubsService; }
    bool Prepare();
    bool SetParameter();
    bool SetColumn();
    bool SetBindParameter();
    bool SetBindColumn();
    bool Execute(SQLHSTMT &a_sqlHStmt);
};

class CZoneInfo : public PDB::PreparedSQL, public CODBClib
{
private:
    char m_szServiceID[LEN_SVC_ID + 1];

    ST_5G_ZONE_INFO m_stZoneInfo;
    vector<ST_5G_ZONE_INFO> vecZoneInfo;

public:
    CZoneInfo();
    ~CZoneInfo();
    bool Initialize();
    // bool SetValue(char *a_pszSvcId);
    vector<ST_5G_ZONE_INFO> *GetValue() { return &vecZoneInfo; }
    bool Prepare();
    // bool SetParameter();
    bool SetColumn();
    // bool SetBindParameter();
    bool SetBindColumn();
    bool Execute(SQLHSTMT &a_sqlHStmt);
};
