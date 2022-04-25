#include "TFile.h"
#include "TTree.h"
#include "TTreeFormula.h"

#include "NTagGlobal.hh"
#include "TaggableTree.hh"
#include "NTagTree.hh"
#include "CandidateTagger.hh"
#include "NTagTMVAManager.hh"
#include "EventNTagManager.hh"

CandidateTagger::CandidateTagger(std::string fitterName, Verbosity verbose)
: fECuts("0"), fNCuts("0"), fECutFormula(nullptr), fNCutFormula(nullptr), TMATCHWINDOW(50), 
  fMsg(fitterName.c_str(), verbose)
{ 
    fName = fitterName;

    for (auto const& key: gMuechkFeatures)
        fFeatureMap[key] = 0;
    
    for (auto const& key: gNTagFeatures)
        fFeatureMap[key] = 0;

    fOutputTree = new TTree("features", "features");

    for (auto& pair: fFeatureMap) {
        fOutputTree->Branch(pair.first.c_str(), &pair.second);
    }
}

CandidateTagger::~CandidateTagger()
{
    delete fOutputTree;
    delete fECutFormula;
    delete fNCutFormula;
}

void CandidateTagger::SetECuts(std::string cuts)
{
    fECuts = cuts;
    fECutFormula = new TTreeFormula("e cuts", fECuts.c_str(), fOutputTree);
}

void CandidateTagger::SetNCuts(std::string cuts)
{
    fNCuts = cuts;
    fNCutFormula = new TTreeFormula("n cuts", fNCuts.c_str(), fOutputTree);
}

void CandidateTagger::Apply(std::string inFilePath, std::string outFilePath, NTagTMVAManager* tmvaManager)
{
    TFile* inFile = TFile::Open(inFilePath.c_str());
    TTree* inSettingsTree = (TTree*)inFile->Get("settings");
    TTree* inEventTree    = (TTree*)inFile->Get("event");
    TTree* inParticleTree = (TTree*)inFile->Get("particle");
    TTree* inTaggableTree = (TTree*)inFile->Get("taggable");
    TTree* inMuechkTree   = (TTree*)inFile->Get("mue");
    TTree* inNtagTree     = (TTree*)inFile->Get("ntag");

    // disable copying of each branch that is to be reevaluated
    inEventTree->SetBranchStatus("NTaggedE", 0);
    inEventTree->SetBranchStatus("NTaggedN", 0);
    inTaggableTree->SetBranchStatus("TaggedType", 0);
    inNtagTree->SetBranchStatus("TagOut", 0);
    inNtagTree->SetBranchStatus("TagClass", 0);

    TFile* outFile = new TFile(outFilePath.c_str(), "recreate");
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
    std::vector<int>   tagClassList;
    std::vector<int>   taggedTypeList;
    TBranch* newNTaggedE         = outEventTree->Branch("NTaggedE", &nTaggedE);
    TBranch* newNTaggedN         = outEventTree->Branch("NTaggedN", &nTaggedN);
    TBranch* newOutBranch        = outNtagTree->Branch("TagOut", &tagOutList);
    TBranch* newClassBranch      = outNtagTree->Branch("TagClass", &tagClassList);
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
            float tagOut = tmvaManager? tmvaManager->GetTMVAOutput(candidate) : 0;
            candidate.Set("TagOut", tagOut);
            int tagClass = Classify(candidate);
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
    outMuechkTree->Write();
    outNtagTree->Write();

    outFile->Close();
    inFile->Close();
    
    OverrideSettings(outFilePath.c_str());

    fMsg.Print(fName + " application complete!                ");
}

void CandidateTagger::OverrideSettings(std::string outFilePath)
{
    TFile* f = new TFile(outFilePath.c_str(), "UPDATE");
    TTree* inSettingsTree = (TTree*)f->Get("settings");
    inSettingsTree->SetBranchStatus("E_CUTS", 0);
    inSettingsTree->SetBranchStatus("N_CUTS", 0);
    inSettingsTree->SetBranchStatus("TMATCHWINDOW", 0);
    TTree* outSettingsTree = inSettingsTree->CloneTree(-1, "fast");
    TBranch* b_E_CUTS = outSettingsTree->Branch("E_CUTS", &fECuts);
    TBranch* b_N_CUTS = outSettingsTree->Branch("N_CUTS", &fNCuts);
    TBranch* b_TMATCHWINDOW = outSettingsTree->Branch("TMATCHWINDOW", &TMATCHWINDOW);

    b_E_CUTS->Fill();
    b_N_CUTS->Fill();
    b_TMATCHWINDOW->Fill();
    outSettingsTree->Write();
    //f->Delete(inSettingsTree);
    
    f->Close();
}

int CandidateTagger::Classify(const Candidate& candidate)
{
    for (auto const& pair: candidate.GetFeatureMap())
        fFeatureMap[pair.first] = pair.second;

    FillTree();

    int tagClass = typeMissed;
    if      (fECutFormula->EvalInstance()) tagClass = typeE;
    else if (fNCutFormula->EvalInstance()) tagClass = typeN;

    ClearTree();
    return tagClass;
}