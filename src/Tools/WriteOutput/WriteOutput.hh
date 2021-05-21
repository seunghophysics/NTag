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
        WriteOutput():tmpVec(), tmpNum(0), tmpStr(""), fillCounter(0), inputIsMC(false)
        { name = "WriteOutput"; }

        virtual bool Initialize();
        virtual bool Execute();
        virtual bool Finalize();

        virtual bool CheckSafety();

    private:
        void CreateTrees();
        bool CheckTreesExist();
        void GetTrees();
        void PrintTrees();
        void WriteTrees(int option);
    
        void MakeBranches(TTree* tree, Store* store);
        void SetBranches(TTree* tree, Store* store);
        void FillBranches(TTree* tree, Store* store);
        
        void MakeParticleTrees(TTree* particleTree, EventParticles* eventParticles);
        void MakeTrueCaptureTrees(TTree* particleTree, EventTrueCaptures* eventTrueCaptures);
    
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
        TVector3 tmpVec; TVector3* tmpVecPtr;
        float tmpNum;
        std::string tmpStr; std::string* tmpStrPtr;
        
        bool inputIsMC;
        TString outputMode;
        unsigned long fillCounter;

        TInterruptHandler* handler;
};

#endif