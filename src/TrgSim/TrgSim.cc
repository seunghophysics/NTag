#include <iostream>

#include "TChain.h"
#include "TRandom3.h"
#include "TUnuran.h"

#include "TrgSim.hh"

TrgSim::TrgSim(Verbosity verbose) : 
fSigFreqHz(0), fTDurationSec(0), fRandomSeed(0),
fSignalTree(nullptr), fNoiseTree(nullptr),
fCurrentSignalEntry(0), fCurrentNoiseEntry(0),
fTotalSignalEntries(0), fTotalNoiseEntries(0),
fSignalIDTQ(nullptr), fNoiseIDTQ(nullptr), fNoiseODTQ(nullptr),
fSegment(), fSegmentLength(10e-3 /* 10 ms */),
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
    fSignalTree->SetBranchAddress("TQREAL", &fSignalIDTQ);
    fTotalSignalEntries = fSignalTree->GetEntries();
}

void TrgSim::SetNoiseFile(std::string noiseFilePath)
{
    TChain* noiseChain = new TChain("data");
    noiseChain->Add(noiseFilePath.c_str());
    fNoiseTree = noiseChain;
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
    
    fSignalTime.clear();
    for(unsigned long i=0; i<tAxisSize; i++) {
        unsigned int nOccurrences = poissonGen.SampleDiscr();
        for (unsigned int occ=0; occ<nOccurrences; occ++)
            fSignalTime.push_back((i+uniformGen.Rndm())*dt);
    }
    
    fMsg.Print(Form("Seed: %d", fRandomSeed), pDEBUG);
    fMsg.Print(Form("Signal frequency: %3.2f Hz, Time duration: %3.2f sec", fSigFreqHz, fTDurationSec), pDEBUG);
    fMsg.Print(Form("Signal occurence: %d, Simulated signal frequency: %3.2f Hz", fSignalTime.size(), fSignalTime.size()/fTDurationSec));
    fMsg.Print(Form("Total input MC events: %d", fTotalSignalEntries));
}

void TrgSim::Simulate()
{
    std::cout << "signal:\n";
    for (int evID=0; evID<fSignalTree->GetEntriesFast(); evID++) {
        fSignalTree->GetEntry(evID);
        auto& hitTimes = fSignalIDTQ->T;
        for (auto const& t: hitTimes) {
            std::cout << t << "\n";
        }
    }
    
    std::cout << "noise:\n";
    for (int evID=0; evID<fNoiseTree->GetEntriesFast(); evID++) {
        fNoiseTree->GetEntry(evID);
        auto& hitTimes = fNoiseIDTQ->T;
        auto& ODhitTimes = fNoiseODTQ->T;
        std::cout << "noise id:\n";
        for (auto const& t: hitTimes) {
            std::cout << t << "\n";
        }
        std::cout << "noise od:\n";
        for (auto const& t: ODhitTimes) {
            std::cout << t << "\n";
        }
        
    }
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