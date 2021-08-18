#ifndef EVENTNTAGMANAGER_HH
#define EVENTNTAGMANAGER_HH

#include "PMTHitCluster.hh"
#include "ParticleCluster.hh"
#include "Printer.hh"

class EventNTagManager
{
    public:
        EventNTagManager(Verbosity verbose=pDEFAULT);
        ~EventNTagManager();
        
        // read ingredients from sk common blocks
        void ReadHits();
        // void ReadMCParticles();
        // void ReadVariables();
        void ReadEventFromCommon();
        
        // set ingredients manually
        // void SetHits(PMTHitCluster&);
        // void SetMCParticles(ParticleCluster&);
        // void SetVariables();
        
        // return private members
        // EventHits& GetHits();
        // ParticleCluster& GetMCParticles();
        // EventVariables& GetVariables();

        //const EventCandidates& GetCandidates(const PMTHitCluster& hitCluster);
        //const EventCandidates& GetCandidates();
        
        // setters
        void SetVerbosity(Verbosity verbose) { fMsg.SetVerbosity(verbose); }
        
        

    private:
        // DataModel:
        PMTHitCluster fEventHits;
        ParticleCluster fEventParticles;
        // EventVariables (trigger settings, prompt vertex, etc.)
        // EventCandidates
        
        // NTag settings
        
        // Utilities
        Printer fMsg;
};

#endif