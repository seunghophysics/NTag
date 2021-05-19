#include "TMVA/Reader.h"

#include "PathGetter.hh"

#include "ApplyTMVA.hh"

bool ApplyTMVA::Initialize()
{
    featureContainer["NHits"] = 0;
    featureContainer["N200"] = 0;
    featureContainer["AngleMean"] = 0.;
    featureContainer["AngleSkew"] = 0.;
    featureContainer["AngleStdev"] = 0.;
    featureContainer["Beta1"] = 0.;
    featureContainer["Beta2"] = 0.;
    featureContainer["Beta3"] = 0.;
    featureContainer["Beta4"] = 0.;
    featureContainer["Beta5"] = 0.;
    featureContainer["DWall"] = 0.;
    featureContainer["DWall_n"] = 0.;
    featureContainer["DWallMeanDir"] = 0.;
    featureContainer["prompt_nfit"] = 0.;
    featureContainer["ThetaMeanDir"] = 0.;
    featureContainer["TRMS"] = 0.;

    weightFilePath = GetENV("NTAGPATH") + std::string("weights/MLP_Gd0.011p_calibration.xml");
    sharedData->ntagInfo.Get("mva_method_name", mvaMethodName);
    sharedData->ntagInfo.Get("weight_file_path", weightFilePath);

    tmvaReader = new TMVA::Reader();

    for (auto& pair: featureContainer)
        tmvaReader->AddVariable(pair.first, &(pair.second));

    tmvaReader->AddSpectator("CaptureType", &(captureType));

    tmvaReader->BookMVA(mvaMethodName, weightFilePath);

    return true;
}

bool ApplyTMVA::CheckSafety()
{
    safeToExecute = true;
    return safeToExecute;
}

bool ApplyTMVA::Execute()
{
    EventCandidates* eventCans = &(sharedData->eventCandidates);
    unsigned int nCandidates = eventCans->GetSize();

    // candidate loop
    for (unsigned int i = 0; i < nCandidates; i++) {
        Candidate* candidate = &(eventCans->At(i));
        float tmvaOutput = GetClassifierOutput(candidate);
        candidate->Set("TMVAOutput", tmvaOutput);
    }

    Log("Candidates information:");
    eventCans->Print();

    return true;
}

bool ApplyTMVA::Finalize()
{
    delete tmvaReader;
    return true;
}

float ApplyTMVA::GetClassifierOutput(Candidate* candidate)
{
    // get features from candidate and fill feature container
    for (auto const& pair: featureContainer) {
        float value = candidate->Get(pair.first);
        featureContainer[pair.first] = value;
    }

    // get spectator
    captureType = candidate->Get("CaptureType");

    return tmvaReader->EvaluateMVA(mvaMethodName);
}