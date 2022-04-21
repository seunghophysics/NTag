#include "TFile.h"
#include "TTree.h"

#include "NTagGlobal.hh"
#include "NTagCutTagger.hh"

NTagCutTagger::NTagCutTagger(Verbosity verbose)
: CandidateTagger("NTagCutTagger", verbose), 
SCINTCUT(1.0), GOODNESSCUT(0.4), DIRKSCUT(0.4), DISTCUT(300) {}
NTagCutTagger::~NTagCutTagger() {}

void NTagCutTagger::OverrideSettings(const char* outFilePath)
{
    CandidateTagger::OverrideSettings(outFilePath);

    TFile* f = new TFile(outFilePath, "UPDATE");
    TTree* inSettingsTree = (TTree*)f->Get("settings");
    inSettingsTree->SetBranchStatus("SCINTCUT", 0);
    inSettingsTree->SetBranchStatus("GOODNESSCUT", 0);
    inSettingsTree->SetBranchStatus("DIRKSCUT", 0);
    inSettingsTree->SetBranchStatus("DISTCUT", 0);

    TTree* outSettingsTree = inSettingsTree->CloneTree(-1, "fast");
    TBranch* b_SCINTCUT = outSettingsTree->Branch("SCINTCUT", &SCINTCUT);
    TBranch* b_GOODNESSCUT = outSettingsTree->Branch("GOODNESSCUT", &GOODNESSCUT);
    TBranch* b_DIRKSCUT = outSettingsTree->Branch("DIRKSCUT", &DIRKSCUT);
    TBranch* b_DISTCUT = outSettingsTree->Branch("DISTCUT", &DISTCUT);

    b_SCINTCUT->Fill();
    b_GOODNESSCUT->Fill();
    b_DIRKSCUT->Fill();
    b_DISTCUT->Fill();
    outSettingsTree->Write();
    //f->Delete(inSettingsTree);
    
    f->Close();
}

int NTagCutTagger::Classify(const Candidate& candidate)
{
    int tagClass = typeMissed;
    bool isEarly = candidate.Get("N50", -1) < 0; // muechk flag

    float n30  = candidate.Get("N30");
    float n200 = candidate.Get("N200");
    float scintLikeliness = (n200-27)/(1.3*n30);

    if (isEarly)
        tagClass = typeE;

    // neutron selection cuts
    else if (   scintLikeliness              < SCINTCUT
             && candidate.Get("FitGoodness") > GOODNESSCUT
             && candidate.Get("BSDirKS")     < DIRKSCUT
             && candidate.Get("DPrompt")     < DISTCUT)
        tagClass = typeN;
    
    else
        tagClass = typeMissed;

    return tagClass;
}