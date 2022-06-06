#ifndef NOISEMANAGER_HH
#define NOISEMANAGER_HH

#include <vector>

#include "Store.hh"
#include "Printer.hh"
#include "PMTHitCluster.hh"

class TChain;
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
        NoiseManager(TChain* tree);
        ~NoiseManager();

        // settings
        void SetNoiseTreeName(TString treename) { fNoiseTreeName = treename; }
        void SetNoisePath(TString dirpath) { fNoisePath = dirpath; }
        void SetNoiseTimeRange(float startTime, float endTime);
        void SetPMTDeadtime(float pmtDeadtime) { fPMTDeadtime = pmtDeadtime; }
        void SetDarkRate(float idRate, float odRate) { fIDDarkRatekHz = idRate; fODDarkRatekHz = odRate; }
        void SetNoiseMaxN200(int idCut, int odCut, bool doCut=true) { fIDMaxN200 = idCut; fODMaxN200 = odCut; fDoN200Cut = doCut; }
        void SetRepeat(bool b) { fDoRepeat = b; }
        void SetSeed(int seed) { fNoiseSeed = seed; ranGen.SetSeed(seed); }
        void DumpSettings();

        // noise tree generation
        void AddNoiseFileToChain(TChain* chain, TString noiseFilePath);
        void SetNoiseTree(TChain* tree);
        void DumpNoiseFileList(TString pathToFileList);

        // full noise tree generation from scratch
        void SetNoiseTreeFromOptions(TString option, int nInputEvents, float tStart, float tEnd, int seed=0);
        void SetNoiseTreeFromList(TString pathToList);
        void SetNoiseTreeFromWildcard(TString wildcard, float tStart=0, float tEnd=535);

        // initialize from Store
        void ApplySettings(Store& store, int nInputEvents);
        
        // event navigation within tree
        void GetNextNoiseEvent(); // checks trigger, tree size and current entry
        void SetNoiseEventHits(); // reads in noise hits from current noise entry
        void SetVerbosity(Verbosity verbose) { fMsg.SetVerbosity(verbose); }

        // add noise to input PMTHitCluster
        void AddIDNoise(PMTHitCluster* signalHits);
        void AddODNoise(PMTHitCluster* signalHits);

    protected:
        void PopulateHitCluster(PMTHitCluster* hitCluster, bool OD=false);
        void AddNoise(PMTHitCluster* signalHits, PMTHitCluster* noiseHits, int& currentHitIndex, float darkRate, bool OD=false);

    private:

        TChain* fNoiseTree;
        TString fNoiseTreeName;

        TString fNoisePath;
        TString fNoiseType;
        TString fNoiseCut;

        Header* fHeader;
        TQReal* fIDTQReal;
        TQReal* fODTQReal;

        int fNoiseSeed;

        float fNoiseEventLength;
        float fNoiseStartTime, fNoiseEndTime, fNoiseWindowWidth;
        float fNoiseT0;

        int fIDMaxN200, fODMaxN200;

        //float fMinHitsLimit, fMaxHitsLimit;
        //float fMinHitDensity, fMaxHitDensity;

        float fPMTDeadtime;
        float fIDDarkRatekHz, fODDarkRatekHz;

        int fCurrentIDHitIndex, fCurrentODHitIndex, fCurrentEntry, fNEntries;
        int fPartID, fNParts;

        bool fDoRepeat, fDoN200Cut;

        std::vector<float> fT, fQ;
        std::vector<int>   fI;

        PMTHitCluster fIDNoiseEventHits;
        PMTHitCluster fODNoiseEventHits;
        
        Printer fMsg;
};

#endif