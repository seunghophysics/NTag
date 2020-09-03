#include <math.h>
#include <cassert>
#include <iostream>
#include <numeric>

#include <TMath.h>

// Size limit of secondary tree/bank
#define SECMAXRNG (4000)

#include <skroot.h>
#undef MAXHWSK
#include <apmringC.h>
#include <apmueC.h>
#include <appatspC.h>
#include <geotnkC.h>
#include <skheadC.h>
#include <sktqC.h>
#include <geopmtC.h>
#include <skvectC.h>
#include <neworkC.h>
#include <apscndryC.h>
#include <loweroot.h>

#include "NTagEventInfo.hh"
#include "SKLibs.hh"

NTagEventInfo::NTagEventInfo(unsigned int verbose)
:PMTXYZ(geopmt_.xyzpm), C_WATER(21.5833),
N10TH(7), N10MX(50), N200MX(140), VTXSRCRANGE(4000.),
T0TH(2.), T0MX(600.), TMATCHWINDOW(40.), TMINPEAKSEP(50.), ODHITMX(16.),
customvx(0.), customvy(0.), customvz(0.),
fVerbosity(verbose), bData(false), bCustomVertex(false)
{
    msg = NTagMessage("", fVerbosity);

    SetN10Limits(7, 50);
    SetN200Max(140);
    SetT0Limits(2., 600.);   // [us]
    SetDistanceCut(4000.);   // [cm]
    SetTMatchWindow(40.);    // [ns]
    SetTPeakSeparation(50.); // [us]
    SetMaxODHitThreshold(16);

    TMVATools = NTagTMVA(verbose);
    TMVATools.SetReader("MLP", "weights/MLP_Gd0.02p.xml");
    TMVATools.SetReaderCutRange("N10", N10TH, N10MX);
    TMVATools.SetReaderCutRange("dt", T0TH*1.e3, T0MX*1.e3);
    TMVATools.DumpReaderCutRange();
}

NTagEventInfo::~NTagEventInfo() {}

void NTagEventInfo::SetEventHeader()
{
    nrun = skhead_.nrunsk;
    nsub = skhead_.nsubsk;
    nev  = skhead_.nevsk;

    qismsk = skq_.qismsk;
}

void NTagEventInfo::SetAPFitInfo()
{
    // Get apcommul bank
    int bank = 0;
    aprstbnk_(&bank);

    // Get APFit vertex
    if (bCustomVertex) {
        pvx = customvx;
        pvy = customvy;
        pvz = customvz;
    }
    else
    {
        pvx = apcommul_.appos[0];
        pvy = apcommul_.appos[1];
        pvz = apcommul_.appos[2];
    }

    float tmp_v[3] = {pvx, pvy, pvz};
    towall = wallsk_(tmp_v);

    msg.Print(Form("APFit vertex: %f, %f, %f", pvx, pvy, pvz), pDEBUG);
    msg.Print(Form("d_wall: %f", towall), pDEBUG);

    // E_vis
    evis = apcomene_.apevis;
    msg.Print(Form("e_vis: %f", evis), pDEBUG);

    // AP ring information
    nring = apcommul_.apnring;
    for (int iRing = 0; iRing < nring; iRing++) {
        vApip.push_back  ( apcommul_.apip[iRing]        );  // PID
        vApamom.push_back( apcommul_.apamom[iRing]      );  // Reconstructed momentum
        vApmome.push_back( appatsp2_.apmsamom[iRing][1] );  // e-like momentum
        vApmomm.push_back( appatsp2_.apmsamom[iRing][2] );  // mu-like momentum
    }
    msg.Print(Form("APFit number of rings: %d", nring), pDEBUG);

    // mu-e check
    nmue = apmue_.apnmue; ndcy = 0;
    for (int iMuE = 0; iMuE < nmue; iMuE++) {
        if (iMuE == 10) break;
        if (apmue_.apmuetype[iMuE] == 1 || apmue_.apmuetype[iMuE] == 4) ndcy++;
    }
}

void NTagEventInfo::SetLowFitInfo()
{
    int lun = 10;

    TreeManager* mgr  = skroot_get_mgr(&lun);
    LoweInfo*    LOWE = mgr->GetLOWE();
    mgr->GetEntry();

    // Get LowFit vertex
    if (bCustomVertex) {
        pvx = customvx;
        pvy = customvy;
        pvz = customvz;
    }
    else
    {
        pvx = LOWE->bsvertex[0];
        pvy = LOWE->bsvertex[1];
        pvz = LOWE->bsvertex[2];
    }

    float tmp_v[3] = {pvx, pvy, pvz};
    towall = wallsk_(tmp_v);

    msg.Print(Form("LowFit vertex: %f, %f, %f", pvx, pvy, pvz), pDEBUG);
    msg.Print(Form("d_wall: %f", towall), pDEBUG);

    // E_vis
    evis = LOWE->bsenergy;
    msg.Print(Form("e_vis: %f", evis), pDEBUG);
}

void NTagEventInfo::AppendRawHitInfo()
{
    // Append number of OD hits
    int tmpNhitac;
    odpc_2nd_s_(&tmpNhitac);
    nhitac += tmpNhitac;
    msg.Print(Form("Number of OD hits after append: %d", nhitac), pDEBUG);

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

    for (int iHit = 0; iHit < sktqz_.nqiskz; iHit++) {

        if (!coincidenceFound && sktqz_.qiskz[iHit] == qLast && sktqz_.icabiz[iHit] == pmtLast) {
            tOffset = tLast - sktqz_.tiskz[iHit];
            coincidenceFound = true;
            msg.Print(Form("Coincidence found: t = %f ns, (offset: %f ns)", tLast, tOffset), pDEBUG);
        }

        if (sktqz_.ihtiflz[iHit] & (1<<1)) {
            vTISKZ.push_back( sktqz_.tiskz[iHit] + tOffset );
            vQISKZ.push_back( sktqz_.qiskz[iHit]           );
            vCABIZ.push_back( sktqz_.icabiz[iHit]          );
        }
    }

    nqiskz = static_cast<int>(vTISKZ.size());
    msg.Print(Form("nqiskz after append: %d", nqiskz), pDEBUG);
}

void NTagEventInfo::SetToFSubtractedTQ()
{
    // Subtract ToF from raw PMT hit time
    float fitVertex[3] = {pvx, pvy, pvz};
    vUnsortedT_ToF = GetToFSubtracted(vTISKZ, vCABIZ, fitVertex, false);

    SortToFSubtractedTQ();
    msg.Print(Form("NQISKZ: %d", nqiskz), pDEBUG);

    // set qismsk as sum of all appended in-gate hits
    // qismsk = accumulate(vQISKZ.begin(), vQISKZ.end(), 0);
}

void NTagEventInfo::SetMCInfo()
{
    // Read trigger offset
    trginfo_(&trgofst);
    msg.Print(Form("Trigger offset: %f", trgofst), pDEBUG);

    // Read SKVECT (primaries)
    skgetv_();
    nvect  = skvect_.nvect;    // number of primaries
    truevx = skvect_.pos[0];   // initial vertex of primaries
    truevy = skvect_.pos[1];
    truevz = skvect_.pos[2];

    msg.Print(Form("True Vector: %d", nvect), pDEBUG);

    for (int iVec = 0; iVec < nvect; iVec++) {
        vIp.push_back  ( skvect_.ip[iVec]     );  // PID of primaries
        vPinx.push_back( skvect_.pin[iVec][0] );  // momentum vector of primaries
        vPiny.push_back( skvect_.pin[iVec][1] );
        vPinz.push_back( skvect_.pin[iVec][2] );
        vPabs.push_back( skvect_.pabs[iVec]   );  // momentum of primaries
    }

    // Read neutrino interaction vector
    float posnu[3];
    nerdnebk_(posnu);

    modene = nework_.modene;       // neutrino interaction mode
    numne  = nework_.numne;        // number of particles in vector
    nN     = 0;                    // number of neutrons
    pnu    = Norm(nework_.pne[0]);

    for (int i = 0; i < numne; i++) {
        vIpne.push_back(nework_.ipne[i]);     // PIDs in vector
        if (vIpne[i] == 2112 && i >= 3) nN++; // count neutrons
    }
    msg.Print(Form("Number of neutrons in NEUT primary stack: %d", nN), pDEBUG);

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
    nscndprt = secndprt_.nscndprt;

    // Loop over all secondaries in secondary common block
    for (int iSec = 0; iSec < nscndprt; iSec++) {

        // Save all neutrons
        if (secndprt_.iprtscnd[iSec] == 2112) {
            SaveSecondary(iSec);
            nSecNeutron++;
            msg.Print(Form("Secondary neutron (#%d): [t = %f ns] [p = %f MeV/c]",
                         nSecNeutron, secndprt_.tscnd[iSec]*1e-3, Norm(secndprt_.pscnd[iSec])), pDEBUG);
        }

        // deuteron, gamma (no capture electrons?)
        else if (secndprt_.iprtscnd[iSec] == 100045 || secndprt_.iprtscnd[iSec] == 22) {

            /* Save capture info from below */
            float vtxR2 = secndprt_.vtxscnd[iSec][0] * secndprt_.vtxscnd[iSec][0]
                        + secndprt_.vtxscnd[iSec][1] * secndprt_.vtxscnd[iSec][1];
            int inPMT;
            inpmt_(secndprt_.vtxscnd[iSec], inPMT);

            // Check if the capture is within ID volume
            if (vtxR2 < dr*dr && fabs(secndprt_.vtxscnd[iSec][2]) < dz && !inPMT) {

                // Save secondary (deuteron, gamma)
                SaveSecondary(iSec);
                bool isNewCapture = true;

                // particle produced by n-capture
                if (secndprt_.lmecscnd[iSec] == 18) {

                    // Check saved capture stack
                    for (int iCheckedCT = 0; iCheckedCT < nTrueCaptures; iCheckedCT++) {
                        // If this capture is already saved:
                        if (fabs((double)(secndprt_.tscnd[iSec] - vCaptureTime[iCheckedCT])) < 1.e-7) {
                            isNewCapture = false;
                            // Add capture product gammas to the pre-existing stack
                            if (secndprt_.iprtscnd[iSec] == 22) {
                                //msg.Print(Form("Gamma from already saved capture... captureID %d", iCheckedCT), pDEBUG);
                                vNGam[iCheckedCT]++;
                                vTotGamE[iCheckedCT] +=  Norm( secndprt_.pscnd[iSec] );
                                vCaptureID[nSavedSec-1] = iCheckedCT;
                            }
                        }
                    }
                    if (isNewCapture) {
                        vCaptureTime.push_back( secndprt_.tscnd[iSec]      );
                        vCapPosx.push_back    ( secndprt_.vtxscnd[iSec][0] );
                        vCapPosy.push_back    ( secndprt_.vtxscnd[iSec][1] );
                        vCapPosz.push_back    ( secndprt_.vtxscnd[iSec][2] );
                        // Add capture product gamma
                        if (secndprt_.iprtscnd[iSec] == 22) {
                            //msg.Print(Form("Gamma from new capture... vCaptureID %d", nTrueCaptures), pDEBUG);
                            vNGam.push_back(1);
                            vTotGamE.push_back( Norm( secndprt_.pscnd[iSec] ) );
                            vCaptureID[nSavedSec-1] = nTrueCaptures;
                        }
                        else { vNGam.push_back(0); vTotGamE.push_back(0.); }
                        // increment total number of captures
                        nTrueCaptures++;
                    }
                }
            }
        }
    }
    assert(nSavedSec == static_cast<int>(vIprtscnd.size()));
    assert(nSavedSec == static_cast<int>(vCaptureID.size()));

    for (int iCapture = 0; iCapture < nTrueCaptures; iCapture++) {
        msg.Print(Form("CaptureID %d: [t: %f us] [Gamma E: %f MeV]",
                          iCapture, vCaptureTime[iCapture]*1e-3, vTotGamE[iCapture]), pDEBUG);
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
        if (firsthit == 0.) firsthit = vSortedT_ToF[iHit];

        // Calculate N10New:
        // number of hits in 10 ns window from the i-th hit
        int N10i = GetNhitsFromStartIndex(vSortedT_ToF, iHit, 10.);

        // If N10TH <= N10i <= N10MX:
        if ((N10i < N10TH) || (N10i >= N10MX+1)) continue;

        // We've found a new peak.
        N10New = N10i;
        float t0New = vSortedT_ToF[iHit];

        // Save maximum N200 and its t0
        float N200New = GetNhitsFromCenterTime(vSortedT_ToF, t0New + 5., 200.);
        if (t0New > 2.e4 && N200New > N200M) {
            N200M = N200New;
            T200M = t0New;
        }

        // If peak t0 diff = t0New - t0Previous > TMINPEAKSEP, save the previous peak.
        // Also check if N200Previous is below N200 cut and if t0Previous is over t0 threshold
        if (t0New - t0Previous > TMINPEAKSEP) {
            if (N200Previous < N200MX && t0Previous*1.e-3 > T0TH) {
                if (t0Previous < 2000) msg.Print(Form("!!! T0: %f", t0Previous), pDEBUG);
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
    SavePeakFromHit(iHitPrevious);

    // Select hits within 50 ns around each capture candidate
    // to calculate beta and feed BONSAI

    int                 n50hits, n1300hits;
    std::vector<int>    index50, index1300, nindex;
    std::vector<int>    cabiz50, cabiz1300;
    std::vector<float>  tiskz50, qiskz50, tiskz1300, qiskz1300;

    msg.Print("Searching for new N10 via TRMS minimization...", pDEBUG);
    msg.Print(Form("Number of candidates : %d", nCandidates), pDEBUG);

    std::vector<float>* dt = TMVATools.fVariables.GetVector("dt");

    // Loop over all found capture candidates
    for (int iCandidate = 0; iCandidate < nCandidates; iCandidate++) {
        index50.clear(); index1300.clear(); nindex.clear();
        cabiz50.clear(); cabiz1300.clear();
        tiskz50.clear(); qiskz50.clear(); tiskz1300.clear(); qiskz1300.clear();

        n50hits = 0;
        n1300hits = 0;

        // Loop over all hits in event
        for (int iHit = 0; iHit < nqiskz; iHit++) {

            // Count N50 and save hit indices in vSortedT_ToF
            //if (fabs(vUnsortedT_ToF[iHit] - vDt[iCandidate]) < 25.) {
            if (fabs(vUnsortedT_ToF[iHit] - dt->at(iCandidate)) < 25.) {
                  index50.push_back(iHit);
                  n50hits++;
            }

            // 1.3 us window to feed BONSAI
            // Count N1300 and save hit indices in vSortedT_ToF
            if (vUnsortedT_ToF[iHit] > dt->at(iCandidate) - 520.8
            &&  vUnsortedT_ToF[iHit] < dt->at(iCandidate) + 779.2) {
                if (n1300hits < 1000) {
                    index1300.push_back(iHit);
                    n1300hits++;
                }
            }
        }

        for (int iHit50 = 0; iHit50 < n50hits; iHit50++) {
            cabiz50.push_back( vCABIZ[ index50[iHit50] ] );
            tiskz50.push_back( vTISKZ[ index50[iHit50] ]  );
            qiskz50.push_back( vQISKZ[ index50[iHit50] ]  );
        }

        for (int iHit1300 = 0; iHit1300 < n1300hits; iHit1300++) {
            cabiz1300.push_back( vCABIZ[ index1300[iHit1300] ] );
            tiskz1300.push_back( vTISKZ[ index1300[iHit1300] ]  );
            qiskz1300.push_back( vQISKZ[ index1300[iHit1300] ]  );
        }

        // Calculate betas for N50 hits
        auto beta_50 = GetBetaArray(cabiz50, 0, n50hits);

        // Save N50, N1300, and betas for N50 hits
        vN1300.push_back( n1300hits );
        vBeta14_50.push_back( beta_50[1] + 4*beta_50[4] );

        TMVATools.fVariables.PushBack("N50",   n50hits);
        TMVATools.fVariables.PushBack("beta1", beta_50[1]);
        TMVATools.fVariables.PushBack("beta2", beta_50[2]);
        TMVATools.fVariables.PushBack("beta3", beta_50[3]);
        TMVATools.fVariables.PushBack("beta4", beta_50[4]);
        TMVATools.fVariables.PushBack("beta5", beta_50[5]);

        // BONSAI fit to each capture candidate
        float tmptbsenergy, tmptbsvx, tmptbsvy, tmptbsvz, tmptbsvt, tmptbsgood, tmptbsdirks, tmptbspatlik, tmptbsovaq;
        float time0 = dt->at(iCandidate);

        // Maybe high or low hit noise.
        if (n1300hits > 999 || n1300hits < 10) {
            tmptbsenergy = 0.;
            tmptbsvx = 9999.;
            tmptbsvy = 9999.;
            tmptbsvz = 9999.;
            tmptbsvt = 0.;
            tmptbsgood = 0.;
            tmptbsdirks = 1.;
            tmptbspatlik = 0.;
            tmptbsovaq = -1.;
        }
        else {
            bonsai_fit_(&bData, &time0, tiskz1300.data(), qiskz1300.data(), cabiz1300.data(), &n1300hits, &tmptbsenergy,
                        &tmptbsvx, &tmptbsvy, &tmptbsvz, &tmptbsvt, &tmptbsgood, &tmptbsdirks, &tmptbspatlik, &tmptbsovaq);
        }

        // Fix tbspatlik->-inf bug
        if (tmptbspatlik < -9999.) tmptbspatlik = -9999.;

        float tbsvertex[3] = {tmptbsvx, tmptbsvy, tmptbsvz};

        vBvx.push_back( tmptbsvx );
        vBvy.push_back( tmptbsvy );
        vBvz.push_back( tmptbsvz );
        vBvt.push_back( tmptbsvt );

        TMVATools.fVariables.PushBack("tbsenergy", tmptbsenergy);
        TMVATools.fVariables.PushBack("tbswall",   wallsk_(tbsvertex));
        TMVATools.fVariables.PushBack("tbsgood",   tmptbsgood);
        TMVATools.fVariables.PushBack("tbsdirks",  tmptbsdirks);
        TMVATools.fVariables.PushBack("tbspatlik", tmptbspatlik);
        TMVATools.fVariables.PushBack("tbsovaq",   tmptbsovaq);

        float nv[3];	// vertex to fit by minimizing tRMS
        float minTRMS = MinimizeTRMS(tiskz50, cabiz50, nv);

        vNvx.push_back   ( nv[0]       );
        vNvy.push_back   ( nv[1]       );
        vNvz.push_back   ( nv[2]       );

        TMVATools.fVariables.PushBack("nwall", wallsk_(nv));
        TMVATools.fVariables.PushBack("trms40", minTRMS);

        auto tiskz50_ToF = GetToFSubtracted(tiskz50, cabiz50, nv, true);

        int N10in, tmpN10n = 0;
        float t0n = 0.;

        // Search for a new best N10 (N10n) from these new ToF corrected hits
        int n10index = 0;
        for (int iHit50 = 0; iHit50 < n50hits; iHit50++) {
            N10in = GetNhitsFromStartIndex(tiskz50_ToF, iHit50, 10.);
            if (N10in > tmpN10n) {
                tmpN10n = N10in; n10index = iHit50;
                t0n = (tiskz50_ToF[iHit50] + tiskz50_ToF[iHit50+tmpN10n-1]) / 2.;
            }
        }

        vTrms.push_back( GetTRMSFromStartIndex(tiskz50_ToF, n10index, 10.) );
        vN10n.push_back( tmpN10n );
        vDtn.push_back(  t0n );

        float promptBonsai 	= Norm(pvx - vBvx[iCandidate],
                               pvy - vBvy[iCandidate],
                               pvz - vBvz[iCandidate]);
        float promptNfit 	= Norm(pvx - vNvx[iCandidate],
                               pvy - vNvy[iCandidate],
                               pvz - vNvz[iCandidate]);
        float bonsaiNfit    = Norm(vNvx[iCandidate] - vBvx[iCandidate],
                               vNvy[iCandidate] - vBvy[iCandidate],
                               vNvz[iCandidate] - vBvz[iCandidate]);

        TMVATools.fVariables.PushBack("prompt_bonsai", promptBonsai);
        TMVATools.fVariables.PushBack("prompt_nfit", promptNfit);
        TMVATools.fVariables.PushBack("bonsai_nfit", bonsaiNfit);
    }

    if (!bData) {
        msg.Print("Setting true capture info...", pDEBUG);
        SetTrueCaptureInfo();
    }
}


void NTagEventInfo::SetTrueCaptureInfo()
{
    std::vector<float> checkedCT;
    int nCheckedCaptures = 0;

    for (int iCandidate = 0; iCandidate < nCandidates; iCandidate++) {

        // Check if a candidate is a true neutron capture (MC only)
        vIsTrueCapture.push_back( IsTrueCapture(iCandidate, true) );

        // if a candidate is actually a true neutron capture
        if (vIsTrueCapture[iCandidate] == 1) {

            vDoubleCount.push_back(0);
            // time diff between true and reconstructed capture time
            vTimeDiff.push_back( ReconCaptureTime(iCandidate) - TrueCaptureTime(iCandidate) );

            bool newCaptureFound = true;

            if (!checkedCT.empty()) {
                for (const auto& CTinList: checkedCT) {
                    if (fabs(TrueCaptureTime(iCandidate) - CTinList) < 1.e-3) {
                        newCaptureFound = false; break;
                    }
                }
            }

            if (newCaptureFound) {
                checkedCT.push_back( TrueCaptureTime(iCandidate) );
            }

            // Check whether two adjacent candidates should be saved as a single true capture
            else {
                if (fabs(vTimeDiff[iCandidate]) < fabs(vTimeDiff[iCandidate-1])) {
                    vDoubleCount[iCandidate-1] = 1; vIsTrueCapture[iCandidate-1] = 0;
                }
                else {
                    vDoubleCount[iCandidate] = 1; vIsTrueCapture[iCandidate] = 0;
                }
            }

            auto tmpTruthV = TrueCaptureVertex(iCandidate);
            vTruth_vx.push_back( tmpTruthV[0] );
            vTruth_vy.push_back( tmpTruthV[1] );
            vTruth_vz.push_back( tmpTruthV[2] );

            // Check whether capture is on Gd or H
            vIsGdCapture.push_back( IsTrueGdCapture(iCandidate) );
        }
        else {
            vDoubleCount.push_back(0);
            vTimeDiff.push_back   (0);
            vTruth_vx.push_back   (0);
            vTruth_vy.push_back   (0);
            vTruth_vz.push_back   (0);
            vIsGdCapture.push_back(0);
        }
    }
}


void NTagEventInfo::GetTMVAoutput()
{
    std::vector<float>* dt = TMVATools.fVariables.GetVector("dt");
    
    for (int iCandidate = 0; iCandidate < nCandidates; iCandidate++) {

        float tmvaOutput = TMVATools.GetOutputFromCandidate(iCandidate);

        TString trueCaptureInfo;
        if (tmvaOutput == -9999.) trueCaptureInfo = "out-of-cut";
        if (!bData) {
            if (vIsTrueCapture[iCandidate]) trueCaptureInfo = "true";
            else                            trueCaptureInfo = "false";
        }
        
        int  N10 = TMVATools.fVariables.Get<int>("N10", iCandidate);
        float dt = TMVATools.fVariables.Get<float>("dt", iCandidate);
        msg.Print(Form("iCandidate: %d T0: %f [ns] N10: %d TMVAOutput: %f [%s]", iCandidate, dt, N10, tmvaOutput, trueCaptureInfo.Data()), pDEBUG);
        vTMVAoutput.push_back( tmvaOutput );
    }
}

float NTagEventInfo::Norm(float vec[3])
{
    return sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
}

float NTagEventInfo::Norm(float x, float y, float z)
{
    return sqrt(x*x + y*y + z*z);
}

float NTagEventInfo::GetDistance(const float vec1[3], float vec2[3])
{
    float tmpVec[3];

    for (int i = 0; i < 3; i++)
        tmpVec[i] = vec1[i] - vec2[i];

    return Norm(tmpVec);
}

float NTagEventInfo::ReconCaptureTime(int candidateID)
{
    // trgofst may change with skdetsim version (13p90)
    return TMVATools.fVariables.Get<float>("dt", candidateID) - trgofst;
}

float NTagEventInfo::TrueCaptureTime(int candidateID)
{
    float tRecon = ReconCaptureTime(candidateID);
    float tClosest = 1.e8;

    if (nscndprt >= SECMAXRNG) return -1;
    for (int iCapture = 0; iCapture < nTrueCaptures; iCapture++) {
        if ( fabs(vCaptureTime[iCapture] - tRecon) < TMATCHWINDOW) {
            return vCaptureTime[iCapture];
        }
    }

    if (!IsTrueCapture(candidateID))
        msg.Print("A false neutron signal is passsed to TrueCaptureVertex!", pWARNING);

    return -9999.;
}

std::array<float, 3> NTagEventInfo::TrueCaptureVertex(int candidateID)
{
    std::array<float, 3> trueCaptureVertex = {0., 0., 0.};
    float tRecon = ReconCaptureTime(candidateID);

    for (int iCapture = 0; iCapture < nTrueCaptures; iCapture++) {
        if (fabs(vCaptureTime[iCapture] - tRecon) < TMATCHWINDOW) {
                trueCaptureVertex[0] = vCapPosx[iCapture];
                trueCaptureVertex[1] = vCapPosy[iCapture];
                trueCaptureVertex[2] = vCapPosz[iCapture];
                return trueCaptureVertex;
        }
    }
    if (!IsTrueCapture(candidateID))
        msg.Print("A false neutron signal is passsed to TrueCaptureVertex!", pWARNING);

    return trueCaptureVertex;
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

float NTagEventInfo::MinimizeTRMS(const std::vector<float>& T, const std::vector<int>& PMTID, float rmsFitVertex[])
{
    float delta;
    bool doSort = true;
    int nHits = static_cast<int>(T.size());
    assert(nHits == static_cast<int>(PMTID.size()));

    (VTXSRCRANGE > 200) ? delta = 100 : delta = VTXSRCRANGE / 2.;
    std::vector<float>  t_ToF;
    std::vector<float>* minTPointer;

    int rMax, zMax;
    zMax = (int)(2*ZPINTK / (float)delta);
    rMax = (int)(2*RINTK / (float)delta);

    // main search position starts from tank center
    std::array<float, 3> vecR = {0., 0., 0.};
    std::array<float, 3> tmpVertex = {0., 0., 0.};      // temp vertex to save minimizing vertex
    std::array<float, 3> srcVertex;                     // loop search vertex

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
                    if (Norm(srcVertex[0] - vecR[0], srcVertex[1] - vecR[1], srcVertex[2] - vecR[2]) > VTXSRCRANGE) continue;

                    t_ToF = GetToFSubtracted(T, PMTID, srcVertex.data(), doSort);

                    tRMS = GetTRMS(t_ToF);

                    if (tRMS < minTRMS) {
                        minTRMS = tRMS;
                        tmpVertex = srcVertex;
                        minTPointer = &t_ToF;
                    }
                }
            }
        }
        vecR = tmpVertex;
        delta = delta / 2.;
    }

    int index[1000];
    TMath::Sort(nHits, minTPointer->data(), index, false);

    rmsFitVertex[0] = vecR[0];
    rmsFitVertex[1] = vecR[1];
    rmsFitVertex[2] = vecR[2];

    return minTRMS;
}

std::array<float, 6> NTagEventInfo::GetBetaArray(const std::vector<int>& PMTID, int tStartIndex, int nHits)
{
    std::array<float, 6> beta = {0., 0., 0., 0., 0., 0};
    if (nHits == 0) return beta;

    float uvx[nHits], uvy[nHits], uvz[nHits];	// direction vector from vertex to each hit PMT

    for (int iHit = 0; iHit < nHits; iHit++) {
        float distFromVertexToPMT;
        float vecFromVertexToPMT[3];
        vecFromVertexToPMT[0] = PMTXYZ[PMTID[tStartIndex+iHit]-1][0] - pvx;
        vecFromVertexToPMT[1] = PMTXYZ[PMTID[tStartIndex+iHit]-1][1] - pvy;
        vecFromVertexToPMT[2] = PMTXYZ[PMTID[tStartIndex+iHit]-1][2] - pvz;
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

float NTagEventInfo::GetLegendreP(int i, float& x)
{
    float result = 0.;

    if (i < 0 || i > 5) {
        msg.Print(Form("Incompatible i (%d) is passed to GetLegendreP.", i), pERROR);
    }

    switch (i) {
        case 1:
            result = x; break;
        case 2:
            result = (3*x*x-1)/2.; break;
        case 3:
            result = (5*x*x*x-3*x)/2; break;
        case 4:
            result = (35*x*x*x*x-30*x*x+3)/8.; break;
        case 5:
            result = (63*x*x*x*x*x-70*x*x*x+15*x)/8.; break;
    }

    return result;
}

void NTagEventInfo::SortToFSubtractedTQ()
{
    nqiskz = static_cast<int>(vTISKZ.size());
    int sortedIndex[nqiskz], pmtID;

    // Sort: early hit first
    TMath::Sort(nqiskz, vUnsortedT_ToF.data(), sortedIndex, false);

    // Save hit info, sorted in (T - ToF)
    for (int iHit = 0; iHit < nqiskz; iHit++) {
        vSortedPMTID.push_back( vCABIZ[ sortedIndex[iHit] ]         );
        vSortedT_ToF.push_back( vUnsortedT_ToF[ sortedIndex[iHit] ] );
        vSortedQ.push_back    ( vQISKZ[ sortedIndex[iHit] ]         );
    }
}

int NTagEventInfo::GetNhitsFromStartIndex(const std::vector<float>& T, int startIndex, float tWidth)
{
    int searchIndex = startIndex;
    int nHits       = T.size();

    while (1) {
        searchIndex++;
        if ((searchIndex > nHits-1) || (TMath::Abs((T[searchIndex] - T[startIndex])) > tWidth))
            break;
    }
    // Return number of hits within the time window
    return TMath::Abs(searchIndex - startIndex);
}

float NTagEventInfo::GetQhitsFromStartIndex(const std::vector<float>& T, const std::vector<float>& Q, int startIndex, float tWidth)
{
    int nHits       = Q.size();
    int searchIndex = startIndex;
    float sumQ      = 0.;

    while (1) {
        sumQ += Q[searchIndex];
        searchIndex++;
        if ((searchIndex > nHits-1) || (TMath::Abs((T[searchIndex] - T[startIndex])) > tWidth))
            break;
    }
    // Return total hit charge within the time window
    return sumQ;
}

float NTagEventInfo::GetToF(float vertex[3], int pmtID)
{
    float vecFromVertexToPMT[3];

    for (int i = 0; i < 3; i++)
        vecFromVertexToPMT[i] = vertex[i] - PMTXYZ[pmtID][i];

    return GetDistance(PMTXYZ[pmtID], vertex) / C_WATER;
}

float NTagEventInfo::GetTRMS(const std::vector<float>& T)
{
    int   nHits  = T.size();
    float tMean = 0.;
    float tVar  = 0.;

    for (int iHit = 0; iHit < nHits; iHit++)
        tMean += T[iHit] / nHits;
    for (int iHit = 0; iHit < nHits; iHit++)
        tVar += (T[iHit]-tMean)*(T[iHit]-tMean) / nHits;

    return sqrt(tVar);
}

float NTagEventInfo::GetTRMSFromStartIndex(const std::vector<float>& T, int startIndex, float tWidth)
{
    int nHits = T.size();
    int searchIndex = startIndex;
    std::vector<float> tList;

    while (1) {
        tList.push_back(T[searchIndex]);
        searchIndex++;
        if ((searchIndex > nHits -1) || (TMath::Abs((T[searchIndex] - T[startIndex])) > tWidth))
            break;
    }

    return GetTRMS(tList);
}

int NTagEventInfo::GetNhitsFromCenterTime(const std::vector<float>& T, float centerTime, float tWidth)
{
    int NXX = 0;

    for (const auto& t: T) {
        if (t < centerTime - tWidth/2.) continue;
        if (t > centerTime + tWidth/2.) break;
        NXX++;
    }

    return NXX;
}

int NTagEventInfo::IsTrueCapture(int candidateID, bool bSave)
{
    float tRecon = ReconCaptureTime(candidateID);

    if (nscndprt >= SECMAXRNG) return -1;
    for (int iCapture = 0; iCapture < nTrueCaptures; iCapture++) {
        if (fabs(vCaptureTime[iCapture] - tRecon) < TMATCHWINDOW ) {
            if (bSave) vCandidateID.push_back(candidateID);
            return 1;
        }
    }
    return 0;
}

int NTagEventInfo::IsTrueGdCapture(int candidateID)
{
    float tRecon = ReconCaptureTime(candidateID);

    if (nscndprt >= SECMAXRNG) return -1;
    for (int iCapture = 0; iCapture < nTrueCaptures; iCapture++) {
        if (fabs(vCaptureTime[iCapture] - tRecon) < TMATCHWINDOW ) {
            if (vTotGamE[iCapture] > 6.) return 1;
            else return 0;
        }
    }

    return -9999;
}

void NTagEventInfo::Clear()
{
    nrun = 0; nsub = 0; nev = 0; trgtype = 0; nhitac = 0; nqiskz = 0;
    trgofst = 0; qismsk = 0;
    nring = 0; nmue = 0; ndcy = 0;
    evis = 0; pvx = 0; pvy = 0; pvz = 0; towall = 0;
    nCandidates = 0; N200M = 0;
    T200M = -9999.; firsthit = -9999.;

    nTrueCaptures = 0;
    nSavedSec = 0; nscndprt = 0;
    nN = modene = numne = 0;
    pnu = 0;
    nvect = 0;
    truevx = 0; truevy = 0; truevz = 0;

    vTISKZ.clear(); vQISKZ.clear(); vCABIZ.clear();

    vSortedPMTID.clear();
    vSortedT_ToF.clear(); vUnsortedT_ToF.clear(); vSortedQ.clear();

    vApip.clear(); vApamom.clear(); vApmome.clear(); vApmomm.clear();
    vTindex.clear(); vN10n.clear(); vN1300.clear();
    vTrms.clear(); vTrms50.clear();
    vDtn.clear(); vNvx.clear(); vNvy.clear(); vNvz.clear();
    vDoubleCount.clear();
    vBvx.clear(); vBvy.clear(); vBvz.clear(); vBvt.clear();
    vBeta14_10.clear(); vBeta14_50.clear();
    vTMVAoutput.clear();
    TMVATools.fVariables.Clear();

    vNGam.clear(); vCandidateID.clear();
    vCaptureTime.clear(); vCapPosx.clear(); vCapPosy.clear(); vCapPosz.clear(); vTotGamE.clear();
    vIsGdCapture.clear(); vIsTrueCapture.clear();
    vTruth_vx.clear(); vTruth_vy.clear(); vTruth_vz.clear(); vTimeDiff.clear();
    vIprtscnd.clear(); vLmecscnd.clear(); vIprntprt.clear(); vCaptureID.clear();
    vVtxscndx.clear(); vVtxscndy.clear(); vVtxscndz.clear(); vPscndx.clear(); vPscndy.clear(); vPscndz.clear();
    vWallscnd.clear(); vPabsscnd.clear(); vTscnd.clear();

    vIpne.clear();
    vIp.clear();
    vPinx.clear(); vPiny.clear(); vPinz.clear(); vPabs.clear();
}

void NTagEventInfo::ClearRawHitInfo()
{
    vTISKZ.clear(); vQISKZ.clear(); vCABIZ.clear();
}

void NTagEventInfo::SaveSecondary(int secID)
{
    vIprtscnd.push_back ( secndprt_.iprtscnd[secID]         );  // PID of secondaries
    vLmecscnd.push_back ( secndprt_.lmecscnd[secID]         );  // creation process
    vIprntprt.push_back ( secndprt_.iprntprt[secID]         );  // parent PID
    vVtxscndx.push_back ( secndprt_.vtxscnd[secID][0]       );  // creation vertex
    vVtxscndy.push_back ( secndprt_.vtxscnd[secID][1]       );
    vVtxscndz.push_back ( secndprt_.vtxscnd[secID][2]       );
    vWallscnd.push_back ( wallsk_(secndprt_.vtxscnd[secID]) );  // distance from wall to creation vertex
    vPscndx.push_back   ( secndprt_.pscnd[secID][0]         );  // momentum vector
    vPscndy.push_back   ( secndprt_.pscnd[secID][1]         );
    vPscndz.push_back   ( secndprt_.pscnd[secID][2]         );
    vPabsscnd.push_back ( Norm(secndprt_.pscnd[secID])      );  // momentum
    vTscnd.push_back    ( secndprt_.tscnd[secID]            );  // time created
    vCaptureID.push_back( -1 );
    nSavedSec++;
}

void NTagEventInfo::SavePeakFromHit(int hitID)
{
    // Calculate betas
    float t0      = vSortedT_ToF[hitID];
    int   N10i    = GetNhitsFromStartIndex(vSortedT_ToF, hitID, 10.);
    int   N200    = GetNhitsFromCenterTime(vSortedT_ToF, t0 + 5., 200.);
    auto  beta    = GetBetaArray(vSortedPMTID, hitID, N10i);
    float tEnd    = vSortedT_ToF[hitID+N10i-1];
    float sumQ    = GetQhitsFromStartIndex(vSortedT_ToF, vSortedQ, hitID, 10.);
    float trmsold = GetTRMSFromStartIndex(vSortedT_ToF, hitID, 10.);

    if ((N10i >= N10TH) && (N10i < N10MX+1)) {
        // Save info
        vTindex.push_back   ( hitID               );
        vBeta14_10.push_back( beta[1] + 4*beta[4] );

        TMVATools.fVariables.PushBack("N10",     N10i);
        TMVATools.fVariables.PushBack("N200",    N200);
        TMVATools.fVariables.PushBack("sumQ",    sumQ);
        TMVATools.fVariables.PushBack("dt",      (t0 + tEnd)/2.);
        TMVATools.fVariables.PushBack("spread",  tEnd - t0);
        TMVATools.fVariables.PushBack("trmsold", trmsold);

        // Increment number of neutron candidates
        nCandidates++;
    }
}