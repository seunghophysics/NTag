#ifndef ADDNOISE_HH
#define ADDNOISE_HH

#include "TChain.h"

#include "Tool.hh"

class TChain;
class TQReal;

class AddNoise : public Tool
{
    public:
        AddNoise(): noiseChain("data")
        { name = "AddNoise"; }
        
        bool Initialize();
        bool Execute();
        bool Finalize();
        
        bool CheckSafety();
        
    private:
        void GetNewEntry();
        void SetNoiseHitCluster();
        
        TChain noiseChain;
        
        TQReal* tqi;
        
        float noiseEventLength;
        float noiseStartTime, noiseEndTime, noiseTimeWindowWidth;
        
        int iEntry, nEntries;
        int iPart, nParts;
        
        unsigned int iHit, nHits;
        float noiseT0;
        std::vector<float> t;
        std::vector<float> q;
        std::vector<int>   i;
        
        PMTHitCluster noiseEventHits;
};

#endif