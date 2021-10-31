#include <iostream>

#include "TChain.h"

#include "TrgSim.hh"

TrgSim::TrgSim() : 
fSigFreqHz(0), fTDurationSec(0),
fSignalTree(nullptr), fNoiseTree(nullptr), 
fSignalIDTQ(nullptr), fNoiseIDTQ(nullptr), fNoiseODTQ(nullptr) 
{}

TrgSim::TrgSim(const char* signalFilePath, float sigFreqHz, float tDurationSec, const char* noiseFilePath)
: TrgSim()
{
    SetSignalFile(signalFilePath);
    SetSignalTime(fSigFreqHz, fTDurationSec);
    
    SetNoiseFile(noiseFilePath);
}

TrgSim::~TrgSim() {}

void TrgSim::SetSignalFile(const char* signalFilePath)
{
    TChain* signalChain = new TChain("data");
    signalChain->Add(signalFilePath);
    fSignalTree = signalChain;
    fSignalTree->SetBranchAddress("TQREAL", &fSignalIDTQ);
}

void TrgSim::SetNoiseFile(const char* noiseFilePath)
{
    TChain* noiseChain = new TChain("data");
    noiseChain->Add(noiseFilePath);
    fNoiseTree = noiseChain;
    fNoiseTree->SetBranchAddress("TQREAL", &fNoiseIDTQ);
    fNoiseTree->SetBranchAddress("TQAREAL", &fNoiseODTQ);
}

void TrgSim::SetSignalFrequency(float sigFreqHz)
{
    fSigFreqHz = sigFreqHz;
}

void TrgSim::SetTimeDuration(float tDurationSec)
{
    fTDurationSec = tDurationSec;
}

void TrgSim::SetSignalTime(float sigFreqHz, float tDurationSec)
{
    SetSignalFrequency(sigFreqHz);
    SetTimeDuration(tDurationSec);
    
    
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