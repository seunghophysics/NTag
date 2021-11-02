#ifndef TRGSIM_HH
#define TRGSIM_HH

#include <vector>

#include "TFile.h"
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
        TrgSim(std::string signalFilePath, float sigFreqHz, float tDurationSec, std::string noiseFilePath, std::string outFilePath, unsigned int seed=0, Verbosity verbose=pDEFAULT);
        ~TrgSim();
        
        // setter
        void SetVerbosity(Verbosity verbose) { fVerbosity = verbose; fMsg.SetVerbosity(verbose); }
        void SetSignalFrequency(float sigFreqHz) { fSigFreqHz = sigFreqHz; }
        void SetTimeDuration(float tDurationSec) { fTDurationSec = tDurationSec; }
        void SetRandomSeed(unsigned int seed) { fRandomSeed = seed; }
        void SetSegmentLength(float segLength) { fSegmentLength = segLength; }
        void SetThreshold(unsigned int thr) { fThreshold = thr; }
        void SetSHEDeadtime(float dt) { fSHEDeadtime = dt; }
        void SetAFTDeadtime(float dt) { fAFTDeadtime = dt; }
        void SetSignalFile(std::string signalFilePath);
        void SetNoiseFile(std::string noiseFilePath);
        void SetOutputFile(std::string outFilePath);
        
        // signal time generation
        void SetSignalTime(float sigFreqHz, float tDurationSec);
        const std::vector<double>& GetSignalTime() const { return fSignalEvTime; }
        
        // simulation
        void Simulate();
        
    private:
        void GetEntry(TTree* tree, unsigned long entryNo);
        void FillSegment(unsigned long segNo);
        void FindTriggerInSegment();
        
        // Signal time
        float fSigFreqHz;
        float fTDurationSec;
        unsigned int fRandomSeed;
        std::vector<double> fSignalEvTime; 
        
        // ROOT I/O
        TFile* fOutFile;
        TTree *fSignalTree, *fNoiseTree, *fOutTree;
        unsigned long fCurrentSignalEntry, fCurrentNoiseEntry,
                      fTotalSignalEntries, fTotalNoiseEntries;
        TQReal *fSignalIDTQ, *fNoiseIDTQ, *fNoiseODTQ, *fOutEvIDTQ, *fOutEvODTQ;
        Header *fNoiseHeader, *fOutHeader;
        
        // Segment
        PMTHitCluster fIDSegment, fODSegment;
        unsigned long fSegmentNo, fNTotalSegments;
        float fSegmentLength;
        
        // Triggering
        unsigned int fThreshold;
        float fSHEDeadtime, fAFTDeadtime;
        
        // Utilities
        Verbosity fVerbosity;
        Printer fMsg;
};

#endif