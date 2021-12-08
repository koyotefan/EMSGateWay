#include <fstream>
#include <sstream>
#include <tuple>
#include <arpa/inet.h>

#include "EGWDefine.hpp"
#include "Netstat.hpp"

extern thread_local char    egwLogName[32]; 

// NetstatObject ///////////////////////////////////////////////////////////////

NetstatObject::NetstatObject() {

}

NetstatObject::~NetstatObject() {

}

bool NetstatObject::Get(std::string & _out, NetstatItem _item) {

    // Type Check
    switch(_item) {
    case NetstatItem::LOCAL_IP:
        _out    = vLocal_address_.front();
        break;
    case NetstatItem::LOCAL_PORT:
        _out    = local_port_;
        break;
    case NetstatItem::REMOTE_IP:
        _out    = vRemote_address_.front();
        break;
    case NetstatItem::REMOTE_PORT:
        _out    = remote_port_;
        break;
    case NetstatItem::STATE:
        _out    = state_;
        break;
    case NetstatItem::TX_QUEUE:
        _out    = tx_queue_;
        break;
    case NetstatItem::RX_QUEUE:
        _out    = rx_queue_;
        break;
    case NetstatItem::UID:
        _out    = uid_;
        break;
    default:
        // Invalid Type
        return false;
    }

    return true;
}

void NetstatObject::Set(NetstatItem _item, const std::string & _in) {

    switch(_item) {
    case NetstatItem::LOCAL_IP:
        vLocal_address_.emplace_back(std::move(_in));
        break;
    case NetstatItem::LOCAL_PORT:
        local_port_     = _in;
        break;
    case NetstatItem::REMOTE_IP:
        vRemote_address_.emplace_back(std::move(_in));
        break;
    case NetstatItem::REMOTE_PORT:
        remote_port_    = _in;
        break;
    case NetstatItem::STATE:
        state_          = _in;
        break;
    case NetstatItem::TX_QUEUE:
        tx_queue_       = _in;
        break;
    case NetstatItem::RX_QUEUE:
        rx_queue_       = _in;
        break;
    case NetstatItem::UID:
        uid_            = _in;
        break;
    default:
        break;
    }
}

void NetstatObject::Set(NetstatItem _item, const std::vector<std::string> & _vec) {
    switch(_item) {
    case NetstatItem::LOCAL_IP:
        vLocal_address_ = _vec;
        break;
    case NetstatItem::REMOTE_IP:
        vRemote_address_ = _vec; 
        break;
    default:
        break;
    }
}

bool NetstatObject::Equals(NetstatItem _item, const std::string & _in) {
    switch(_item) {
    case NetstatItem::LOCAL_IP:
        for(auto & item : vLocal_address_) {
            if(item.compare(_in) == 0)
                return true;
        }
        return false;
    case NetstatItem::LOCAL_PORT:
        return ( local_port_.compare(_in)    == 0 );
    case NetstatItem::REMOTE_IP:
        for(auto & item : vRemote_address_) {
            if(item.compare(_in) == 0)
                return true;
        }
        return false;
    case NetstatItem::REMOTE_PORT:
        return ( remote_port_.compare(_in)    == 0 );
    case NetstatItem::STATE:
        return ( state_.compare(_in)          == 0 ) ;
    case NetstatItem::TX_QUEUE:
        return ( tx_queue_.compare(_in)       == 0 );
    case NetstatItem::RX_QUEUE:
        return ( rx_queue_.compare(_in)       == 0 );
    case NetstatItem::UID:
        return (uid_.compare(_in)             == 0 );
    default:
        break;
    }

    return false;
}


// NetstatTCP4 ////////////////////////////////////////////////////////////////

const std::string NetstatTCP4::fname = "/proc/net/tcp";

NetstatTCP4::NetstatTCP4() :
    fMonitor_(NetstatTCP4::fname) {

}

NetstatTCP4::~NetstatTCP4() {

}

bool NetstatTCP4::Load() {

    bChanged_ = false;

    // tcp 파일을 매 초마다 변경되요.. 매 초마다 읽으니까, CPU 부하가 심하더군요.
    // 해서, 2초마다 읽을려구요.
    if(time(nullptr) % 2 == 0)
        return true;

    // 값이 변경되지 않았다면, 동작을 하지 않습니다.
    // 어차피 이게 초 단위이기 때문에, 1sec 에 한번 동작하겠구만요.
    if(fMonitor_.IsChanged() == false)
        return true;

    // File Loading 후, vecRaws_ 를 채웁니다.
    vecRaws_.clear();
    vecFiltered_.clear();

    try {
        load();
    } catch (...) {
        E_THD_LOG(egwLogName, 
            "File Loading fail [%s]", NetstatTCP4::fname.c_str());
        return false;
    }

    bChanged_ = true;

    return true;
}

bool NetstatTCP4::load() {
    std::ifstream   istream;
    istream.open(NetstatTCP4::fname);

    if(istream.good() == false) {
        E_THD_LOG(egwLogName, 
            "File open fail [%s]", NetstatTCP4::fname.c_str());
        return false;
    }

    /*-
    sl local_address rem_address   st tx_queue rx_queue tr tm->when retrnsmt   uid  timeout inode 
    0: 0100007F:8B06 00000000:0000 0A 00000000:00000000 00:00000000 00000000  1011        0 161098
    1: 00000000:0CEA 00000000:0000 0A 00000000:00000000 00:00000000 00000000    27        0 33871 
    -*/

    vecRaws_.clear();

    std::string     line;
    
    std::string     sl;
    std::string     local_address;
    std::string     rem_address;
    std::string     st;
    //std::string     tx_rx_queue;
    //std::string     tr_tm_when;
    //std::string     retrnsmt;
    //std::string     uid;

    std::istringstream iss(line);

    while(std::getline(istream, line)) {

        iss.str(line);

        // std::istringstream iss(line);

        // CPU 사용률이 크더라구요.. 그래서 절반만 읽는 것으로 변경했어요.
        /*-
        if( !(iss >> sl >> local_address >> rem_address >> st >> tx_rx_queue >> tr_tm_when >>  retrnsmt >> uid ) ) {
            continue;
        }
        -*/
        if( !(iss >> sl >> local_address >> rem_address >> st) ) {
            continue;
        }
        
        NetstatObject   object;
        object.Set(NetstatItem::LOCAL_IP,       extractIp(local_address));
        object.Set(NetstatItem::LOCAL_PORT,     extractPort(local_address));
        object.Set(NetstatItem::REMOTE_IP,      extractIp(rem_address));
        object.Set(NetstatItem::REMOTE_PORT,    extractPort(rem_address));
        object.Set(NetstatItem::STATE,          extractState(st));
        //object.Set(NetstatItem::TX_QUEUE,       extractTxQueue(tx_rx_queue));
        //object.Set(NetstatItem::RX_QUEUE,       extractRxQueue(tx_rx_queue));
        //object.Set(NetstatItem::UID,            extractUid(uid));
        
        vecRaws_.emplace_back(object);
    }

    istream.close();

    return true;
}

std::string NetstatTCP4::extractIp(std::string & _in) {
    // 0100007F:8B06 

    std::string     temp(_in);
    std::tuple<std::string, std::string> tup = EGWCommon::split(temp, ":");

    if(std::get<0>(tup).length() == 0) {
        return std::string("");
    }

    unsigned long ul = std::strtoul(std::get<0>(tup).c_str(), nullptr, 16);
    // std::cout << ul << std::endl;

    char    str[32] = {'\0', };
    if(inet_ntop(AF_INET, &ul, str, 32) == nullptr) {
        return std::string("");
    }

    return std::string(str);
}

std::string NetstatTCP4::extractPort(std::string & _in) {
    // 0100007F:8B06 

    std::string     temp(_in);
    std::tuple<std::string, std::string> tup = EGWCommon::split(temp, ":");

    if(std::get<1>(tup).length() == 0) {
        return std::string("");
    }

    unsigned long ul = std::strtoul(std::get<1>(tup).c_str(), nullptr, 16);
    return std::to_string(ul);
}

std::string NetstatTCP4::extractState(std::string & _state) {
    // 0A
    unsigned long ul = std::strtoul(_state.c_str(), nullptr, 16);
    return std::to_string(ul);
}

std::string NetstatTCP4::extractTxQueue(std::string & _tx_rx_queue) {
    // 00000000:00000000 

    std::string     temp(_tx_rx_queue);
    std::tuple<std::string, std::string> tup = EGWCommon::split(temp, ":");

    return std::get<0>(tup);
}

std::string NetstatTCP4::extractRxQueue(std::string & _tx_rx_queue) {
    // 00000000:00000000 

    std::string     temp(_tx_rx_queue);
    std::tuple<std::string, std::string> tup = EGWCommon::split(temp, ":");

    return std::get<1>(tup);
}

std::string NetstatTCP4::extractUid(std::string & _uid) {
    // 1011  
    return _uid;
}

NetstatTCP4 * NetstatTCP4::Filter(NetstatItem _item, const std::string && _value) {

    /*-
    for(auto iter = vecFiltered_.begin(); iter != vecFiltered_.end(); ) {
        NetstatObject & obj = *iter; 

        if( obj.Equals(_item, _value) == false )
            vecFiltered_.erase(iter);
        else 
            ++iter;
    }
    -*/

    std::vector<NetstatObject>  result;
    size_t  size = vecFiltered_.size();

    for(size_t idx=0; idx < size; ++idx) {
        NetstatObject obj = vecFiltered_.at(idx);

        if( obj.Equals(_item, _value) )
            result.emplace_back(obj);
    }

    vecFiltered_ = result;

    return this;
}

NetstatTCP4 * NetstatTCP4::Filter(NetstatItem _item, const NetstatValue _value) {
    std::string  str = std::to_string(static_cast<int>(_value));
    return Filter(_item, str);

}

// NetstatSCTP ////////////////////////////////////////////////////////////////

const std::string NetstatSCTP::fname = "/proc/net/sctp/assocs";

NetstatSCTP::NetstatSCTP() :
    fMonitor_(NetstatSCTP::fname) {

}

NetstatSCTP::~NetstatSCTP() {

}

bool NetstatSCTP::Load() {

    bChanged_ = false;

    // tcp 파일을 매 초마다 변경되요.. 매 초마다 읽으니까, CPU 부하가 심하더군요.
    // 해서, 2초마다 읽을려구요.
    if(time(nullptr) % 2 == 0)
        return true;

    // 값이 변경되지 않았다면, 동작을 하지 않습니다.
    // 어차피 이게 초 단위이기 때문에, 1sec 에 한번 동작하겠구만요.
    if(fMonitor_.IsChanged() == false)
        return true;

    // File Loading 후, vecRaws_ 를 채웁니다.
    vecRaws_.clear();
    vecFiltered_.clear();

    try {
        load();
    } catch (...) {
        E_THD_LOG(egwLogName, 
            "File Loading fail [%s]", NetstatSCTP::fname.c_str());
        return false;
    }

    bChanged_ = true;

    return true;
}

bool NetstatSCTP::load() {
    std::ifstream   istream;
    istream.open(NetstatSCTP::fname);

    if(istream.good() == false) {
        E_THD_LOG(egwLogName, 
            "File open fail [%s]", NetstatSCTP::fname.c_str());
        return false;
    }

    /*-
    ASSOC     SOCK   STY SST ST HBKT ASSOC-ID TX_QUEUE RX_QUEUE UID INODE LPORT RPORT LADDRS <-> RADDRS HBINT INS OUTS MAXRT T1X T2X RTXC wmema wmemq sndbuf rcvbuf
    ffff96a14c8d1000 ffff96a4c628ac00 2   1   3  0    1827707        0        0    1003 137248804 3878  55312  172.1.218.253 40.40.0.59 60.30.171.38 60.30.167.36 100.100.150.18 172.1.206.38 <-> *40.40.0.58 60.30.171.143 60.30.167.142 100.100.150.17      30000    10    10   10    0    0        0        1        0  4194304  4194304
    -*/

    vecRaws_.clear();

    std::string     line;

    std::vector<std::string>        words;
    std::vector<std::string>        local_addresses;
    std::vector<std::string>        rem_addresses;
    bool                            bBefore = true;

    std::string                     local_port;
    std::string                     rem_port;
    std::string                     st;

    // 첫째줄을skip 하기위함
    std::getline(istream, line);

    while(std::getline(istream, line)) {
        
        words.clear();

        EGWCommon::split(words, line, " ");

        st          = words.at(3);
        local_port  = words.at(11);
        rem_port    = words.at(12);

        local_addresses.clear();
        rem_addresses.clear();
        bBefore     = true;

        for(size_t i=13; i < words.size(); ++i) {
            auto & word = words.at(i);

            if(word.find('-') != std::string::npos) {
                bBefore = false;
                continue;
            }

            if(word.find('.') == std::string::npos &&
               word.find(':') == std::string::npos)
                break;

            if(bBefore)
                local_addresses.emplace_back(word);
            else
                rem_addresses.emplace_back(word);
        }

        NetstatObject   object;
        object.Set(NetstatItem::LOCAL_IP,       local_addresses);
        object.Set(NetstatItem::LOCAL_PORT,     local_port);
        object.Set(NetstatItem::REMOTE_IP,      rem_addresses);
        object.Set(NetstatItem::REMOTE_PORT,    rem_port);
        object.Set(NetstatItem::STATE,          st);
        
        vecRaws_.emplace_back(object);
    }

    istream.close();

    return true;
}

NetstatSCTP * NetstatSCTP::Filter(NetstatItem _item, const std::string && _value) {

    std::vector<NetstatObject>  result;
    size_t  size = vecFiltered_.size();

    for(size_t idx=0; idx < size; ++idx) {
        NetstatObject obj = vecFiltered_.at(idx);

        if( obj.Equals(_item, _value) )
            result.emplace_back(obj);
    }

    vecFiltered_ = result;

    return this;
}

NetstatSCTP * NetstatSCTP::Filter(NetstatItem _item, const NetstatValue _value) {
    std::string  str = std::to_string(static_cast<int>(_value));
    return Filter(_item, str);

}

