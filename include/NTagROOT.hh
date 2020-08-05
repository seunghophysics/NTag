#ifndef NTAGROOT_HH
#define NTAGROOT_HH 1

#include "NTagIO.hh"

class NTagROOT : public NTagIO
{
    public:
        NTagROOT(const char* fileName, bool useData, unsigned int verbose);
        ~NTagROOT();
        
        // Initialize
        void Initialize();
        
        // File I/O
        void OpenFile(const char* fileName);
        void ReadFile();

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