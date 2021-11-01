#ifndef TRGSIM_HH
#define TRGSIM_HH

#include <vector>

#include "TTree.h"

#include "tqrealroot.h"

#include "PMTHitCluster.hh"
#include "Printer.hh"

class TrgSim
{
    public:
        TrgSim(Verbosity verbose=pDEFAULT);
        TrgSim(std::string signalFilePath, float sigFreqHz, float tDurationSec, std::string noiseFilePath, unsigned int seed=0, Verbosity verbose=pDEFAULT);
        ~TrgSim();
        
        // setter
        void SetVerbosity(Verbosity verbose) { fVerbosity = verbose; fMsg.SetVerbosity(verbose); }
        void SetSignalFrequency(float sigFreqHz) { fSigFreqHz = sigFreqHz; }
        void SetTimeDuration(float tDurationSec) { fTDurationSec = tDurationSec; }
        void SetRandomSeed(unsigned int seed) { fRandomSeed = seed; }
        void SetSegmentLength(float segLength) { fSegmentLength = segLength; }
        void SetSignalFile(std::string signalFilePath);
        void SetNoiseFile(std::string noiseFilePath);
        
        // signal time generation
        void SetSignalTime(float sigFreqHz, float tDurationSec);
        const std::vector<double>& GetSignalTime() const { return fSignalTime; }
        
        // simulation
        void Simulate();
        
    private:
        void FillSegement();
        
        PMTHitCluster GetSignalHits(double global_t_min, double global_t_max);
        PMTHitCluster GetNoiseHits(double global_t_min, double global_t_max);
    
        float fSigFreqHz;
        float fTDurationSec;
        unsigned int fRandomSeed;
        std::vector<double> fSignalTime; 
        
        TTree *fSignalTree, *fNoiseTree;
        unsigned int fCurrentSignalEntry, fCurrentNoiseEntry,
                     fTotalSignalEntries, fTotalNoiseEntries;
        
        TQReal *fSignalIDTQ, *fNoiseIDTQ, *fNoiseODTQ;
        
        PMTHitCluster fSegment;
        float fSegmentLength;
        
        Verbosity fVerbosity;
        Printer fMsg;
};

#endif