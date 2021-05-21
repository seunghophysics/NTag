#ifndef NTUPLEMATCHER_HH
#define NTUPLEMATCHER_HH

#include "Tool.hh"

class TFile;
class TTree;

class NTupleMatcher : public Tool
{
    public:
        NTupleMatcher() { name = "NTupleMatcher"; }
        
        virtual bool Initialize();
        virtual bool Execute();
        virtual bool Finalize();
        
        virtual bool CheckSafety();
        
    private:
        void GetNewEntry();
        
        TFile* ntupleFile;
        TTree* ntuple;
        
        int iEntry, nEntries;
        int eventNo;
};

#endif