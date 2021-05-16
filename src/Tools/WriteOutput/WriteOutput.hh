#ifndef WRITEOUTPUT_HH
#define WRITEOUTPUT_HH

class TFile;
class TTree;
class TBranch;

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
        WriteOutput():tmpVec(), tmpNum(0), tmpStr(""), fillCounter(0) 
        { name = "WriteOutput"; }

        bool Initialize();
        bool Execute();
        bool Finalize();

        bool CheckSafety();

    private:
        void MakeBranches(TTree* tree, Store* store);
        void FillBranches(TTree* tree, Store* store);
        void FillTempValuesFromStore(Store* store);
    
        TFile* outFile;
        TTree* variableTree;
        TTree* candidateTree;
        
        // MC
        TTree* primaryTree;
        TTree* secondaryTree;
        TTree* trueCaptureTree;
        
        // NTag
        TTree* ntagInfoTree;
        
        // temporary variables for branching
        TVector3 tmpVec;
        float tmpNum;
        std::string tmpStr;
        
        unsigned long fillCounter;

        TInterruptHandler* handler;
};

#endif