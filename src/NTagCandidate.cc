#include <algorithm>
#include <cassert>
#include <numeric>

#include <geotnkC.h>

#include "SKLibs.hh"
#include "NTagCalculator.hh"
#include "NTagCandidate.hh"
#include "NTagEventInfo.hh"

NTagCandidate::NTagCandidate(int id, NTagEventInfo* eventInfo)
:fVerbosity(eventInfo->fVerbosity), currentEvent(eventInfo)
{
    candidateID = id;
    msg = NTagMessage("Candidate", fVerbosity);
    TWIDTH = currentEvent->TWIDTH;
}

NTagCandidate::~NTagCandidate() {}

void NTagCandidate::SetHitInfo(const std::vector<float>& rawT,
                               const std::vector<float>& resT,
                               const std::vector<float>& pmtQ,
                               const std::vector<int>& cabI,
                               const std::vector<int>& sigF)
{
    vHitRawTimes = rawT;
    vHitResTimes = resT;
    vHitChargePE = pmtQ;
    vHitCableIDs = cabI;
    vHitSigFlags = sigF;
}

void NTagCandidate::SetVariables()
{
    iVarMap["NHits"] = vHitResTimes.size();
    iVarMap["N200"] = GetNhitsFromCenterTime(currentEvent->vSortedT_ToF, vHitResTimes[0]+TWIDTH/2., 200.);
    fVarMap["TRMS"] = GetTRMS(vHitResTimes);
    fVarMap["QSum"] = std::accumulate(vHitChargePE.begin(), vHitChargePE.end(), 0.);
    fVarMap["ReconCT"] = (vHitResTimes.back() + vHitResTimes[0]) / 2.;
    fVarMap["TSpread"] = (vHitResTimes.back() - vHitResTimes[0]);

    float pv[3] = {currentEvent->pvx, currentEvent->pvy, currentEvent->pvz};
    auto beta = GetBetaArray(vHitCableIDs, pv);
    fVarMap["Beta1"] = beta[1];
    fVarMap["Beta2"] = beta[2];
    fVarMap["Beta3"] = beta[3];
    fVarMap["Beta4"] = beta[4];
    fVarMap["Beta5"] = beta[5];

    fVarMap["DWall"] = wallsk_(pv);
    fVarMap["DWallMeanDir"] = GetDWallInMeanDirection(vHitCableIDs, pv);
    fVarMap["ThetaMeanDir"] = GetMeanAngleInMeanDirection(vHitCableIDs, pv);

    const auto& openingAngleStats = GetOpeningAngleStats(vHitCableIDs, pv);
    fVarMap["AngleMean"]   = openingAngleStats[0];
    fVarMap["AngleMedian"] = openingAngleStats[1];
    fVarMap["AngleStdev"]  = openingAngleStats[2];
    fVarMap["AngleSkew"]   = openingAngleStats[3];

    if (currentEvent->bUseNeutFit) {
        if (currentEvent->bUseResidual)
            SetVariablesForMode(tNEUTFIT);
        else
            SetVariablesForMode(tNEUTFIT_RAW);
    }

    SetVariablesForMode(tBONSAI);
    if (currentEvent->bUseNeutFit)
        fVarMap["bonsai_nfit"] = Norm(fVarMap["bsvx"] - fVarMap["nvx"],
                                      fVarMap["bsvy"] - fVarMap["nvy"],
                                      fVarMap["bsvz"] - fVarMap["nvz"]);

    if (!currentEvent->bData)  SetTrueInfo();

    if (currentEvent->bUseTMVA) {
        SetNNVariables();
        SetTMVAOutput();
    }
}

void NTagCandidate::SetVariablesForMode(ExtractionMode tWindow)
{
    float leftEdge = 0;
    float rightEdge = 0;

    if (tWindow == tNEUTFIT) {
        leftEdge = -tNEUTFIT*0.5; rightEdge = +tNEUTFIT*0.5;
    }
    else if (tWindow == tNEUTFIT_RAW) {
        leftEdge = -tNEUTFIT_RAW*0.25; rightEdge = +tNEUTFIT_RAW*0.75;
    }
    else if (tWindow == tBONSAI) {
        leftEdge = -tBONSAI*0.4; rightEdge = +tBONSAI*0.6;
    }
    else {
        msg.Print("In function NTagCandidate::SetVariablesForMode", pWARNING);
        msg.Print(Form("Input time window %d is not compatible.", tWindow), pERROR);
    }

    std::vector<int>    index, cabiz;
    std::vector<float>  tiskz, qiskz;

    // Save hit indices within time window from reconstructed capture time
    for (int iHit = 0; iHit < currentEvent->nqiskz; iHit++) {
        if (currentEvent->vUnsortedT_ToF[iHit] > fVarMap["ReconCT"] + leftEdge
        &&  currentEvent->vUnsortedT_ToF[iHit] < fVarMap["ReconCT"] + rightEdge) {
            index.push_back(iHit);
        }
    }

    for (unsigned int iHit = 0; iHit < index.size(); iHit++) {
        cabiz.push_back( currentEvent->vCABIZ[ index[iHit] ] );
        tiskz.push_back( currentEvent->vTISKZ[ index[iHit] ] );
        qiskz.push_back( currentEvent->vQISKZ[ index[iHit] ] );
    }

    // 50 ns window
    if (tWindow == tNEUTFIT) {
        iVarMap["N50"] = tiskz.size();

        float nv[3];
        fVarMap["MinTRMS50_n"] = MinimizeTRMS(tiskz, cabiz, nv);
        fVarMap["nvx"] = nv[0]; fVarMap["nvy"] = nv[1]; fVarMap["nvz"] = nv[2];

        auto beta_n = GetBetaArray(vHitCableIDs, nv);
        fVarMap["Beta1_n"] = beta_n[1];
        fVarMap["Beta2_n"] = beta_n[2];
        fVarMap["Beta3_n"] = beta_n[3];
        fVarMap["Beta4_n"] = beta_n[4];
        fVarMap["Beta5_n"] = beta_n[5];

        fVarMap["DWall_n"] = wallsk_(nv);
        fVarMap["DWallMeanDir_n"] = GetDWallInMeanDirection(vHitCableIDs, nv);

        const auto& openingAngleStats = GetOpeningAngleStats(vHitCableIDs, nv);
        fVarMap["AngleMean_n"]   = openingAngleStats[0];
        fVarMap["AngleMedian_n"] = openingAngleStats[1];
        fVarMap["AngleStdev_n"]  = openingAngleStats[2];
        fVarMap["AngleSkew_n"]   = openingAngleStats[3];

        auto tiskz50_ToF = currentEvent->GetToFSubtracted(tiskz, cabiz, nv, true);

        int NHitsn_iHit, tmpBestNHitsn = 0;

        // Search for a new best NHits (NHitsn) from these new ToF corrected hits
        int bestIndex = 0;
        for (int iHit = 0; iHit < iVarMap["N50"]; iHit++) {
            NHitsn_iHit = GetNhitsFromStartIndex(tiskz50_ToF, iHit, TWIDTH);
            if (NHitsn_iHit > tmpBestNHitsn) {
                tmpBestNHitsn = NHitsn_iHit; bestIndex = iHit;
                iVarMap["NHits_n"] = tmpBestNHitsn;
                fVarMap["ReconCT_n"] = (tiskz50_ToF[iHit] + tiskz50_ToF[iHit+tmpBestNHitsn-1]) / 2.;
            }
        }
        fVarMap["TRMS_n"] = GetTRMSFromStartIndex(tiskz50_ToF, bestIndex, TWIDTH);

        fVarMap["prompt_nfit"] = Norm(currentEvent->pvx - fVarMap["nvx"],
                                      currentEvent->pvy - fVarMap["nvy"],
                                      currentEvent->pvz - fVarMap["nvz"]);
    }

    // 200 ns window
    else if (tWindow == tNEUTFIT_RAW) {
        iVarMap["N200Raw"] = tiskz.size();

        float nv[3];
        fVarMap["MinTRMS30_n"] = MinimizeTRMS(vHitRawTimes, vHitCableIDs, nv);
        fVarMap["nvx"] = nv[0]; fVarMap["nvy"] = nv[1]; fVarMap["nvz"] = nv[2];

        auto beta_100 = GetBetaArray(vHitCableIDs, nv);
        fVarMap["Beta1_n"] = beta_100[1];
        fVarMap["Beta2_n"] = beta_100[2];
        fVarMap["Beta3_n"] = beta_100[3];
        fVarMap["Beta4_n"] = beta_100[4];
        fVarMap["Beta5_n"] = beta_100[5];

        fVarMap["DWall_n"] = wallsk_(nv);
        fVarMap["DWallMeanDir_n"] = GetDWallInMeanDirection(vHitCableIDs, nv);

        const auto& openingAngleStats = GetOpeningAngleStats(vHitCableIDs, nv);
        fVarMap["AngleMean_n"]   = openingAngleStats[0];
        fVarMap["AngleMedian_n"] = openingAngleStats[1];
        fVarMap["AngleStdev_n"]  = openingAngleStats[2];
        fVarMap["AngleSkew_n"]   = openingAngleStats[3];

        auto tiskz200_ToF = currentEvent->GetToFSubtracted(tiskz, cabiz, nv, true);

        int NHitsn_iHit, tmpBestNHitsn = 0;

        // Search for a new best NHits (NHitsn) from these new ToF corrected hits
        int bestIndex = 0;
        for (int iHit = 0; iHit < iVarMap["N200Raw"]; iHit++) {
            NHitsn_iHit = GetNhitsFromStartIndex(tiskz200_ToF, iHit, TWIDTH);
            if (NHitsn_iHit > tmpBestNHitsn) {
                tmpBestNHitsn = NHitsn_iHit; bestIndex = iHit;
                iVarMap["NHits_n"] = tmpBestNHitsn;
                fVarMap["ReconCT_n"] = (tiskz200_ToF[iHit] + tiskz200_ToF[iHit+tmpBestNHitsn-1]) / 2.;
            }
        }
        fVarMap["TRMS_n"] = GetTRMSFromStartIndex(tiskz200_ToF, bestIndex, TWIDTH);
    }

    // 1300 ns window
    else if (tWindow == tBONSAI) {
        if (currentEvent->nProcessedEvents == 0 && candidateID == 0)
            msg.PrintBlock("Initializing BONSAI lfallfit...", pSUBEVENT);

        iVarMap["N1300"] = tiskz.size();
        int isData = 0; if (currentEvent->bData) isData = 1;

        bonsai_fit_(&isData, &fVarMap["ReconCT"], tiskz.data(), qiskz.data(), cabiz.data(), &iVarMap["N1300"],
                    &fVarMap["BSenergy"], &fVarMap["bsvx"], &fVarMap["bsvy"], &fVarMap["bsvz"],
                    &fVarMap["BSReconCT"], &fVarMap["BSgood"], &fVarMap["BSdirks"], &fVarMap["BSpatlik"], &fVarMap["BSovaq"]);

        // Fix bsPatlik->-inf bug
        if (fVarMap["BSpatlik"] < -9999.) fVarMap["BSpatlik"] = -9999.;

        fVarMap["prompt_bonsai"] = Norm(currentEvent->pvx - fVarMap["bsvx"],
                                        currentEvent->pvy - fVarMap["bsvy"],
                                        currentEvent->pvz - fVarMap["bsvz"]);
    }
}

void NTagCandidate::SetTrueInfo()
{
    // Default: not a capture
    iVarMap["CaptureType"] = 0;
    iVarMap["TrueCaptureID"] = -1;

    // If signal flag vector is not provided by the user,
    // label candidates as signal if a true capture with matching capture time exists
    if (vHitSigFlags.empty()) {
        for (int iCapture = 0; iCapture < currentEvent->nTrueCaptures; iCapture++) {
            if (fabs(currentEvent->vTrueCT[iCapture] + currentEvent->trgOffset - fVarMap["ReconCT"])
                < currentEvent->TMATCHWINDOW) {
                iVarMap["TrueCaptureID"] = iCapture;
                if (currentEvent->vTotGammaE[iCapture] > 6.) iVarMap["CaptureType"] = 2; // Gd
                else                                         iVarMap["CaptureType"] = 1; // H
            }
        }
    }

    // If signal flag vector is given, use it to label candidates
    else {
        fVarMap["SignalRatio"] = GetMean(vHitSigFlags);

        // label as noise if signal ratio is lower than threshold
        if (fVarMap["SignalRatio"] < currentEvent->SIGRATIOTHR)
            iVarMap["CaptureType"] = 0;

        // label as signal if signal ratio is higher than threshold
        else {
            float minCTDiff = 1e38;
            int iMatchingCapture = 0;

            // look for closest true capture and label 1: H or 2: Gd,
            // according to the emitted total gamma energy of the closest true capture
            for (int iCapture = 0; iCapture < currentEvent->nTrueCaptures; iCapture++) {
                float ctDiff = fabs(currentEvent->vTrueCT[iCapture] + currentEvent->trgOffset - fVarMap["ReconCT"]);
                if (ctDiff < minCTDiff) {
                    minCTDiff = ctDiff;
                    iMatchingCapture = iCapture;
                }
            }
            iVarMap["TrueCaptureID"] = iMatchingCapture;
            if (currentEvent->vTotGammaE[iMatchingCapture] > 6.) iVarMap["CaptureType"] = 2; // Gd
            else                                                 iVarMap["CaptureType"] = 1; // H
        }
    }
}

void NTagCandidate::SetNNVariables()
{
    // Set TMVA integer variable map
    for (auto const& pair: iVarMap) {
        if (currentEvent->TMVATools.fVariables.IsTMVAVariable(pair.first)) {
            currentEvent->TMVATools.fVariables.PushBack(pair.first, pair.second);
        }
    }

    // Set TMVA float variable map
    for (auto const& pair: fVarMap) {
        if (currentEvent->TMVATools.fVariables.IsTMVAVariable(pair.first)) {
            currentEvent->TMVATools.fVariables.PushBack(pair.first, pair.second);
        }
    }

    if (currentEvent->bUseTMVA) {
        currentEvent->TMVATools.fVariables.SetCaptureType(iVarMap["CaptureType"]);
    }
}

void NTagCandidate::SetTMVAOutput()
{
    fVarMap["TMVAOutput"] = currentEvent->TMVATools.GetOutputFromCandidate(candidateID);
}

void NTagCandidate::DumpHitInfo()
{
    msg.Print("---------------------------------------------------------------------------");
    msg.Print("RawHitT [ns]   ResHitT [ns]   Q [p.e.]       PMT ID         IsSignalHit?   ");
    msg.Print("---------------------------------------------------------------------------");

    for (unsigned int iHit = 0; iHit < vHitRawTimes.size(); iHit++) {
        msg.Print("", pDEFAULT, false);
        std::cout << std::left << std::setw(15) << vHitRawTimes[iHit];
        std::cout << std::left << std::setw(15) << vHitResTimes[iHit];
        std::cout << std::left << std::setw(15) << vHitChargePE[iHit];
        std::cout << std::left << std::setw(15) << vHitCableIDs[iHit];
        if (!vHitSigFlags.empty()) std::cout << std::left << std::setw(15) << vHitSigFlags[iHit];
        std::cout << std::endl;
    }
}

void NTagCandidate::DumpVariables()
{
    msg.PrintBlock(Form("CandidateID: %d", candidateID), pCANDIDATE, pDEFAULT, false);

    DumpHitInfo();

    msg.Print("----------------------------------");
    msg.Print("Variable       : Value            ");
    msg.Print("----------------------------------");

    for (auto const& pair: iVarMap) {
        msg.Print("", pDEFAULT, false);
        std::cout << std::left << std::setw(15) << pair.first << ": " << pair.second << std::endl;
    }
    for (auto const& pair: fVarMap) {
        msg.Print("", pDEFAULT, false);
        std::cout << std::left << std::setw(15) << pair.first << ": " << pair.second << std::endl;
    }

    std::cout << "\n" << std::endl;
}

std::array<float, 6> NTagCandidate::GetBetaArray(const std::vector<int>& PMTID, float v[3])
{
    std::array<float, 6> beta = {0., 0., 0., 0., 0., 0};
    int nHits = PMTID.size();
    if (nHits == 0) return beta;

    // direction vector from vertex to each hit PMT
    float uvx[nHits], uvy[nHits], uvz[nHits];

    for (int iHit = 0; iHit < nHits; iHit++) {
        float distFromVertexToPMT;
        float vecFromVertexToPMT[3];
        for (int dim = 0; dim < 3; dim++)
            vecFromVertexToPMT[dim] = NTagConstant::PMTXYZ[PMTID[iHit]-1][dim] - v[dim];
        distFromVertexToPMT = Norm(vecFromVertexToPMT);
        uvx[iHit] = vecFromVertexToPMT[0] / distFromVertexToPMT;
        uvy[iHit] = vecFromVertexToPMT[1] / distFromVertexToPMT;
        uvz[iHit] = vecFromVertexToPMT[2] / distFromVertexToPMT;
    }

    for (int i = 0; i < nHits-1; i++) {
        for (int j = i+1; j < nHits; j++) {
            // cosine angle between two consecutive uv vectors
            float cosTheta = uvx[i]*uvx[j] + uvy[i]*uvy[j] + uvz[i]*uvz[j];
            for (int k = 1; k <= 5; k++)
                beta[k] += GetLegendreP(k, cosTheta);
        }
    }

    for (int k = 1; k <= 5; k++)
        beta[k] = 2.*beta[k] / float(nHits) / float(nHits-1);

    // Return calculated beta array
    return beta;
}

float NTagCandidate::MinimizeTRMS(const std::vector<float>& T, const std::vector<int>& PMTID, float rmsFitVertex[])
{
    float maxSearchRange = currentEvent->VTXSRCRANGE;
    float gridWidth;
    bool doSort = true;
    int nHits = static_cast<int>(T.size());
    assert(nHits == static_cast<int>(PMTID.size()));

    (maxSearchRange > 200) ? gridWidth = 500 : gridWidth = maxSearchRange / 2.;
    std::vector<float>  t_ToF;

    int nGridsInR, nGridsInZ;
    nGridsInZ = (int)(2*ZPINTK / gridWidth);
    nGridsInR = (int)(2*RINTK / gridWidth);

    // Grid search starts from tank center
    std::array<float, 3> gridOrigin = {0., 0., 0.};       // grid origin in the grid search loop
    std::array<float, 3> minGridPoint = {0., 0., 0.};     // temp array to save TRMS-minimizing grid point
    std::array<float, 3> gridPoint;                       // point in grid to find TRMS

    float minTRMS = 9999.;
    float tRMS;

    // Repeat until grid width gets small enough
    while (gridWidth > currentEvent->MINGRIDWIDTH) {

        // Allocate coordinates to a grid point, X and Y
        for (int iGridX = 0; iGridX < nGridsInR; iGridX++) {
            gridPoint[0] = gridOrigin[0] + (iGridX - nGridsInR/2.) * gridWidth ;

            for (int iGridY = 0; iGridY < nGridsInR; iGridY++) {
                gridPoint[1] = gridOrigin[1] + (iGridY - nGridsInR/2.) * gridWidth;

                // Skip grid point with R outside of tank
                if (sqrt(gridPoint[0]*gridPoint[0] + gridPoint[1]*gridPoint[1]) > RINTK) continue;

                // Allocate coordinates to a grid point, Z
                for (int iGridZ = 0; iGridZ < nGridsInZ; iGridZ++) {
                    gridPoint[2] = gridOrigin[2] + (iGridZ - nGridsInZ/2.) * gridWidth;

                    // Skip grid point with Z outside of tank
                    if (gridPoint[2] > ZPINTK || gridPoint[2] < -ZPINTK) continue;

                    // Skip grid point further away from the maximum search range
                    if (GetDistance(gridOrigin.data(), gridPoint.data()) > maxSearchRange) continue;

                    // Subtract ToF from the search vertex
                    t_ToF = currentEvent->GetToFSubtracted(T, PMTID, gridPoint.data(), doSort);
                    // Get TRMS from the residual hit times
                    tRMS = GetTRMS(t_ToF);

                    // Save TRMS minimizing grid point
                    if (tRMS < minTRMS) {
                        minTRMS = tRMS;
                        minGridPoint = gridPoint;
                    }
                }
            }
        }

        // Change grid origin to the TRMS-minimizing grid point,
        // shorten the grid width,
        // and repeat until grid width gets small enough!
        gridOrigin = minGridPoint;
        gridWidth = gridWidth / 2.;
    }

    // Output fit vertex = final grid origin
    rmsFitVertex[0] = gridOrigin[0];
    rmsFitVertex[1] = gridOrigin[1];
    rmsFitVertex[2] = gridOrigin[2];

    return minTRMS;
}