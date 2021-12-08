#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <poll.h>

#include "EGWDefine.hpp"
#include "FileMonitor.hpp"

extern thread_local char egwLogName[32];

FileMonitor::FileMonitor() 
    : lastModifyT_(0),
      bDuplicatedLog_(false) {

}

FileMonitor::FileMonitor(const std::string & _fileName) 
    : fileName_(_fileName), 
      lastModifyT_(0),
      bDuplicatedLog_(false) {

}

FileMonitor::FileMonitor(const std::string && _fileName) 
    : fileName_(_fileName), 
      lastModifyT_(0),
      bDuplicatedLog_(false) {

}

bool FileMonitor::IsChanged() {
    // 파일이 내가 알고 있는 변경시각과 동일한지 확인 합니다.
    // 만약 다르면, true 를 리턴하고, lastModifyT_ 를 갱신합니다.
    if(fileName_.empty())
        return false;

    time_t  mT;
    if(getLastModifyTime(mT, fileName_) == false)
        return false;

    if(mT != lastModifyT_) {
        poll(nullptr, 0, 50); // 왜냐하면, 혹시나 쓰기작업이 완료되지 않았을 까봐 무서워서요.
        lastModifyT_ = mT;
        return true;
    }

    return false;
}

bool FileMonitor::getLastModifyTime(time_t & _outT, const std::string & _fileName) {

    struct stat stBuf;
    if(lstat(_fileName.c_str(), &stBuf) != 0) {

        if(bDuplicatedLog_ == false) {
            char    errBuf[128];
            W_THD_LOG(egwLogName,"stat() fail [%s] [%d:%s]", 
                _fileName.c_str(), errno, strerror_r(errno, errBuf, sizeof(errBuf)));
            bDuplicatedLog_ = true; 
        }
        return false;
    }

    _outT = stBuf.st_mtime;
    return true;
}

bool FileMonitor::SetFileName(const std::string _fileName) {

    // 파일이 존재하는지 확인하고, 존재하면 값을 설정 합니다.
    time_t  mT;
    if(getLastModifyTime(mT, _fileName) == false)
        return false;

    fileName_       = _fileName;
    lastModifyT_    = mT;

    return true;
}

/*-
bool FileMonitor::SetFileName(std::string && _fileName) {
    return SetFileName(_fileName);
}

bool FileMonitor::IsChanged() {
    return IsChanged(fileName_);
}

bool FileMonitor::IsChanged(std::string & _fileName) {

    // 파일이 내가 알고 있는 변경시각과 동일한지 확인 합니다.
    // 만약 다르면, true 를 리턴하고, lastModifyT_ 를 갱신합니다.

    time_t  mT;
    if(getLastModifyTime(mT, _fileName) == false)
        return false;

    if(mT != lastModifyT_) {
        poll(nullptr, 0, 50); // 왜냐하면, 혹시나 쓰기작업이 완료되지 않았을 까봐 무서워서요.
        lastModifyT_ = mT;
        return true;
    }

    return false;
}

bool FileMonitor::getLastModifyTime(time_t & _outT, const std::string & _fileName) {

    struct stat stBuf;
    if(lstat(_fileName.c_str(), &stBuf) != 0) {
        char    errBuf[128];
        W_THD_LOG(egwLogName,"stat() fail [%s] [%d:%s]", 
            _fileName.c_str(), errno, strerror_r(errno, errBuf, sizeof(errBuf)));
        return false;
    }

    _outT = stBuf.st_mtime;
    return true;
}
-*/
