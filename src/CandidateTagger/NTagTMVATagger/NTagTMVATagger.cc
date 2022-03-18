#include "TFile.h"
#include "TTree.h"

#include "Calculator.hh"
#include "NTagGlobal.hh"
#include "NTagTMVATagger.hh"

NTagTMVATagger::NTagTMVATagger(Verbosity verbose)
: CandidateTagger("NTagTMVATagger", verbose), 
fDoTagE(false), E_NHITSCUT(50), E_TIMECUT(20), N_OUTCUT(0.7) {}
NTagTMVATagger::~NTagTMVATagger() {}

void NTagTMVATagger::Initialize(std::string weightPath)
{
    if (weightPath == "bonsai" || weightPath == "trms" || weightPath == "prompt")
        fTMVAManager.SetWeightPath(GetENV("NTAGLIBPATH")+ "weights/" + weightPath + "/NTagTMVAFactory_MLP.weights.xml");
    else if (!weightPath.empty())
        fTMVAManager.SetWeightPath(weightPath);
    fTMVAManager.InitializeReader();
}

void NTagTMVATagger::OverrideSettings(const char* outFilePath)
{
    CandidateTagger::OverrideSettings(outFilePath);

    TFile* f = new TFile(outFilePath, "UPDATE");
    TTree* inSettingsTree = (TTree*)f->Get("settings");
    inSettingsTree->SetBranchStatus("E_NHITSCUT", 0);
    inSettingsTree->SetBranchStatus("E_TIMECUT", 0);
    inSettingsTree->SetBranchStatus("N_OUTCUT", 0);

    TTree* outSettingsTree = inSettingsTree->CloneTree(-1, "fast");
    TBranch* b_E_NHITSCUT = outSettingsTree->Branch("E_NHITSCUT", &E_NHITSCUT);
    TBranch* b_E_TIMECUT = outSettingsTree->Branch("E_TIMECUT", &E_TIMECUT);
    TBranch* b_N_OUTCUT = outSettingsTree->Branch("N_OUTCUT", &N_OUTCUT);

    b_E_NHITSCUT->Fill();
    b_E_TIMECUT->Fill();
    b_N_OUTCUT->Fill();
    outSettingsTree->Write();
    //f->Delete(inSettingsTree);
    
    f->Close();
}

int NTagTMVATagger::Classify(const Candidate& candidate)
{
    int tagClass = 0;
    bool isEarly = candidate.Get("N50", -1) < 0; // muechk flag
    int nHits = candidate.Get("NHits");
    float fitT = candidate.Get("FitT");
    float tmvaOut = isEarly ? /* muechk */ 1 : 
                              /* ntag */   GetLikelihood(candidate);

    // simple cuts mode for e/n separation
    if (fDoTagE) {
        if (tmvaOut < N_OUTCUT)                          tagClass = typeMissed;
        else if (nHits > E_NHITSCUT && fitT < E_TIMECUT) tagClass = typeE;
        else                                             tagClass = typeN;
    }
    // naive tagging mode without e/n separation
    else {
        if (isEarly)                 tagClass = typeE;      // e: muechk
        else if (tmvaOut > N_OUTCUT) tagClass = typeN;      // n: ntag && out cut
        else                         tagClass = typeMissed; // otherwise noise
    }

    return tagClass;
}