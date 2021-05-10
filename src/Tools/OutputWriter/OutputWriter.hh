#ifndef OUTPUTWRITER_HH
#define OUTPUTWRITER_HH

class TFile;
class TTree;

#include "TSysEvtHandler.h"
#include "Tool.hh"

class TInterruptHandler : public TSignalHandler
{
   public:
        TInterruptHandler(Tool* tool):TSignalHandler(kSigInterrupt, kFALSE)
        { outputWriter = tool; }

        virtual Bool_t Notify()
        {
            std::cerr << "Received SIGINT. Writing output..." << std::endl;
            outputWriter->Finalize();
            _exit(2);
            return kTRUE;
        }

    private:
        Tool* outputWriter;
};

class OutputWriter : public Tool
{
    public:
        OutputWriter() { name = "OutputWriter"; }

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