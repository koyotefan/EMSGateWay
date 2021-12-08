#ifndef EGW_TCP_CLIENT_HPP
#define EGW_TCP_CLIENT_HPP

#include <string>
#include "EGWSocket.hpp"

class EGWTcpClient : public EGWSocket {
public:
    explicit EGWTcpClient();
    explicit EGWTcpClient(const std::string & _ip, const int _port);
    ~EGWTcpClient();

    bool Connect(const std::string & _ip, const int _port);
    bool Connect();
    const char * GetIp() { return ip_.c_str(); }
    int  GetPort() { return port_; }

    void SetIp(std::string & _ip) { ip_ = _ip; }
    void SetPort(int _port) { port_ = _port; }

private:
    bool connect4(const std::string & _ip, const int _port);
    bool connect6(const std::string & _ip, const int _port);

private:
    std::string ip_;
    int         port_ = -1;

};

#endif // EGW_TCP_CLIENT_HPP
