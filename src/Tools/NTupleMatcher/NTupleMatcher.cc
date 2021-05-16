#include "TFile.h"
#include "TTree.h"

#include "skheadC.h"

#include "NTupleMatcher.hh"

bool NTupleMatcher::Initialize()
{
    eventNo = 0; iEntry = 0; nEntries = 0;

    // get ntuple file name and file id
    TString ntupleFilePath; int fileID;
    sharedData->ntagInfo.Get("ntuple_file_path", ntupleFilePath);
    sharedData->ntagInfo.Get("mc_file_id", fileID);
    Log("NTuple file path: " + ntupleFilePath);
    Log(Form("MC file ID: %d", fileID));
    
    // get ntuple and set branch address of event number
    TFile* f = TFile::Open(ntupleFilePath);
    ntuple = (TTree*)f->Get("h1");
    ntuple->SetBranchAddress("nev", &eventNo);
    nEntries = ntuple->GetEntries();
    
    // fill the first event number
    int prevEventNo = -1;
    
    // skip file
    for (int i=0; i<fileID; i++) {
        while (prevEventNo <= eventNo) {
            prevEventNo = eventNo;
            GetNewEntry();
        }
        prevEventNo = eventNo;
    }
    Log(Form("NTuple iEntry: %d, nEntries: %d", iEntry, nEntries), pDEBUG);
    
    return true;
}

bool NTupleMatcher::CheckSafety()
{
    return true;
}

bool NTupleMatcher::Execute()
{
    // get event number from skread
    if (skhead_.nevsk < eventNo) {
        Log(Form("No matching event in the ntuple. Skipping...", skhead_.nevsk, eventNo));
        throw eSKIPEVENT;
    }
    else if (skhead_.nevsk == eventNo) {
        sharedData->eventVariables.Set("EventNo", eventNo);
        Log(Form("A matching event exists in the ntuple! Continuing toolchain execution...", eventNo));
        try { GetNewEntry(); }
        catch (ExceptionBehavior& e) { throw e; }
        return true;
    }

    return true;
}

bool NTupleMatcher::Finalize()
{
    return true;
}

void NTupleMatcher::GetNewEntry()
{
    iEntry++;
    if (iEntry < nEntries) {
        ntuple->GetEntry(iEntry);
    }
    else {
        Log("Reached the end of ntuple!");
        throw eENDRUN;
    }
}