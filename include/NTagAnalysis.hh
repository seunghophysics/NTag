#ifndef NTAGGDINFO_HH
#define NTAGGDINFO_HH 1

#include <TMVA/Reader.h>
#include "NTagInfo.hh"

class NTagGdInfo : public NTagInfo
{
    public:
        NTagGdInfo() {}
        ~NTagGdInfo() {}

    private:
        TMVA::Reader* reader;
        
};

#endif