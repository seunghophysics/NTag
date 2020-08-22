#ifndef NTAGZBS_HH
#define NTAGZBS_HH 1

#include "NTagIO.hh"

class NTagZBS : public NTagIO
{
    public:
        NTagZBS(const char* inFileName, const char* outFileName="out/NTagOut.root", 
                bool useData=false, unsigned int verbose=pDEFAULT);
        ~NTagZBS();
        
        // Initialize
        void Initialize();
        
         // File I/O
        void OpenFile();
        void ReadFile();
        void ReadEvent();
        void ReadMCEvent();
        void ReadDataEvent();
        
        // Data I/O
        void ReadSHEEvent();
        void ReadAFTEvent();

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
