
#ifndef __FILE_MONITOR_H__
#define __FILE_MONITOR_H__

#include <string>

class FileMonitor {
public:

    explicit    FileMonitor();
    explicit    FileMonitor(const std::string & _fileName);
    explicit    FileMonitor(const std::string && _fileName);
    ~FileMonitor() = default;

    bool        SetFileName(const std::string _fileName);
    const char * GetFileName() { return fileName_.c_str(); }
    bool        IsChanged();

private:
    bool        getLastModifyTime(time_t & _outT, const std::string & _fileName); 

private:
    std::string     fileName_;
    time_t          lastModifyT_;
    bool            bDuplicatedLog_;
    
};

#endif
