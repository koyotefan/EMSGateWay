#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include <string>
#include <unordered_map>

#include "EGWDefine.hpp"
#include "EGWSocket.hpp"
//#include "HttpRequest.hpp"
#include "EGWTcpClient.hpp"

class HttpResponse {
public:
    explicit HttpResponse();
    ~HttpResponse();

    bool Send(EGWSocket * _sock);
    bool Receive(EGWTcpClient * _client);

    void SetStatusCode(int _code);
    void SetHeader(std::string _header, std::string _value);
    void SetBody(std::string & _body);

    const char * GetStatusLine() const { return statusLine_.c_str(); }
    const char * GetHeader(const char * _header);
    const char * GetBody() const { return body_.c_str(); }

private:
    std::string makeMessage();

private:
    std::string     statusLine_;

    std::unordered_map<std::string, std::string>    headers_;
    std::string     body_;

};


#endif // HTTP_RESPONSE_HPP
