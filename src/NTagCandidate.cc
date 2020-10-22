#include <algorithm>
#include <cassert>

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
    iVarMap["N10"] = vHitResTimes.size();
    iVarMap["N200"] = GetNhitsFromCenterTime(currentEvent->vSortedT_ToF, vHitResTimes[0]+5., 200.);
    fVarMap["TRMS10"] = GetTRMS(vHitResTimes);
    fVarMap["QSum10"] = std::accumulate(vHitChargePE.begin(), vHitChargePE.end(), 0.);
    fVarMap["ReconCT"] = (vHitResTimes.back() + vHitResTimes[0]) / 2.;
    fVarMap["TSpread10"] = (vHitResTimes.back() - vHitResTimes[0]);

    beta_10 = GetBetaArray(vHitCableIDs);
    fVarMap["Beta1"] = beta_10[1];
    fVarMap["Beta2"] = beta_10[2];
    fVarMap["Beta3"] = beta_10[3];
    fVarMap["Beta4"] = beta_10[4];
    fVarMap["Beta5"] = beta_10[5];
    
    SetVariablesWithinTWindow(50);
    SetVariablesWithinTWindow(1300);
    fVarMap["bonsai_nfit"] = Norm(fVarMap["bsvx"] - fVarMap["nvx"],
                                  fVarMap["bsvy"] - fVarMap["nvy"],
                                  fVarMap["bsvz"] - fVarMap["nvz"]);

    if (!currentEvent->bData)  SetTrueInfo();
    if (currentEvent->bUseTMVA) {
        SetNNVariables();
        SetTMVAOutput();
    }
}

void NTagCandidate::SetVariablesWithinTWindow(int tWindow)
{
    float leftEdge = 0;
    float rightEdge = 0;

    if (tWindow == 50) {
        leftEdge = -25; rightEdge = +25;
    }
    else if (tWindow == 1300) {
        leftEdge = -520.8; rightEdge = +779.2;
    }
    else {
        msg.Print("In function NTagCandidate::SetVariablesWithinTWindow", pWARNING);
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
    if (tWindow == 50) {
        iVarMap["N50"] = tiskz.size();
        beta_50 = GetBetaArray(cabiz);

        float nv[3];
        fVarMap["TRMS50"] = MinimizeTRMS(tiskz, cabiz, nv);
        fVarMap["nvx"] = nv[0]; fVarMap["nvy"] = nv[1]; fVarMap["nvz"] = nv[2];

        fVarMap["DWalln"] = wallsk_(nv);
        fVarMap["DWallnMeanDir"] = GetDWallInMeanDirection(vHitCableIDs, nv);

        const auto& openingAngleStats = GetOpeningAngleStats(vHitCableIDs, nv);
        fVarMap["AngleMean"]   = openingAngleStats[0];
        fVarMap["AngleMedian"] = openingAngleStats[1];
        fVarMap["AngleStdev"]  = openingAngleStats[2];
        fVarMap["AngleSkew"]   = openingAngleStats[3];

        auto tiskz50_ToF = currentEvent->GetToFSubtracted(tiskz, cabiz, nv, true);

        int N10n_iHit, tmpBestN10n = 0;

        // Search for a new best N10 (N10n) from these new ToF corrected hits
        int bestIndex = 0;
        for (int iHit = 0; iHit < iVarMap["N50"]; iHit++) {
            N10n_iHit = GetNhitsFromStartIndex(tiskz50_ToF, iHit, 10.);
            if (N10n_iHit > tmpBestN10n) {
                tmpBestN10n = N10n_iHit; bestIndex = iHit;
                iVarMap["N10n"] = tmpBestN10n;
                fVarMap["ReconCTn"] = (tiskz50_ToF[iHit] + tiskz50_ToF[iHit+tmpBestN10n-1]) / 2.;
            }
        }
        fVarMap["TRMS10n"] = GetTRMSFromStartIndex(tiskz50_ToF, bestIndex, 10.);

        fVarMap["prompt_nfit"] = Norm(currentEvent->pvx - fVarMap["nvx"],
                                      currentEvent->pvy - fVarMap["nvy"],
                                      currentEvent->pvz - fVarMap["nvz"]);
    }

    // 1300 ns window
    else if (tWindow == 1300) {
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

    // Search for matching capture time within true capture vector of current event
    for (int iCapture = 0; iCapture < currentEvent->nTrueCaptures; iCapture++) {
        if (fabs(currentEvent->vTrueCT[iCapture] + currentEvent->trgOffset - fVarMap["ReconCT"])
            < currentEvent->TMATCHWINDOW ) {
            iVarMap["TrueCaptureID"] = iCapture;
            if (currentEvent->vTotGammaE[iCapture] > 6.) iVarMap["CaptureType"] = 2; // Gd
            else                                         iVarMap["CaptureType"] = 1; // H
        }
    }
}

void NTagCandidate::SetNNVariables()
{
    // Copy integer map to float map
    for (auto const& pair: iVarMap) {
        fVarMap[pair.first] = pair.second;
    }

    // Set TMVA float variable map
    for (auto const& pair: fVarMap) {
        if (currentEvent->TMVATools.fVariables.IsTMVAVariable(pair.first)) {
            currentEvent->TMVATools.fVariables.Set(pair.first, pair.second);
        }
    }
    
    if (currentEvent->bUseTMVA) {
        currentEvent->TMVATools.fVariables.SetCaptureType(iVarMap["CaptureType"]);
    }

    // Push back to TMVA float variable vector map
    currentEvent->TMVATools.fVariables.FillVectorMap();
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
    msg.Print("Variable       : value            ");
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

std::array<float, 6> NTagCandidate::GetBetaArray(const std::vector<int>& PMTID)
{
    std::array<float, 6> beta = {0., 0., 0., 0., 0., 0};
    int nHits = PMTID.size();
    if (nHits == 0) return beta;

    // direction vector from vertex to each hit PMT
    float uvx[nHits], uvy[nHits], uvz[nHits];

    for (int iHit = 0; iHit < nHits; iHit++) {
        float distFromVertexToPMT;
        float vecFromVertexToPMT[3];
        vecFromVertexToPMT[0] = NTagConstant::PMTXYZ[PMTID[iHit]-1][0] - currentEvent->pvx;
        vecFromVertexToPMT[1] = NTagConstant::PMTXYZ[PMTID[iHit]-1][1] - currentEvent->pvy;
        vecFromVertexToPMT[2] = NTagConstant::PMTXYZ[PMTID[iHit]-1][2] - currentEvent->pvz;
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
    float vertexSearchRange = currentEvent->VTXSRCRANGE;
    float delta;
    bool doSort = true;
    int nHits = static_cast<int>(T.size());
    assert(nHits == static_cast<int>(PMTID.size()));

    (vertexSearchRange > 200) ? delta = 100 : delta = vertexSearchRange / 2.;
    std::vector<float>  t_ToF;
    // std::vector<float>* minTPointer;

    int rMax, zMax;
    zMax = (int)(2*ZPINTK / (float)delta);
    rMax = (int)(2*RINTK / (float)delta);

    // main search position starts from tank center
    std::array<float, 3> vecR = {0., 0., 0.};
    std::array<float, 3> tmpVertex = {0., 0., 0.}; // temp vertex to save minimizing vertex
    std::array<float, 3> srcVertex;                // loop search vertex

    float minTRMS = 9999.;
    float tRMS;

    while (delta > 0.5) {

        for (float x = 0; x < rMax; x++) {
            srcVertex[0] = delta * (x - rMax/2.) + vecR[0];
            for (float y = 0; y < rMax; y++) {
                srcVertex[1] = delta * (y - rMax/2.) + vecR[1];

                if (sqrt(srcVertex[0]*srcVertex[0] + srcVertex[1]*srcVertex[1]) > RINTK) continue;
                for (float z = 0; z < zMax; z++) {
                    srcVertex[2] = delta * (z - zMax/2.) + vecR[2];

                    if (srcVertex[2] > ZPINTK || srcVertex[2] < -ZPINTK) continue;
                    if (Norm(srcVertex[0] - vecR[0], srcVertex[1] - vecR[1], srcVertex[2] - vecR[2])
                        > vertexSearchRange) continue;

                    t_ToF = currentEvent->GetToFSubtracted(T, PMTID, srcVertex.data(), doSort);

                    tRMS = GetTRMS(t_ToF);

                    if (tRMS < minTRMS) {
                        minTRMS = tRMS;
                        tmpVertex = srcVertex;
                        //minTPointer = &t_ToF;
                    }
                }
            }
        }
        vecR = tmpVertex;
        delta = delta / 2.;
    }

    //int index[nHits];
    //TMath::Sort(nHits, minTPointer->data(), index, false);

    rmsFitVertex[0] = vecR[0];
    rmsFitVertex[1] = vecR[1];
    rmsFitVertex[2] = vecR[2];

    return minTRMS;
}