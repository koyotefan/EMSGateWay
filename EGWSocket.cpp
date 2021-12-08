
#include "EGWDefine.hpp"
#include "EGWSocket.hpp"

EGWSocket::EGWSocket() {
    memset(&fds_, 0, sizeof(fds_));
    memset(&addr_, 0, sizeof(addr_));
    addrlen_ = 0;
}

EGWSocket::EGWSocket(int _fd, struct sockaddr * _addr, socklen_t _addrlen) {
    Init(_fd, _addr, _addrlen);
}

EGWSocket::EGWSocket(const EGWSocket & _rhs) {
    fd_ = _rhs.fd_;
    milliseconds_ = _rhs.milliseconds_;
    memcpy(&fds_, &_rhs.fds_, sizeof(fds_));
    memcpy(&addr_, &_rhs.addr_, sizeof(addr_));
}

void EGWSocket::SetWait(int _milliseconds) {
    if(_milliseconds >= 0)
        milliseconds_ = _milliseconds;
}

void EGWSocket::SetLinger() {
    if(fd_ < 0)
        return ;

    struct linger st = {1, 1};
    if(setsockopt(fd_, SOL_SOCKET, SO_LINGER, &st, sizeof(st)) < 0) {
        char buf[128];
        W_THD_LOG(egwLogName,"setsockopt() - SO_LINGER fail [%d:%s]",
            errno, strerror_r(errno, buf, sizeof(buf)));
    }
}

void EGWSocket::Close() {
    if(fd_ >= 0) {
        close(fd_);
        fd_ = -1;
    }

    memset(&fds_, 0, sizeof(fds_));
    memset(&addr_, 0, sizeof(addr_));
    addrlen_ = 0;
}


void EGWSocket::Init(int _fd, struct sockaddr * _addr, socklen_t _addrlen) {
    fd_ = _fd;

    memset(&fds_, 0, sizeof(fds_));
    fds_.fd     = fd_;
    fds_.events = POLLIN;

    // option
    struct timeval  to;
    to.tv_sec = 3;
    to.tv_usec = 0;

    if(setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO, &to, sizeof(to)) < 0) {
        char buf[128];
        W_THD_LOG(egwLogName,"setsockopt() - SO_RCVTIMEO fail [%d:%s]",
            errno, strerror_r(errno, buf, sizeof(buf)));
    }

    if(setsockopt(fd_, SOL_SOCKET, SO_SNDTIMEO, &to, sizeof(to)) < 0) {
        char buf[128];
        W_THD_LOG(egwLogName,"setsockopt() - SO_SNDTIMEO fail [%d:%s]",
            errno, strerror_r(errno, buf, sizeof(buf)));
    }

    memset(&addr_, 0, sizeof(addr_));
    memcpy(&addr_, _addr, _addrlen);
    addrlen_ = _addrlen;

}

bool EGWSocket::Send(const char * _ptr, size_t _size) {

    if(fd_ < 0)
        return false;

    size_t sendt = 0;

    while(_size > sendt) {
        ssize_t sendn = send(fd_, _ptr + sendt, _size - sendt, 0);

        if(sendn <= 0) {
            char buf[128];
            W_THD_LOG(egwLogName,"send() fail [%zd] [%d:%s]",
                sendn, errno, strerror_r(errno, buf, sizeof(buf)));

            return false;
        }

        sendt += sendn;
    }

    return true;
}

int EGWSocket::Recv(char * _ptr, size_t _size) {
    if(fd_ < 0)
        return -1;

    struct pollfd l_fds;
    memcpy(&l_fds, &fds_, sizeof(fds_));

    int ret = poll(&fds_, 1, milliseconds_);
    if(ret <= 0) {
        if(ret < 0) {
            char buf[128];
            W_THD_LOG(egwLogName,"poll() fail [%d] [%d:%s]",
                ret, errno, strerror_r(errno, buf, sizeof(buf)));
        }
        return ret;
    }

    size_t readt = 0;
    while(_size > readt) {
        ssize_t readn = recv(fd_, _ptr + readt, _size - readt, 0);

        if(readn <= 0) {
            char buf[128];
            W_THD_LOG(egwLogName,"recv() fail [%zu] [%zd] [%d:%s]",
                readt, readn, errno, strerror_r(errno, buf, sizeof(buf)));

            return -1;
        }

        readt += readn;
    }

    return (int)readt;
}
