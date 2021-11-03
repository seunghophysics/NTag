#include <iostream>

#include "TROOT.h"
#include "TChain.h"
#include "TRandom3.h"
#include "TUnuran.h"

#include "Calculator.hh"
#include "TrgSim.hh"

TrgSim::TrgSim(std::string outFilePath, Verbosity verbose) : 
fTDurationSec(0),
fOutFile(nullptr), fOutEvIDTQ(nullptr), fOutEvODTQ(nullptr), fOutHeader(nullptr),
fIDSegment(), fODSegment(), fSegmentNo(0), fNTotalSegments(0), fSegmentLength(21e-3 /* 21 ms */), fLastEvEndT(-99e99),
fThreshold(60 /* SHE */), fSHEDeadtime(35e3), fAFTDeadtime(535e3),
fVerbosity(verbose), fMsg("TrgSim", verbose)
{
    SetOutputFile(outFilePath);
}


TrgSim::~TrgSim() {}

void TrgSim::PreprocessRootReader(RootReader* rootReader)
{
    rootReader->SetTimeDuration(fTDurationSec);
    rootReader->SetEventTime();
    rootReader->SetVerbosity(fVerbosity);
}

void TrgSim::AddSignal(SignalReader* signal)
{ 
    PreprocessRootReader(signal);
    fSignalList.push_back(signal); 
}

void TrgSim::AddNoise(NoiseReader* noise)
{ 
    PreprocessRootReader(noise);
    fNoiseList.push_back(noise); 
}

void TrgSim::SetOutputFile(std::string outputFilePath)
{
    fOutFile = new TFile(outputFilePath.c_str(), "recreate");
    
    fOutTree = new TTree("data", "Simulated trigger events");
    
    fOutTree->Branch("HEADER", &fOutHeader);
    fOutTree->Branch("TQREAL", &fOutEvIDTQ);
    fOutTree->Branch("TQAREAL", &fOutEvODTQ);
}

void TrgSim::Simulate()
{    
    int fNTotalSegments = (int)(fTDurationSec/fSegmentLength);
    if (fmod(fTDurationSec, fSegmentLength)>1e-6) fNTotalSegments++;
    
    //for (fSegmentNo=0; fSegmentNo<fNTotalSegments; fSegmentNo++) {
    //    fMsg.Print(Form("Processing segment #%d / %d...", fSegmentNo+1, fNTotalSegments));
    //    FillSegment(fSegmentNo);
        FillSegment(52);
        FindTriggerInSegment();
    //}

    fOutTree->Write();
    fOutFile->Close();
}

void TrgSim::FillSegment(unsigned long segNo)
{
    // work in nanoseconds

    double tSegStart = segNo * fSegmentLength * 1e9 - 1e6; // additional 1 ms in the beginning 
    double tSegEnd = (segNo+1) * fSegmentLength * 1e9;
    
    fMsg.Print(Form("Segment #%d: (%3.2f, %3.2f) msec", tSegStart*1e-6, tSegEnd*1e-6), pDEBUG);
    
    // last segment 
    if (segNo == fNTotalSegments-1)
        tSegEnd = fTDurationSec * 1e9 - tSegStart;
    
    fIDSegment.Clear(); fODSegment.Clear();
    
    // signal
    for (auto const& signal: fSignalList) {
        signal->FillSegments(fIDSegment, fODSegment, tSegStart, tSegEnd);
    }
    
    // noise
    for (auto const& noise: fNoiseList) {
        noise->FillSegments(fIDSegment, fODSegment, tSegStart, tSegEnd);
    }

    fIDSegment.Sort();
    fODSegment.Sort();
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
    
    // first trgStartT
    hit.SetIndex(0);
    double tSegStart = hit.t;
    double segTrgStart = tSegStart;
    
    if (fLastEvEndT > fIDSegment[0].t()) {
        fMsg.Print(Form("Previous trigger has a tail in the beginning of the segment. Starting search from %3.2f ns...", fLastEvEndT), pDEBUG);
        hit.SetIndex(fIDSegment.GetLowerBoundIndex(fLastEvEndT));
    }
    
    double segTrgEnd  = fIDSegment[fIDSegment.GetSize()-1].t() - 1e6;
    
    fMsg.Print(Form("Triggering range of this segment: (%3.2f, %3.2f) msec", segTrgStart*1e-6, segTrgEnd*1e-6), pDEBUG);
    
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
            
            fLastEvEndT = hit.t + evEndT;
            
            fMsg.Print(std::string(evEndT>50000 ? "AFT" : "SHE") + Form(" trigger at t = %3.2f msec", hit.t * 1e-6));
            fMsg.Print(Form("N200: %d (signal: %3.2f%)", N200, 100*nSig/float(N200)), pDEBUG);
        }
        
        else {
            hit.SetIndex(hit.i+1);
        }
    }
}