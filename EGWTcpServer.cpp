
#include "EGWDefine.hpp"
#include "EGWTcpServer.hpp"

extern thread_local char egwLogName[32];

EGWTcpServer::EGWTcpServer() {
    memset(&fds_, 0, sizeof(fds_));
}

EGWTcpServer::~EGWTcpServer() {
    Cleanup();
}

bool EGWTcpServer::Init() {

    if(port_ < 0) {
        E_THD_LOG(egwLogName,"server port is invalid [%d]. you should call SetPort()", port_);
        return false;
    }

    clear();

    bool bret = (bIpv6_)?createV6():createV4();

    if(bret == false)
        clear();

    return bret;
}

void EGWTcpServer::Cleanup() {
    clear();
}

void EGWTcpServer::clear() {
    if(fd_ >= 0) {
        close(fd_);
        fd_ = -1;
    }

    memset(&fds_, 0, sizeof(fds_));

}

bool EGWTcpServer::createV6() {

    struct sockaddr_in6  addr;
    memset(&addr, 0, sizeof(struct sockaddr_in6));

    if((fd_ = socket(PF_INET6, SOCK_STREAM, 0)) < 0) {
        char buf[128];
        E_THD_LOG(egwLogName,"socket() fail [%d:%s]",
            errno, strerror_r(errno, buf, sizeof(buf)));

        return false;
    }

    int nval = 1;
    if(setsockopt(fd_,
                  SOL_SOCKET,
                  SO_REUSEADDR,
                  &nval,
                  sizeof(nval)) < 0)
    {
        char buf[128];
        E_THD_LOG(egwLogName,"setsockopt() SO_REUSEADDR fail [%d:%s]",
            errno, strerror_r(errno, buf, sizeof(buf)));

        return false;
    }

    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons((short)port_);
    addr.sin6_addr = in6addr_any;

    return ready((struct sockaddr *)&addr, sizeof(addr));
}

bool EGWTcpServer::createV4() {

    struct sockaddr_in  addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));

    if((fd_ = socket(PF_INET, SOCK_STREAM, 0)) < 0) {

        char buf[128];
        E_THD_LOG(egwLogName,"socket() fail [%d:%s]",
            errno, strerror_r(errno, buf, sizeof(buf)));

        return false;
    }

    int nval = 1;
    if(setsockopt(fd_,
                  SOL_SOCKET,
                  SO_REUSEADDR,
                  &nval,
                  sizeof(nval)) < 0)
    {
        char buf[128];
        E_THD_LOG(egwLogName,"setsockopt() SO_REUSEADDR fail [%d:%s]",
            errno, strerror_r(errno, buf, sizeof(buf)));

        return false;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons((short)port_);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    return ready((struct sockaddr *)&addr, sizeof(addr));
}

bool EGWTcpServer::ready(struct sockaddr * _addr, socklen_t _addrlen) {

    if(bind(fd_, _addr, _addrlen) < 0)
    {
        char buf[128];
        E_THD_LOG(egwLogName,"bind() fail [%d:%s]",
            errno, strerror_r(errno, buf, sizeof(buf)));

        return false;
    }

    if(listen(fd_, 5) < 0)
    {
        char buf[128];
        E_THD_LOG(egwLogName,"listen() fail [%d:%s]",
            errno, strerror_r(errno, buf, sizeof(buf)));

        return false;
    }

    memset(&fds_, 0, sizeof(fds_));
    fds_.fd      = fd_;
    fds_.events  = POLLIN;

    return true;
}

void EGWTcpServer::reInit() {
    clear();
    Init();
}

std::shared_ptr<EGWSocket>   EGWTcpServer::GetNewClient() {

    if(fd_ < 0)
        return nullptr;

    struct pollfd   l_fds;
    memcpy(&l_fds, &fds_, sizeof(fds_));

    int ret = poll(&fds_, 1, period_);

    switch(ret) {
    case 0:
        return nullptr;
    case -1:
        char buf[128];
        W_THD_LOG(egwLogName,"poll() fail [%d:%s]",
            errno, strerror_r(errno, buf, sizeof(buf)));

        reInit();
        return nullptr;
    default:
        break;
    }

    if(fds_.revents != POLLIN)
    {
        W_THD_LOG(egwLogName,"poll() unexpected event [%d]", fds_.revents);

        reInit();
        return nullptr;
    }

    int                 conn;
    struct sockaddr_in6  addr;
    socklen_t           addrlen;

    conn = accept(fd_, (struct sockaddr *)&addr, &addrlen);

    if(conn < 0) {
        char buf[128];
        W_THD_LOG(egwLogName,"accept() fail [%d:%s]",
            errno, strerror_r(errno, buf, sizeof(buf)));

        return nullptr;
    }

    return  make_shared<EGWSocket>(conn, (struct sockaddr *)&addr, addrlen);
}
