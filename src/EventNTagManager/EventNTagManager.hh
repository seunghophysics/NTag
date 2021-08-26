#ifndef EVENTNTAGMANAGER_HH
#define EVENTNTAGMANAGER_HH

#include "PMTHitCluster.hh"
#include "ParticleCluster.hh"
#include "DecayECluster.hh"
#include "NCaptureCluster.hh"
#include "Printer.hh"
#include "Store.hh"

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
        
        // set ingredients manually
        void ClearData();
        // void SetHits(PMTHitCluster&);
        // void SetMCParticles(ParticleCluster&);
        // void SetVariables();
        
        // return private members
        const Store& GetVariables() { return fEventVariables; };
        const PMTHitCluster& GetHits() { return fEventHits; };
        const ParticleCluster& GetParticles() { return fEventParticles; }
        const DecayECluster& GetDecayEs() { return fEventDecayEs; }
        const NCaptureCluster& GetNCaptures() { return fEventNCaptures; }

        //const EventCandidates& GetCandidates(const PMTHitCluster& hitCluster);
        //const EventCandidates& GetCandidates();
        
        // setters
        void SetVerbosity(Verbosity verbose) { fMsg.SetVerbosity(verbose); }
        template <typename T>
        void Set(std::string key, T value) { fSettings.Set(key, value); }
        
        // printers
        void DumpSettings() { fSettings.Print(); }

    private:
        // DataModel:
        Store fEventVariables;
        PMTHitCluster fEventHits;
        ParticleCluster fEventParticles;
        DecayECluster fEventDecayEs;
        NCaptureCluster fEventNCaptures;
        // EventCandidates
        
        // NTag settings
        Store fSettings;

        // Utilities
        Printer fMsg;
};

#endif