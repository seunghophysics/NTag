#include <math.h>
#include <cassert>
#include <iostream>
#include <numeric>

#include <TMath.h>

// Size limit of secondary tree/bank
#define MAXNSCNDPRT (4000)

#include <skroot.h>
#undef MAXHWSK
#include <apmringC.h>
#include <apmueC.h>
#include <appatspC.h>
#include <geotnkC.h>
#include <skheadC.h>
#include <sktqC.h>
#include <skvectC.h>
#include <neworkC.h>
#include <apscndryC.h>
#include <loweroot.h>

#include "NTagPath.hh"
#include "NTagCalculator.hh"
#include "NTagEventInfo.hh"
#include "SKLibs.hh"

NTagEventInfo::NTagEventInfo(Verbosity verbose)
: N10TH(NTagDefault::N10TH), N10MX(NTagDefault::N10MX), N200MX(NTagDefault::N200MX),
T0TH(NTagDefault::T0TH), T0MX(NTagDefault::T0MX), TRBNWIDTH(NTagDefault::TRBNWIDTH),
TMATCHWINDOW(NTagDefault::TMATCHWINDOW), TMINPEAKSEP(NTagDefault::TMINPEAKSEP), ODHITMX(NTagDefault::ODHITMX),
VTXSRCRANGE(NTagDefault::VTXSRCRANGE), customvx(0.), customvy(0.), customvz(0.),
fVerbosity(verbose), bData(false), bUseTMVA(true), bSaveTQ(false)
{
    nProcessedEvents = 0;
    preRawTrigTime[0] = -1;
    candidateVariablesInitialized = false;

    msg = NTagMessage("", fVerbosity);

    TMVATools = NTagTMVA(verbose);
    TMVATools.SetReader("MLP", (GetENV("NTAGPATH")+"weights/MLP_Gd0.02p.xml").c_str());
}

NTagEventInfo::~NTagEventInfo()
{
    if (fSigTQFile) fSigTQFile->Close();
}

void NTagEventInfo::SetEventHeader()
{
    runNo    = skhead_.nrunsk;
    subrunNo = skhead_.nsubsk;
    eventNo  = skhead_.nevsk;

    // Mimic QISMSK: sum all Q of ID hits within 1.3 usec gate
    qismsk = 0.;
    for (int iHit = 0; iHit < sktqz_.nqiskz; iHit++) {
        float hitTime = sktqz_.tiskz[iHit];
        if (479.2 < hitTime && hitTime < 1779.2) {
            qismsk += sktqz_.qiskz[iHit];
        }
    }

    msg.Print(Form("qismsk: %f", qismsk), pDEBUG);

    // Number of OD hits
    odpc_2nd_s_(&nhitac);
    msg.Print(Form("Number of OD hits: %d", nhitac), pDEBUG);

    // Read trigger offset
    trginfo_(&trgOffset);
    msg.Print(Form("Trigger offset: %f", trgOffset), pDEBUG);
}

void NTagEventInfo::SetPromptVertex()
{
    switch (fVertexMode) {
        case mAPFIT: {
            // Get apcommul bank
            int bank = 0;
            aprstbnk_(&bank);
            pvx = apcommul_.appos[0];
            pvy = apcommul_.appos[1];
            pvz = apcommul_.appos[2]; break; }
        case mBONSAI: {
            int lun = 10;
            TreeManager* mgr  = skroot_get_mgr(&lun);
            LoweInfo*    LOWE = mgr->GetLOWE();
            mgr->GetEntry();
            pvx = LOWE->bsvertex[0];
            pvy = LOWE->bsvertex[1];
            pvz = LOWE->bsvertex[2]; break; }
        case mCUSTOM: {
            pvx = customvx;
            pvy = customvy;
            pvz = customvz; break; }
        case mTRUE: {
            skgetv_();
            pvx = skvect_.pos[0];
            pvy = skvect_.pos[1];
            pvz = skvect_.pos[2]; break; }
        case mSTMU: {
            /* STMU */ break; }
    }

    float tmp_v[3] = {pvx, pvy, pvz};
    dWall = wallsk_(tmp_v);

    msg.Print(Form("Prompt vertex: %f, %f, %f", pvx, pvy, pvz), pDEBUG);
    msg.Print(Form("d_wall: %f", dWall), pDEBUG);
}

void NTagEventInfo::SetAPFitInfo()
{
    // E_vis
    evis = apcomene_.apevis;
    msg.Print(Form("e_vis: %f", evis), pDEBUG);

    // AP ring information
    apNRings = apcommul_.apnring;
    for (int iRing = 0; iRing < apNRings; iRing++) {
        vAPRingPID.push_back( apcommul_.apip[iRing]         );  // PID
        vAPMom.    push_back( apcommul_.apamom[iRing]       );  // Reconstructed momentum
        vAPMomE.   push_back( appatsp2_.apmsamom[iRing][1]  );  // e-like momentum
        vAPMomMu.  push_back( appatsp2_.apmsamom[iRing][2]  );  // mu-like momentum
    }
    msg.Print(Form("APFit number of rings: %d", apNRings), pDEBUG);

    // mu-e check
    apNMuE = apmue_.apnmue; apNDecays = 0;
    for (int iMuE = 0; iMuE < apNMuE; iMuE++) {
        if (iMuE == 10) break;
        if (apmue_.apmuetype[iMuE] == 1 || apmue_.apmuetype[iMuE] == 4) apNDecays++;
    }
}

void NTagEventInfo::SetLowFitInfo()
{
    int lun = 10;

    TreeManager* mgr  = skroot_get_mgr(&lun);
    LoweInfo*    LOWE = mgr->GetLOWE();
    mgr->GetEntry();

    // E_vis
    evis = LOWE->bsenergy;
    msg.Print(Form("e_vis: %f", evis), pDEBUG);
}

void NTagEventInfo::AppendRawHitInfo()
{
    if (fSigTQTree) {
        fSigTQTree->GetEntry(nProcessedEvents);
    }

    float tOffset = 0.;
    float tLast   = 0.;
    float qLast   = 0.;
    int   pmtLast = 0.;

    bool  coincidenceFound = true;

    if (!vTISKZ.empty()) {
        coincidenceFound = false;
        tLast   = vTISKZ.back();
        qLast   = vQISKZ.back();
        pmtLast = vCABIZ.back();
    }

    int nFoundSigHits = 0;
    int nRemovedHits = 0;

    for (int iHit = 0; iHit < sktqz_.nqiskz; iHit++) {

        if (!coincidenceFound && sktqz_.qiskz[iHit] == qLast && sktqz_.icabiz[iHit] == pmtLast) {
            tOffset = tLast - sktqz_.tiskz[iHit];
            coincidenceFound = true;
            msg.Print(Form("Coincidence found: t = %f ns, (offset: %f ns)", tLast, tOffset), pDEBUG);
        }

        int hitPMTID = sktqz_.icabiz[iHit];
        float hitTime = sktqz_.tiskz[iHit] + tOffset;

        // Use hits that are in-gate and within MAXPM only
        if (sktqz_.ihtiflz[iHit] & (1<<1) && hitPMTID <= MAXPM) {

            if (fabs(hitTime - vPMTHitTime[hitPMTID]) < TRBNWIDTH*1.e3) {
                nRemovedHits++;
                continue;
            }

            vTISKZ.push_back( hitTime );
            vQISKZ.push_back( sktqz_.qiskz[iHit] );
            vCABIZ.push_back( hitPMTID           );
            vPMTHitTime[hitPMTID] = hitTime;

            if (vSIGT) {
                bool isSignal = false;
                // Look for matching hits between sig+bkg TQ and sig TQ
                for (unsigned int iSigHit = 0; iSigHit < vSIGT->size(); iSigHit++) {
                    // If both hit time and PMT ID match, then the current hit iHit is from signal
                    if (fabs(hitTime - vSIGT->at(iSigHit)) < 1e-3
                        && sktqz_.icabiz[iHit] == vSIGI->at(iSigHit)) {
                        isSignal = true;
                    }
                }
                if (isSignal) { vISIGZ.push_back(1); nFoundSigHits++; }
                else            vISIGZ.push_back(0);
            }
        }
    }

    msg.Print(Form("Removed hits: %d", nRemovedHits), pDEBUG);
    if (vSIGT) msg.Print(Form("%d / %lu signal hits saved!", nFoundSigHits, vSIGT->size()), pDEBUG);

    nqiskz = static_cast<int>(vTISKZ.size());
    msg.Print(Form("nqiskz after append: %d", nqiskz), pDEBUG);
}

void NTagEventInfo::SetToFSubtractedTQ()
{
    // Subtract ToF from raw PMT hit time
    float fitVertex[3] = {pvx, pvy, pvz};
    vUnsortedT_ToF = GetToFSubtracted(vTISKZ, vCABIZ, fitVertex, false);

    SortToFSubtractedTQ();
}

void NTagEventInfo::SetMCInfo()
{
    // Read SKVECT (primaries)
    skgetv_();
    nVec = skvect_.nvect;   // number of primaries
    vecx = skvect_.pos[0];  // initial vertex of primaries
    vecy = skvect_.pos[1];
    vecz = skvect_.pos[2];

    msg.Print(Form("True Vector: %d", nVec), pDEBUG);

    for (int iVec = 0; iVec < nVec; iVec++) {
        vVecPID.push_back( skvect_.ip[iVec]     );  // PID of primaries
        vVecPX. push_back( skvect_.pin[iVec][0] );  // momentum vector of primaries
        vVecPY. push_back( skvect_.pin[iVec][1] );
        vVecPZ. push_back( skvect_.pin[iVec][2] );
        vVecMom.push_back( skvect_.pabs[iVec]   );  // momentum of primaries

        if (vVecPID[iVec] == 13) // Neutron code in Geant3 is 13
        msg.Print(Form("Primary neutron %d: [p = %f MeV/c] [dwall: %f cm]",
                            iVec, vVecMom[iVec], dWall), pDEBUG);
    }

    // Read neutrino interaction vector
    float posnu[3];
    nerdnebk_(posnu);

    neutIntMode = nework_.modene;       // neutrino interaction mode
    nVecInNeut  = nework_.numne;        // number of particles in vector
    nNInNeutVec = 0;                    // number of neutrons
    neutIntMom  = Norm(nework_.pne[0]);

    for (int i = 0; i < nVecInNeut; i++) {
        vNeutVecPID.push_back(nework_.ipne[i]); // PIDs in vector
        if (vNeutVecPID[i] == 2112 && i >= 3) nNInNeutVec++; // count neutrons
    }
    msg.Print(Form("Number of neutrons in NEUT primary stack: %d", nNInNeutVec), pDEBUG);

    // Initialize number of n captures
    nTrueCaptures = 0;
    nSavedSec = 0;

    // ?
    float ZBLST = 5.30;
    float dr    = RINTK - ZBLST;
    float dz    = 0.5 * HIINTK - ZBLST;

    // Read secondary bank
    ReadSecondaries();

    int nSecNeutron = 0;
    nAllSec = secndprt_.nscndprt;

    // Loop over all secondaries in secondary common block
    for (int iSec = 0; iSec < nAllSec; iSec++) {

        float secMom = Norm(secndprt_.pscnd[iSec]);

        // Save all neutrons
        if (secndprt_.iprtscnd[iSec] == 2112) {
            SaveSecondary(iSec);
            nSecNeutron++;
            msg.Print(Form("Secondary neutron (#%d): [t = %f ns] [p = %f MeV/c]",
                         nSecNeutron, secndprt_.tscnd[iSec]*1e-3, Norm(secndprt_.pscnd[iSec])), pDEBUG);
        }

        // deuteron, gamma, electrons
        else if (secndprt_.iprtscnd[iSec] == 100045 ||
                 secndprt_.iprtscnd[iSec] == 22 ||
                 // electrons over Cherenkov threshold momentum, from interaction other than multiple scattering
                 (fabs(secndprt_.iprtscnd[iSec]) == 11 && secMom > 0.579 && secndprt_.lmecscnd[iSec] != 2)) {

            /* Save capture info from below */
            float vtxR2 = secndprt_.vtxscnd[iSec][0] * secndprt_.vtxscnd[iSec][0]
                        + secndprt_.vtxscnd[iSec][1] * secndprt_.vtxscnd[iSec][1];
            int inPMT;
            inpmt_(secndprt_.vtxscnd[iSec], inPMT);

            // Check if the capture is within ID volume
            if (vtxR2 < dr*dr && fabs(secndprt_.vtxscnd[iSec][2]) < dz && !inPMT) {

                // Save secondary (deuteron, gamma, electrons)
                SaveSecondary(iSec);
                bool isNewCapture = true;

                // particle produced by n-capture
                if (secndprt_.lmecscnd[iSec] == 18) {

                    // Check saved capture stack
                    for (int iCheckedCT = 0; iCheckedCT < nTrueCaptures; iCheckedCT++) {
                        // If this capture is already saved:
                        if (fabs((double)(secndprt_.tscnd[iSec] - vTrueCT[iCheckedCT])) < 1.e-7) {
                            isNewCapture = false;
                            // Add capture product gammas to the pre-existing stack
                            if (secndprt_.iprtscnd[iSec] == 22) {
                                vNGamma[iCheckedCT]++;
                                vTotGammaE[iCheckedCT] +=  Norm( secndprt_.pscnd[iSec] );
                                vCapID[nSavedSec-1] = iCheckedCT;
                            }
                        }
                    }
                    if (isNewCapture) {
                        vTrueCT.push_back( secndprt_.tscnd[iSec]      );
                        vCapVX.push_back    ( secndprt_.vtxscnd[iSec][0] );
                        vCapVY.push_back    ( secndprt_.vtxscnd[iSec][1] );
                        vCapVZ.push_back    ( secndprt_.vtxscnd[iSec][2] );
                        // Add capture product gamma
                        if (secndprt_.iprtscnd[iSec] == 22) {
                            vNGamma.push_back(1);
                            vTotGammaE.push_back( Norm( secndprt_.pscnd[iSec] ) );
                            vCapID[nSavedSec-1] = nTrueCaptures;
                        }
                        else { vNGamma.push_back(0); vTotGammaE.push_back(0.); }
                        // increment total number of captures
                        nTrueCaptures++;
                    }
                }
            }
        }
    }
    assert(nSavedSec == static_cast<int>(vSecPID.size()));
    assert(nSavedSec == static_cast<int>(vCapID.size()));

    for (int iCapture = 0; iCapture < nTrueCaptures; iCapture++) {
        msg.Print(Form("CaptureID %d: [t: %f us] [Gamma E: %f MeV] [x: %f y: %f z: %f]",
                        iCapture, vTrueCT[iCapture]*1e-3, vTotGammaE[iCapture],
                        vCapVX[iCapture], vCapVY[iCapture], vCapVZ[iCapture]), pDEBUG);
        msg.Print(Form("Neutron travel distance from prompt vertex: %f cm",
                        Norm(pvx-vCapVX[iCapture], pvy-vCapVY[iCapture], pvz-vCapVZ[iCapture])), pDEBUG);
    }
    msg.Print(Form("Number of secondary neutrons saved in bank: %d", nSecNeutron), pDEBUG);
    msg.Print(Form("Number of captures: %d", nTrueCaptures), pDEBUG);
}

void NTagEventInfo::ReadSecondaries()
{
    apflscndprt_();
}

void NTagEventInfo::SearchCaptureCandidates()
{
    int   iHitPrevious    = 0;
    int   N10New          = 0;
    int   N10Previous     = 0;
    int   N200Previous    = 0;
    float t0Previous      = -1.;

    // Loop over the saved TQ hit array from current event
    for (int iHit = 0; iHit < nqiskz; iHit++) {

        // the Hit timing w/o TOF is larger than limit, or less smaller than t0
        if (vSortedT_ToF[iHit]*1.e-3 < T0TH) continue;

        // Save time of first hit
        if (firstHitTime_ToF == 0.) firstHitTime_ToF = vSortedT_ToF[iHit];

        // Calculate N10New:
        // number of hits in 10 ns window from the i-th hit
        int N10_iHit = GetNhitsFromStartIndex(vSortedT_ToF, iHit, 10.);

        // If N10TH <= N10_iHit <= N10MX:
        if ((N10_iHit < N10TH) || (N10_iHit >= N10MX+1)) continue;

        // We've found a new peak.
        N10New = N10_iHit;
        float t0New = vSortedT_ToF[iHit];

        // Save maximum N200 and its t0
        float N200New = GetNhitsFromCenterTime(vSortedT_ToF, t0New + 5., 200.);
        if (t0New*1.e-3 > T0TH && N200New > maxN200) {
            maxN200 = N200New;
            maxN200Time = t0New;
        }

        // If peak t0 diff = t0New - t0Previous > TMINPEAKSEP, save the previous peak.
        // Also check if N200Previous is below N200 cut and if t0Previous is over t0 threshold
        if (t0New - t0Previous > TMINPEAKSEP) {
            if (N200Previous < N200MX && t0Previous*1.e-3 > T0TH) {
                SavePeakFromHit(iHitPrevious);
            }
            // Reset N10Previous,
            // if peaks are separated enough
            N10Previous = 0;
        }

        // If N10 is not greater than previous, skip
        if ( N10New <= N10Previous ) continue;

        iHitPrevious = iHit;
        t0Previous   = t0New;
        N10Previous  = N10New;
        N200Previous = N200New;
    }
    // Save the last peak
    if (N10Previous >= N10TH)
        SavePeakFromHit(iHitPrevious);
}

void NTagEventInfo::SavePeakFromHit(int hitID)
{
    // Initialize candidate
    vCandidates.push_back(NTagCandidate(vCandidates.size(), this));

    // Containers for hit info
    float tWidth = 10.;
    std::vector<float> resTVec = GetVectorFromStartIndex(vSortedT_ToF, hitID, tWidth);
    int n10 = resTVec.size();

    std::vector<float> rawTVec = SliceVector(vTISKZ, hitID, n10, reverseIndex.data());
    std::vector<float> pmtQVec = SliceVector(vSortedQ, hitID, n10);
    std::vector<int>   cabIVec = SliceVector(vSortedPMTID, hitID, n10);
    std::vector<int>   sigFVec = SliceVector(vSortedSigFlag, hitID, n10);

    // Save hit info to candidate
    vCandidates.back().SetHitInfo(rawTVec, resTVec, pmtQVec, cabIVec, sigFVec);
    //vCandidates.back().DumpHitInfo();
    vCandidates.back().SetVariables();

    vHitRawTimes->push_back(rawTVec);
    vHitResTimes->push_back(resTVec);
    vHitCableIDs->push_back(cabIVec);
    vHitSigFlags->push_back(sigFVec);

    // Increment number of neutron candidates
    nCandidates++;
}

void NTagEventInfo::SetCandidateVariables()
{
    if (vCandidates.size() > 0) {
        if (!candidateVariablesInitialized)
            InitializeCandidateVariableVectors();

        ExtractCandidateVariables();
        DumpCandidateVariables();
    }
}


void NTagEventInfo::InitializeCandidateVariableVectors()
{
    for (auto const& pair: vCandidates[0].iVarMap) {
        msg.Print(Form("Initializing variable %s...", pair.first), pDEBUG);
        iCandidateVarMap[pair.first] = new std::vector<int>();
    }
    for (auto const& pair: vCandidates[0].fVarMap) {
        msg.Print(Form("Initializing variable %s...", pair.first), pDEBUG);
        fCandidateVarMap[pair.first] = new std::vector<float>();
    }
    candidateVariablesInitialized = true;
}

void NTagEventInfo::ExtractCandidateVariables()
{
    for (auto candidate: vCandidates) {
        for (auto const& pair: candidate.iVarMap) {
            iCandidateVarMap[pair.first]->push_back(pair.second);
        }
        for (auto const& pair: candidate.fVarMap) {
            fCandidateVarMap[pair.first]->push_back(pair.second);
        }
    }
}

void NTagEventInfo::DumpCandidateVariables()
{
    for (auto candidate: vCandidates) {
        candidate.DumpVariables();
    }

    std::cout << std::endl;
}

float NTagEventInfo::GetToF(float vertex[3], int pmtID)
{
    float vecFromVertexToPMT[3];

    for (int i = 0; i < 3; i++)
        vecFromVertexToPMT[i] = vertex[i] - NTagConstant::PMTXYZ[pmtID][i];

    return GetDistance(NTagConstant::PMTXYZ[pmtID], vertex) / NTagConstant::C_WATER;
}

std::vector<float> NTagEventInfo::GetToFSubtracted(const std::vector<float>& T, const std::vector<int>& PMTID, float vertex[3], bool doSort)
{
    std::vector<float> t_ToF;
    std::vector<float> doSortT_ToF;

    int nHits = static_cast<int>(T.size());
    assert(nHits == static_cast<int>(PMTID.size()));

    // Subtract TOF from PMT hit time
    for (int iHit = 0; iHit < nHits; iHit++) {
        int pmtID = PMTID[iHit] - 1;
        t_ToF.push_back( T[iHit] - GetToF(vertex, pmtID) );
    }

    if (doSort) {
        int sortedIndex[nHits];
        TMath::Sort(nHits, t_ToF.data(), sortedIndex, false);
        for (int iHit = 0; iHit < nHits; iHit++)
           doSortT_ToF.push_back( t_ToF[ sortedIndex[iHit] ] );
        return doSortT_ToF;
    }
    else return t_ToF;
}

void NTagEventInfo::SortToFSubtractedTQ()
{
    int sortedIndex[nqiskz], pmtID;
    reverseIndex.clear(); reverseIndex.resize(nqiskz);

    // Sort: early hit first
    TMath::Sort(nqiskz, vUnsortedT_ToF.data(), sortedIndex, false);

    // Save hit info, sorted in (T - ToF)
    for (int iHit = 0; iHit < nqiskz; iHit++) {
        vSortedPMTID.push_back  ( vCABIZ[ sortedIndex[iHit] ]         );
        vSortedT_ToF.push_back  ( vUnsortedT_ToF[ sortedIndex[iHit] ] );
        vSortedQ.push_back      ( vQISKZ[ sortedIndex[iHit] ]         );
        reverseIndex[sortedIndex[iHit]] = iHit;
    }

    if (!vISIGZ.empty()) {
        for (int iHit = 0; iHit < nqiskz; iHit++) {
            vSortedSigFlag.push_back( vISIGZ[ sortedIndex[iHit] ]         );
        }
    }
}

void NTagEventInfo::Clear()
{
    runNo = 0; subrunNo = 0; eventNo = 0; nhitac = 0; nqiskz = 0; trgType = 0;
    trgOffset = 1000; qismsk = 0; tDiff = 0;
    apNRings = 0; apNMuE = 0; apNDecays = 0;
    evis = 0; pvx = 0; pvy = 0; pvz = 0; dWall = 0;
    nCandidates = 0; maxN200 = 0;
    maxN200Time = -9999.; firstHitTime_ToF = -9999.;

    nTrueCaptures = 0;
    nSavedSec = 0; nAllSec = 0;
    nNInNeutVec = neutIntMode = nVecInNeut = 0;
    neutIntMom = 0;
    nVec = 0;
    vecx = 0; vecy = 0; vecz = 0;

    vTISKZ.clear(); vQISKZ.clear(); vCABIZ.clear(); vISIGZ.clear(); vPMTHitTime.fill(0);

    vSortedPMTID.clear();
    vSortedT_ToF.clear(); vUnsortedT_ToF.clear(); vSortedQ.clear(); vSortedSigFlag.clear();

    vAPRingPID.clear(); vAPMom.clear(); vAPMomE.clear(); vAPMomMu.clear();
    vFirstHitID.clear();
    TMVATools.fVariables.Clear();

    vHitRawTimes->clear();
    vHitResTimes->clear();
    vHitCableIDs->clear();
    vHitSigFlags->clear();

    vNGamma.clear(); vCandidateID.clear();
    vTrueCT.clear(); vCapVX.clear(); vCapVY.clear(); vCapVZ.clear(); vTotGammaE.clear();

    vSecPID.clear(); vSecIntID.clear(); vParentPID.clear(); vCapID.clear();
    vSecVX.clear(); vSecVY.clear(); vSecVZ.clear(); vSecPX.clear(); vSecPY.clear(); vSecPZ.clear();
    vSecDWall.clear(); vSecMom.clear(); vSecT.clear();

    vNeutVecPID.clear();
    vVecPID.clear();
    vVecPX.clear(); vVecPY.clear(); vVecPZ.clear(); vVecMom.clear();

    vCandidates.clear();

    for (auto pair: iCandidateVarMap) {
        pair.second->clear();
    }
    for (auto pair: fCandidateVarMap) {
        pair.second->clear();
    }
}

void NTagEventInfo::SaveSecondary(int secID)
{
    vSecPID.   push_back( secndprt_.iprtscnd[secID]         );  // PID of secondaries
    vSecIntID. push_back( secndprt_.lmecscnd[secID]         );  // creation process
    vParentPID.push_back( secndprt_.iprntprt[secID]         );  // parent PID
    vSecVX.    push_back( secndprt_.vtxscnd[secID][0]       );  // creation vertex
    vSecVY.    push_back( secndprt_.vtxscnd[secID][1]       );
    vSecVZ.    push_back( secndprt_.vtxscnd[secID][2]       );
    vSecDWall. push_back( wallsk_(secndprt_.vtxscnd[secID]) );  // distance from wall to creation vertex
    vSecPX.    push_back( secndprt_.pscnd[secID][0]         );  // momentum vector
    vSecPY.    push_back( secndprt_.pscnd[secID][1]         );
    vSecPZ.    push_back( secndprt_.pscnd[secID][2]         );
    vSecMom.   push_back( Norm(secndprt_.pscnd[secID])      );  // momentum
    vSecT.     push_back( secndprt_.tscnd[secID]            );  // time created
    vCapID.    push_back( -1 );
    nSavedSec++;

    msg.Print(Form("Saved secondary %d: [PID: %d] [Int code: %d] [Parent PID: %d] [x: %f y: %f z: %f]",
                    secID,
                    secndprt_.iprtscnd[secID],
                    secndprt_.lmecscnd[secID],
                    secndprt_.iprntprt[secID],
                    secndprt_.vtxscnd[secID][0],
                    secndprt_.vtxscnd[secID][1],
                    secndprt_.vtxscnd[secID][2]), pDEBUG);
}