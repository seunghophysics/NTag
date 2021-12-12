#include "TFile.h"
#include "TTree.h"

#include "TaggableTree.hh"
#include "NTagTree.hh"

#include "CandidateTagger.hh"
#include "EventNTagManager.hh"

void CandidateTagger::Apply(const char* inFilePath, const char* outFilePath, float tMatchWindow)
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
    inTaggableTree->SetBranchStatus("TaggedType", 0);

    fMsg.Print("Cloning trees...");
    TFile* outFile = new TFile(outFilePath, "recreate");
    fMsg.Print("Cloning ntag tree...");
    TTree* outNtagTree = inNtagTree->CloneTree(-1, "fast");
    fMsg.Print("Cloning taggable tree...");
    TTree* outTaggableTree = inTaggableTree->CloneTree(-1, "fast");
    fMsg.Print("Cloning settings tree...");
    TTree* outSettingsTree = inSettingsTree->CloneTree();
    fMsg.Print("Cloning event tree...");
    TTree* outEventTree = inEventTree->CloneTree();
    fMsg.Print("Cloning particle tree...");
    TTree* outParticleTree = inParticleTree->CloneTree();
    TTree* outMuechkTree = 0;
    if (inMuechkTree) {
        fMsg.Print("Cloning muechk tree...");
        outMuechkTree = inMuechkTree->CloneTree();
    }
    fMsg.Print("Cloning complete!");
    
    NTagTree ntagTreeReader(inNtagTree);
    TaggableTree taggableTreeReader(inTaggableTree);

    // Replace old output with new one
    std::vector<float> tagOutList;
    std::vector<int> tagClassList;
    std::vector<int> taggedTypeList;
    TBranch* newOutBranch = outNtagTree->Branch("TagOut", &tagOutList);
    TBranch* newClassBranch = outNtagTree->Branch("TagClass", &tagClassList);
    TBranch* newTaggedTypeBranch = outTaggableTree->Branch("TaggedType", &taggedTypeList);

    long nEntries = inNtagTree->GetEntries();

    for (long iEntry = 0; iEntry < nEntries; iEntry++) {

        fMsg.Print(Form("Processing entry %ld / %ld...\r", iEntry, nEntries), pDEFAULT, false);
        std::cout << std::flush;

        tagOutList.clear();
        tagClassList.clear();
        taggedTypeList.clear();

        //if (!inNtagTree->GetEntry(iEntry) || 
        //    !inTaggableTree->GetEntry(iEntry)) continue;
        
        ntagTreeReader.GetEntry(iEntry);
        taggableTreeReader.GetEntry(iEntry);

        for (auto const& candidate: ntagTreeReader.cluster) {
            tagOutList.push_back(GetLikelihood(candidate));
            tagClassList.push_back(Classify(candidate));
        }

        EventNTagManager::Map(taggableTreeReader.cluster, ntagTreeReader.cluster, tMatchWindow);
        
        for (auto const& taggable: taggableTreeReader.cluster) {
            taggedTypeList.push_back(taggable.TaggedType());
        }

        newOutBranch->Fill();
        newClassBranch->Fill();
        newTaggedTypeBranch->Fill();
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

    fMsg.Print(fName + " application complete!                ");
}