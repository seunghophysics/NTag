#ifndef EVENTNTAGMANAGER_HH
#define EVENTNTAGMANAGER_HH

#include "PMTHitCluster.hh"
#include "ParticleCluster.hh"
#include "DecayECluster.hh"
#include "NCaptureCluster.hh"
#include "CandidateCluster.hh"
#include "Printer.hh"
#include "Store.hh"

enum VertexMode
{
    mNONE,
    mAPFIT,
    mBONSAI,
    mCUSTOM,
    mTRUE,
    mSTMU
};

enum TriggerType
{
    tELSE,
    tSHE,
    tAFT
};
class EventNTagManager
{
    public:
        EventNTagManager(Verbosity verbose=pDEFAULT);
        ~EventNTagManager();
        
        // read ingredients from sk common blocks
        void ReadVariables();
        void ReadHits();
        void ReadParticles();
        void ReadEventFromCommon();
        
        void SearchCandidates();
        
        // set ingredients manually
        void ClearData();
        void ApplySettings();
        // void SetHits(PMTHitCluster&);
        // void SetMCParticles(ParticleCluster&);
        // void SetVariables();
        
        // return private members
        const Store& GetVariables() { return fEventVariables; };
        const PMTHitCluster& GetHits() { return fEventHits; };
        const ParticleCluster& GetParticles() { return fEventParticles; }
        const DecayECluster& GetDecayEs() { return fEventDecayEs; }
        const NCaptureCluster& GetNCaptures() { return fEventNCaptures; }
        const CandidateCluster& GetCandidates() { return fEventCandidates; }

        //const CandidateCluster& GetCandidates(const PMTHitCluster& hitCluster);
        
        // setters
        void SetVerbosity(Verbosity verbose) { fMsg.SetVerbosity(verbose); }
        template <typename T>
        void Set(std::string key, T value) { fSettings.Set(key, value); ApplySettings(); }
        
        // printers
        void DumpSettings() { fSettings.Print(); }

    private:
        void SubtractToF();
        void FindFeatures(Candidate& candidate);
    
        // DataModel:
        Store fEventVariables;
        PMTHitCluster fEventHits;
        ParticleCluster fEventParticles;
        DecayECluster fEventDecayEs;
        NCaptureCluster fEventNCaptures;
        CandidateCluster fEventCandidates;
        
        // NTag settings
        Store fSettings;
        float T0TH, T0MX, TWIDTH, TMINPEAKSEP, TMATCHWINDOW;
        int NHITSTH, NHITSMX, N200TH, N200MX;
        float INITGRIDWIDTH, MINGRIDWIDTH, GRIDSHRINKRATE, VTXSRCRANGE;

        // Utilities
        Printer fMsg;
};

#endif