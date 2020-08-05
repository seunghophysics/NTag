#ifndef NTAGZBS_HH
#define NTAGZBS_HH 1

#include "NTagIO.hh"

class NTagZBS : public NTagIO
{
    public:
        NTagZBS(const char* fileName, bool useData, unsigned int verbose);
        ~NTagZBS();
        
        // Initialize
        void Initialize();
        
         // File I/O
        void OpenFile(const char* fileName);
        void ReadFile();

        // Tree-related
        //void CreateBranchesToTruthTree();
        //void CreateBranchesToNTvarTree();
        //void SetBranchAddressToTruthTree(TTree* tree);
        //void SetBranchAddressToNTvarTree(TTree* tree);

    private:
        // File I/O (logical unit)
        int lun;
    
};

#endif