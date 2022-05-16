#ifndef NOISEMANAGER_HH
#define NOISEMANAGER_HH

#include <vector>

#include "Printer.hh"
#include "PMTHitCluster.hh"

class TChain;
class TTree;
class TQReal;
class Header;

enum NoiseTriggerType
{
    mRandomWide = 2048,
    mT2KDummy = -2147483648
};

class NoiseManager
{
    public:
        NoiseManager();
        NoiseManager(TTree* tree);
        ~NoiseManager();

        void GenerateNoiseTree(TString option, int nInputEvents, float tStart, float tEnd, int seed=0);
        void AddNoiseFileToChain(TChain* chain, TString noiseFilePath);
        void SetNoiseTree(TTree* tree);
        void SetNoiseTreeName(TString treename) { fNoiseTreeName = treename; }
        void SetNoisePath(TString dirpath) { fNoisePath = dirpath; }
        void SetNoiseTimeRange(float startTime, float endTime);
        void SetNoiseEventHits();
        void GetNextNoiseEvent();
        void SetPMTDeadtime(float pmtDeadtime) { fPMTDeadtime = pmtDeadtime; }
        void SetRepeat(bool b) { fDoRepeat = b; }
        void SetVerbosity(Verbosity verbose) { fMsg.SetVerbosity(verbose); }

        void AddNoise(PMTHitCluster* signalHits);

        void DumpSettings();

    private:
        TTree* fNoiseTree;
        TString fNoiseTreeName;
        
        TString fNoisePath;
        TString fNoiseType;
        TString fNoiseCut;

        Header* fHeader;
        TQReal* fTQReal;

        float fNoiseEventLength;
        float fNoiseStartTime, fNoiseEndTime, fNoiseWindowWidth;
        float fNoiseT0;

        float fMinHitsLimit, fMaxHitsLimit;
        float fMinHitDensity, fMaxHitDensity;

        float fPMTDeadtime;

        int fCurrentHitID, fCurrentEntry, fNEntries;
        int fPartID, fNParts;

        bool fDoRepeat;

        std::vector<float> fT;
        std::vector<float> fQ;
        std::vector<int>   fI;

        PMTHitCluster fNoiseEventHits;
        
        Printer fMsg;
};

#endif