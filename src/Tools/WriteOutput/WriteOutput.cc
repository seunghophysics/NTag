#include "TFile.h"
#include "TTree.h"
#include "TSysEvtHandler.h"

#include "WriteOutput.hh"

bool WriteOutput::Initialize()
{
    TString outFilePath;
    sharedData->ntagInfo.Get("output_file_path", outFilePath);

    handler = new TInterruptHandler(this);
    handler->Add();

    outFile = new TFile(outFilePath, "recreate");
    candidateTree = new TTree("ntvar", "NTag candidates");

    return true;
}

bool WriteOutput::CheckSafety()
{
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
    return true;
}

bool WriteOutput::Finalize()
{
    candidateTree->Print();
    candidateTree->Write();
    outFile->Close();
    return true;
}