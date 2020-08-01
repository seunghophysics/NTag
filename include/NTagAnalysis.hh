#ifndef NTAGANALYSIS_HH
#define NTAGANALYSIS_HH 1

#include <TMVA/Reader.h>
#include "NTagEventInfo.hh"

class NTagAnalysis : public NTagEventInfo
{
    public:
        NTagAnalysis(const char* fileName, bool useData=false, unsigned int verbose=vDefault);
        virtual ~NTagAnalysis();

        // File I/O
		void OpenFile(const char* fileName);
		void ReadFile();
		void ReadEvent();

        // TMVA
		virtual void SearchNeutron();

		// Tree-related
		virtual void CreateBranchesToTruthTree();
		virtual void CreateBranchesToNTvarTree();
		//virtual void SetBranchAddressToTruthTree(TTree* tree);
		//virtual void SetBranchAddressToNTvarTree(TTree* tree);

    private:
        TMVA::Reader* reader;
        TTree* truthTree;
        TTree* ntvarTree;
        bool bData;

		// File I/O (logical unit)
		int lun;
};

#endif