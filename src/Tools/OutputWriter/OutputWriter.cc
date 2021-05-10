#include "TFile.h"
#include "TTree.h"
#include "TSysEvtHandler.h"

#include "OutputWriter.hh"

bool OutputWriter::Initialize()
{
    TString outFilePath;
    sharedData->ntagInfo.Get("output_file_path", outFilePath);

    handler = new TInterruptHandler(this);
    handler->Add();

    outFile = new TFile(outFilePath, "recreate");
    candidateTree = new TTree("ntvar", "NTag candidates");

    return true;
}

bool OutputWriter::CheckSafety()
{
    sharedData->eventCandidates.FillVectorMap();

    for (auto& pair: sharedData->eventCandidates.featureVectorMap) {
        std::cout << pair.first << " " << pair.second << std::endl;
        candidateTree->Branch(pair.first.c_str(), &(pair.second));
    }

    safeToExecute = true;
    return safeToExecute;
}

bool OutputWriter::Execute()
{
    sharedData->eventCandidates.FillVectorMap();
    candidateTree->Fill();
    return true;
}

bool OutputWriter::Finalize()
{
    candidateTree->Print();
    candidateTree->Write();
    outFile->Close();

    return true;
}