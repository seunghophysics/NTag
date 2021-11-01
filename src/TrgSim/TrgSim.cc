#include <iostream>

#include "TChain.h"
#include "TRandom3.h"
#include "TUnuran.h"

#include "Calculator.hh"
#include "TrgSim.hh"

TrgSim::TrgSim(Verbosity verbose) : 
fSigFreqHz(0), fTDurationSec(0), fRandomSeed(0),
fSignalTree(nullptr), fNoiseTree(nullptr),
fCurrentSignalEntry(0), fCurrentNoiseEntry(0),
fTotalSignalEntries(0), fTotalNoiseEntries(0),
fSignalIDTQ(nullptr), fNoiseIDTQ(nullptr), fNoiseODTQ(nullptr), fNoiseHeader(nullptr),
fIDSegment(), fODSegment(), fSegmentNo(0), fSegmentLength(10e-3 /* 10 ms */),
fVerbosity(verbose), fMsg("TrgSim", verbose)
{}

TrgSim::TrgSim(std::string signalFilePath, float sigFreqHz, float tDurationSec, std::string noiseFilePath, unsigned int seed, Verbosity verbose)
: TrgSim(verbose)
{
    SetSignalFile(signalFilePath);
    SetRandomSeed(seed);
    SetSignalTime(sigFreqHz, tDurationSec);
    
    SetNoiseFile(noiseFilePath);
}

TrgSim::~TrgSim() {}

void TrgSim::SetSignalFile(std::string signalFilePath)
{
    TChain* signalChain = new TChain("data");
    signalChain->Add(signalFilePath.c_str());
    fSignalTree = signalChain;
    fSignalTree->SetBranchStatus("*", 0);
    fSignalTree->SetBranchStatus("TQREAL", 1);
    fSignalTree->SetBranchAddress("TQREAL", &fSignalIDTQ);
    fTotalSignalEntries = fSignalTree->GetEntries();
}

void TrgSim::SetNoiseFile(std::string noiseFilePath)
{
    TChain* noiseChain = new TChain("data");
    noiseChain->Add(noiseFilePath.c_str());
    fNoiseTree = noiseChain;
    fNoiseTree->SetBranchStatus("*", 0);
    fNoiseTree->SetBranchStatus("HEADER", 1);
    fNoiseTree->SetBranchStatus("TQREAL", 1);
    fNoiseTree->SetBranchStatus("TQAREAL", 1);
    fNoiseTree->SetBranchAddress("HEADER", &fNoiseHeader);
    fNoiseTree->SetBranchAddress("TQREAL", &fNoiseIDTQ);
    fNoiseTree->SetBranchAddress("TQAREAL", &fNoiseODTQ);
    fTotalNoiseEntries = fNoiseTree->GetEntries();
}

void TrgSim::SetSignalTime(float sigFreqHz, float tDurationSec)
{
    SetSignalFrequency(sigFreqHz);
    SetTimeDuration(tDurationSec);
    
    double dt = 100e-6;
    unsigned long tAxisSize = (unsigned long)(fTDurationSec/dt);
    double poissonMeanDt = dt * fSigFreqHz;
    
    TUnuran poissonGen; poissonGen.SetSeed(fRandomSeed);
    TRandom3 uniformGen(fRandomSeed);
    poissonGen.InitPoisson(poissonMeanDt);
    
    fSignalEvTime.clear();
    for(unsigned long i=0; i<tAxisSize; i++) {
        unsigned int nOccurrences = poissonGen.SampleDiscr();
        for (unsigned int occ=0; occ<nOccurrences; occ++)
            fSignalEvTime.push_back((i+uniformGen.Rndm())*dt);
    }
    
    fMsg.Print(Form("Seed: %d", fRandomSeed), pDEBUG);
    fMsg.Print(Form("Signal frequency: %3.2f Hz, Time duration: %3.2f sec", fSigFreqHz, fTDurationSec), pDEBUG);
    fMsg.Print(Form("Signal occurence: %d, Simulated signal frequency: %3.2f Hz", fSignalEvTime.size(), fSignalEvTime.size()/fTDurationSec));
    fMsg.Print(Form("Total input MC events: %d", fTotalSignalEntries));
}

void TrgSim::Simulate()
{
    FillSegment(0);
}

void TrgSim::GetEntry(TTree* tree, unsigned long entryNo)
{
    tree->GetEntry(entryNo % (tree->GetEntries()));
}

void TrgSim::FillSegment(unsigned long segNo)
{
    // work in nanoseconds

    float tSegStart = segNo * fSegmentLength * 1e9;
    float tSegEnd = (segNo+1) * fSegmentLength * 1e9;
    
    fIDSegment.Clear(); fODSegment.Clear();
    
    // signal
    // search for events that might have hits within segment
    auto signalEvID = GetRangeIndex(fSignalEvTime, tSegStart*1e-9 -1e-3 /* additional 1 ms */, tSegEnd*1e-9);
    
    for (unsigned long iEv=0; iEv<signalEvID.size(); iEv++) {
        double evTime = fSignalEvTime[iEv] * 1e9;
        auto evID = signalEvID[iEv];
        GetEntry(fSignalTree, evID);
        fCurrentSignalEntry = evID;
        fMsg.Print(Form("Signal event ID: %d, Time: %3.2f nsec", evID, evTime));

        PMTHitCluster signalIDHits = PMTHitCluster(fSignalIDTQ, 2) + (evTime - 1000) ;
        
        for (auto const& hit: signalIDHits) {
            if ((tSegStart < hit.t()) && (hit.t() < tSegEnd))
                fIDSegment.Append(hit);
        }
    }
    
    // noise
    double addedNoiseLength = 0;
    unsigned long evID = -1;
    while (addedNoiseLength < fSegmentLength*1e9) {
        
        // get entry until we get random-wide trigger
        evID++; GetEntry(fNoiseTree, evID);
        while (fNoiseHeader->idtgsk >= 0) {
            evID++; GetEntry(fNoiseTree, evID);
        }
        fCurrentNoiseEntry = evID;
        
        PMTHitCluster noiseIDHits(fNoiseIDTQ, 1);
        PMTHitCluster noiseODHits(fNoiseODTQ, 1);
        fMsg.Print(Form("NoiseODHits size: %d", fNoiseODTQ->nhits), pDEBUG);
        
        auto noiseEvHitT = noiseIDHits.GetProjection(HitFunc::T);
        double rawEvNoiseStartT = noiseEvHitT[GetMinIndex(noiseEvHitT)];
        noiseIDHits = noiseIDHits + (addedNoiseLength - rawEvNoiseStartT);
        noiseODHits = noiseODHits + (addedNoiseLength - rawEvNoiseStartT);
        
        for (auto const& hit: noiseIDHits) {
            if ((tSegStart < hit.t()) && (hit.t() < tSegEnd)) {
                fIDSegment.Append(hit);
            }
        }

        for (auto const& hit: noiseODHits) {
            if ((tSegStart < hit.t()) && (hit.t() < tSegEnd)) {
                fODSegment.Append(hit);
            }
        }

        double noiseEvLength = noiseEvHitT[GetMaxIndex(noiseEvHitT)] - rawEvNoiseStartT;
        fMsg.Print(Form("Noise event ID: %d, length: %3.2f ms", evID, noiseEvLength*1e-6));
        addedNoiseLength += noiseEvLength;
    }
    
    fIDSegment.Sort();
    fODSegment.Sort();
    
    //fMsg.Print("ID hits: ");
    //fIDSegment.DumpAllElements();
    //fMsg.Print("OD hits: ");
    //fODSegment.DumpAllElements();
}

PMTHitCluster TrgSim::GetSignalHits(double global_t_min, double global_t_max)
{
    PMTHitCluster cluster;
    
    // get signal entry
    
    return cluster;
}

PMTHitCluster TrgSim::GetNoiseHits(double global_t_min, double global_t_max)
{
    PMTHitCluster cluster;
    
    return cluster;
}