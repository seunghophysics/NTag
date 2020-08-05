#ifndef NTAGANALYSIS_HH
#define NTAGANALYSIS_HH 1

#include <TMVA/Reader.h>
#include "NTagEventInfo.hh"

class NTagAnalysis : public NTagEventInfo
{
    public:
        NTagAnalysis(const char* fileName, bool useData=false, unsigned int verbose=pDEFAULT);
        virtual ~NTagAnalysis();

        // File I/O
        virtual void OpenFile(const char* fileName);
        virtual void ReadFile();
        virtual void ReadEvent();
        virtual void WriteOutput();

        // Tree-related
        virtual void CreateBranchesToTruthTree();
        virtual void CreateBranchesToNTvarTree();
        //virtual void SetBranchAddressToTruthTree(TTree* tree);
        //virtual void SetBranchAddressToNTvarTree(TTree* tree);

    private:
        TTree* truthTree;
        TTree* ntvarTree;

        // File I/O (logical unit)
        int lun;
};

#endif