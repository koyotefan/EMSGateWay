#include "HttpClient.hpp"

bool HttpClient::Init(std::string & _ip, int _port) {

    ip_   = _ip;
    port_ = _port;

    client_.SetIp(ip_);
    client_.SetPort(port_);

    return true;
}

bool HttpClient::Request(const std::string & _path, const std::string & _out) {

    req_.SetMethod("POST");

    //char path[64];
    //snprintf(path, "http://%s:%d/node/status/pg", _ip.c_str(), _port);
    //req_.SetPath("/node/status/pg");
    req_.SetPath(_path);

    req_.SetVersion("HTTP/1.1");

    req_.SetHeader("Connection", "Keep-Alive");
    req_.SetHeader("Content-Type", "application/json");
    req_.SetBody(_out.c_str());

    return req_.Send(&client_);
}

bool HttpClient::Response() {

    if(resp_.Receive(&client_) == false) {
        W_THD_LOG(egwLogName, "Response [%s]", resp_.GetStatusLine());
        return false;
    }

    I_THD_LOG(egwLogName, "Response [%s]", resp_.GetStatusLine());
    return true;
}
