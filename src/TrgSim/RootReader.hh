#ifndef ROOTREADER_HH
#define ROOTREADER_HH

#include "TTree.h"
#include "TChain.h"
#include "TRandom3.h"
#include "TUnuran.h"

#include "tqrealroot.h"

#include "Calculator.hh"
#include "Printer.hh"

class RootReader
{
    public:
        RootReader(std::string name, std::string filePath):
        fTree(nullptr),
        fCurrentEntry(-1), fNEntries(0),
        fHeader(nullptr), fIDTQ(nullptr), fODTQ(nullptr),
        fTDurationSec(0), fEvTime(),
        fName(name), fMsg(name.c_str())
        {
            TChain* chain = new TChain("data");
            chain->Add(filePath.c_str());
            fTree = chain;
            fTree->SetBranchStatus("*", 0);
            fTree->SetBranchStatus("HEADER", 1);
            fTree->SetBranchStatus("TQREAL", 1);
            fTree->SetBranchStatus("TQAREAL", 1);
            fTree->SetBranchStatus("TQREAL", 1);
            fTree->SetBranchAddress("HEADER", &fHeader);
            fTree->SetBranchAddress("TQREAL", &fIDTQ);
            fTree->SetBranchAddress("TQAREAL", &fODTQ);
            fNEntries = fTree->GetEntries();
        }
        ~RootReader() {}
        
        virtual void GetEntry(unsigned long entry)
        {
            fTree->GetEntry(entry % fNEntries);
            fCurrentEntry = entry;
        }
        
        void SetTimeDuration(double tDurationSec) { fTDurationSec = tDurationSec; }
        void SetVerbosity(Verbosity verbose) { fMsg.SetVerbosity(verbose); }
        
        virtual void SetEventTime()=0;
        const std::vector<double>& GetEventTime() const { return fEvTime; }
        
        virtual void FillSegments(PMTHitCluster& segmentID, PMTHitCluster& segmentOD, double tSegStart, double tSegEnd)=0;
        
    protected:
        TTree* fTree;
        unsigned long fCurrentEntry, fNEntries;
        Header* fHeader;
        TQReal *fIDTQ, *fODTQ;
        double fTDurationSec;
        std::vector<double> fEvTime;
        
        std::string fName;
        Printer fMsg;
};

class SignalReader : public RootReader
{
    public:
        SignalReader(std::string name, std::string filePath)
        : RootReader(name, filePath), fRandomSeed(0) {}
        
        void SetSignalFrequency(double sigFreqHz) { fSigFreqHz = sigFreqHz; }
        void SetRandomSeed(unsigned int seed) { fRandomSeed = seed; }

        void SetEventTime()
        {
            double dt = 100e-6;
            unsigned long tAxisSize = (unsigned long)(fTDurationSec/dt);
            double poissonMeanDt = dt * fSigFreqHz;
            
            TUnuran poissonGen; poissonGen.SetSeed(fRandomSeed);
            TRandom3 uniformGen(fRandomSeed);
            poissonGen.InitPoisson(poissonMeanDt);
            
            fEvTime.clear();
            for(unsigned long i=0; i<tAxisSize; i++) {
                unsigned int nOccurrences = poissonGen.SampleDiscr();
                for (unsigned int occ=0; occ<nOccurrences; occ++)
                    fEvTime.push_back((i+uniformGen.Rndm())*dt);
            }
            
            fMsg.Print(Form("Seed: %d", fRandomSeed));
            fMsg.Print(Form("Input signal frequency: %3.2f Hz, Time duration: %3.2f sec", fSigFreqHz, fTDurationSec));
            fMsg.Print(Form("Signal occurence: %d, Simulated signal frequency: %3.2f Hz", fEvTime.size(), fEvTime.size()/fTDurationSec));
            fMsg.Print(Form("Total input MC events: %d", fNEntries));
        }

        void FillSegments(PMTHitCluster& segmentID, PMTHitCluster& segmentOD, double tSegStart, double tSegEnd)
        {
            auto signalEvID = GetRangeIndex(fEvTime, tSegStart*1e-9 - 1e-6, tSegEnd*1e-9);

            for (unsigned long iEv=0; iEv<signalEvID.size(); iEv++) {
                double evTime = fEvTime[signalEvID[iEv]] * 1e9;
                auto evID = signalEvID[iEv];
                GetEntry(evID);
                fMsg.Print(Form("Signal event ID: %d, Global time: %3.2f msec", evID, evTime*1e-6));
        
                PMTHitCluster signalIDHits = PMTHitCluster(fIDTQ, 2) + (evTime - 1000);
                // disable signal OD for now
                //PMTHitCluster signalODHits = PMTHitCluster(fODTQ, 2) + (evTime - 1000);
                
                for (auto const& hit: signalIDHits) {
                    if ((tSegStart < hit.t()) && (hit.t() < tSegEnd))
                        segmentID.Append(hit);
                }
                
                //for (auto const& hit: signalODHits) {
                //    if ((tSegStart < hit.t()) && (hit.t() < tSegEnd))
                //        segmentOD.Append(hit);
                //}
            }
        }

    private:
        double fSigFreqHz;
        unsigned int fRandomSeed;
};

class NoiseReader : public RootReader
{
    public:
        NoiseReader(std::string name, std::string filePath)
        : RootReader(name, filePath) {}
        
        void SetEventTime()
        {
            double addedNoiseLength = 0;
            unsigned long evID = 0;
            fEvTime.push_back(0);

            while (addedNoiseLength < fTDurationSec) {
                GetEntry(evID);
                addedNoiseLength += (fIDTQ->T[fIDTQ->nhits-1] - fIDTQ->T[0]) * 1e-9;
                fEvTime.push_back(addedNoiseLength);
                evID++;
            }
            
            //for (unsigned int i=0; i<fEvTime.size(); i++) {
            //    double t = fEvTime[i];
            //    fMsg.Print(Form("%d: %3.9f", i, t));
            //}
        }

        void FillSegments(PMTHitCluster& segmentID, PMTHitCluster& segmentOD, double tSegStart, double tSegEnd)
        {
            auto evIDList = GetRangeIndex(fEvTime, tSegStart*1e-9, tSegEnd*1e-9);
            
            if (evIDList[0]>1)
                evIDList.insert(evIDList.begin(), evIDList[0]-1);
            
            for (unsigned long iEv=0; iEv<evIDList.size(); iEv++) {
                double evTime = fEvTime[evIDList[iEv]] * 1e9;
                double nextEvTime = fEvTime[evIDList[iEv]+1] * 1e9;
                auto evID = evIDList[iEv];
                GetEntry(evID);
        
                PMTHitCluster noiseIDHits = PMTHitCluster(fIDTQ, 1) ;
                PMTHitCluster noiseODHits = PMTHitCluster(fODTQ, 1);
                
                //std::vector<Float> noiseEvHitT = noiseIDHits.GetProjection(HitFunc::T);
                //double rawEvNoiseStartT = noiseEvHitT[GetMinIndex(noiseEvHitT)];
                double rawEvNoiseStartT = fIDTQ->T[0];
                
                noiseIDHits = noiseIDHits + (evTime - rawEvNoiseStartT);
                noiseODHits = noiseODHits + (evTime - rawEvNoiseStartT);
                
                double tMinLimit = evTime>tSegStart ? evTime : tSegStart;
                double tMaxLimit = nextEvTime>tSegEnd ? tSegEnd : nextEvTime;
                
                unsigned int appendedHits = 0;
                for (auto const& hit: noiseIDHits) {
                    if ((tMinLimit < hit.t()) && (hit.t() < tMaxLimit)) {
                        segmentID.Append(hit);
                        appendedHits++;
                    }
                }
                
                for (auto const& hit: noiseODHits) {
                    if ((tMinLimit < hit.t()) && (hit.t() < tMaxLimit)) {
                        segmentOD.Append(hit);
                    }
                }
                
                fMsg.Print(Form("Noise event ID: %d, Fill range: (%3.2f, %3.2f) msec, %d hits", evID, tMinLimit*1e-6, tMaxLimit*1e-6, appendedHits), pDEBUG);
            }
        }
};

#endif