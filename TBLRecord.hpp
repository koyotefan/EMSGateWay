#ifndef __TBLRecord_HPP__
#define __TBLRecord_HPP__

#include <string>

#define DEFAULT_UPDATE_CNT_TBL_PERIOD_SEC   300


#define DB_NSMF_MCC_LEN     3
#define DB_NSMF_MNC_LEN     3
#define DB_PCF_MCC_MNC_LEN  6

#define DB_S_NSSAI_SST_LEN  6 
#define DB_S_NSSAI_SD_LEN   6
#define DB_DNN_LEN          64
#define DB_NSMF_DNN_LEN     256

#define DB_HOSTNAME         32



extern thread_local char egwLogName[32];

struct stNsmfSessionInfoRecord {
    std::string     mcc;
    std::string     mnc;
    int             sst = 0;
    std::string     sd;
    std::string     dnn;

    int             nowCnt = 0;
    int             rejecting=0;
    std::string     hostname;

    stNsmfSessionInfoRecord() = default;

    stNsmfSessionInfoRecord(const stNsmfSessionInfoRecord & _st) {
        mcc         = _st.mcc;
        mnc         = _st.mnc;
        sst         = _st.sst;
        sd          = _st.sd;
        dnn         = _st.dnn;

        nowCnt      = _st.nowCnt;
        rejecting   = _st.rejecting;
        hostname    = _st.hostname;
    }

    stNsmfSessionInfoRecord & operator=(const stNsmfSessionInfoRecord & _st) {
        if(&_st != this) {
            mcc         = _st.mcc;
            mnc         = _st.mnc;
            sst         = _st.sst;
            sd          = _st.sd;
            dnn         = _st.dnn;

            nowCnt      = _st.nowCnt;
            rejecting   = _st.rejecting;
            hostname    = _st.hostname;
        }
        return *this;
    }
};

#endif    
