
#include <string>
#include <poll.h>

#include "EGWDefine.hpp"
#include "NetstatReporter.hpp"

NetstatReporter::NetstatReporter() {

}


bool NetstatReporter::Init() {

    return true;
}

void NetstatReporter::run(std::shared_ptr<bool>   _pRefCounter) {

    I_THD_LOG(egwLogName,"NetstatReporter in service");

    (void)_pRefCounter;

    while(bRunf_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        // D_THD_LOG(egwLogName,"NetstatReporter running");
    }

    W_THD_LOG(egwLogName,"NetstatReporter out of srvice");
}
