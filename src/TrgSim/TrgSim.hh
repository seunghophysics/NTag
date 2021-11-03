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
#include "RootReader.hh"

class TrgSim
{
    public:
        TrgSim(std::string outFilePath, Verbosity verbose=pDEFAULT);
        ~TrgSim();
        
        // setter
        void SetVerbosity(Verbosity verbose) { fVerbosity = verbose; fMsg.SetVerbosity(verbose); }
        void SetTimeDuration(float tDurationSec) { fTDurationSec = tDurationSec; }
        void SetSegmentLength(float segLength) { fSegmentLength = segLength; }
        void SetThreshold(unsigned int thr) { fThreshold = thr; }
        void SetSHEDeadtime(float dt) { fSHEDeadtime = dt; }
        void SetAFTDeadtime(float dt) { fAFTDeadtime = dt; }
        void SetOutputFile(std::string outFilePath);
        
        void PreprocessRootReader(RootReader* rootReader);
        void AddSignal(SignalReader* signal);
        void AddNoise(NoiseReader* noise);
        
        // simulation
        void Simulate();
        
    private:
        void FillSegment(unsigned long segNo);
        void FindTriggerInSegment();
        
        float fTDurationSec;
        
        // ROOT output
        TFile* fOutFile;
        TTree *fOutTree;
        TQReal *fOutEvIDTQ, *fOutEvODTQ;
        Header *fOutHeader;
        
        // Signal and noise
        std::vector<SignalReader*> fSignalList;
        std::vector<NoiseReader*> fNoiseList;
        
        // Segment
        PMTHitCluster fIDSegment, fODSegment;
        unsigned long fSegmentNo, fNTotalSegments;
        float fSegmentLength, fLastEvEndT;
        
        // Triggering
        unsigned int fThreshold;
        float fSHEDeadtime, fAFTDeadtime;
        
        // Utilities
        Verbosity fVerbosity;
        Printer fMsg;
};

#endif