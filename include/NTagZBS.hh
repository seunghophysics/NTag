#ifndef NTAGZBS_HH
#define NTAGZBS_HH 1

#include "NTagIO.hh"

class NTagZBS : public NTagIO
{
    public:
        NTagZBS(const char* inFileName, const char* outFileName="out/NTagOut.root",
                bool useData=false, unsigned int verbose=pDEFAULT);
        ~NTagZBS();

         // File I/O
        void OpenFile();
        void CloseFile();

        void SetFitInfo();

        // Tree-related
        //void CreateBranchesToTruthTree();
        //void CreateBranchesToNTvarTree();
        //void SetBranchAddressToTruthTree(TTree* tree);
        //void SetBranchAddressToNTvarTree(TTree* tree);
};

#endif