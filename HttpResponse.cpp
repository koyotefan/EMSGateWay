
#include <vector>


#include "HttpResponse.hpp"
#include "HttpData.hpp"

// extern thread_local char egwLogName[32];

HttpResponse::HttpResponse() {

    // Default Headers
    SetStatusCode(500);

    SetHeader("Connection", "close");
    SetHeader("Access-Control-Allow-Origin", "*");
    SetHeader("Access-Control-Allow-Headers", "origin, x-requested-with, content-type, accept, ajax");
}

HttpResponse::~HttpResponse() {

}


bool HttpResponse::Send(EGWSocket * _sock) {

    std::string out = makeMessage();
    D_THD_LOG(egwLogName,"-- Response [%s]", out.c_str());
    return _sock->Send(out.c_str(), out.length());
}

bool HttpResponse::Receive(EGWTcpClient * _client) {

    _client->SetWait(5000);

    if(HttpData::ReadUntilCRLF(statusLine_, _client) == false) {
        W_THD_LOG(egwLogName, "status line read fail");
        return false;
    }

    if(HttpData::ReadHeaders(headers_, _client) == false) {
        W_THD_LOG(egwLogName,"headers read fail");
        return false;
    }

    size_t contentLength = HttpData::GetContentLength(headers_);

    if(HttpData::ReadData(body_, _client, contentLength) == false) {
        W_THD_LOG(egwLogName,"body read fail");
        return false;
    }

    return true; 
} 

void HttpResponse::SetStatusCode(int _code) {

    std::string ver("HTTP/1.1");
    std::string status;

    switch(_code)
    {
    case 200:
        status = "200 OK";
        break;
    case 400:
        status = "400 Bad Request";
        break;
    case 404:
        status = "404 Not Found";
        break;
    case 503:
        status = "503 Service Unavailable";
        break;
    default:
        status = "500 Internal Server Error";
        break;
    }

    statusLine_ = ver;
    statusLine_.append(" ");
    statusLine_.append(status);
}

void HttpResponse::SetHeader(std::string _header, std::string _value) {
    headers_[_header] = _value;
}

void HttpResponse::SetBody(std::string & _body) {

    body_ = _body;
    SetHeader("Content-Length", std::to_string(body_.length()));
    SetHeader("Content-Type", "application/json");

}

const char * HttpResponse::GetHeader(const char * _header) {

    auto iter = headers_.find(_header);

    if(iter == headers_.end())
        return nullptr;

    return (iter->second).c_str();
}

std::string HttpResponse::makeMessage() {

    std::string     out;
    out.reserve(256);

    out.append(statusLine_).append("\r\n");

    for(auto & item : headers_) {
        out.append(item.first).append(": ").append(item.second)
           .append("\r\n");
    }

    if(body_.empty() == false) {
        out.append("\r\n");
        out.append(body_);
    }

    return out;

/*-
    std::vector<char>   vec;
    vec.reserve(256);

    vec.insert(vec.end(), statusLine_.begin(), statusLine_.end());
    vec.push_back('\r');
    vec.push_back('\n');

    for(auto citer : headers_) {
        vec.insert(vec.end(), citer.first.begin(), citer.first.end());
        vec.push_back(':');
        vec.insert(vec.end(), citer.second.begin(), citer.second.end());
        vec.push_back('\r');
        vec.push_back('\n');
    }

    if(body_.length() > 0) {
        vec.push_back('\r');
        vec.push_back('\n');
        vec.insert(vec.end(), body_.begin(), body_.end());
    }

    return std::string(vec.data(), vec.size());
-*/

}
