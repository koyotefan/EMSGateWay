#ifndef Http_Data_HPP
#define Http_Data_HPP

#include <unordered_map>
#include "EGWSocket.hpp"

class HttpData {
public:
    HttpData() = delete;
    ~HttpData() = delete;

    static bool ReadData(std::string & _out, EGWSocket * _sock, size_t _length);
    static size_t GetContentLength(std::unordered_map<std::string, std::string> & _headers);
    static bool ReadUntilSpace(std::string & _out, EGWSocket * _sock);
    static bool ReadUntilCRLF(std::string & _out, EGWSocket * _sock);
    static bool ReadHeaders(std::unordered_map<std::string, std::string> & _headers, EGWSocket * _sock);


};

#endif