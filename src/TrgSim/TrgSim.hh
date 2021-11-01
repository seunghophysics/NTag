#ifndef TRGSIM_HH
#define TRGSIM_HH

#include <vector>

#include "TTree.h"

#include "tqrealroot.h"
#undef MAXPM
#undef MAXPMA

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
        const std::vector<double>& GetSignalTime() const { return fSignalEvTime; }
        
        // simulation
        void Simulate();
        
    private:
        void GetEntry(TTree* tree, unsigned long entryNo);
        void FillSegment(unsigned long segNo);
        
        PMTHitCluster GetSignalHits(double global_t_min, double global_t_max);
        PMTHitCluster GetNoiseHits(double global_t_min, double global_t_max);
    
        float fSigFreqHz;
        float fTDurationSec;
        unsigned int fRandomSeed;
        std::vector<double> fSignalEvTime; 
        
        TTree *fSignalTree, *fNoiseTree;
        unsigned long fCurrentSignalEntry, fCurrentNoiseEntry,
                      fTotalSignalEntries, fTotalNoiseEntries;
        
        TQReal *fSignalIDTQ, *fNoiseIDTQ, *fNoiseODTQ;
        Header *fNoiseHeader;
        
        PMTHitCluster fIDSegment, fODSegment;
        unsigned long fSegmentNo;
        float fSegmentLength;
        
        Verbosity fVerbosity;
        Printer fMsg;
};

#endif