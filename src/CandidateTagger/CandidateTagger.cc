#include "TFile.h"
#include "TTree.h"

#include "TaggableTree.hh"
#include "NTagTree.hh"

#include "CandidateTagger.hh"
#include "EventNTagManager.hh"

void CandidateTagger::Apply(const char* inFilePath, const char* outFilePath, float tMatchWindow)
{
    TMATCHWINDOW = tMatchWindow;

    TFile* inFile = TFile::Open(inFilePath);
    TTree* inSettingsTree = (TTree*)inFile->Get("settings");
    TTree* inEventTree = (TTree*)inFile->Get("event");
    TTree* inParticleTree = (TTree*)inFile->Get("particle");
    TTree* inTaggableTree = (TTree*)inFile->Get("taggable");
    TTree* inMuechkTree = (TTree*)inFile->Get("muechk");
    TTree* inNtagTree = (TTree*)inFile->Get("ntag");

    // disable copying of each branch that is to be reevaluated
    inEventTree->SetBranchStatus("NTaggedE", 0);
    inEventTree->SetBranchStatus("NTaggedN", 0);
    inTaggableTree->SetBranchStatus("TaggedType", 0);
    inNtagTree->SetBranchStatus("TagOut", 0);
    inNtagTree->SetBranchStatus("TagClass", 0);

    TFile* outFile = new TFile(outFilePath, "recreate");
    fMsg.Print("Cloning ntag tree...");
    TTree* outNtagTree = inNtagTree->CloneTree(-1, "fast");
    fMsg.Print("Cloning taggable tree...");
    TTree* outTaggableTree = inTaggableTree->CloneTree(-1, "fast");
    fMsg.Print("Cloning settings tree...");
    TTree* outSettingsTree = inSettingsTree->CloneTree();
    fMsg.Print("Cloning event tree...");
    TTree* outEventTree = inEventTree->CloneTree(-1, "fast");
    fMsg.Print("Cloning particle tree...");
    TTree* outParticleTree = inParticleTree->CloneTree();
    TTree* outMuechkTree = 0;
    if (inMuechkTree) {
        fMsg.Print("Cloning muechk tree...");
        outMuechkTree = inMuechkTree->CloneTree();
    }
    
    NTagTree ntagTreeReader(inNtagTree);
    TaggableTree taggableTreeReader(inTaggableTree);

    // Replace old output with new one
    int nTaggedE, nTaggedN;
    std::vector<float> tagOutList;
    std::vector<int> tagClassList;
    std::vector<int> taggedTypeList;
    TBranch* newNTaggedE = outEventTree->Branch("NTaggedE", &nTaggedE);
    TBranch* newNTaggedN = outEventTree->Branch("NTaggedN", &nTaggedN);
    TBranch* newOutBranch = outNtagTree->Branch("TagOut", &tagOutList);
    TBranch* newClassBranch = outNtagTree->Branch("TagClass", &tagClassList);
    TBranch* newTaggedTypeBranch = outTaggableTree->Branch("TaggedType", &taggedTypeList);

    long nEntries = inNtagTree->GetEntries();

    for (long iEntry = 0; iEntry < nEntries; iEntry++) {

        fMsg.Print(Form("Processing entry %ld / %ld...\r", iEntry, nEntries), pDEFAULT, false);
        std::cout << std::flush;

        nTaggedE = 0; nTaggedN = 0;
        tagOutList.clear();
        tagClassList.clear();
        taggedTypeList.clear();

        ntagTreeReader.GetEntry(iEntry);
        taggableTreeReader.GetEntry(iEntry);

        for (auto& candidate: ntagTreeReader.cluster) {
            float tagOut = GetLikelihood(candidate);
            int tagClass = Classify(candidate);
            candidate.Set("TagOut", tagOut);
            candidate.Set("TagClass", tagClass);
            tagOutList.push_back(tagOut);
            tagClassList.push_back(tagClass);
        }
        
        nTaggedE = std::count_if(tagClassList.begin(), tagClassList.end(), [](int tagclass){ return tagclass==typeE; });
        nTaggedN = std::count_if(tagClassList.begin(), tagClassList.end(), [](int tagclass){ return tagclass==typeN; });

        EventNTagManager::ResetTaggableMapping(taggableTreeReader.cluster);
        EventNTagManager::Map(taggableTreeReader.cluster, ntagTreeReader.cluster, TMATCHWINDOW);
        
        for (auto const& taggable: taggableTreeReader.cluster) {
            taggedTypeList.push_back(taggable.TaggedType());
        }

        newNTaggedE->Fill();
        newNTaggedN->Fill();
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
    
    OverrideSettings(outFilePath);

    fMsg.Print(fName + " application complete!                ");
}

void CandidateTagger::OverrideSettings(const char* outFilePath)
{
    TFile* f = new TFile(outFilePath, "UPDATE");
    TTree* inSettingsTree = (TTree*)f->Get("settings");
    inSettingsTree->SetBranchStatus("TMATCHWINDOW", 0);
    TTree* outSettingsTree = inSettingsTree->CloneTree(-1, "fast");
    TBranch* b_TMATCHWINDOW = outSettingsTree->Branch("TMATCHWINDOW", &TMATCHWINDOW);

    b_TMATCHWINDOW->Fill();
    outSettingsTree->Write();
    //f->Delete(inSettingsTree);
    
    f->Close();
}