#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <unordered_map>
#include <tuple>

#include "EGWDefine.hpp"
#include "EGWSocket.hpp"
#include "EGWTcpClient.hpp"

class HttpRequest {
public:
    explicit HttpRequest();
    ~HttpRequest();

    bool Receive(EGWSocket * _sock);
    const char * GetMethod() const { return method_.c_str(); }
    const char * GetPath() const { return path_.c_str(); }
    const char * GetVersion() const { return version_.c_str(); }
    const char * GetBody() const { return body_.c_str(); }
    const char * GetHeader(const char * _header);

    bool Send(EGWTcpClient * _client);
    void SetMethod(const char * _str) { 
        headers_.clear();
        method_ = _str; 
    }
    void SetPath(const char * _str)   { path_ = _str; }
    void SetVersion(const char * _str) { version_ = _str; }
    void SetPath(const std::string & _str)   { path_ = _str; }
    void SetVersion(const std::string & _str) { version_ = _str; }
    void ClearHeader() { headers_.clear(); }
    void SetHeader(const char * _h, const char * _v) {
        headers_.insert({_h, _v});
    }
    void SetBody(const char * _str) {

        if(_str == nullptr || *_str == '\0')
            return ;

        body_.clear();
        body_ = _str;

        std::string clength = std::to_string(body_.length());
        headers_.insert({"Content-Length", clength.c_str()});
    }


private:

/*-
    bool readData(std::string & _out, EGWSocket * _sock, size_t _length);
    size_t GetContentLength();
    bool readUntilSpace(std::string & _out, EGWSocket * _sock);
    bool readUntilCRLF(std::string & _out, EGWSocket * _sock);
    bool readHeaders(EGWSocket * _sock);
//    std::string trim(std::string & _out);
//    std::tuple<std::string, std::string> split(std::string & _line, const std::string _sep);
-*/


private:
    std::string method_;
    std::string path_;
    std::string version_;
    std::unordered_map<std::string, std::string>     headers_;
    std::string body_;

    std::string out_;

};

#endif // HTTP_REQUEST_HPP
