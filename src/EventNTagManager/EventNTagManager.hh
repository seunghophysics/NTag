#ifndef EVENTNTAGMANAGER_HH
#define EVENTNTAGMANAGER_HH

#include "TMVA/Reader.h"

#include "SKLibs.hh"
#include "PMTHitCluster.hh"
#include "ParticleCluster.hh"
#include "TaggableCluster.hh"
#include "CandidateCluster.hh"
#include "Printer.hh"
#include "Store.hh"

enum VertexMode
{
    mNONE, mAPFIT, mBONSAI, mCUSTOM, mTRUE, mSTMU
};

enum TriggerType
{
    tELSE, tSHE, tAFT
};

class EventNTagManager
{
    public:
        EventNTagManager(Verbosity verbose=pDEFAULT);
        ~EventNTagManager();

        // read ingredients from sk common blocks
        void ReadPromptVertex(VertexMode mode);
        void ReadVariables();
        void ReadHits();
        void AddHits();
        void ReadParticles();
        void ReadEarlyCandidates();

        // read
        void ReadEventFromCommon();
        // process
        void SearchAndFill();
        
        // automatic event processing
        void ProcessEvent();
        void ProcessDataEvent();
        void ProcessFlatEvent();

        // main search function
        void SearchCandidates();

        // MC taggable mapping
        void MapTaggables();
        void ResetTaggableMapping();

        // settings
        void ApplySettings();
        void ReadArguments(const ArgParser& argParser);

        // TMVA
        void InitializeTMVA();
        float GetTMVAOutput(Candidate& candidate);

        // root
        void MakeTrees();
        void FillTrees();
        void WriteTrees(bool doCloseFile=false);

        // clear
        void ClearData();

        // printers
        void DumpSettings() { fSettings.Print(); }
        void DumpEvent();

        // getters
        Store& GetSettings() { return fSettings; };
        Store& GetVariables() { return fEventVariables; };
        PMTHitCluster& GetHits() { return fEventHits; };
        ParticleCluster& GetParticles() { return fEventParticles; }
        TaggableCluster& GetTaggables() { return fEventTaggables; }
        CandidateCluster& GetEarlyCandidates() { return fEventEarlyCandidates; }
        CandidateCluster& GetCandidates() { return fEventCandidates; }

        // setters
        void SetVerbosity(Verbosity verbose) { fMsg.SetVerbosity(verbose); }
        template <typename T>
        void Set(std::string key, T value) { fSettings.Set(key, value); ApplySettings(); }
        void SetHits(const PMTHitCluster& cluster) { fEventHits = cluster; }
        void SetTaggables(const TaggableCluster& cluster) { fEventTaggables = cluster; }
        void SetEarlyCandidates(const CandidateCluster& cluster) { fEventEarlyCandidates = cluster; }
        void SetCandidates(const CandidateCluster& cluster) { fEventCandidates = cluster; }

    private:
        // ToF subtraction
        void SubtractToF();

        // feature extraction
        void FindFeatures(Candidate& candidate);

        // MC taggable mapping
        void MapCandidateClusters(CandidateCluster& candidateCluster);

        // tag class for candidate
        int FindTagClass(const Candidate& candidate);

        // tagged type for taggable
        void SetTaggedType(Taggable& taggable, Candidate& candidate);

        // duplicate candidate pruning
        void PruneCandidates();

        // zbs common filling
        void FillNTagCommon();

        // DataModel:
        Store fEventVariables;
        PMTHitCluster fEventHits;
        ParticleCluster fEventParticles;
        TaggableCluster fEventTaggables;
        CandidateCluster fEventCandidates;
        CandidateCluster fEventEarlyCandidates;
        TVector3 fPromptVertex;

        // NTag settings
        Store fSettings;
        VertexMode fVertexMode;
        float PVXRES;
        float T0TH, T0MX, TWIDTH, TMINPEAKSEP, TMATCHWINDOW;
        int NHITSTH, NHITSMX, N200TH, N200MX;
        float INITGRIDWIDTH, MINGRIDWIDTH, GRIDSHRINKRATE, VTXSRCRANGE;
        float E_N50CUT, E_TIMECUT, N_OUTCUT;

        // TMVA
        TMVA::Reader fTMVAReader;
        std::map<std::string, float> fFeatureContainer;
        int fCandidateLabel;

        // ROOT
        std::string fOutFilePath;

        // Utilities
        Printer fMsg;

        // booleans
        bool fIsBranchSet, fIsInputSKROOT, fIsMC, fDoUseECut;
};

#include "TSysEvtHandler.h"

class TInterruptHandler : public TSignalHandler
{
   public:
        TInterruptHandler(EventNTagManager* manager):TSignalHandler(kSigInterrupt, kFALSE)
        { fNTagManager = manager; }

        virtual Bool_t Notify()
        {
            std::cerr << "Received SIGINT. Writing output..." << std::endl;
            fNTagManager->WriteTrees(true);
            int lun = 20; skclosef_(&lun);
            
            _exit(2);
            return kTRUE;
        }

    private:
        EventNTagManager* fNTagManager;
};

#endif