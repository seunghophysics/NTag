#ifndef NTAGIO_HH
#define NTAGIO_HH 1

#include "NTagEventInfo.hh"

class NTagIO : public NTagEventInfo
{
    public:
        NTagIO(const char* fileName, bool useData=false, unsigned int verbose=pDEFAULT);
        NTagIO(const char* ifileName, const char* ofileName, bool useData=false, unsigned int verbose=pDEFAULT);
        ~NTagIO();
        
        // Initialize
        virtual void Initialize();
        
        // File I/O
        virtual void OpenFile(const char* fileName) = 0;
        virtual void OpenFile(const char* infileName, const char* ofilename) = 0;
        virtual void ReadFile() = 0;
        virtual void ReadEvent();
        virtual void WriteOutput();

        // Tree-related
        virtual void CreateBranchesToTruthTree();
        virtual void CreateBranchesToNTvarTree();
        //virtual void SetBranchAddressToTruthTree(TTree* tree);
        //virtual void SetBranchAddressToNTvarTree(TTree* tree);

    protected:
        const char* fInFileName;
        const char* fOutFileName;
        
        TTree* truthTree;
        TTree* ntvarTree;
};

#endif
