#ifndef __Netstat_HPP__
#define __Netstat_HPP__

#include <vector>

#include "FileMonitor.hpp"

enum class NetstatItem {
    LOCAL_IP = 0,
    LOCAL_PORT,
    REMOTE_IP,
    REMOTE_PORT,
    STATE,
    TX_QUEUE,
    RX_QUEUE,
    UID
};

enum class NetstatValue {
    TCP_NONE = 0,
    TCP_ESTABLISHED = 1,
    TCP_SYN_SENT,
    TCP_SYN_RECV,
    TCP_FIN_WAIT1,
    TCP_FIN_WAIT2,
    TCP_TIME_WAIT,
    TCP_CLOSE,
    TCP_CLOSE_WAIT,
    TCP_LAST_ACK,
    TCP_LISTEN,
    TCP_CLOSING   /* now a valid state */
};

class NetstatObject {

public:
    explicit NetstatObject();
    ~NetstatObject();

    bool Get(std::string & _out,    NetstatItem    _item);
    void Set(NetstatItem _item,     const std::string & _in);
    void Set(NetstatItem _item,     const std::vector<std::string> & _vec);
    bool Equals(NetstatItem _item,  const std::string & _in);

private:
    std::vector<std::string>     vLocal_address_;
    std::vector<std::string>     vRemote_address_;

    std::string     local_port_;
    std::string     remote_port_;

    std::string     state_;
    std::string     tx_queue_;
    std::string     rx_queue_;
    std::string     uid_;
};

class NetstatTCP4
{
public:
    explicit NetstatTCP4();
    ~NetstatTCP4();

    bool Load();
    bool IsChanged() { return bChanged_; }

    void ClearResult() { vecFiltered_ = vecRaws_; }

    NetstatTCP4 * Filter(NetstatItem _item, const int _value) 
        { return Filter(_item, std::to_string(_value)); }
    NetstatTCP4 * Filter(NetstatItem _item, const std::string & _value) 
        { return Filter(_item, std::move(_value)); }
    NetstatTCP4 * Filter(NetstatItem _item, const std::string && _value);
    NetstatTCP4 * Filter(NetstatItem _item, const NetstatValue _value);

    size_t  GetFilteredCnt() const { return vecFiltered_.size(); }

private:
    bool            load();

    std::string     extractIp(std::string & _address);
    std::string     extractPort(std::string & _address);
    std::string     extractState(std::string & _state);
    std::string     extractTxQueue(std::string & _tx_rx_queue);
    std::string     extractRxQueue(std::string & _tx_rx_queue);   
    std::string     extractUid(std::string & _uid);

private:


private:
    std::vector<NetstatObject>      vecRaws_;
    std::vector<NetstatObject>      vecFiltered_;

    static const std::string        fname;
    FileMonitor                     fMonitor_;

    bool                            bChanged_ = false;
};

class NetstatSCTP
{
public:
    explicit NetstatSCTP();
    ~NetstatSCTP();

    bool Load();
    bool IsChanged() { return bChanged_; }

    void ClearResult() { vecFiltered_ = vecRaws_; }

    NetstatSCTP * Filter(NetstatItem _item, const int _value) 
        { return Filter(_item, std::to_string(_value)); }
    NetstatSCTP * Filter(NetstatItem _item, const std::string & _value) 
        { return Filter(_item, std::move(_value)); }
    NetstatSCTP * Filter(NetstatItem _item, const std::string && _value);
    NetstatSCTP * Filter(NetstatItem _item, const NetstatValue _value);

    size_t  GetFilteredCnt() const { return vecFiltered_.size(); }

private:
    bool            load();

private:


private:
    std::vector<NetstatObject>      vecRaws_;
    std::vector<NetstatObject>      vecFiltered_;

    static const std::string        fname;
    FileMonitor                     fMonitor_;

    bool                            bChanged_ = false;
};

#endif