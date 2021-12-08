#ifndef EGW_SOCKET_HPP
#define EGW_SOCKET_HPP

#include <cstring>
#include <poll.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
//#include <netdb.h>

extern thread_local char egwLogName[32];

class EGWSocket {
public:
    explicit EGWSocket();
    explicit EGWSocket(int _fd, struct sockaddr * _addr, socklen_t _addrlen);
    explicit EGWSocket(const EGWSocket & _rhs);
    virtual ~EGWSocket() { Close(); }

    void Init(int _fd, struct sockaddr * _addr, socklen_t _addrlen);

    void SetWait(int _milliseconds);
    void SetLinger();
    void Close();

    bool Send(const char * _ptr, size_t _size);
    int Recv(char * _ptr, size_t _size);

    bool IsConnected() { return fd_ >= 0; }
    int GetSocket() { return fd_; }

protected:
    int     fd_ = -1;
    int     milliseconds_ = 100;

    struct pollfd           fds_;
    struct sockaddr_in6     addr_;
    socklen_t               addrlen_;
};



#endif // EGW_SOCKET_HPP
