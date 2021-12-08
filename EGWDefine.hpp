
#ifndef EGW_DEFINE_HPP
#define EGW_DEFINE_HPP

#ifdef __T_DEBUG
    #include <cstring>
    #define I_THD_LOG(egwLogName,msg, ...)     printf("[%16s:%04d]" msg "\n", basename(__FILE__),__LINE__, ##
    #define D_THD_LOG(egwLogName,msg, ...)     printf("[%16s:%04d]" msg "\n", basename(__FILE__),__LINE__, ##
    #define W_THD_LOG(egwLogName,msg, ...)     printf("[%16s:%04d]" msg "\n", basename(__FILE__),__LINE__, ##
    #define E_THD_LOG(egwLogName,msg, ...)     printf("[%16s:%04d]" msg "\n", basename(__FILE__),__LINE__, ##
#else
    #include "NDFServiceLog.hpp"
#endif

#include <tuple>
#include <vector>

class EGWCommon {

public:
    explicit EGWCommon() = delete;
    ~EGWCommon() = delete;

    static bool IsLB(std::string & _sysName) {
        return (_sysName.find("mp") == std::string::npos && 
        _sysName.find("MP") == std::string::npos)?true:false; 
    }

    static std::string trim(std::string & _out);
    static std::tuple<std::string, std::string> split(std::string & _line, const std::string _sep); 
    static size_t split(std::vector<std::string> & _vec, const std::string & _line, const std::string _sep); 

};



#endif // EGW_DEFINE_HPP
