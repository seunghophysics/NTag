#ifndef NTUPLEMATCHER_HH
#define NTUPLEMATCHER_HH

#include "Tool.hh"

class TTree;

class NTupleMatcher : public Tool
{
    public:
        NTupleMatcher() { name = "NTupleMatcher"; }
        
        bool Initialize();
        bool Execute();
        bool Finalize();
        
        bool CheckSafety();
        
    private:
        void GetNewEntry();
        
        TTree* ntuple;
        
        int iEntry, nEntries;
        int eventNo;
        
};

#endif