#ifndef EVENTNTAGMANAGER_HH
#define EVENTNTAGMANAGER_HH

#include "TMVA/Reader.h"

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
        void ReadVariables();
        void ReadHits();
        void ReadPromptVertex(VertexMode mode);
        void AddHits();
        void ReadParticles();
        void ReadEarlyCandidates();
        void ReadEventFromCommon();

        void SearchCandidates();
        void MapTaggables();
        void ResetTaggableMapping();

        // set ingredients manually
        void ClearData();
        void ApplySettings();
        void ReadArguments(const ArgParser& argParser);
        void InitializeTMVA();
        // void SetHits(PMTHitCluster&);
        // void SetMCParticles(ParticleCluster&);
        // void SetVariables();

        // return private members
        Store& GetSettings() { return fSettings; };
        Store& GetVariables() { return fEventVariables; };
        PMTHitCluster& GetHits() { return fEventHits; };
        ParticleCluster& GetParticles() { return fEventParticles; }
        TaggableCluster& GetTaggables() { return fEventTaggables; }
        CandidateCluster& GetEarlyCandidates() { return fEventEarlyCandidates; }
        CandidateCluster& GetCandidates() { return fEventCandidates; }
        void SetHits(const PMTHitCluster& cluster) { fEventHits = cluster; }
        void SetTaggables(const TaggableCluster& cluster) { fEventTaggables = cluster; }
        void SetEarlyCandidates(const CandidateCluster& cluster) { fEventEarlyCandidates = cluster; }
        void SetCandidates(const CandidateCluster& cluster) { fEventCandidates = cluster; }

        //const CandidateCluster& GetCandidates(const PMTHitCluster& hitCluster);

        // setters
        void SetVerbosity(Verbosity verbose) { fMsg.SetVerbosity(verbose); }
        template <typename T>
        void Set(std::string key, T value) { fSettings.Set(key, value); ApplySettings(); }

        // tmva
        float GetTMVAOutput(Candidate& candidate);

        // root
        void FillTrees();
        void WriteTrees(bool doCloseFile=false);

        // printers
        void DumpSettings() { fSettings.Print(); }
        void DumpEvent();

    private:
        void SubtractToF();
        void FindFeatures(Candidate& candidate);
        void MapTaggable(Taggable& taggable, int iCandidate, const std::string& key);
        void MapCandidateClusters(CandidateCluster& candidateCluster);
        void PruneCandidates();
        void FillNTagCommon();
        int GetMaxNHitsIndex(PMTHitCluster& hitCluster);
        int FindTagClass(const Candidate& candidate);
        void SetTaggedType(Taggable& taggable, Candidate& candidate);

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
        bool fInputIsSKROOT;
        VertexMode fVertexMode;
        float PVXRES;
        float T0TH, T0MX, TWIDTH, TMINPEAKSEP, TMATCHWINDOW;
        int NHITSTH, NHITSMX, N200TH, N200MX;
        float INITGRIDWIDTH, MINGRIDWIDTH, GRIDSHRINKRATE, VTXSRCRANGE;
        bool fUseECut;
        float E_N50CUT, E_TIMECUT, N_OUTCUT;

        // TMVA
        TMVA::Reader fTMVAReader;
        std::map<std::string, float> fFeatureContainer;
        int fCandidateLabel;

        // ROOT
        std::string fOutFilePath;
        bool fIsBranchSet;

        // Utilities
        Printer fMsg;
        
        // Trigger option
        bool fIsMC;
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
            _exit(2);
            return kTRUE;
        }

    private:
        EventNTagManager* fNTagManager;
};

#endif