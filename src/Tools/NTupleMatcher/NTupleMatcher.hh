#ifndef NTUPLEMATCHER_HH
#define NTUPLEMATCHER_HH

#include "Tool.hh"

class TFile;
class TTree;

class NTupleReader;

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
        void SetNTupleVariables();
        
        TFile* ntupleFile;
        TTree* ntuple;
        
        NTupleReader* ntupleReader;
        
        int iEntry, nEntries;
        int eventNo;
};

#endif