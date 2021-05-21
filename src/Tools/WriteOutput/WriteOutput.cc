#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TSysEvtHandler.h"

#include "WriteOutput.hh"

bool WriteOutput::Initialize()
{
    sharedData->ntagInfo.Get("is_mc", inputIsMC);

    TString outFilePath;
    sharedData->ntagInfo.Get("output_file_path", outFilePath);
    sharedData->ntagInfo.Get("output_mode", outputMode);
    
    Log("Output mode: " + outputMode);

    handler = new TInterruptHandler(this);
    handler->Add();

    outFile = new TFile(outFilePath, outputMode);
    outFile->cd();

    if (outputMode == "update") {
        if (CheckTreesExist()) {
            Log("Trees exist!");
            GetTrees();
        }
        else {
            Log("Trees don't exist!");
            CreateTrees();
            outputMode = "recreate";
        }
    }
    else
        CreateTrees();

    tmpVecPtr = &tmpVec;
    tmpStrPtr = &tmpStr;

    return true;
}

bool WriteOutput::CheckSafety()
{
    sharedData->eventCandidates.FillVectorMap();
    
    if (outputMode == "update") {
        SetBranches(variableTree, &(sharedData->eventVariables));
        SetBranches(ntagInfoTree, &(sharedData->ntagInfo));
        for (auto& pair: sharedData->eventCandidates.featureVectorMap)
            candidateTree->SetBranchAddress(pair.first.c_str(), &(pair.second));
    }
    else {
        MakeBranches(variableTree, &(sharedData->eventVariables));
        MakeBranches(ntagInfoTree, &(sharedData->ntagInfo));
        for (auto& pair: sharedData->eventCandidates.featureVectorMap)
            candidateTree->Branch(pair.first.c_str(), &(pair.second));
    }
    FillBranches(ntagInfoTree, &(sharedData->ntagInfo));
    

    safeToExecute = true;
    return safeToExecute;
}

bool WriteOutput::Execute()
{
    sharedData->eventCandidates.FillVectorMap();
    candidateTree->Fill();
    
    FillBranches(variableTree, &(sharedData->eventVariables));
    
    sharedData->eventVariables.Print();
    return true;
}

bool WriteOutput::Finalize()
{
    int verbose = pDEFAULT;
    sharedData->ntagInfo.Get("verbose", verbose);
    if (verbose > pDEFAULT) PrintTrees();
    
    outFile->cd();
    
    int option = outputMode == "update" ? TObject::kWriteDelete : TObject::kOverwrite;
    WriteTrees(option);
    
    outFile->Close();
    
    return true;
}

void WriteOutput::CreateTrees()
{
    variableTree = new TTree("variables", "Event variables");
    candidateTree = new TTree("candidates", "Neutron-signal candidate features");
    
    // NTag
    ntagInfoTree = new TTree("ntaginfo", "NTag options and information");
    
    // MC
    if (inputIsMC) {
        primaryTree = new TTree("primaries", "Event primaries");
        secondaryTree = new TTree("secondaries", "Event secondaries");
        trueCaptureTree = new TTree("truecaptures", "Event true captures");
    }
}

bool WriteOutput::CheckTreesExist()
{
    bool variableTreeExists = (outFile->Get("variables") != nullptr);
    bool candidateTreeExists = (outFile->Get("candidates") != nullptr);
    bool ntagInfoTreeExists = (outFile->Get("ntaginfo") != nullptr);
    bool primaryTreeExists = (outFile->Get("primaries") != nullptr);
    bool secondaryTreeExists = (outFile->Get("secondaries") != nullptr);
    bool trueCaptureTreeExists = (outFile->Get("truecaptures") != nullptr);
    
    bool dataTreesExist = variableTreeExists && candidateTreeExists && ntagInfoTreeExists;
    bool mcTreesExist = primaryTreeExists && secondaryTreeExists && trueCaptureTreeExists;
    
    if (!inputIsMC)
        return dataTreesExist;
    else
        return dataTreesExist && mcTreesExist;
}

void WriteOutput::GetTrees()
{
    variableTree = (TTree*)outFile->Get("variables");
    candidateTree = (TTree*)outFile->Get("candidates");
     
    // NTag
    ntagInfoTree = (TTree*)outFile->Get("ntaginfo");
    
    // MC
    if (inputIsMC) {
        primaryTree = (TTree*)outFile->Get("primaries");
        secondaryTree = (TTree*)outFile->Get("secondaries");
        trueCaptureTree = (TTree*)outFile->Get("truecaptures");
    }
    
    fillCounter = variableTree->GetEntries();
}

void WriteOutput::PrintTrees()
{
    ntagInfoTree->Print();
    variableTree->Print();
    candidateTree->Print();
    
    if (inputIsMC) {
        primaryTree->Print();
        secondaryTree->Print();
        trueCaptureTree->Print();
    }
}

void WriteOutput::WriteTrees(int option)
{
    ntagInfoTree->Write(0, option, 0);
    variableTree->Write(0, option, 0);
    candidateTree->Write(0, option, 0);
    
    if (inputIsMC) {
        primaryTree->Write(0, option, 0);
        secondaryTree->Write(0, option, 0);
        trueCaptureTree->Write(0, option, 0);
    }
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

void WriteOutput::SetBranches(TTree* tree, Store* store)
{
    for (auto const& pair: store->GetMap()) {
        auto key = pair.first.c_str();
        auto value = pair.second;
        
        if (store->Get(key, tmpVec)) {
            tree->SetBranchAddress(key, &tmpVecPtr);
        }
        else if (store->Get(key, tmpNum)) {
            tree->SetBranchAddress(key, &tmpNum);
        }
        else {
            tmpStr = value;
            tree->SetBranchAddress(key, &tmpStrPtr);
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
    tree->SetEntries(fillCounter==1 ? 1 : fillCounter-1);
}