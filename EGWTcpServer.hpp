#ifndef EGW_TCP_SERVER_HPP
#define EGW_TCP_SERVER_HPP

#include <memory>
#include <poll.h>

#include "EGWSocket.hpp"

class EGWTcpServer {
public:
    explicit EGWTcpServer();
    ~EGWTcpServer();

    void SetIpv6(bool _on=false) { bIpv6_ = _on; }
    void SetPort(int _port) { port_ = _port; }
    void SetWaitPeriod(int _period) { period_ = _period; }

    bool Init();
    void Cleanup();

    std::shared_ptr<EGWSocket> GetNewClient();

private:
    bool    createV4();
    bool    createV6();
    bool    ready(struct sockaddr * _addr, socklen_t _addrlen);
    void    reInit();
    void    clear();

private:
    bool    bIpv6_ = false;
    int     port_ = -1;
    int     period_ = 100;

    int     fd_ = -1;
    struct  pollfd  fds_;

};


#endif // EGW_TCP_SERVER_HPP
