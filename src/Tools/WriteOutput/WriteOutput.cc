#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TSysEvtHandler.h"

#include "WriteOutput.hh"

bool WriteOutput::Initialize()
{
    TString outFilePath;
    sharedData->ntagInfo.Get("output_file_path", outFilePath);

    handler = new TInterruptHandler(this);
    handler->Add();

    outFile = new TFile(outFilePath, "recreate");
    variableTree = new TTree("variables", "Event variables");
    candidateTree = new TTree("candidates", "Neutron-signal candidate features");
    
    // MC
    primaryTree = new TTree("primaries", "Event primaries");
    secondaryTree = new TTree("secondaries", "Event secondaries");
    trueCaptureTree = new TTree("truecaptures", "Event true captures");
    
    // NTag
    ntagInfoTree = new TTree("ntaginfo", "NTag options and information");

    return true;
}

bool WriteOutput::CheckSafety()
{
    MakeBranches(variableTree, &(sharedData->eventVariables));
    MakeBranches(ntagInfoTree, &(sharedData->ntagInfo));
    FillBranches(ntagInfoTree, &(sharedData->ntagInfo));
    
    sharedData->eventCandidates.FillVectorMap();
    for (auto& pair: sharedData->eventCandidates.featureVectorMap)
        candidateTree->Branch(pair.first.c_str(), &(pair.second));

    safeToExecute = true;
    return safeToExecute;
}

bool WriteOutput::Execute()
{
    sharedData->eventCandidates.FillVectorMap();
    candidateTree->Fill();
    
    FillBranches(variableTree, &(sharedData->eventVariables));
    return true;
}

bool WriteOutput::Finalize()
{
    variableTree->Print();
    variableTree->Write();
    
    candidateTree->Print();
    candidateTree->Write();
    
    ntagInfoTree->Print();
    ntagInfoTree->Write();
    
    outFile->Close();
    return true;
}

void WriteOutput::MakeBranches(TTree* tree, Store* store)
{
    for (auto const& pair: store->GetMap()) {
        auto key = pair.first.c_str();
        auto value = pair.second;
        
        if (store->Get(key, tmpVec)) {
            tree->Branch(key, &tmpVec);
        }
        else if (store->Get(key, tmpNum)) {
            tree->Branch(key, &tmpNum);
        }
        else {
            tmpStr = value;
            tree->Branch(key, &tmpStr);
        }       
    }
}

void WriteOutput::FillBranches(TTree* tree, Store* store)
{
    for (auto const& pair: store->GetMap()) {
        auto key = pair.first;
        if (!store->Get(key, tmpVec)) {
            if (!store->Get(key, tmpNum)) {
                store->Get(key, tmpStr);
            }
        }
        
        tree->GetBranch(key.c_str())->Fill();
    }
    
    fillCounter++;
    tree->SetEntries(fillCounter-1);
}