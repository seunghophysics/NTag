#ifndef TREEOUT_HH
#define TREEOUT_HH

#include "TTree.h"

class TreeOut
{
    public:
        TreeOut(): fOutputTree(NULL), fIsOutputTreeSet(false) {}

        // TTree access
        virtual void SetTree(TTree* tree) { fOutputTree = tree; fIsOutputTreeSet = true; }
        TTree* GetTree() { return fOutputTree; }
        virtual void ClearTree() { if (fIsOutputTreeSet) fOutputTree->Reset(); }
        virtual void FillTree() { if (fIsOutputTreeSet) fOutputTree->Fill(); }
        virtual void WriteTree() { if (fIsOutputTreeSet) fOutputTree->Write(); }
        virtual void MakeBranches() {}

    protected:
        TTree* fOutputTree;
        bool fIsOutputTreeSet;
};

#endif