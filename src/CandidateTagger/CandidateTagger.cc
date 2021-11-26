#include "TFile.h"
#include "TTree.h"

#include "NTagTree.hh"

#include "CandidateTagger.hh"

void CandidateTagger::Apply(const char* inFilePath, const char* outFilePath)
{
    TFile* inFile = TFile::Open(inFilePath);
    TTree* inNtagTree = (TTree*)inFile->Get("ntag");
    TTree* inSettingsTree = (TTree*)inFile->Get("settings");
    TTree* inEventTree = (TTree*)inFile->Get("event");
    TTree* inParticleTree = (TTree*)inFile->Get("particle");
    TTree* inTaggableTree = (TTree*)inFile->Get("taggable");
    TTree* inMuechkTree = (TTree*)inFile->Get("muechk");

    if (inNtagTree->GetListOfBranches()->FindObject("TagOut"))
        inNtagTree->SetBranchStatus("TagOut", 0);
    if (inNtagTree->GetListOfBranches()->FindObject("TagClass"))
        inNtagTree->SetBranchStatus("TagClass", 0);

    TFile* outFile = new TFile(outFilePath, "recreate");
    TTree* outNtagTree = inNtagTree->CloneTree(-1, "fast");
    TTree* outSettingsTree = inSettingsTree->CloneTree();
    TTree* outEventTree = inEventTree->CloneTree();
    TTree* outParticleTree = inParticleTree->CloneTree();
    TTree* outTaggableTree = inTaggableTree->CloneTree();
    TTree* outMuechkTree = 0;
    if (inMuechkTree)
        outMuechkTree = inMuechkTree->CloneTree();
    
    NTagTree ntagTreeReader(inNtagTree);

    // Replace old output with new one
    std::vector<float> tagOutList;
    std::vector<int> tagClassList;
    TBranch* newOutBranch = outNtagTree->Branch("TagOut", &tagOutList);
    TBranch* newClassBranch = outNtagTree->Branch("TagClass", &tagClassList);

    long nEntries = inNtagTree->GetEntries();

    for(long iEntry = 0; iEntry < nEntries; iEntry++){

        fMsg.Print(Form("Processing entry %ld / %ld...\r", iEntry, nEntries), pDEFAULT, false);
        std::cout << std::flush;

        tagOutList.clear();
        tagClassList.clear();

        if (!inNtagTree->GetEntry(iEntry)) continue;
        
        ntagTreeReader.GetEntry(iEntry);
        for (auto const& candidate: ntagTreeReader.cluster) {
            tagOutList.push_back(GetLikelihood(candidate));
            tagClassList.push_back(Classify(candidate));
        }

        newOutBranch->Fill();
        newClassBranch->Fill();
    }

    outSettingsTree->Write();
    outEventTree->Write();
    outParticleTree->Write();
    outTaggableTree->Write();
    if (outMuechkTree)
        outMuechkTree->Write();
    outNtagTree->Write();

    outFile->Close();
    inFile->Close();

    fMsg.Print(fName + " application complete!");
}