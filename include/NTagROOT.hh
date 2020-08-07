#ifndef NTAGROOT_HH
#define NTAGROOT_HH 1

#include "NTagIO.hh"

class NTagROOT : public NTagIO
{
    public:
        NTagROOT(const char* inFileName, const char* outFileName="out/NTagOut.root", 
                 bool useData=false, unsigned int verbose=pDEFAULT);
        ~NTagROOT();
        
        // Initialize
        void Initialize();
        
        // File I/O
        void OpenFile();
        void ReadFile();
        void ReadEvent();

        // Tree-related 
        //void CreateBranchesToTruthTree();
        //void CreateBranchesToNTvarTree();
        //virtual void SetBranchAddressToTruthTree(TTree* tree);
        //virtual void SetBranchAddressToNTvarTree(TTree* tree);    
    
    private:
        // File I/O (logical unit)
        int lun; /* remove if unnecessary */
};

#endif
