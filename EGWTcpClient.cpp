
#include <arpa/inet.h>

#include "EGWDefine.hpp"
#include "EGWTcpClient.hpp"

EGWTcpClient::EGWTcpClient() {

}

EGWTcpClient::EGWTcpClient(const std::string & _ip, const int _port)
    : ip_(_ip),
      port_(_port) {

}

EGWTcpClient::~EGWTcpClient() {

}

bool EGWTcpClient::Connect(const std::string & _ip, const int _port) {
    if(_ip.find(".") != std::string::npos)
        return connect4(_ip, _port);

    return connect6(_ip, _port);
}

bool EGWTcpClient::Connect() {
    return Connect(ip_, port_);
}

bool EGWTcpClient::connect4(const std::string & _ip, const int _port) {

    struct sockaddr_in  addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));

    int fd;

    if((fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        char buf[128];
        W_THD_LOG(egwLogName,"socket() fail [%d:%s]",
            errno, strerror_r(errno, buf, sizeof(buf)));

        return false;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons((short)_port);

    struct in_addr      in;

    if((in.s_addr = inet_addr(_ip.c_str())) != (unsigned int)-1)
        addr.sin_addr.s_addr = in.s_addr;
    else
    {
        char buf[128];
        W_THD_LOG(egwLogName,"inet_addr() fail [%d:%s]",
             errno, strerror_r(errno, buf, sizeof(buf)));

        close(fd);
        return false;
    }

    if(connect(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0)
    {
        char buf[128];
        W_THD_LOG(egwLogName,"connect() [%s] fail [%d:%s]",
            _ip.c_str(), errno, strerror_r(errno, buf, sizeof(buf)));

        close(fd);
        return false;
    }

    EGWSocket::Init(fd, (struct sockaddr *)&addr, sizeof(addr));

    return true;

}

bool EGWTcpClient::connect6(const std::string & _ip, const int _port) {

    struct sockaddr_in6  addr;
    memset(&addr, 0, sizeof(addr));

    int fd;

    if((fd = socket(PF_INET6, SOCK_STREAM, 0)) < 0) {
        char buf[128];
        W_THD_LOG(egwLogName,"socket() fail [%d:%s]",
            errno, strerror_r(errno, buf, sizeof(buf)));

        return false;
    }

    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons((short)_port);

    if(inet_pton(AF_INET6, _ip.c_str(), &(addr.sin6_addr)) <= 0)
    {
        char buf[128];
        W_THD_LOG(egwLogName,"inet_addr() fail [%d:%s]",
            errno, strerror_r(errno, buf, sizeof(buf)));

        close(fd);
        return false;
    }

    if(connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        char buf[128];
        W_THD_LOG(egwLogName,"connect() [%s] fail [%d:%s]",
            _ip.c_str(), errno, strerror_r(errno, buf, sizeof(buf)));

        close(fd);
        return false;
    }

    EGWSocket::Init(fd, (struct sockaddr *)&addr, sizeof(addr));

    return true;

}