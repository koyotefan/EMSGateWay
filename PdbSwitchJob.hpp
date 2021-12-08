#ifndef PDB_SWITCH_HPP
#define PDB_SWITCH_HPP

#include <vector>

#include "NetconfPdbInfo.hpp"
#include "EGWJob.hpp"

class PdbSwitchJob : public EGWJob {
public:
    PdbSwitchJob() {}
    ~PdbSwitchJob() {}

    bool Init();
    bool Do(HttpRequest & _req, HttpResponse & _resp);

private:
    void setActiveNames(std::vector<std::string> & _vec, std::string & _body);

private:

    NetconfPdbInfo  netconfInfo_;
};


#endif // PDB_SWITCH_JOB_HPP
