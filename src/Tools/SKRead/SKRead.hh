#ifndef SKREAD_HH
#define SKREAD_HH

#include "Tool.hh"

enum ReadStatus
{
    readOK,
    readError,
    readEOF
};

class SKRead : public Tool
{
    public:
        SKRead() { name = "SKRead"; }

        bool Initialize();
        bool Execute();
        bool Finalize();

        bool CheckSafety();

        inline int GetReadStatus() { return readStatus; }

    private:
        bool inputIsSKROOT;
        int readStatus;
};

#endif