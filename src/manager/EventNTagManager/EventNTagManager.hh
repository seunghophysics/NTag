#ifndef EVENTNTAGMANAGER_HH
#define EVENTNTAGMANAGER_HH

#include "SKLibs.hh"
#include "SKIO.hh"
#include "PMTHitCluster.hh"
#include "ParticleCluster.hh"
#include "TaggableCluster.hh"
#include "CandidateCluster.hh"
#include "TRMSFitManager.hh"
#include "BonsaiManager.hh"
#include "NTagTMVAManager.hh"
#include "Printer.hh"
#include "Store.hh"
#include "NTagGlobal.hh"

class NoiseManager;

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
        void AddNoise();
        void ReadParticles();
        void ReadEarlyCandidates();

        // read
        void ReadInfoFromCommon();
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

        // settings
        void ApplySettings();
        void ReadArguments(const ArgParser& argParser);

        // root
        void MakeTrees(TFile* outFile=nullptr);
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
        void SetNoiseManager(NoiseManager* noiseManager) { fNoiseManager = noiseManager; }
        void SetOutDataFile(SKIO* outfile) { fOutDataFile = outfile; }
        void SetHits(const PMTHitCluster& cluster) { fEventHits = cluster; }
        void SetTaggables(const TaggableCluster& cluster) { fEventTaggables = cluster; }
        void SetEarlyCandidates(const CandidateCluster& cluster) { fEventEarlyCandidates = cluster; }
        void SetCandidates(const CandidateCluster& cluster) { fEventCandidates = cluster; }
        
        // MC taggable mapping
        static void Map(TaggableCluster& taggableCluster, CandidateCluster& candidateCluster, float tMatchWindow=200);
        static void ResetTaggableMapping(TaggableCluster& taggableCluster);
        static void ResetCandidateClass(CandidateCluster& candidateCluster);
        // tagged type for taggable
        static void SetTaggedType(Taggable& taggable, Candidate& candidate);

    private:
        // check if MC
        void CheckMC();
    
        // ToF subtraction
        void PrepareEventHitsForSearch();
        //void SetToF(const TVector3& vertex);
        //void UnsetToF();

        // read vertex mode from key
        void SetVertexMode(VertexMode& mode, std::string key);

        // delayed vertex fit and max hit search
        void FindDelayedCandidate(unsigned int iHit);

        // feature extraction
        void FindFeatures(Candidate& candidate);

        // tag class for candidate
        //int FindTagClass(const Candidate& candidate);

        // prune duplicate candidates with TagClass==e and same FitT
        // among early and delayed candidates
        void PruneCandidates();

        // zbs common filling
        void FillNTagCommon();
        
        // output data file
        SKIO* fOutDataFile;
        
        // noise manager
        NoiseManager* fNoiseManager;

        // data models
        Store fEventVariables;
        PMTHitCluster fEventHits;
        ParticleCluster fEventParticles;
        TaggableCluster fEventTaggables;
        CandidateCluster fEventCandidates;
        CandidateCluster fEventEarlyCandidates;
        TVector3 fPromptVertex;

        // NTag settings
        Store fSettings;
        VertexMode fPromptVertexMode, fDelayedVertexMode;
        float PVXRES, PVXBIAS;
        float T0TH, T0MX, TWIDTH, TCANWIDTH, TMINPEAKSEP, TMATCHWINDOW;
        int NHITSTH, NHITSMX, N200TH, N200MX, MINNHITS, MAXNHITS;
        float TRMSTWIDTH, INITGRIDWIDTH, MINGRIDWIDTH, GRIDSHRINKRATE, VTXMAXRADIUS;
        float E_NHITSCUT, E_TIMECUT, TAGOUTCUT;
        float SCINTCUT, GOODNESSCUT, DIRKSCUT, DISTCUT, ECUT;

        // delayed vertex fitters
        VertexFitManager* fDelayedVertexManager;
        TRMSFitManager fTRMSFitManager;
        BonsaiManager fBonsaiManager;

        // TMVA
        NTagTMVAManager fTMVAManager;

        // Tagger
        CandidateTagger fTagger;

        // ROOT
        std::string fOutFilePath;

        // utilities
        Printer fMsg;

        // booleans
        bool fIsBranchSet, fIsMC;
        FileFormat fFileFormat;
};

#include "TSysEvtHandler.h"

class TInterruptHandler : public TSignalHandler
{
   public:
        TInterruptHandler(EventNTagManager* manager)
        : TSignalHandler(kSigInterrupt, kFALSE)
        { fNTagManager = manager; }

        virtual Bool_t Notify()
        {
            std::cerr << "Received SIGINT. Writing output..." << std::endl;
            fNTagManager->WriteTrees(true);
            int lun = 10; skclosef_(&lun);
                lun = 20; skclosef_(&lun);

            _exit(2);
            return kTRUE;
        }

    private:
        EventNTagManager* fNTagManager;
};

#endif