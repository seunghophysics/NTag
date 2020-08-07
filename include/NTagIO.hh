#ifndef NTAGIO_HH
#define NTAGIO_HH 1

#include "NTagEventInfo.hh"

class NTagIO : public NTagEventInfo
{
    public:
        NTagIO(const char* inFileName, const char* outFileName, bool useData, unsigned int verbose);
        ~NTagIO();
        
        // Initialize
        virtual void Initialize();
        
        // File I/O
        virtual void OpenFile() = 0;
        virtual void ReadFile() = 0;
        virtual void ReadEvent() = 0;
        virtual void WriteOutput();
        
        // Tree-related
        virtual void CreateBranchesToTruthTree();
        virtual void CreateBranchesToNTvarTree();
        //virtual void SetBranchAddressToTruthTree(TTree* tree);
        //virtual void SetBranchAddressToNTvarTree(TTree* tree);

    protected:
        const char* fInFileName;
        const char* fOutFileName;
        
        TTree*      truthTree;
        TTree*      ntvarTree;
};

#endif
