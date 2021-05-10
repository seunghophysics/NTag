#ifndef READMCINFO_HH
#define READMCINFO_HH

#include "Tool.hh"

class ReadMCInfo : public Tool
{
    public:
        ReadMCInfo() { name = "ReadMCInfo"; }

        bool Initialize();
        bool Execute();
        bool Finalize();

        bool CheckSafety();

    private:
        bool inputIsSKROOT;
};

#endif