#ifndef Http_Client_HPP
#define Http_Client_HPP

#include "EGWTcpClient.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class HttpClient {
public:
    explicit HttpClient() = default;
    ~HttpClient() = default;

    bool Init(std::string & _ip, int _port);
    bool Request(const std::string & _path, const std::string & _out) ;
    bool Response() ;

private:
    EGWTcpClient    client_;
    HttpRequest     req_;
    HttpResponse    resp_;

    std::string     ip_;
    int             port_ = -1;
};

#endif
