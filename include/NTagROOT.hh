#ifndef NTAGROOT_HH
#define NTAGROOT_HH 1

#include "NTagIO.hh"

class NTagROOT : public NTagIO
{
    public:
        NTagROOT(const char* inFileName, const char* outFileName="out/NTagOut.root",
                 unsigned int verbose=pDEFAULT);
        ~NTagROOT();

        // File I/O
        void OpenFile();
        void CloseFile();
        
        // Secondary bank/tree I/O
        void ReadSecondaries();
        
        // Fit Info I/O
        void SetFitInfo();
};

#endif