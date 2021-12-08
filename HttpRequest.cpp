
// #include <memory>
#include <algorithm>

#include "HttpRequest.hpp"
#include "HttpData.hpp"

// extern thread_local char egwLogName[32];

HttpRequest::HttpRequest() {
}

HttpRequest::~HttpRequest() {

}

bool HttpRequest::Send(EGWTcpClient * _client) {
    out_.clear();

    out_.append(method_).append(" ")
        .append(path_).append(" ")
        .append(version_).append("\r\n");

    for(auto & item : headers_) {
        out_.append(item.first).append(": ").append(item.second)
            .append("\r\n");
    }

    if(body_.empty() == false) {
        out_.append("\r\n").append(body_);
    }

    // _sock 이 이상하면, 연결하면서 해야 해요.
    if(_client->Send(out_.c_str(), out_.length()) == false) {
        _client->Close();

        if(_client->Connect() == false) {
            W_THD_LOG(egwLogName,"Client Connection fail [%s:%d]",
                _client->GetIp(),
                _client->GetPort());
            return false;
        }

        if(_client->Send(out_.c_str(), out_.length()) == false) {
            W_THD_LOG(egwLogName,"Send fail [%s]", out_.c_str());
            _client->Close();
            return false;
        }
    }

    return true;
}

bool HttpRequest::Receive(EGWSocket * _sock) {

    if(HttpData::ReadUntilSpace(method_, _sock) == false) {
        W_THD_LOG(egwLogName,"method read fail");
        _sock->Close();
        return false;
    }

    if(HttpData::ReadUntilSpace(path_, _sock) == false) {
        W_THD_LOG(egwLogName,"path read fail");
        _sock->Close();
        return false;
    }

    if(HttpData::ReadUntilSpace(version_, _sock) == false) {
        W_THD_LOG(egwLogName,"version read fail");
        _sock->Close();
        return false;
    }

    std::string     dummy;
    if(HttpData::ReadUntilCRLF(dummy, _sock) == false) {
        return true;
    }

    if(HttpData::ReadHeaders(headers_, _sock) == false) {
        W_THD_LOG(egwLogName,"headers read fail");
        _sock->Close();
        return false;
    }

    size_t contentLength = HttpData::GetContentLength(headers_);

    if(HttpData::ReadData(body_, _sock, contentLength) == false) {
        W_THD_LOG(egwLogName,"body read fail");
        _sock->Close();
        return false;
    }

    return true;
}

const char * HttpRequest::GetHeader(const char * _header) {
    std::string     h(_header);
    std::transform(h.begin(), h.end(), h.begin(), ::toupper);

    auto iter = headers_.find(h);

    if(iter == headers_.end())
        return nullptr;

    return (iter->second).c_str();
}

/*-
bool HttpRequest::readData(std::string & _out, EGWSocket * _sock, size_t _length) {

    auto p = std::unique_ptr<char[]>(new char[_length+1]);

    if(_sock->Recv(p.get(), _length) != (int)_length)
        return false;

    p[_length] = '\0';

    _out = p.get();
    return true;
}

size_t HttpRequest::GetContentLength() {
    auto iter = headers_.find("CONTENT-LENGTH");

    if(iter == headers_.end())
        return 0;

    return std::stoull(iter->second);
}

bool HttpRequest::readUntilSpace(std::string & _out, EGWSocket * _sock) {

    char c;

    while(true) {
        if(_sock->Recv(&c, 1) != 1)
            return false;

        if(_out.length() > 0 && (c == ' ' || c == '\t' || c == '\r' || c == '\n'))
            break;

        if(c == ' ' || c == '\t' || c == '\r' || c == '\n')
            continue;

        if(c == '\0')
            return false;

        _out.append(1, c);
    }

    _out = EGWCommon::trim(_out);
    return true;
}

bool HttpRequest::readUntilCRLF(std::string & _out,
                                EGWSocket * _sock) {

    char c;

    while(true) {
        if(_sock->Recv(&c, 1) != 1)
            return false;

        if(c == '\n' || (_out.length() > 0 && _out.back() == '\r'))
            break;

        _out.append(1, c);
    }

    _out = EGWCommon::trim(_out);
    return true;
}

bool HttpRequest::readHeaders(EGWSocket * _sock) {

    while(true) {

        std::string     line;

        if(readUntilCRLF(line, _sock) == false) {
            W_THD_LOG(egwLogName,"read until CRLF [%s]", line.c_str());
            return false;
        }

        if(line.length() == 0)
            break;

        std::tuple<std::string, std::string> tup = EGWCommon::split(line, ":");

        if(std::get<0>(tup).length() == 0 || std::get<1>(tup).length() == 0) {
            W_THD_LOG(egwLogName,"read header [%s]", line.c_str());
            return false;
        }

        std::string key = std::get<0>(tup);
        std::transform(key.begin(), key.end(), key.begin(), ::toupper);

        headers_.emplace(key, std::get<1>(tup));
    }

    return true;
}

std::string HttpRequest::trim(std::string & _out) {

    const std::string & chars = "\t\n\v\f\r ";
    _out.erase(_out.find_last_not_of(chars)+1);
    _out.erase(0, _out.find_first_not_of(chars));
    return _out;
}

std::tuple<std::string, std::string> HttpRequest::splite(std::string & _line, const std::string _sep) {

  std::size_t found = _line.find_first_of(_sep);

  if(found == std::string::npos) {
    std::string out = trim(_line);
    std::transform(out.begin(), out.end(), out.begin(), ::toupper);
    return std::tuple<std::string, std::string>(out, "");
  }

  std::string first = _line.substr(0, found);
  std::string second = _line.substr(found+1);

  first = trim(first);
  std::transform(first.begin(), first.end(), first.begin(), ::toupper);
  second = trim(second);

  return std::tuple<std::string, std::string>(first, second);

}
-*/
