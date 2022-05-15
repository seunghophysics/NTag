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

static const float MINDUMMYHITS = 50e3;
static const float MAXDUMMYHITS = 100e3;

static TString DUMMYDIR = "/disk02/calib3/usr/han/dummy/";
static TString DUMMYCUT = Form("HEADER.idtgsk & %d || HEADER.idtgsk == %d", mRandomWide, mT2KDummy);

class NoiseManager
{
    public:
        NoiseManager();
        NoiseManager(TString option, int nInputEvents, float tStart, float tEnd, int seed=0);
        NoiseManager(TTree* tree);
        ~NoiseManager();

        void AddNoiseFileToChain(TChain* chain, TString noiseFilePath);
        void SetNoiseTree(TTree* tree);
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
        
        TString fNoiseType;

        Header* fHeader;
        TQReal* fTQReal;

        float fNoiseEventLength;
        float fNoiseStartTime, fNoiseEndTime, fNoiseWindowWidth;
        float fNoiseT0;

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