#include "skheadC.h"
#include "geotnkC.h"

#include "SKLibs.hh"

#include "Calculator.hh"
#include "ExtractFeatures.hh"

bool ExtractFeatures::Initialize()
{
    // read ntag options
    if (!sharedData->ntagInfo.Get("TWIDTH", tWidth))
        Log("TWIDTH not specified!", pERROR);

    // read trms-fit options
    if (!sharedData->ntagInfo.Get("INITGRIDWIDTH", initGridWidth)) initGridWidth = 800;
    if (!sharedData->ntagInfo.Get("MINGRIDWIDTH", minGridWidth)) minGridWidth = 50;
    if (!sharedData->ntagInfo.Get("GRIDSHRINKRATE", gridShrinkRate)) gridShrinkRate = 0.5;
    if (!sharedData->ntagInfo.Get("VTXSRCRANGE", vertexSearchRange)) vertexSearchRange = 5000;

    // read MC true capture match options
    if (Tool::inputIsMC) {
        if (!sharedData->ntagInfo.Get("TMATCHWINDOW", tMatchWindow)) tMatchWindow = 50;
    }
    
    // define key list to branch
    std::vector<std::string>keyList = {"NHits", "N50", "N200", "N1300", "ReconCT", "TRMS", "QSum", 
                                       "Beta1", "Beta2", "Beta3", "Beta4", "Beta5", 
                                       "AngleMean", "AngleSkew", "AngleStdev", "CaptureType",
                                       "DWall", "DWallMeanDir", "ThetaMeanDir", "DWall_n", "prompt_nfit", "decay_e_like"};
    
    // register keys to the feature map
    sharedData->eventCandidates.RegisterFeatureNames(keyList);

    return true;
}

bool ExtractFeatures::CheckSafety()
{
    // EventPMTHits must be filled
    if (sharedData->eventPMTHits.IsEmpty()) {
        Log("EventPMTHits class is empty. Skipping...", pWARNING);
        return false;
    }
    else if (sharedData->eventCandidates.IsEmpty()) {
        Log("No candidates found in the event! Skipping...", pWARNING);
        return false;
    }
    else
        return true;
}

bool ExtractFeatures::Execute()
{
    TVector3 promptVertex(0,0,0);
    float dWall = 0.0;
    sharedData->eventVariables.Get("prompt_vertex", promptVertex);
    sharedData->eventVariables.Get("d_wall", dWall);

    EventPMTHits* eventHits = &(sharedData->eventPMTHits);
    EventCandidates* eventCans = &(sharedData->eventCandidates);
    EventTrueCaptures* eventCaps = &(sharedData->eventTrueCaptures);
    EventParticles* eventSecs = &(sharedData->eventSecondaries);

    unsigned int nCandidates = eventCans->GetSize();

    if (!eventHits->HasVertex())
        eventHits->SetVertex(promptVertex);

    // loop over candidates
    for (unsigned int i = 0; i < nCandidates; i++) {
        Candidate* candidate = &(eventCans->At(i));
        int firstHitID = candidate->HitID();

        PMTHitCluster hitsInTWIDTH = eventHits->Slice(firstHitID, tWidth);
        PMTHitCluster hitsIn50ns   = eventHits->Slice(firstHitID, tWidth/2.- 50, tWidth/2.+ 50);
        PMTHitCluster hitsIn200ns  = eventHits->Slice(firstHitID, tWidth/2.-100, tWidth/2.+100);
        PMTHitCluster hitsIn1300ns = eventHits->Slice(firstHitID, tWidth/2.-520, tWidth/2.+780);

        // Number of hits
        candidate->Set("NHits", hitsInTWIDTH.GetSize());
        candidate->Set("N50",   hitsIn50ns.GetSize());
        candidate->Set("N200",  hitsIn200ns.GetSize());
        candidate->Set("N1300", hitsIn1300ns.GetSize());

        // Time
        float reconCT = hitsInTWIDTH.Find(HitFunc::T, Calc::Mean) * 1e-3;
        candidate->Set("ReconCT", reconCT);
        candidate->Set("TRMS", hitsInTWIDTH.Find(HitFunc::T, Calc::RMS));

        // Charge
        candidate->Set("QSum", hitsInTWIDTH.Find(HitFunc::Q, Calc::Sum));

        // Beta's
        auto beta = hitsInTWIDTH.GetBetaArray();
        candidate->Set("Beta1", beta[1]);
        candidate->Set("Beta2", beta[2]);
        candidate->Set("Beta3", beta[3]);
        candidate->Set("Beta4", beta[4]);
        candidate->Set("Beta5", beta[5]);

        // DWall
        auto dirVec = hitsInTWIDTH[HitFunc::Dir];
        auto meanDir = GetMean(dirVec).Unit();
        candidate->Set("DWall", dWall);
        candidate->Set("DWallMeanDir", GetDWallInDirection(promptVertex, meanDir));

        // Mean angle formed by all hits and the mean hit direction
        std::vector<float> angles;
        for (auto const& dir: dirVec) {
            angles.push_back((180/M_PI)*meanDir.Angle(dir));
        }
        float meanAngleWithMeanDirection = GetMean(angles);
        candidate->Set("ThetaMeanDir", meanAngleWithMeanDirection);

        // Opening angle stats
        auto openingAngleStats = hitsInTWIDTH.GetOpeningAngleStats();
        candidate->Set("AngleMean",  openingAngleStats.mean);
        candidate->Set("AngleStdev", openingAngleStats.stdev);
        candidate->Set("AngleSkew",  openingAngleStats.skewness);

        // TRMS-fit
        auto trmsFitVertex = hitsInTWIDTH.FindTRMSMinimizingVertex(/* TRMS-fit options */
                                                                   initGridWidth, minGridWidth, gridShrinkRate, vertexSearchRange);
        candidate->Set("DWall_n", GetDWall(trmsFitVertex));
        candidate->Set("prompt_nfit", (promptVertex-trmsFitVertex).Mag());

        int passDecayECut = 0;
        if ((candidate->Get("N50") > 50) && reconCT < 20) {
            passDecayECut = 1;
        }
        candidate->Set("decay_e_like", passDecayECut);

        // BONSAI

        // MC info
        if (Tool::inputIsMC) {
            Log("MC!");
            // default: not a capture
            int captureType = 0;
            int nTrueCaptures = eventCaps->GetSize();
            int nSecondaries = eventSecs->GetSize();

            // label candidates as signal if a true capture with matching capture time exists
            for (int iCapture = 0; iCapture < nTrueCaptures; iCapture++) {
                auto& capture = eventCaps->At(iCapture);
                if (fabs(capture.Time() - reconCT*1e3) < tMatchWindow) {
                    if (capture.Energy() > 6.) captureType = 2; // Gd
                    else                        captureType = 1; // H
                }
            }
            
            // search for a matching decay electron and overwrite label
            for (int iSec = 0; iSec < nSecondaries; iSec++) {
                auto& secondary = eventSecs->At(iSec);

                // decay electron from a parent muon
                if (fabs(secondary.PID()) == 11 &&
                    fabs(secondary.ParentPID()) == 13 &&
                    secondary.IntID() == 5) {
                    // check if ReconCT matches the decay within TMATCHWINDOW
                    if (fabs(secondary.Time() - reconCT*1e3) < tMatchWindow) {
                        captureType = 3; // decay electron
                    }
                }
            }
            candidate->Set("CaptureType", captureType);
        }
    }

    return true;
}

bool ExtractFeatures::Finalize()
{
    return true;
}