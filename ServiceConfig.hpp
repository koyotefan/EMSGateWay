#ifndef __ServiceConfig_HPP__
#define __ServiceConfig_HPP__

#include <vector>

#include "rapidjson/document.h"

// network 상태를 감시하기 위한 대상 서비스 설정 정보를 읽습니다.
#include "FileMonitor.hpp"
#include "Netstat.hpp"

struct stConnectedCnt {
    std::string     name;
    std::string     ip;
    unsigned short  connCnt = 0;
};

struct stStatusByIp {
    std::string     name;
    std::string     status; // OK or NOK
    std::string     ip;
};

enum class ServiceProtocol : int {
    TCP = 0,
    UDP,
    SCTP,
    ERR
}; 

class ServiceItem {
public:
    explicit ServiceItem();
    ServiceItem(const ServiceItem & _inst) = default;
    ServiceItem & operator=(const ServiceItem & _inst) = default;
    ~ServiceItem();

    bool IsOK() const;
    const char * GetPolicy() const { return svcPolicy_.c_str(); }

private:
    bool    policy_default() const ;
    bool    policy_5G_HDV_Routing()     const;
    bool    policy_LTE_HDV_Routing()    const;
    bool    policy_5G_CN_USER_Noti()    const;
    bool    policy_EPC_PG_Noti()        const;
    bool    policy_Zone_InOut()         const;
    bool    policy_PG_Local_DB()        const;
    bool    policy_Baro_D_InOut()       const;


public:
    std::string                 name_;
    std::string                 svcPolicy_;
    std::string                 targetSystem_;
    std::vector<stConnectedCnt> vConnectedIpCnt_;
    bool                        bServer_;
    int                         nPort_;
    ServiceProtocol             proto_;
    int                         nMinConnectionLimit_;
    int                         nMinSystemCntLimit_; 
};

class AlarmForExternalConnection
{
public:
    explicit AlarmForExternalConnection();
    ~AlarmForExternalConnection();

    void Trap(ServiceItem & _item);

private:
    bool isChangedAlarmStatus(const std::string & _serviceName, bool _bAlarm);

private:
    std::unordered_map<std::string, bool>   m_;
};

class ServiceConfig
{
public:
    explicit ServiceConfig(const std::string & _fName);
    ~ServiceConfig();

    bool Load();
    bool IsChanged() { return bChanged_; }
    bool UpdateConnection(NetstatTCP4 * _tcp4, NetstatSCTP * _sctp);
    void MakeAtomReportMessageForPG(std::string & _out);
    void MakeAtomReportMessageForPGW(std::string & _out);

    static bool IsPGWItem(std::string _name);

private:
    bool load();
    bool toParse(std::string && _str);
    bool setService(std::vector<ServiceItem> & _vec, rapidjson::Document & _doc);
    bool setIp(std::vector<ServiceItem> & _vec, rapidjson::Document & _doc);
    void setAlternativeNodeId();
    void prnItem();

    int getConnectedCnt(NetstatTCP4 * _tcp4, std::string & _ip, int _nPort, bool _bServer);
    int getConnectedCnt(NetstatSCTP * _sctp, std::string & _ip, int _nPort, bool _bServer);

    void makeServiceList(std::string & _out, const ServiceItem & _item);
    void makeIngressList(std::string & _out, const ServiceItem & _item);

    bool doYouHavePGWType();
    
private:
    FileMonitor         fMonitor_;
    bool                bChanged_ = false;

    std::vector<ServiceItem>  vItems_;
    std::string         alternativeNodeId_;

    AlarmForExternalConnection  alarm_;

};

#endif
