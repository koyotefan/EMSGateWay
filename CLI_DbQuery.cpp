#include "CLI_DbQuery.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////
CSessionInfo::CSessionInfo()
{
    className_ = typeid(*this).name();
    Initialize();
}

CSessionInfo::~CSessionInfo()
{
}

bool CSessionInfo::Initialize()
{
    bool bRtn = true;

    try
    {
        Prepare();
        SetParameter();
        SetColumn();
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }

    return bRtn;
}

bool CSessionInfo::Prepare()
{
    bool bRtn = true;
    try
    {
        char *ptrQuery = (char *)"SELECT "
                                 "SM_POLICY_ID                                                        " //1 
                                 ",SUPI                                                               " //2 
                                 ",PDU_SESSION_ID                                                     " //3 
                                 ",GPSI                                                               " //4 
                                 ",MDN                                                                " //5 
                                 ",IP_ADDR                                                            " //6 
                                 ",DNN                                                                " //7 
                                 ",S_NSSAI_SST                                                        " //8 
                                 ",S_NSSAI_SD                                                         " //9 
                                 ",LOC_ID                                                             " //10
                                 ",MCC_MNC                                                            " //11
                                 ",RAT_TYPE                                                           " //12
                                 ",OUT_HTTP_PAYLOAD                                                   " //13
                                 ",NODE_ID                                                            " //14
                                 ",PROC_ID                                                            " //15
                                 ",SMF_ID                                                             " //16
                                 ",(CREATE_TIME - to_date('01-jan-1970','dd-mon-yyyy')) * (24*60*60)  " //17
                                 ",(UPDATE_TIME - to_date('01-jan-1970','dd-mon-yyyy')) * (24*60*60)  " //18
                                 "FROM    T_SMF_SESSION_INFO "
                                 "WHERE   MDN = ?            "
                                 "ORDER BY CREATE_TIME DESC  ";

        bRtn = SetSQL(PDB::eDefDBType::SessionDetail, ptrQuery, strlen(ptrQuery));
        if (bRtn != true)
        {
            THROW(1, "SqlSQL Error");
        }
        THD_LOG(NDF_LOG_T_INFO, "__CALL__");
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

bool CSessionInfo::SetParameter()
{
    bool bRtn = true;

    try
    {
        const int nParamCnt = 1;
        m_nParamCnt = nParamCnt;
        m_indParam = new SQLLEN[nParamCnt];
        for (int i = 0; i < nParamCnt; i++)
        {
            m_indParam[i] = SQL_NTS;
        }
        m_pstBindParam = new STBindParameter[nParamCnt]{
            {1, SQL_C_CHAR, SQL_CHAR, LEN_MDN, 0, m_szMDN, LEN_MDN + 1, &m_indParam[0]},
        };
        SetBindParameter();
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

bool CSessionInfo::SetColumn()
{
    bool bRtn = true;

    try
    {
        const int nColumnCnt = 18;
        m_nColumnCnt = nColumnCnt;
        m_indColumn = new SQLLEN[nColumnCnt];
        for (int i = 0; i < nColumnCnt; i++)
        {
            m_indColumn[i] = SQL_NTS;
        }
        m_pstBindColumn = new STBindColumn[nColumnCnt]{
            {1, SQL_C_CHAR,   m_stSmfSessionInfo.szSMPolicyID,     LEN_SM_POLICY_ID + 1, &m_indColumn[0]},
            {2, SQL_C_CHAR,   m_stSmfSessionInfo.szSUPI,           LEN_SUPI + 1,         &m_indColumn[1]},
            {3, SQL_C_SLONG, &m_stSmfSessionInfo.nPDUSessionID,    0,                    &m_indColumn[2]},
            {4, SQL_C_CHAR,   m_stSmfSessionInfo.szGPSI,           LEN_GPSI + 1,         &m_indColumn[3]},
            {5, SQL_C_CHAR,   m_stSmfSessionInfo.szMDN,            LEN_MDN + 1,          &m_indColumn[4]},
            {6, SQL_C_CHAR,   m_stSmfSessionInfo.szIpAddr,         LEN_IP_ADDR + 1,      &m_indColumn[5]},
            {7, SQL_C_CHAR,   m_stSmfSessionInfo.szDNN,            LEN_DNN + 1,          &m_indColumn[6]},
            {8, SQL_C_SLONG, &m_stSmfSessionInfo.nSNssaiSst,       0,                    &m_indColumn[7]},
            {9, SQL_C_CHAR,   m_stSmfSessionInfo.szSNssaiSd,       LEN_SNSSAI_SD + 1,    &m_indColumn[8]},
            {10, SQL_C_CHAR,  m_stSmfSessionInfo.szLocId,          LEN_LOC_ID   + 1,     &m_indColumn[9]},
            {11, SQL_C_CHAR,  m_stSmfSessionInfo.szMCCMNC,         LEN_MCC_MNC + 1,      &m_indColumn[10]},
            {12, SQL_C_CHAR,  m_stSmfSessionInfo.szRatType,        LEN_RATTYPE + 1,      &m_indColumn[11]},
            {13, SQL_C_CHAR,  m_stSmfSessionInfo.szOutHttpPayload, 896 + 1,              &m_indColumn[12]},
            {14, SQL_C_CHAR,  m_stSmfSessionInfo.szNodeID,         32 + 1,               &m_indColumn[13]},
            {15, SQL_C_CHAR,  m_stSmfSessionInfo.szProcID,         16 + 1,               &m_indColumn[14]},
            {16, SQL_C_CHAR,  m_stSmfSessionInfo.szSMFId,          LEN_SMF_ID + 1,       &m_indColumn[15]},
            {17, SQL_C_SLONG,&m_stSmfSessionInfo.tCreateTime,      0,                    &m_indColumn[16]},
            {18, SQL_C_SLONG,&m_stSmfSessionInfo.tUpdateTime,      0,                    &m_indColumn[17]},
        };
        SetBindColumn();
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

bool CSessionInfo::SetBindParameter()
{
    bool bRtn = true;

    try
    {
        PDB::PreparedSQL::SetBindParameter(m_pstBindParam, m_nParamCnt);
        THD_LOG(NDF_LOG_T_INFO, "__CALL__");
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

bool CSessionInfo::SetBindColumn()
{
    bool bRtn = true;

    try
    {
        PDB::PreparedSQL::SetBindColumn(m_pstBindColumn, m_nColumnCnt);
        THD_LOG(NDF_LOG_T_INFO, "__CALL__");
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

bool CSessionInfo::SetValue(char *a_pszMDN)
{
    bool bRtn = true;
    try
    {
        SNPRINTF(m_szMDN, "%s", a_pszMDN);
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

bool CSessionInfo::Execute(SQLHSTMT &a_sqlHStmt)
{
    bool bRtn = true;

    int nIdx = 0;
    vecSmfSessionInfo.clear();
    try
    {
        sret_ = SQLExecute(a_sqlHStmt);

        while (true)
        {
            sret_ = SQLFetch(a_sqlHStmt);
            if (!SQL_SUCCEEDED(sret_))
            {
                if (SQL_NO_DATA == sret_)
                {
                    if (nIdx == 0)
                    {
                        //THROW(1, "SQL_NO_DATA");
                    }
                    break;
                }
            }
            vecSmfSessionInfo.push_back(m_stSmfSessionInfo);
            nIdx++;
        }        
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            if (SQL_NO_DATA == sret_)
            {
                THD_LOG(NDF_LOG_T_INFO, "[%s]", e.what());
            }
            else
            {
                GetDiagRec(SQL_HANDLE_STMT, a_sqlHStmt, (char *)className_.c_str(), sret_, e.what());
            }
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }

    return bRtn;
}
//////////////////////////////////////////////////////////////////////////////////////////////
CSessionCount::CSessionCount()
{
    className_ = typeid(*this).name();
    Initialize();
}

CSessionCount::~CSessionCount()
{
}

bool CSessionCount::Initialize()
{
    bool bRtn = true;

    try
    {
        Prepare();
        SetParameter();
        SetColumn();
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }

    return bRtn;
}

bool CSessionCount::Prepare()
{
    bool bRtn = true;
    try
    {
        char *ptrQuery = (char *)"SELECT                                "
                                 "cast(? as varchar(128)) COL                            " //1 
                                 ",COUNT(*)                                                " //2 
                                 "FROM    T_PCF_BINDING_INFO                    "
                                 "GROUP BY cast(? as varchar(128))                 ";
        
        bRtn = SetSQL(PDB::eDefDBType::PCF, ptrQuery, strlen(ptrQuery));
        if (bRtn != true)
        {
            THROW(1, "SqlSQL Error");
        }
        THD_LOG(NDF_LOG_T_INFO, "__CALL__");
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

bool CSessionCount::SetParameter()
{
    bool bRtn = true;

    try
    {
        const int nParamCnt = 2;
        m_nParamCnt = nParamCnt;
        m_indParam = new SQLLEN[nParamCnt];
        for (int i = 0; i < nParamCnt; i++)
        {
            m_indParam[i] = SQL_NTS;
        }
        m_pstBindParam = new STBindParameter[nParamCnt]{
            {1, SQL_C_CHAR, SQL_CHAR, 128, 0, m_szColumn, 128 + 1, &m_indParam[0]},
            {2, SQL_C_CHAR, SQL_CHAR, 128, 0, m_szColumn, 128 + 1, &m_indParam[1]},
        };
        SetBindParameter();
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

bool CSessionCount::SetColumn()
{
    bool bRtn = true;

    try
    {
        const int nColumnCnt = 2;
        m_nColumnCnt = nColumnCnt;
        m_indColumn = new SQLLEN[nColumnCnt];
        for (int i = 0; i < nColumnCnt; i++)
        {
            m_indColumn[i] = SQL_NTS;
        }
        m_pstBindColumn = new STBindColumn[nColumnCnt]{
            {1, SQL_C_CHAR,  m_stSessCnt.VALUE,   256 + 1, &m_indColumn[0]},
            {2, SQL_C_SLONG,&m_stSessCnt.COUNT,   0,       &m_indColumn[1]},

        };
        SetBindColumn();
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

bool CSessionCount::SetBindParameter()
{
    bool bRtn = true;

    try
    {
        PDB::PreparedSQL::SetBindParameter(m_pstBindParam, m_nParamCnt);
        THD_LOG(NDF_LOG_T_INFO, "__CALL__");
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

bool CSessionCount::SetBindColumn()
{
    bool bRtn = true;

    try
    {
        PDB::PreparedSQL::SetBindColumn(m_pstBindColumn, m_nColumnCnt);
        THD_LOG(NDF_LOG_T_INFO, "__CALL__");
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

bool CSessionCount::SetValue(char *a_pszColumn)
{
    bool bRtn = true;
    try
    {
        SNPRINTF(m_szColumn, "%s", a_pszColumn);
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

bool CSessionCount::Execute(SQLHSTMT &a_sqlHStmt)
{
    bool bRtn = true;

    int nIdx = 0;
    vecSessCnt.clear();
    try
    {
        sret_ = SQLExecute(a_sqlHStmt);

        while (true)
        {
            sret_ = SQLFetch(a_sqlHStmt);
            if (!SQL_SUCCEEDED(sret_))
            {
                if (SQL_NO_DATA == sret_)
                {
                    if (nIdx == 0)
                    {
                        //THROW(1, "SQL_NO_DATA");
                    }
                    break;
                }
            }
            vecSessCnt.push_back(m_stSessCnt);
            nIdx++;
        }
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            if (SQL_NO_DATA == sret_)
            {
                THD_LOG(NDF_LOG_T_INFO, "[%s]", e.what());
            }
            else
            {
                GetDiagRec(SQL_HANDLE_STMT, a_sqlHStmt, (char *)className_.c_str(), sret_, e.what());
            }
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }    
    return bRtn;
}

//////////////////////////////////////////////////////////////////////////////////////////////
CSubscriberCount::CSubscriberCount()
{
    className_ = typeid(*this).name();
    Initialize();
}

CSubscriberCount::~CSubscriberCount()
{
}

bool CSubscriberCount::Initialize()
{
    bool bRtn = true;

    try
    {
        Prepare();
        SetParameter();
        SetColumn();
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }

    return bRtn;
}

bool CSubscriberCount::Prepare()
{
    bool bRtn = true;
    try
    {
        char *ptrQuery = (char *)   "SELECT LIMIT as VALUE, VALID as ISVALID, COUNT(1) as COUNT  "
                                    "FROM "
                                    "   (SELECT LIMIT "
                                    "          ,CASE WHEN LIMIT_VALID_TIME < TO_CHAR(SYSDATE, 'YYYYMMDDHH24MISS') "
                                    "             THEN 'VALID' "
                                    "             ELSE 'NOT VALID' "
                                    "           END AS VALID "
                                    "      FROM  T_5G_SUBS_SERVICE "
                                    "      WHERE  SVC_ID = ? "
                                    "      AND ACTIVE_TIME <= to_char(SYSDATE,'YYYYMMDDHH24MISS') "
                                    "      AND EXPIRE_TIME > to_char(SYSDATE,'YYYYMMDDHH24MISS')) "
                                    "GROUP BY LIMIT,VALID "
                                    "ORDER BY 2 desc ,1 asc ";

        bRtn = SetSQL(PDB::eDefDBType::PCF, ptrQuery, strlen(ptrQuery));
        if (bRtn != true)
        {
            THROW(1, "SqlSQL Error");
        }
        THD_LOG(NDF_LOG_T_INFO, "__CALL__");
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

bool CSubscriberCount::SetParameter()
{
    bool bRtn = true;

    try
    {
        const int nParamCnt = 1;
        m_nParamCnt = nParamCnt;
        m_indParam = new SQLLEN[nParamCnt];
        for (int i = 0; i < nParamCnt; i++)
        {
            m_indParam[i] = SQL_NTS;
        }
        m_pstBindParam = new STBindParameter[nParamCnt]{
            {1, SQL_C_CHAR, SQL_CHAR, LEN_SVC_ID, 0, m_szServiceID, LEN_SVC_ID + 1, &m_indParam[0]},
        };
        SetBindParameter();
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

bool CSubscriberCount::SetColumn()
{
    bool bRtn = true;

    try
    {
        const int nColumnCnt = 3;
        m_nColumnCnt = nColumnCnt;
        m_indColumn = new SQLLEN[nColumnCnt];
        for (int i = 0; i < nColumnCnt; i++)
        {
            m_indColumn[i] = SQL_NTS;
        }
        m_pstBindColumn = new STBindColumn[nColumnCnt]{
            {1, SQL_C_CHAR,  m_stSubsCnt.VALUE,   LEN_SVC_ID + 1, &m_indColumn[0]},
            {2, SQL_C_CHAR,  m_stSubsCnt.ISVALID, 20 + 1,         &m_indColumn[1]},
            {3, SQL_C_SLONG,&m_stSubsCnt.COUNT,   0,              &m_indColumn[2]},

        };
        SetBindColumn();
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

bool CSubscriberCount::SetBindParameter()
{
    bool bRtn = true;

    try
    {
        PDB::PreparedSQL::SetBindParameter(m_pstBindParam, m_nParamCnt);
        THD_LOG(NDF_LOG_T_INFO, "__CALL__");
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

bool CSubscriberCount::SetBindColumn()
{
    bool bRtn = true;

    try
    {
        PDB::PreparedSQL::SetBindColumn(m_pstBindColumn, m_nColumnCnt);
        THD_LOG(NDF_LOG_T_INFO, "__CALL__");
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

bool CSubscriberCount::SetValue(char *a_pszSvcId)
{
    bool bRtn = true;
    try
    {
        SNPRINTF(m_szServiceID, "%s", a_pszSvcId);
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

bool CSubscriberCount::Execute(SQLHSTMT &a_sqlHStmt)
{
    bool bRtn = true;

    int nIdx = 0;
    vecSubsCnt.clear();
    try
    {
        sret_ = SQLExecute(a_sqlHStmt);

        while (true)
        {
            sret_ = SQLFetch(a_sqlHStmt);
            if (!SQL_SUCCEEDED(sret_))
            {
                if (SQL_NO_DATA == sret_)
                {
                    if (nIdx == 0)
                    {
                        //THROW(1, "SQL_NO_DATA");
                    }
                    break;
                }
            }
            vecSubsCnt.push_back(m_stSubsCnt);
            nIdx++;
        }
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            if (SQL_NO_DATA == sret_)
            {
                THD_LOG(NDF_LOG_T_INFO, "[%s]", e.what());
            }
            else
            {
                GetDiagRec(SQL_HANDLE_STMT, a_sqlHStmt, (char *)className_.c_str(), sret_, e.what());
            }
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }    
    return bRtn;
}


//////////////////////////////////////////////////////////////////////////////////////////////
CSubscriberProfile::CSubscriberProfile()
{
    className_ = typeid(*this).name();
    Initialize();
}

CSubscriberProfile::~CSubscriberProfile()
{
}

bool CSubscriberProfile::Initialize()
{
    bool bRtn = true;

    try
    {
        Prepare();
        SetParameter();
        SetColumn();
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }

    return bRtn;
}

bool CSubscriberProfile::Prepare()
{
    bool bRtn = true;
    try
    {
        char *ptrQuery = (char *)"SELECT                     "
                                 "        MDN                " // 1
                                 "       ,MSISDN             " // 2
                                 "       ,MVNO               " // 3
                                 "       ,OCS                " // 4
                                 "       ,CREATE_TIME        " // 5
                                 "       ,UPDATE_TIME        " // 6
                                 "       ,UE_CATEGORY_LTE    " // 7
                                 "       ,UE_CATEGORY_5G     " // 8
                                 "       ,UE_DEVICE_TYPE     " // 9
                                 "       ,PRODUCT_ID         " // 10
                                 "FROM    T_5G_SUBS_PROFILE  "
                                 "WHERE                      "
                                 "        MDN = ?            "
                                 "LIMIT 1                    ";

        bRtn = SetSQL(PDB::eDefDBType::PCF, ptrQuery, strlen(ptrQuery));
        if (bRtn != true)
        {
            THROW(1, "SqlSQL Error");
        }
        THD_LOG(NDF_LOG_T_INFO, "__CALL__");
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

bool CSubscriberProfile::SetParameter()
{
    bool bRtn = true;

    try
    {
        const int nParamCnt = 1;
        m_nParamCnt = nParamCnt;
        m_indParam = new SQLLEN[nParamCnt];
        for (int i = 0; i < nParamCnt; i++)
        {
            m_indParam[i] = SQL_NTS;
        }
        m_pstBindParam = new STBindParameter[nParamCnt]{
            {1, SQL_C_CHAR, SQL_CHAR, LEN_MDN, 0, m_szMDN, LEN_MDN + 1, &m_indParam[0]},
        };
        SetBindParameter();
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

bool CSubscriberProfile::SetColumn()
{
    bool bRtn = true;

    try
    {
        const int nColumnCnt = 10;
        m_nColumnCnt = nColumnCnt;
        m_indColumn = new SQLLEN[nColumnCnt];
        for (int i = 0; i < nColumnCnt; i++)
        {
            m_indColumn[i] = SQL_NTS;
        }
        m_pstBindColumn = new STBindColumn[nColumnCnt]{
            {1,  SQL_C_CHAR, m_stSubsProfile.szMDN             ,LEN_MDN              +1, &m_indColumn[0]},
            {2,  SQL_C_CHAR, m_stSubsProfile.szMSISDN          ,LEN_MSISDN           +1, &m_indColumn[1]},
            {3,  SQL_C_CHAR, m_stSubsProfile.szMVNO            ,LEN_MVNO             +1, &m_indColumn[2]},
            {4,  SQL_C_CHAR, m_stSubsProfile.szOCS             ,LEN_FLAG             +1, &m_indColumn[3]},
            {5,  SQL_C_CHAR, m_stSubsProfile.szCreateTime      ,LEN_DATE_TIME        +1, &m_indColumn[4]},
            {6,  SQL_C_CHAR, m_stSubsProfile.szUpdateTime      ,LEN_DATE_TIME        +1, &m_indColumn[5]},
            {7,  SQL_C_CHAR, m_stSubsProfile.szUECategoryLTE   ,LEN_UE_CATEGORY_LTE  +1, &m_indColumn[6]},
            {8,  SQL_C_CHAR, m_stSubsProfile.szUECategory5G    ,LEN_UE_CATEGORY_5G   +1, &m_indColumn[7]},
            {9,  SQL_C_CHAR, m_stSubsProfile.szUEDeviceType    ,LEN_UE_DEVICE_TYPE   +1, &m_indColumn[8]},
            {10, SQL_C_CHAR, m_stSubsProfile.szProductID       ,LEN_PRODUCT_ID       +1, &m_indColumn[9]},
        };
        SetBindColumn();
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

bool CSubscriberProfile::SetBindParameter()
{
    bool bRtn = true;

    try
    {
        PDB::PreparedSQL::SetBindParameter(m_pstBindParam, m_nParamCnt);
        THD_LOG(NDF_LOG_T_INFO, "__CALL__");
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

bool CSubscriberProfile::SetBindColumn()
{
    bool bRtn = true;

    try
    {
        PDB::PreparedSQL::SetBindColumn(m_pstBindColumn, m_nColumnCnt);
        THD_LOG(NDF_LOG_T_INFO, "__CALL__");
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

bool CSubscriberProfile::SetValue(char *a_pszMDN)
{
    bool bRtn = true;
    try
    {
        SNPRINTF(m_szMDN, "%s", a_pszMDN);
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

bool CSubscriberProfile::Execute(SQLHSTMT &a_sqlHStmt)
{
    bool bRtn = true;
    try
    {

        sret_ = SQLExecute(a_sqlHStmt);

        if (SQL_SUCCEEDED(sret_))
        {
            sret_ = SQLFetch(a_sqlHStmt);
            if (!SQL_SUCCEEDED(sret_))
            {
                if (SQL_NO_DATA != sret_)
                {
                    bRtn = false;
                    GetDiagRec(SQL_HANDLE_STMT, a_sqlHStmt, (char *)className_.c_str(), sret_, __func__);
                    THD_LOG(NDF_LOG_T_WARNING, "SQLRETURN[%d:%s]", sret_, m_szODBCError);
                }
            }
        }
        else
        {
            if (SQL_NO_DATA != sret_)
            {
                bRtn = false;
                GetDiagRec(SQL_HANDLE_STMT, a_sqlHStmt, (char *)className_.c_str(), sret_, __func__);
                THD_LOG(NDF_LOG_T_WARNING, "SQLRETURN[%d:%s]", sret_, m_szODBCError);
            }
        }
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            if (SQL_NO_DATA == sret_)
            {
                THD_LOG(NDF_LOG_T_INFO, "[%s]", e.what());
            }
            else
            {
                GetDiagRec(SQL_HANDLE_STMT, a_sqlHStmt, (char *)className_.c_str(), sret_, e.what());
            }
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }

    return bRtn;
}

//////////////////////////////////////////////////////////////////////////////////////////////
CSubscriberService::CSubscriberService()
{
    className_ = typeid(*this).name();
    Initialize();
}

CSubscriberService::~CSubscriberService()
{
}

bool CSubscriberService::Initialize()
{
    bool bRtn = true;

    try
    {
        Prepare();
        SetParameter();
        SetColumn();
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }

    return bRtn;
}

bool CSubscriberService::Prepare()
{
    bool bRtn = true;
    try
    {
        char *ptrQuery = (char *)"SELECT                       "
                                 "        MDN                  " // 1
                                 "       ,SVC_ID               " // 2
                                 "       ,CNUM                 " // 3
                                 "       ,SVC_TYPE             " // 4
                                 "       ,CREATE_TIME          " // 5
                                 "       ,nvl(UPDATE_TIME,' ') " // 6
                                 "       ,JOB_CODE             " // 7
                                 "       ,ACTIVE_TIME          " // 8
                                 "       ,EXPIRE_TIME          " // 9
                                 "       ,TIME_PERIOD_ID       " // 10
                                 "       ,LIMIT                " // 11
                                 "       ,LIMIT_VALID_TIME     " // 12
                                 "       ,LIMIT_VALID_NOTI     " // 13
                                 "FROM    T_5G_SUBS_SERVICE    "
                                 "WHERE                        "
                                 "        MDN = ?              ";

        bRtn = SetSQL(PDB::eDefDBType::PCF, ptrQuery, strlen(ptrQuery));
        if (bRtn != true)
        {
            THROW(1, "SqlSQL Error");
        }
        THD_LOG(NDF_LOG_T_INFO, "__CALL__");
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

bool CSubscriberService::SetParameter()
{
    bool bRtn = true;

    try
    {
        const int nParamCnt = 1;
        m_nParamCnt = nParamCnt;
        m_indParam = new SQLLEN[nParamCnt];
        for (int i = 0; i < nParamCnt; i++)
        {
            m_indParam[i] = SQL_NTS;
        }
        m_pstBindParam = new STBindParameter[nParamCnt]{
            {1, SQL_C_CHAR, SQL_CHAR, LEN_MDN, 0, m_szMDN, LEN_MDN + 1, &m_indParam[0]},
        };
        SetBindParameter();
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

bool CSubscriberService::SetColumn()
{
    bool bRtn = true;

    try
    {
        const int nColumnCnt = 13;
        m_nColumnCnt = nColumnCnt;
        m_indColumn = new SQLLEN[nColumnCnt];
        for (int i = 0; i < nColumnCnt; i++)
        {
            m_indColumn[i] = SQL_NTS;
        }
        m_pstBindColumn = new STBindColumn[nColumnCnt]{
            {1,  SQL_C_CHAR, m_stSubsService.szMDN               , LEN_MDN              +1, &m_indColumn[0]},
            {2,  SQL_C_CHAR, m_stSubsService.szSVCID             , LEN_SVC_ID           +1, &m_indColumn[1]},
            {3,  SQL_C_CHAR, m_stSubsService.szCNUM              , LEN_CNUM             +1, &m_indColumn[2]},
            {4,  SQL_C_CHAR, m_stSubsService.szSVCType           , LEN_SVC_TYPE         +1, &m_indColumn[3]},
            {5,  SQL_C_CHAR, m_stSubsService.szCreateTime        , LEN_DATE_TIME        +1, &m_indColumn[4]},
            {6,  SQL_C_CHAR, m_stSubsService.szUpdateTime        , LEN_DATE_TIME        +1, &m_indColumn[5]},
            {7,  SQL_C_CHAR, m_stSubsService.szJobCode           , 4                    +1, &m_indColumn[6]},
            {8,  SQL_C_CHAR, m_stSubsService.szActiveTime        , LEN_DATE_TIME        +1, &m_indColumn[7]},
            {9,  SQL_C_CHAR, m_stSubsService.szExpireTime        , LEN_DATE_TIME        +1, &m_indColumn[8]},
            {10, SQL_C_CHAR, m_stSubsService.szTimePeroidId      , LEN_TIME_PERIOD_ID   +1, &m_indColumn[9]},
            {11, SQL_C_CHAR, m_stSubsService.szLimit             , LEN_LIMIT            +1, &m_indColumn[10]},
            {12, SQL_C_CHAR, m_stSubsService.szLimitValidTime    , LEN_DATE_TIME        +1, &m_indColumn[11]},
            {13, SQL_C_CHAR, m_stSubsService.szLimitValidNoti    , LEN_FLAG             +1, &m_indColumn[12]},
        };
        SetBindColumn();
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

bool CSubscriberService::SetBindParameter()
{
    bool bRtn = true;

    try
    {
        PDB::PreparedSQL::SetBindParameter(m_pstBindParam, m_nParamCnt);
        THD_LOG(NDF_LOG_T_INFO, "__CALL__");
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

bool CSubscriberService::SetBindColumn()
{
    bool bRtn = true;

    try
    {
        PDB::PreparedSQL::SetBindColumn(m_pstBindColumn, m_nColumnCnt);
        THD_LOG(NDF_LOG_T_INFO, "__CALL__");
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

bool CSubscriberService::SetValue(char *a_pszMDN)
{
    bool bRtn = true;
    try
    {
        SNPRINTF(m_szMDN, "%s", a_pszMDN);
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

bool CSubscriberService::Execute(SQLHSTMT &a_sqlHStmt)
{
    bool bRtn = true;
 
    int nIdx = 0;
    vecSubsService.clear();
    try
    {
        sret_ = SQLExecute(a_sqlHStmt);

        while (true)
        {
            sret_ = SQLFetch(a_sqlHStmt);
            if (!SQL_SUCCEEDED(sret_))
            {
                if (SQL_NO_DATA == sret_)
                {
                    if (nIdx == 0)
                    {
                        //THROW(1, "SQL_NO_DATA");
                    }
                    break;
                }
            }
            vecSubsService.push_back(m_stSubsService);
            nIdx++;
        }
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            if (SQL_NO_DATA == sret_)
            {
                THD_LOG(NDF_LOG_T_INFO, "[%s]", e.what());
            }
            else
            {
                GetDiagRec(SQL_HANDLE_STMT, a_sqlHStmt, (char *)className_.c_str(), sret_, e.what());
            }
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }    
    return bRtn; 
}



//////////////////////////////////////////////////////////////////////////////////////////////
CZoneInfo::CZoneInfo()
{
    className_ = typeid(*this).name();
    Initialize();
}

CZoneInfo::~CZoneInfo()
{
}

bool CZoneInfo::Initialize()
{
    bool bRtn = true;

    try
    {
        Prepare();
        // SetParameter();
        SetColumn();
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }

    return bRtn;
}

bool CZoneInfo::Prepare()
{
    bool bRtn = true;
    try
    {
        char *ptrQuery = (char *)"SELECT                  "
                                 "        SVC_ID          " // 1
                                 "       ,ZONE_CODE       " // 2
                                 "       ,POSITION        " // 3
                                 "FROM    T_5G_ZONE_INFO  ";
        bRtn = SetSQL(PDB::eDefDBType::PCF, ptrQuery, strlen(ptrQuery));
        if (bRtn != true)
        {
            THROW(1, "SqlSQL Error");
        }
        THD_LOG(NDF_LOG_T_INFO, "__CALL__");
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

// bool CZoneInfo::SetParameter()
// {
//     bool bRtn = true;

//     try
//     {
//         const int nParamCnt = 1;
//         m_nParamCnt = nParamCnt;
//         m_indParam = new SQLLEN[nParamCnt];
//         for (int i = 0; i < nParamCnt; i++)
//         {
//             m_indParam[i] = SQL_NTS;
//         }
//         m_pstBindParam = new STBindParameter[nParamCnt]{
//             {1, SQL_C_CHAR, SQL_CHAR, LEN_SVC_ID, 0, m_szServiceID, LEN_SVC_ID + 1, &m_indParam[0]},
//         };
//         SetBindParameter();
//     }
//     catch (CPCFException &e)
//     {
//         if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
//         {
//             THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
//         }
//         if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
//         {
//         }
//         if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
//         {
//         }
//         bRtn = false;
//     }
//     catch (std::exception &e)
//     {
//         THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
//         bRtn = false;
//     }
//     return bRtn;
// }

bool CZoneInfo::SetColumn()
{
    bool bRtn = true;

    try
    {
        const int nColumnCnt = 3;
        m_nColumnCnt = nColumnCnt;
        m_indColumn = new SQLLEN[nColumnCnt];
        for (int i = 0; i < nColumnCnt; i++)
        {
            m_indColumn[i] = SQL_NTS;
        }
        m_pstBindColumn = new STBindColumn[nColumnCnt]{
            {1, SQL_C_CHAR,  m_stZoneInfo.szSVCID,    LEN_SVC_ID + 1,    &m_indColumn[0]},
            {2, SQL_C_CHAR,  m_stZoneInfo.szZoneCode, LEN_ZONE_CODE + 1, &m_indColumn[1]},
            {3, SQL_C_SLONG,&m_stZoneInfo.nPosition,  0,                 &m_indColumn[2]},
        };
        SetBindColumn();
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

// bool CZoneInfo::SetBindParameter()
// {
//     bool bRtn = true;

//     try
//     {
//         PDB::PreparedSQL::SetBindParameter(m_pstBindParam, m_nParamCnt);
//         THD_LOG(NDF_LOG_T_INFO, "__CALL__");
//     }
//     catch (CPCFException &e)
//     {
//         if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
//         {
//             THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
//         }
//         if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
//         {
//         }
//         if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
//         {
//         }
//         bRtn = false;
//     }
//     catch (std::exception &e)
//     {
//         THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
//         bRtn = false;
//     }
//     return bRtn;
// }

bool CZoneInfo::SetBindColumn()
{
    bool bRtn = true;

    try
    {
        PDB::PreparedSQL::SetBindColumn(m_pstBindColumn, m_nColumnCnt);
        THD_LOG(NDF_LOG_T_INFO, "__CALL__");
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }
    return bRtn;
}

// bool CZoneInfo::SetValue(char *a_pszSvcId)
// {
//     bool bRtn = true;
//     try
//     {
//         SNPRINTF(m_szServiceID, "%s", a_pszSvcId);
//     }
//     catch (CPCFException &e)
//     {
//         if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
//         {
//             THD_LOG(NDF_LOG_T_WARNING, "[%s]", e.what());
//         }
//         if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
//         {
//         }
//         if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
//         {
//         }
//         bRtn = false;
//     }
//     catch (std::exception &e)
//     {
//         THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
//         bRtn = false;
//     }
//     return bRtn;
// }

bool CZoneInfo::Execute(SQLHSTMT &a_sqlHStmt)
{
    bool bRtn = true;

    int nIdx = 0;
    vecZoneInfo.clear();
    try
    {
        sret_ = SQLExecute(a_sqlHStmt);

        while (true)
        {
            sret_ = SQLFetch(a_sqlHStmt);
            if (!SQL_SUCCEEDED(sret_))
            {
                if (SQL_NO_DATA == sret_)
                {
                    if (nIdx == 0)
                    {
                        //THROW(1, "SQL_NO_DATA");
                    }
                    break;
                }
            }
            vecZoneInfo.push_back(m_stZoneInfo);
            nIdx++;
        }
    }
    catch (CPCFException &e)
    {
        if ((e.m_nECode & E_ERROR_LOG) == E_ERROR_LOG)
        {
            if (SQL_NO_DATA == sret_)
            {
                THD_LOG(NDF_LOG_T_INFO, "[%s]", e.what());
            }
            else
            {
                GetDiagRec(SQL_HANDLE_STMT, a_sqlHStmt, (char *)className_.c_str(), sret_, e.what());
            }
        }
        if ((e.m_nECode & E_ERROR_STAT_ADD) == E_ERROR_STAT_ADD)
        {
        }
        if ((e.m_nECode & E_ERROR_SEND) == E_ERROR_SEND)
        {
        }
        bRtn = false;
    }
    catch (std::exception &e)
    {
        THD_LOG(NDF_LOG_T_INFO, "[%s][%s]", typeid(e).name(), e.what());
        bRtn = false;
    }    
    return bRtn;
}
