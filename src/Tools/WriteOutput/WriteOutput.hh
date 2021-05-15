#ifndef WRITEOUTPUT_HH
#define WRITEOUTPUT_HH

class TFile;
class TTree;

#include "TSysEvtHandler.h"
#include "Tool.hh"

class TInterruptHandler : public TSignalHandler
{
   public:
        TInterruptHandler(Tool* tool):TSignalHandler(kSigInterrupt, kFALSE)
        { WriteOutput = tool; }

        virtual Bool_t Notify()
        {
            std::cerr << "Received SIGINT. Writing output..." << std::endl;
            WriteOutput->Finalize();
            _exit(2);
            return kTRUE;
        }

    private:
        Tool* WriteOutput;
};

class WriteOutput : public Tool
{
    public:
        WriteOutput() { name = "WriteOutput"; }

        bool Initialize();
        bool Execute();
        bool Finalize();

        bool CheckSafety();

    private:
        TFile* outFile;
        TTree* candidateTree;

        TInterruptHandler* handler;
};

#endif