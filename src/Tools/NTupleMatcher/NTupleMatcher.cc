#include "TFile.h"
#include "TTree.h"

#include "skheadC.h"

#include "NTupleReader.hh"
#include "NTupleMatcher.hh"

bool NTupleMatcher::Initialize()
{
    iEntry = 0; nEntries = 0; eventNo = 1;

    // get ntuple file name and file id
    TString ntupleFilePath; int fileID;
    sharedData->ntagInfo.Get("ntuple_file_path", ntupleFilePath);
    sharedData->ntagInfo.Get("mc_file_id", fileID);
    
    // get ntuple and set branch address of event number
    ntupleFile = TFile::Open(ntupleFilePath);
    ntuple = (TTree*)ntupleFile->Get("h1");
    //ntuple->SetBranchAddress("nev", &eventNo);
    nEntries = ntuple->GetEntries();
    ntupleReader = new NTupleReader(ntuple);
    ntupleReader->nev = 1;
    
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
    Log(Form("NTuple iEntry: %d, nEntries: %d", iEntry, nEntries));
    std::cout << "\n";
    
    return true;
}

bool NTupleMatcher::CheckSafety()
{
    return true;
}

bool NTupleMatcher::Execute()
{
    // get event number from skread
    Log(Form("skhead_.nevsk: %d, eventNo: %d", skhead_.nevsk, eventNo));
    if (skhead_.nevsk < eventNo) {
        Log(Form("No matching event in the ntuple. Skipping...", skhead_.nevsk, eventNo));
        throw eSKIPEVENT;
    }
    else if (skhead_.nevsk == eventNo) {
        SetNTupleVariables();
        Log(Form("A matching event exists in the ntuple! Continuing toolchain execution...", eventNo));
        try { GetNewEntry(); }
        catch (ExceptionBehavior& e) { throw e; }
        return true;
    }
    else {
        Log(Form("skhead_.nevsk %d is larger than eventNo %d! Ending toolchain execution...", skhead_.nevsk, eventNo));
        throw eENDRUN;
    }

    return true;
}

bool NTupleMatcher::Finalize()
{
    ntupleFile->Close();
    delete ntupleReader;
    return true;
}

void NTupleMatcher::GetNewEntry()
{
    iEntry++;
    if (iEntry < nEntries) {
        ntupleReader->GetEntry(iEntry);
        eventNo = ntupleReader->nev;
    }
    else {
        Log("Reached the end of ntuple!");
        throw eENDRUN;
    }
}

void NTupleMatcher::SetNTupleVariables()
{
    sharedData->eventVariables.Set("nev", eventNo);
    sharedData->eventVariables.Set("nring", ntupleReader->nring);
    sharedData->eventVariables.Set("nhitac", ntupleReader->nhitac);
    sharedData->eventVariables.Set("evis", ntupleReader->evis);
    sharedData->eventVariables.Set("wall", ntupleReader->wall);
    sharedData->eventVariables.Set("ip", ntupleReader->ip[0]);
    sharedData->eventVariables.Set("amome", ntupleReader->amome[0]);
    sharedData->eventVariables.Set("amomm", ntupleReader->amomm[0]);
    sharedData->eventVariables.Set("potot", ntupleReader->potot);
    sharedData->eventVariables.Set("nn", ntupleReader->ntag_nn);
    sharedData->eventVariables.Set("mctruth_nn", ntupleReader->ntag_mctruth_nn);
}