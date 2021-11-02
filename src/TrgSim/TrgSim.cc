#include <iostream>

#include "TROOT.h"
#include "TChain.h"
#include "TRandom3.h"
#include "TUnuran.h"

#include "Calculator.hh"
#include "TrgSim.hh"

TrgSim::TrgSim(Verbosity verbose) : 
fSigFreqHz(0), fTDurationSec(0), fRandomSeed(0),
fOutFile(nullptr), fSignalTree(nullptr), fNoiseTree(nullptr), fOutTree(nullptr),
fCurrentSignalEntry(-1), fCurrentNoiseEntry(-1),
fTotalSignalEntries(0), fTotalNoiseEntries(0),
fSignalIDTQ(nullptr), fNoiseIDTQ(nullptr), fNoiseODTQ(nullptr), fOutEvIDTQ(nullptr), fOutEvODTQ(nullptr), fNoiseHeader(nullptr), fOutHeader(nullptr),
fIDSegment(), fODSegment(), fSegmentNo(0), fNTotalSegments(0), fSegmentLength(21e-3 /* 21 ms */),
fThreshold(60 /* SHE */), fSHEDeadtime(35e3), fAFTDeadtime(535e3),
fVerbosity(verbose), fMsg("TrgSim", verbose)
{
    //gROOT->LoadMacro("/disk02/usr6/han/skg4/lib/libSKG4Root.so");
    //gROOT->LoadMacro("/home/skofl/sklib_gcc4.8.5/skofl-trunk/lib/libmcinfo.so");
    //gROOT->LoadMacro("/home/skofl/sklib_gcc4.8.5/skofl-trunk/lib/libtqrealroot.so");
    //gROOT->LoadMacro("/home/skofl/sklib_gcc4.8.5/skofl-trunk/lib/libDataDefinition.so");
}

TrgSim::TrgSim(std::string signalFilePath, float sigFreqHz, float tDurationSec, std::string noiseFilePath, std::string outFilePath, unsigned int seed, Verbosity verbose)
: TrgSim(verbose)
{
    SetOutputFile(outFilePath);
    
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

void TrgSim::SetOutputFile(std::string outputFilePath)
{
    fOutFile = new TFile(outputFilePath.c_str(), "recreate");
    
    fOutTree = new TTree("data", "Simulated trigger events");
    
    fOutTree->Branch("HEADER", &fOutHeader);
    fOutTree->Branch("TQREAL", &fOutEvIDTQ);
    fOutTree->Branch("TQAREAL", &fOutEvODTQ);
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
    
    //for (unsigned int i=0; i<fSignalEvTime.size(); i++) {
    //   std::cout << i << " " << fSignalEvTime[i]*1e9 << "\n";
    //}
    
    fMsg.Print(Form("Seed: %d", fRandomSeed), pDEBUG);
    fMsg.Print(Form("Signal frequency: %3.2f Hz, Time duration: %3.2f sec", fSigFreqHz, fTDurationSec), pDEBUG);
    fMsg.Print(Form("Signal occurence: %d, Simulated signal frequency: %3.2f Hz", fSignalEvTime.size(), fSignalEvTime.size()/fTDurationSec));
    fMsg.Print(Form("Total input MC events: %d", fTotalSignalEntries));
}

void TrgSim::Simulate()
{
    int fNTotalSegments = (int)(fTDurationSec/fSegmentLength);
    if (fmod(fTDurationSec, fSegmentLength)>1e-6) fNTotalSegments++;
    
    for (fSegmentNo=0; fSegmentNo<fNTotalSegments; fSegmentNo++) {
        fMsg.Print(Form("Processing segment #%d / %d...", fSegmentNo+1, fNTotalSegments));
        FillSegment(fSegmentNo);
        FindTriggerInSegment();
    }

    fOutTree->Write();
    fOutFile->Close();
}

void TrgSim::GetEntry(TTree* tree, unsigned long entryNo)
{
    unsigned long nEntries = tree->GetEntries();
    
    if (entryNo > nEntries)
        fMsg.Print("");
        
    tree->GetEntry(entryNo % (tree->GetEntries()));
}

void TrgSim::FillSegment(unsigned long segNo)
{
    // work in nanoseconds

    float tSegStart = segNo * fSegmentLength * 1e9;
    float tSegEnd = (segNo+1) * fSegmentLength * 1e9;
    
    // last segment 
    if (segNo == fNTotalSegments-1)
        tSegEnd = fTDurationSec * 1e9 - tSegStart;
    
    fIDSegment.Clear(); fODSegment.Clear();
    
    // signal
    // search for events that might have hits within segment
    auto signalEvID = GetRangeIndex(fSignalEvTime, tSegStart*1e-9 -1e-3 /* additional 1 ms */, tSegEnd*1e-9);
    
    //for (unsigned int i=0; i<signalEvID.size(); i++) {
    //   std::cout << "Signal ev: " << i << " " << signalEvID[i]<< "\n";
    //}
    
    for (unsigned long iEv=0; iEv<signalEvID.size(); iEv++) {
        double evTime = fSignalEvTime[signalEvID[iEv]] * 1e9;
        auto evID = signalEvID[iEv];
        fSignalTree->GetEntry(evID%fTotalSignalEntries);
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
    unsigned long evID = fCurrentNoiseEntry;
    while (addedNoiseLength < fSegmentLength*1e9) {
        
        // get entry until we get random-wide trigger
        // this takes quite a long time
        evID++; fNoiseTree->GetEntry(evID%fTotalNoiseEntries);
        while (fNoiseHeader->idtgsk >= 0) {
            evID++; fNoiseTree->GetEntry(evID%fTotalNoiseEntries);
        }
        fCurrentNoiseEntry = evID;
        
        PMTHitCluster noiseIDHits(fNoiseIDTQ, 1);
        PMTHitCluster noiseODHits(fNoiseODTQ, 1);
        
        auto noiseEvHitT = noiseIDHits.GetProjection(HitFunc::T);
        double rawEvNoiseStartT = noiseEvHitT[GetMinIndex(noiseEvHitT)];
        noiseIDHits = noiseIDHits + (tSegStart + addedNoiseLength - rawEvNoiseStartT);
        noiseODHits = noiseODHits + (tSegStart + addedNoiseLength - rawEvNoiseStartT);
        
        for (auto const& hit: noiseIDHits) {
            if (hit.t() < tSegEnd) {
                fIDSegment.Append(hit);
            }
        }

        for (auto const& hit: noiseODHits) {
            if (hit.t() < tSegEnd) {
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

void TrgSim::FindTriggerInSegment()
{
    struct Hit {
        unsigned int i;
        double t;
        PMTHitCluster* cluster;
        
        double SetIndex(unsigned int index) { i = index; t = cluster->At(index).t(); }
    };
    
    Hit hit;
    hit.cluster = &fIDSegment;
    
    hit.SetIndex(0);
    
    double segTrgEnd  = fIDSegment[fIDSegment.GetSize()-1].t() - fAFTDeadtime;
    
    //std::vector<double> trgTList;
    
    bool isFirstTrigger = true;
    
    while (hit.t < segTrgEnd) {
        double trgStartT = hit.t;
        double trgEndT = hit.t + 200;
        
        auto n200Hits = fIDSegment.SliceRange(trgStartT, trgEndT);
        unsigned int N200 = n200Hits.GetSize();
        
        unsigned int nSig = 0;
        for (auto const& h: n200Hits) {
            if (h.f() == 2) nSig++;
        }

        // found trigger
        if (N200 >= fThreshold) {
            double evEndT = fSHEDeadtime;
            fOutHeader->idtgsk = -1;
            // AFT is only allowed for the first trigger
            if (isFirstTrigger) {
                evEndT = fAFTDeadtime;
                fOutHeader->idtgsk = -2;
                isFirstTrigger = false;
            }

            auto eventIDHits = fIDSegment.SliceRange(hit.t, -5000, evEndT) + (-hit.t + 1000);
            auto eventODHits = fODSegment.SliceRange(hit.t, -5000, evEndT) + (-hit.t + 1000);
            
            eventIDHits.FillTQReal(fOutEvIDTQ);
            eventODHits.FillTQReal(fOutEvODTQ);
            
            fOutTree->Fill();

            hit.SetIndex(fIDSegment.GetLowerBoundIndex(hit.t + evEndT));
            
            fMsg.Print(Form("Found trigger at t=%3.2f ns, evEndT: %3.2f ns", hit.t, evEndT));
            fMsg.Print(Form("Trigger region (%3.2f, %3.2f) ns, N200: %d (signal: %3.2f%)", trgStartT, trgEndT, N200, 100*nSig/float(N200)), pDEBUG);
        }
        
        else {
            hit.SetIndex(hit.i+1);
        }
    }
}