#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <memory>
#include <chrono>

#include "PDBDefine.hpp"
#include "RestrictFileMonitor.hpp"
#include "RestrictSet.hpp"

#include <poll.h>


int main(int argc, char const *argv[])
{
#ifndef __T_DEBUG
    NDF_OPEN_SERVICE_LOG("./", "egwC", 1, 15, 0, 0);
    //NDF_INIT_THD_LOG();
#endif
    if(argc != 4) {

        E_LOG("invalid argument");
        I_LOG("ex) pdbt [nodeName] [procName] [mode]");
        return 0;
    }


    std::string fname = "restrict.data";

    RestrictFileMonitor     fMonitor;
    if(fMonitor.SetFileName(fname) == false) {

        E_LOG("SetFileName() fail");
        return 0;
    }


    RestrictSet     oset;

    RestrictSet     rset;

    stRestrictCondition     item;
    item.sst = "202";
    item.sd  = "00001";
    item.dnn = "5g.sktelecom.com";


    rset.Insert(item);

    item.sst = "202";
    item.sd  = "00002";
    item.dnn = "5g.sktelecom.com";

    rset.Insert(item);

    item.sst = "202";
    item.sd  = "00003";
    item.dnn = "ims";

    rset.Insert(item);

    if(oset.Equal(rset) == false) {
        I_LOG("Changed---- Set");
        oset = rset;

        if(oset.WriteToFile(fname) == false) {
            E_LOG("oset.writeToFile() Fail");
            return 0;
        }

        I_LOG("oset.WriteToFile() Success");
    }

    if(oset.Equal(rset) == true) {
        I_LOG("oset.Equal() Success");
    }

    for(int n=0; n < 10; ++n) {
        if(fMonitor.IsChanged() == true)
            I_LOG("File Changed");

        // poll(nullptr, 0, 3000);
    }

    I_LOG("END");

    return 0;
}
