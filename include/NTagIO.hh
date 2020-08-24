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

            /* format-specific */
            virtual void OpenFile() {}
            virtual void CloseFile() {}
            virtual void SetFitInfo() {}

            /* format-independent */
            virtual void SKInitialize();
            virtual void ReadFile();
            virtual void ReadEvent();
            virtual void ReadMCEvent();
            virtual void ReadDataEvent();
            virtual void ReadSHEEvent();
            virtual void ReadAFTEvent();
            virtual void WriteOutput();

            virtual void DoWhenInterrupted();
            static void  SIGINTHandler(int sigNo);

        // Tree-related
        virtual void CreateBranchesToTruthTree();
        virtual void CreateBranchesToNTvarTree();
        //virtual void SetBranchAddressToTruthTree(TTree* tree);
        //virtual void SetBranchAddressToNTvarTree(TTree* tree);

        virtual void CheckMC();

    protected:
        const char* fInFileName;
        const char* fOutFileName;
        int         nProcessedEvents;
        int         lun;

        TTree*      truthTree;
        TTree*      ntvarTree;

    private:
        static NTagIO* instance;
};

#endif
