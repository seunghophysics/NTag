#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TSysEvtHandler.h"

#include "WriteOutput.hh"

bool WriteOutput::Initialize()
{
    TClass::GetClass("TVector3")->IgnoreTObjectStreamer();

    TString outFilePath;
    sharedData->ntagInfo.Get("output_file_path", outFilePath);
    sharedData->ntagInfo.Get("output_mode", outputMode);
    
    Log("Output mode: " + outputMode);

    handler = new TInterruptHandler(this);
    handler->Add();

    outFile = new TFile(outFilePath, outputMode);

    tmpVecPtr = &tmpVec;
    tmpStrPtr = &tmpStr;

    return true;
}

bool WriteOutput::CheckSafety()
{
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

    if (inputIsMC) {
        Log("MC!");
        primaries = 0;
        secondaries = 0;
        trueCaptures = 0;
    }
    else
        Log("Data!");

    sharedData->eventCandidates.FillVectorMap();
    
    if (outputMode == "update") {
        SetBranches(variableTree, &(sharedData->eventVariables));
        SetBranches(ntagInfoTree, &(sharedData->ntagInfo));
        for (auto& pair: sharedData->eventCandidates.featureVectorMap)
            candidateTree->SetBranchAddress(pair.first.c_str(), &(pair.second));
    }
    else {
        MakeBranches(ntagInfoTree, &(sharedData->ntagInfo));
        MakeBranches(variableTree, &(sharedData->eventVariables));
        for (auto& pair: sharedData->eventCandidates.featureVectorMap)
            candidateTree->Branch(pair.first.c_str(), &(pair.second));
            
        if (inputIsMC) {
            mcTree->Branch("primaries", &primaries);
            mcTree->Branch("secondaries", &secondaries);
            mcTree->Branch("captures", &trueCaptures);
            
            primaries = &(sharedData->eventPrimaries);
            secondaries = &(sharedData->eventSecondaries);
            trueCaptures = &(sharedData->eventTrueCaptures);
        }
    }
    FillBranches(ntagInfoTree, &(sharedData->ntagInfo));
    
    safeToExecute = true;
    return safeToExecute;
}

bool WriteOutput::Execute()
{
    outFile->cd();
    
    sharedData->eventCandidates.FillVectorMap();
    candidateTree->Fill();
    
    FillBranches(variableTree, &(sharedData->eventVariables));
    sharedData->eventVariables.Print();
    
    if (inputIsMC) {
        //primaries->Copy(&(sharedData->eventPrimaries));
        //secondaries->Copy(&(sharedData->eventSecondaries));
        //trueCaptures->Copy(&(sharedData->eventTrueCaptures));
        
        primaries->DumpAllElements();
        secondaries->DumpAllElements();
        trueCaptures->DumpAllElements();
        
        mcTree->Fill();
    }
    
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
    
    //if (inputIsMC) {
    //    delete primaries;
    //    delete secondaries;
    //    delete trueCaptures;
    //}

    return true;
}

void WriteOutput::CreateTrees()
{
    variableTree = new TTree("variables", "Event variables");
    candidateTree = new TTree("candidates", "Neutron-signal candidate features");
    
    // NTag
    ntagInfoTree = new TTree("ntaginfo", "NTag options and information");
    
    // MC
    if (inputIsMC)
        mcTree = new TTree("mc", "MC truth information");
}

bool WriteOutput::CheckTreesExist()
{
    bool variableTreeExists = (outFile->Get("variables") != nullptr);
    bool candidateTreeExists = (outFile->Get("candidates") != nullptr);
    bool ntagInfoTreeExists = (outFile->Get("ntaginfo") != nullptr);
    bool mcTreeExists = (outFile->Get("mc") != nullptr);

    
    bool dataTreesExist = variableTreeExists && candidateTreeExists && ntagInfoTreeExists;
    
    if (!inputIsMC)
        return dataTreesExist;
    else
        return dataTreesExist && mcTreeExists;
}

void WriteOutput::GetTrees()
{
    variableTree = (TTree*)outFile->Get("variables");
    candidateTree = (TTree*)outFile->Get("candidates");
     
    // NTag
    ntagInfoTree = (TTree*)outFile->Get("ntaginfo");
    
    // MC
    if (inputIsMC)
        mcTree = (TTree*)outFile->Get("mc");
    
    fillCounter = variableTree->GetEntries();
}

void WriteOutput::PrintTrees()
{
    ntagInfoTree->Print();
    variableTree->Print();
    candidateTree->Print();
    
    if (inputIsMC)
        mcTree->Print();
}

void WriteOutput::WriteTrees(int option)
{
    ntagInfoTree->Write(0, option, 0);
    variableTree->Write(0, option, 0);
    candidateTree->Write(0, option, 0);
    
    if (inputIsMC)
        mcTree->Write(0, option, 0);
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