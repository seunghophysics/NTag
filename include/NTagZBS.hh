#ifndef NTAGZBS_HH
#define NTAGZBS_HH 1

#include "NTagIO.hh"

class NTagZBS : public NTagIO
{
    public:
        NTagZBS(const char* inFileName, const char* outFileName="out/NTagOut.root",
                Verbosity verbose=pDEFAULT);
        ~NTagZBS();

         // File I/O
        void OpenFile();
        void CloseFile();

        void SetFitInfo();
};

#endif