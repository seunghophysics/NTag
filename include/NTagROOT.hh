#ifndef NTAGROOT_HH
#define NTAGROOT_HH 1

#include "NTagIO.hh"

class NTagROOT : public NTagIO
{
    public:
        NTagROOT(const char* iFileName, const char* oFileName, bool useData, unsigned int verbose);
        ~NTagROOT();
        
        // Initialize
        void Initialize();
        
        // File I/O
        void OpenFile(const char* iFileName);
        void OpenFile(const char* iFileName, const char* oFileName);
        void ReadFile();
        void ReadEvent();
        void SetLowFitInfo();

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
