
#include <memory>
#include <algorithm>

#include "EGWDefine.hpp"
#include "HttpData.hpp"

bool HttpData::ReadData(std::string & _out, EGWSocket * _sock, size_t _length) {

    auto p = std::unique_ptr<char[]>(new char[_length+1]);

    if(_sock->Recv(p.get(), _length) != (int)_length)
        return false;

    p[_length] = '\0';

    _out = p.get();
    return true;
}

size_t HttpData::GetContentLength(std::unordered_map<std::string, std::string> & _headers) {
    auto iter = _headers.find("CONTENT-LENGTH");

    if(iter == _headers.end())
        return 0;

    return std::stoull(iter->second);
}

bool HttpData::ReadUntilSpace(std::string & _out, EGWSocket * _sock) {

    _out.clear();

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

bool HttpData::ReadUntilCRLF(std::string & _out,
                                EGWSocket * _sock) {

    _out.clear();

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

bool HttpData::ReadHeaders(std::unordered_map<std::string, std::string> & _headers, EGWSocket * _sock) {

    _headers.clear();

    while(true) {

        std::string     line;

        if(HttpData::ReadUntilCRLF(line, _sock) == false) {
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

        _headers.emplace(key, std::get<1>(tup));
    }

    return true;
}

