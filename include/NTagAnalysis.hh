#ifndef NTAGANALYSIS_HH
#define NTAGANALYSIS_HH 1

#include "skparmC.h"
#include "apmringC.h"
//#include "apflscndprtC.h"

#include <TMVA/Reader.h>
#include "NTagEventInfo.hh"

class NTagAnalysis : public NTagEventInfo
{
    public:
        NTagAnalysis();
        virtual ~NTagAnalysis();

        // File I/O
		void OpenFile(const char* fileName);
		void ReadFile();
		void ReadEvent();

        // Event handling
		virtual void Clear();
		virtual void SetEventHeader();
		virtual void SetTruthInfo();
		virtual void SetTQ();
		virtual void SearchNeutron();

		// Tree-related
		virtual void SetBranchAddressOfTree(TTree* tree, bool forTruth);
		virtual void CreateBranchesToTree(TTree* tree, bool forTruth);

        // Calculations
		float GetDWall(float& x, float& y, float& z);

    private:
        TMVA::Reader* reader;
        TTree* truthTree;
        TTree* ntvarTree;
        bool bData;       
};

#endif