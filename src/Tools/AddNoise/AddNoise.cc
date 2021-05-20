#include "TROOT.h"
#include "TChain.h"

#include "tqrealroot.h"
#undef MAXPM
#undef MAXPMA

#include "PathGetter.hh"

#include "AddNoise.hh"

bool AddNoise::Initialize()
{
    iEntry = 0; nEntries = 0;
    iPart = 0; nParts = 0;
    iHit = 0; nHits = 0;

    TString noiseFilePath;
    sharedData->ntagInfo.Get("noise_file_path", noiseFilePath);
    
    tqi = new TQReal;
    
    // read in noise file
    noiseChain.Add(noiseFilePath);
    Log("Getting number of entries in the noise chain...");
    nEntries = noiseChain.GetEntries();
    Log(Form("Number of entries in the noise chain: %d", nEntries));
    noiseChain.SetBranchAddress("TQREAL", &tqi);
    std::cout << std::endl;
    
    // read in config files
    sharedData->ntagInfo.Get("noise_start_time", noiseStartTime);
    sharedData->ntagInfo.Get("noise_end_time", noiseEndTime);
    noiseTimeWindowWidth = noiseEndTime - noiseStartTime;
    if (noiseTimeWindowWidth < 0) {
        Log(Form("Noise start time (%3.2f us) is later than noise end time (%3.2f us)!", noiseStartTime, noiseEndTime), pWARNING);
        Log("Please check the config file and correct the options.", pERROR);
    }
    noiseStartTime *= 1e3; noiseEndTime *= 1e3; noiseTimeWindowWidth *= 1e3; // microseconds to nanoseconds
    
    return true;
}

bool AddNoise::CheckSafety()
{
    return true;
}

bool AddNoise::Execute()
{
    if (iPart == nParts) {
        GetNewEntry();
        SetNoiseHitCluster();
    }

    Log(Form("Starting adding part %d", iPart));
    
    PMTHitCluster* eventHits = &(sharedData->eventPMTHits); 
    
    //Log("Before appending noise");
    //eventHits->DumpAllElements();
    
    float partStartTime = noiseT0 + iPart * noiseTimeWindowWidth;
    float partEndTime = partStartTime + noiseTimeWindowWidth;
    Log(Form("part start time: %3.2f ns, part end time: %3.2f", partStartTime, partEndTime));
    
    Log(Form("t[ihit]: %3.2f ns", t[iHit]));
    
    while (noiseEventHits[iHit].t() < partStartTime) iHit++;
    
    Log(Form("t[ihit]: %3.2f ns", t[iHit]));

    while (noiseEventHits[iHit].t() < partEndTime) {
        PMTHit hit = noiseEventHits[iHit];
        PMTHit shiftedHit(hit.t() - partStartTime + noiseStartTime, hit.q(), hit.i());
        eventHits->Append(shiftedHit);
        iHit++;
    }

    //Log("After appending noise");
    eventHits->Sort();
    //eventHits->DumpAllElements();

    Log(Form("Completed adding part %d", iPart));
    iPart++;

    return true;
}

bool AddNoise::Finalize()
{
    delete tqi;
    return true;
}

void AddNoise::GetNewEntry()
{
    Log("Getting new entry");
    iPart = 0; iHit = 0;
    iEntry++; noiseEventHits.Clear();
    if (iEntry < nEntries) {
        noiseChain.GetEntry(iEntry);
    }
    else {
        Log("Reached the end of ntuple!");
        throw eENDRUN;
    }
}

void AddNoise::SetNoiseHitCluster()
{
    t = tqi->T; 
    q = tqi->Q;
    i = tqi->cables;
    int nRawHits = t.size();
    
    for (unsigned int j=0; j<=nRawHits; j++)
        noiseEventHits.Append({t[j], q[j], i[j]&0x0000FFFF});
        
    noiseEventHits.Sort();
    nHits = noiseEventHits.GetSize();
    noiseEventLength = noiseEventHits[nHits-1].t() - noiseEventHits[0].t();
    Log(Form("first hit time: %3.2f last hit time: %3.2f", noiseEventHits[0].t(), noiseEventHits[nHits-1].t()));
    nParts = (int)(noiseEventLength / noiseTimeWindowWidth);
    Log(Form("nparts: %d", nParts));
    
    Log("Event noise:");
    //noiseEventHits.DumpAllElements();
    
    Log(Form("noise nHits: %d, eventLength: %3.2f us", nHits, noiseEventLength*1e-3));
    if (nParts == 0) {
        Log(Form("The noise event entry %d is not long enough: the minimum required length is %3.2f us.", 
                 iEntry, noiseTimeWindowWidth), pWARNING);
        Log("Skipping this noise event...", pWARNING); 
        GetNewEntry();
    }
    noiseT0 = t[0] + (noiseEventLength - nParts*noiseTimeWindowWidth)/2.;
    Log(Form("noiseT0: %3.2f ns", noiseT0));
}