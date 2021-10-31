#ifndef TRGSIM_HH
#define TRGSIM_HH

#include <vector>

#include "TTree.h"

#include "tqrealroot.h"

class TrgSim
{
    public:
        TrgSim();
        TrgSim(const char* signalFilePath, float sigFreqHz, float tDurationSec, const char* noiseFilePath);
        ~TrgSim();
        
        // setter
        void SetSignalFrequency(float sigFreqHz);
        void SetTimeDuration(float tDuration);
        void SetSignalFile(const char* signalFilePath);
        void SetNoiseFile(const char* noiseFilePath);
        
        // signal time generation
        void SetSignalTime(float sigFreqHz, float tDurationSec);
        const std::vector<double>& GetSignalTime() const { return fSignalTime; }
        
        // simulation
        void Simulate();
        
    private:
        float fSigFreqHz;
        float fTDurationSec;
        std::vector<double> fSignalTime; 
        
        TTree* fSignalTree;
        TTree* fNoiseTree;
        
        TQReal* fSignalIDTQ;
        TQReal* fNoiseIDTQ;
        TQReal* fNoiseODTQ;
};

#endif