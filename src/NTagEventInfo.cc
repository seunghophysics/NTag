#include <math.h>
#include <cassert>
#include <iostream>
#include <vector>

#include <TMath.h>

#include <skparmC.h>
#include <apmringC.h>
#include <apmueC.h>
#include <appatspC.h>
#include <geotnkC.h>
#include <skheadC.h>
#include <sktqC.h>
#include <geopmtC.h>
#include <skvectC.h>
#include <neworkC.h>

#include <NTagEventInfo.hh>
#include <SKLibs.hh>

NTagEventInfo::NTagEventInfo()
:xyz(geopmt_.xyzpm), C_WATER(21.5833),
N10TH(5), N10MX(50), N200MX(140), T0TH(2.), DISTCUT(4000.), TMATCHWINDOW(40.), TMINPEAKSEP(50.)
fVerbosity(pDEFAULT), bData(false)
{
    reader = new TMVA::Reader("!Color:!Silent");
    SetTMVAReader();
}

NTagEventInfo::~NTagEventInfo() {}

void NTagEventInfo::SetEventHeader()
{
    nrun = skhead_.nrunsk;
    nsub = skhead_.nsubsk;
    nev = skhead_.nevsk;
    PrintMessage(Form("RUN %d SUBRUN %d EVENT %d", nrun, nsub, nev), pDEBUG);

    // Get number of OD hits
    odpc_2nd_s_(&nhitac);
    PrintMessage(Form("Number of OD hits: %d", nhitac), pDEBUG);

    trginfo_(&trgofst);
    qismsk = skq_.qismsk;
    PrintMessage(Form("Trigger offset: %f", trgofst), pDEBUG);
    PrintMessage(Form("Total charge in ID: %f", qismsk), pDEBUG);
}

void NTagEventInfo::SetAPFitInfo()
{
    // Get apcommul bank
    int bank = 0;
    aprstbnk_(&bank);

    // Get APFit vertex
    apvx = apcommul_.appos[0];
    apvy = apcommul_.appos[1];
    apvz = apcommul_.appos[2];

    float tmp_v[3] = {apvx, apvy, apvz};
    towall = wallsk_(tmp_v);

    PrintMessage(Form("APFit vertex: %f, %f, %f", apvx, apvy, apvz), pDEBUG);
    PrintMessage(Form("d_wall: %f", towall), pDEBUG);

    // E_vis
    evis = apcomene_.apevis;
    PrintMessage(Form("e_vis: %f", evis), pDEBUG);

    // AP ring information
    nring = apcommul_.apnring;
    for (int iRing = 0; iRing < nring; iRing++) {
        vApip[iRing].push_back(   apcommul_.apip[iRing]        );  // PID
        vApamom[iRing].push_back( apcommul_.apamom[iRing]      );  // Reconstructed momentum
        vApmome[iRing].push_back( appatsp2_.apmsamom[iRing][1] );  // e-like momentum
        vApmomm[iRing].push_back( appatsp2_.apmsamom[iRing][2] );  // mu-like momentum
    }
    PrintMessage(Form("APFit number of rings: %d", nring), pDEBUG);

    // mu-e check
    nmue = apmue_.apnmue; ndcy = 0;
    for (int iMuE = 0; iMuE < nmue; iMuE++) {
        if (iMuE == 10) break;
        if (apmue_.apmuetype[iMuE] == 1 || apmue_.apmuetype[iMuE] == 4) ndcy++;
    }
}

void NTagEventInfo::SetToFSubtractedTQ()
{
    nqiskz = sktqz_.nqiskz;
    int sortedIndex[nqiskz], pmtID;
<<<<<<< HEAD

    std::vector<float> tiskz(sktqz_.tiskz, sktqz_.tiskz + nqiskz);
    std::vector<float> cabiz(sktqz_.icabiz, sktqz_.icabiz + nqiskz);

=======
    
    std::vector<float> tiskz(sktqz_.tiskz, sktqz_.tiskz + nqiskz);
    std::vector<float> cabiz(sktqz_.icabiz, sktqz_.icabiz + nqiskz);
    
>>>>>>> 3890d4fd5abf222ebf035b152ea1377e60e735f9
    PrintMessage(Form("NQISKZ: %d", nqiskz), pDEBUG);

    // Subtract TOF from PMT hit time
    std::array<float,3> apfitVertex = {apvx, apvy, apvz};
    vUnsortedT_ToF = GetToFSubtracted(tiskz, cabiz, apfitVertex);

    // Sort: first hit first
    TMath::Sort(nqiskz, vUnsortedT_ToF.data(), sortedIndex, false);

    // Save hit info, sorted in (T - ToF)
    for (int iHit = 0; iHit < nqiskz; iHit++) {
        vSortedPMTID.push_back( cabiz[ sortedIndex[iHit] ]  );
        vSortedT_ToF.push_back( vUnsortedT_ToF[ sortedIndex[iHit] ] );
        vSortedQ.push_back(     sktqz_.qiskz[ sortedIndex[iHit] ]   );
        //PrintMessage(Form("iHit: %d    sortedT: %f", iHit, vSortedT_ToF[iHit]), pDEBUG);
    }
}

void NTagEventInfo::SetMCInfo()
{
    // Read SKVECT (primaries)
    skgetv_();
    nvect  = skvect_.nvect;                       // number of primaries
    truevx = skvect_.pos[0];                      // initial vertex of primaries
    truevy = skvect_.pos[1];
    truevz = skvect_.pos[2];

    for (int iVec = 0; iVec < nvect; iVec++) {
        vIp.push_back(   skvect_.ip[iVec]     );  // PID of primaries
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
        vIpne.push_back(nework_.ipne[i]);    // PIDs in vector
        if (vIpne[i] == 2112 && i >= 3) nN++; // count neutrons
    }
    PrintMessage(Form("Number of neutrons in primary stack: %d", nN), pDEBUG);

    // Initialize number of n captures
    nCT   = 0;
    nscnd = 0;

    // ?
    float ZBLST = 5.30;
    float dr    = RINTK - ZBLST;
    float dz    = 0.5 * HIINTK - ZBLST;

    // Read secondary bank
    typedef secndprt_ sec;
    apflscndprt_();
    int nSecNeutron = 0;
    nscndprt = sec.nscndprt;
<<<<<<< HEAD

=======
    
>>>>>>> 3890d4fd5abf222ebf035b152ea1377e60e735f9
    // Loop over all secondaries in secondary common block
    for (int iSec = 0; iSec < nscndprt; iSec++) {

        // Save all neutrons
        if (sec.iprtscnd[iSec] == 2112) {
            SaveSecondary(iSec);
            nSecNeutron++; nscnd++;
            PrintMessage(Form("Secondary neutron (#%d): [t = %f ns] [p = %f MeV/c]",
                         nSecNeutron, sec.tscnd[iSec]*1e-3, Norm(sec.pscnd[iSec])), pDEBUG);
        }
<<<<<<< HEAD

        // deuteron, gamma (no capture electrons?)
        else if (sec.iprtscnd[iSec] == 100045 || sec.iprtscnd[iSec] == 22) {

            // particle produced by n-capture
            if (sec.mecscnd[iSec] == 18) {

                // Save secondary
                SaveSecondary(iSec);
                nscnd++;

                /* Save capture info from below */
                float vtxR2 = sec.vtxscnd[iSec][0] * sec.vtxscnd[iSec][0]
                            + sec.vtxscnd[iSec][1] * sec.vtxscnd[iSec][1];
=======
        
        // deuteron, gamma (no capture electrons?)
        else if (sec.iprtscnd[iSec] == 100045 || sec.iprtscnd[iSec] == 22) {
            
            // particle produced by n-capture
            if (sec.mecscnd[iSec] == 18) {
                
                // Save secondary
                SaveSecondary(iSec);
                nscnd++;
            
                /* Save capture info from below */
                float vtxR2 = sec.vtxscnd[iSec][0] * sec.vtxscnd[iSec][0]
                            + sec.vtxscnd[iSec][1] * sec.vtxscnd[iSec][1]; 
>>>>>>> 3890d4fd5abf222ebf035b152ea1377e60e735f9
                int inPMT;
                inpmt_(vtxscnd[iSec], inPMT);

                // Check if the capture is within ID volume
                if (vtxR2 < dr*dr && fabs(vtxscnd[iSec][2]) < dz && !inPMT) {
<<<<<<< HEAD

                    bool isNewCapture = true;

=======
                
                    bool isNewCapture = true;
                    
>>>>>>> 3890d4fd5abf222ebf035b152ea1377e60e735f9
                    // Check saved capture stack
                    for (int iCheckedCT = 0; iCheckedCT < nCT; iCheckedCT++) {
                        // If this capture is already saved:
                        if (fabs((double)(sec.tscnd[iSec] - vCaptureTime[iCheckedCT])) < 1.e-7) {
                            isNewCapture = false;
                            // Add capture product gammas to the pre-existing stack
                            if (sec.iprtscnd[iSec] == 22) {
                                PrintMessage(Form("Gamma from already saved capture... vCaptureID %d", iCheckedCT), pDEBUG);
                                vNGam[iCheckedCT] += 1;
                                vTotGamE[iCheckedCT] += vPabsscnd[nscnd];
                                vCaptureID[nscnd] = iCheckedCT;
                            }
                        }
                    }

                    // If this capture product is new,
                    // save it as a new element in each array
                    if (isNewCapture) {
                        vCaptureTime.push_back( sec.tscnd[iSec] );
                        vCapPosx.push_back( sec.vtxscnd[iSec][0] );
                        vCapPosy.push_back( sec.vtxscnd[iSec][1] );
                        vCapPosz.push_back( sec.vtxscnd[iSec][2] );
<<<<<<< HEAD

=======
                        
>>>>>>> 3890d4fd5abf222ebf035b152ea1377e60e735f9
                        // Add capture product gamma
                        if (sec.iprtscnd[iSec] == 22) {
                            PrintMessage(Form("Gamma from new capture... vCaptureID %d", nCT), pDEBUG);
                            vNGam.push_back(      1                    );
                            vTotGamE.push_back(   Norm(sec.pscnd[iSec] );
                            vCaptureID.push_back( nCT                  );
                        }
                        else { vNGam.push_back(0); vTotGamE.push_back(0.); }
<<<<<<< HEAD

=======
                        
>>>>>>> 3890d4fd5abf222ebf035b152ea1377e60e735f9
                        // nCT++ with new capture
                        nCT++;
                    }
                }
            }
        }
    }
    assert(nscnd == vIprtscnd.size());
    assert(nCT  == vCaptureID.size());
<<<<<<< HEAD

=======
    
>>>>>>> 3890d4fd5abf222ebf035b152ea1377e60e735f9

    for (int iCapture = 0; iCapture < nCT; iCapture++) {
        PrintMessage(Form("CaptureID %d: [t: %f us] [Gamma E: %f MeV]",
                          iCapture, vCaptureTime[iCapture]*1e-3, vTotGamE[iCapture]), pDEBUG);
    }
    PrintMessage(Form("Number of secondary neutrons saved in bank: %d", nSecNeutron), pDEBUG);
    PrintMessage(Form("Number of captures: %d", nCT), pDEBUG);
}

void NTagEventInfo::SearchCaptureCandidates()
{
    int     iHitPrevious;
    float	t0Previous      = 0;
    float   N200Previous    = 0;
<<<<<<< HEAD

=======
    
>>>>>>> 3890d4fd5abf222ebf035b152ea1377e60e735f9
    // Loop over the saved TQ hit array from current event
    for (int iHit = 0; iHit < nqiskz; iHit++) {

        // Save time of first hit
        if (firsthit == 0.) firsthit = vSortedT_ToF[iHit];
<<<<<<< HEAD

=======
        
>>>>>>> 3890d4fd5abf222ebf035b152ea1377e60e735f9
        // Calculate N10i:
        // number of hits in 10 ns window from the i-th hit
        int N10i = GetNhitsFromStartIndex(vSortedT_ToF, nqiskz, iHit, 10.);

        // If N10TH <= N10i <= N10MX:
        if ((N10i < N10TH) || (N10i > N10MX)) continue;
        // We've found a new peak.

        // t0 of new peak
        float t0New = vSortedT_ToF[iHit];

        // Save maximum N200 and its t0
        float N200New = GetNhitsFromCenterTime(vSortedT_ToF, t0New + 5., 200.);
        if (t0New > 2.e4 && N200New > N200M) {
            N200M = N200New;
            T200M = t0New;
        }
        
        // If peak t0 diff = t0New - t0Previous > TMINPEAKSEP, save the previous peak.
        // Also check if N200Previous is below N200 cut and if t0Previous is over t0 threshold
        if (   t0New - t0Previous > TMINPEAKSEP 
           && N200Previous < N200MX 
           && t0Previous*1.e-3 > T0TH) {

            SavePeakFromHit(iHitPrevious);

<<<<<<< HEAD
        // If peak t0 diff = t0New - t0Previous > TMINPEAKSEP, save the previous peak.
        // Also check if N200Previous is below N200 cut and if t0Previous is over t0 threshold
        if (   t0New - t0Previous > TMINPEAKSEP
           && N200Previous < N200MX
           && t0Previous*1.e-3 > T0TH) {

            SavePeakFromHit(iHitPrevious);

=======
>>>>>>> 3890d4fd5abf222ebf035b152ea1377e60e735f9
            if (nCandidates >= MAXNP-1) {
                PrintMessage(Form("Run %d Subrun %d Event %d", nrun, nsub, nev), pWARNING);
                PrintMessage(Form("Number of neutron candidates reached limit (MAXNP=%d)", MAXNP), pWARNING);
                PrintMessage(Form("Skipping remaining neutrons..."), pWARNING);
                break;
            }
        }
        iHitPrevious = iHit;
        t0Previous   = t0New;
        N200Previous = N200New;
    }

    // Select hits within 50 ns around each capture candidate
    // to calculate beta and feed BONSAI
<<<<<<< HEAD

=======
    
>>>>>>> 3890d4fd5abf222ebf035b152ea1377e60e735f9
    int                 n50hits, n1300hits;
    std::vector<int>    index50, index1300, nindex;
    std::vector<int>    cabiz50, cabiz1300;
    std::vector<float>  tiskz50, qiskz50, tiskz1300, qiskz1300;

    vTrms.resize(nCandidates);
<<<<<<< HEAD

=======
    
>>>>>>> 3890d4fd5abf222ebf035b152ea1377e60e735f9
    // Loop over all found capture candidates
    for (int iCapture = 0; iCapture < nCandidates; iCapture++) {
        n50hits = 0;
        n1300hits = 0;

        // Loop over all hits in event
        for (int iHit = 0; iHit < nqiskz; iHit++) {

            // Count N50 and save hit indices in vSortedT_ToF
            if (fabs(vUnsortedT_ToF[iHit] - vDtn[iCapture]) < 25.) {
                  index50.push_back(iHit);
                  n50hits++;
            }

            // Count N1300 and save hit indices in vSortedT_ToF
            if (vUnsortedT_ToF[iHit] > vDtn[iCapture] - 520.8
            &&  vUnsortedT_ToF[iHit] < vDtn[iCapture] + 779.2) {
                if (n1300hits < 1000) {
                    index1300.push_back(iHit);
                    n1300hits++;
                }
            }
        }

        for (int iHit50 = 0; iHit50 < n50hits; iHit50++) {
            cabiz50.push_back( sktqz_.icabiz[ index50[iHit50] ] );
            tiskz50.push_back( sktqz_.tiskz[ index50[iHit50] ]  );
            qiskz50.push_back( sktqz_.qiskz[ index50[iHit50] ]  );
        }

        for (int iHit1300 = 0; iHit1300 < n1300hits; iHit1300++) {
            cabiz1300.push_back( sktqz_.icabiz[ index1300[iHit1300] ] );
            tiskz1300.push_back( sktqz_.tiskz[ index1300[iHit1300] ]  );
            qiskz1300.push_back( sktqz_.qiskz[ index1300[iHit1300] ]  );
        }

        // Calculate betas for N50 hits
        auto beta_50 = GetBetaArray(cabiz50, 0, n50hits);

        // Save N50, N1300, and betas for N50 hits
        vN50.push_back(       n50hits                   );
        vN1300.push_back(     n1300hits                 );
        vBeta1_50.push_back(  beta_50[1]                );
        vBeta2_50.push_back(  beta_50[2]                );
        vBeta3_50.push_back(  beta_50[3]                );
        vBeta4_50.push_back(  beta_50[4]                );
        vBeta5_50.push_back(  beta_50[5]                );
        vBeta14_50.push_back( beta_50[1] + 4*beta_50[4] );

        // BONSAI fit to each capture candidate
        float tmptbsenergy, tmptbsvx, tmptbsvy, tmptbsvz, tmptbsvt, tmptbsgood, tmptbsdirks, tmptbspatlik, tmptbsovaq;
        float time0 = vDtn[iCapture];

        bonsai_fit_(&time0, tiskz1300, qiskz1300, cabiz1300, &n1300hits, &tmptbsenergy, &tmptbsvx, &tmptbsvy, &tmptbsvz,
        &tmptbsvt, &tmptbsgood, &tmptbsdirks, &tmptbspatlik, &tmptbsovaq);

        float tbsvertex[3] = {tmptbsvx, tmptbsvy, tmptbsvz};

        // Save BONSAI fit results
        vBenergy.push_back( tmptbsenergy       );
        vBvx.push_back( 	tmptbsvx           );
        vBvy.push_back( 	tmptbsvy           );
        vBvz.push_back( 	tmptbsvz           );
        vBvt.push_back( 	tmptbsvt           );
        vBwall.push_back( 	wallsk_(tbsvertex) );
        vBgood.push_back( 	tmptbsgood         );
        vBdirks.push_back( 	tmptbsdirks        );
        vBpatlik.push_back( tmptbspatlik       );
        vBovaq.push_back( 	tmptbsovaq         );

        float nv[3];	// vertex to fit by minimizing tRMS
        float minTRMS = MinimizeTRMS(tiskz50, cabiz50, n50hits, nv);

        vNvx.push_back(    nv[0]       );
        vNvy.push_back(    nv[1]       );
        vNvz.push_back(    nv[2]       );
        vNwall.push_back(  wallsk_(nv) );
        vTrms50.push_back( minTRMS     );
<<<<<<< HEAD

=======
        
>>>>>>> 3890d4fd5abf222ebf035b152ea1377e60e735f9
        auto tiskz50_ToF = GetToFSubtracted(tiskz50, cabiz50, n50hits, nv, true);

        int N10in, tmpN10n = 0;
        float t0n = 0.;

        // Search for a new best N10 (N10n) from these new ToF corrected hits
        int n10index = 0;
        for (int iHit50 = 0; iHit50 < n50hits; iHit50++) {
            N10in = GetNhitsFromStartIndex(tiskz50_ToF, n50hits, iHit50, 10.);
            if (N10in > tmpN10n) {
                tmpN10n = N10in; n10index = iHit50;
                t0n = (tiskz50_ToF[iHit50] + tiskz50_ToF[iHit50+tmpN10n-1]) / 2.;
            }
        }

        vTrms.push_back( GetTRMSFromStartIndex(tiskz50_ToF, n10index, 10.) );
        vN10n.push_back( tmpN10n );
        vDtn.push_back(  t0n );
    }

    if (!bData) {
        SetTrueCaptureInfo();
    }
}

void NTagEventInfo::SetTrueCaptureInfo()
{
    std::vector<float> checkedCT;
    int nCheckedCaptures = 0;

    for (int iCapture = 0; iCapture < nCandidates; iCapture++) {

        // Check if a candidate is a true neutron capture (MC only)
        vIsTrueCapture.push_back( IsTrueCapture(iCapture) );

        // if a candidate is actually a true neutron capture
        if (vIsTrueCapture[iCapture] == 1) {

            vDoubleCount.push_back(0);
            //time diff between true and reconstructed capture time
            vTimeDiff.push_back( ReconCaptureTime(iCapture) - TrueCaptureTime(iCapture) );

            bool newCaptureFound = true;

            if (!checkedCT.empty()) {
                for (const auto& CTinList: checkedCT) {
                    if (fabs(TrueCaptureTime(iCapture) - CTinList) < 1.e-3) {
                        newCaptureFound = false; break;
                    }
                }
            }

            if (newCaptureFound) {
                checkedCT.push_back( TrueCaptureTime(iCapture) );
            }
<<<<<<< HEAD

            // Check whether two adjacent candidates should be saved as a single true capture
=======
            
        .push_bakck     )     // Check whether two adjacent candidates should be saved as a single true capture
>>>>>>> 3890d4fd5abf222ebf035b152ea1377e60e735f9
            else {
                if (fabs(vTimeDiff[iCapture]) < fabs(vTimeDiff[iCapture-1])) {
                    vDoubleCount[iCapture-1] = 1; vIsTrueCapture[iCapture-1] = 0;
                }
                else {
                    vDoubleCount[iCapture] = 1; vIsTrueCapture[iCapture] = 0;
                }
            }
<<<<<<< HEAD

=======
            
>>>>>>> 3890d4fd5abf222ebf035b152ea1377e60e735f9
            auto tmpTruthV = TrueCaptureVertex(iCapture);
            vTruth_vx.push_back( tmpTruthV[0] );
            vTruth_vy.push_back( tmpTruthV[1] );
            vTruth_vz.push_back( tmpTruthV[2] );

            // Check whether capture is on Gd or H
            vIsGdCapture.push_back( IsTrueGdCapture(iCapture) );
        }
    }
}

void NTagEventInfo::GetTMVAoutput()
{
    for (int iCapture = 0; iCapture < nCandidates; iCapture++) {

        // Check conditions for non-dummy TMVA output:
        // * Number of OD hits < 16
        // * N10 >= 7
        // * 0 < Reconstructed capture time < 200 us
        if (!(nhitac < 16)) {
            vTMVAoutput.push_bakck(-9999); continue;
        }
        if (!(N10[iCapture] >= 7 && 0 < vDt[iCapture] && vDt[iCapture] < 2.e5)) {
            vTMVAoutput.push_bakck(-9999); continue;
        }

        mva_N10 		= vN10[iCapture];
        mva_N200 		= vN200[iCapture];
        mva_N50 		= N50[iCapture];
        mva_dt 			= vDt[iCapture];
        mva_sumQ 		= vSumQ[iCapture];
        mva_spread 		= vSpread[iCapture];
        mva_trmsold 	= vTrmsold[iCapture];
        mva_beta1_50 	= vBeta1_50[iCapture];
        mva_beta2_50 	= vBeta2_50[iCapture];
        mva_beta3_50 	= vBeta3_50[iCapture];
        mva_beta4_50 	= vBeta4_50[iCapture];
        mva_beta5_50 	= vBeta5_50[iCapture];
        mva_tbsenergy 	= vBenergy[iCapture];
        mva_tbswall 	= vBwall[iCapture];
        mva_tbsgood 	= vBgood[iCapture];
        mva_tbsdirks 	= vBdirks[iCapture];
        mva_tbspatlik 	= vBpatlik[iCapture];
        mva_tbsovaq 	= vBovaq[iCapture];
        mva_nwall 		= vNwall[iCapture];
        mva_trms50 		= vTrms50[iCapture];

        mva_AP_BONSAI 	= Norm(apvx - vBvx[iCapture],
                               apvy - vBvy[iCapture],
                               apvz - vBvz[iCapture]);
        mva_AP_Nfit 	= Norm(apvx - vNvx[iCapture],
                               apvy - vNvy[iCapture],
                               apvz - vNvz[iCapture]);
        mva_Nfit_BONSAI = Norm(vNvx[iCapture] - vBvx[iCapture],
                               vNvy[iCapture] - vBvy[iCapture],
                               vNvz[iCapture] - vBvz[iCapture]);

        vTMVAoutput.push_back( reader->EvaluateMVA("BDT method") );
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
    // for skedsim before 13p90 use -trgofst, 13p90 and after use +trgofst
    return dt[candidateID] - 1000 + trgofst;
}

float NTagEventInfo::TrueCaptureTime(int candidateID)
{
    float tRecon = ReconCaptureTime(candidateID);

    if (nscndprt >= SECMAXRNG) return -1;
    for (int iCapture = 0; iCapture < nCT; iCapture++) {
        if (fabs(vCaptureTime[iCapture] - tRecon) < TMATCHWINDOW)
            return vCaptureTime[iCapture];
    }
    if (!IsTrueCapture(candidateID))
        PrintMessage("A false neutron signal is passsed to TrueCaptureTime!", pERROR);

    return 0.;
}

std::array<float, 3> NTagEventInfo::TrueCaptureVertex(int candidateID)
{
    std::array<float, 3> trueCaptureVertex = {0., 0., 0.};
    float tRecon = ReconCaptureTime(candidateID);

    for (int iCapture = 0; iCapture < nCT; iCapture++) {
        if (fabs(vCaptureTime[iCapture] - tRecon) < TMATCHWINDOW) {
            for (int k = 0; k < 3; k++)
                trueCaptureVertex[k] = capPos[iCapture][k];
        }
    }
    if (!IsTrueCapture(candidateID))
        PrintMessage("A false neutron signal is passsed to TrueCaptureVertex!", pERROR);

    return trueCaptureVertex;
}

std::vector<float> NTagEventInfo::GetToFSubtracted(std::vector<float>& T, std::vector<int>& PMTID, float vertex[3], bool doSort)
{
    std::vector<float> t_ToF;
<<<<<<< HEAD

    int nHits = T.size();
    assert(nHits == PMTID.size());

=======
    
    int nHits = T.size();
    assert(nHits == PMTID.size());
    
>>>>>>> 3890d4fd5abf222ebf035b152ea1377e60e735f9
    // Subtract TOF from PMT hit time
    for (int iHit = 0; iHit < nHits; iHit++) {
        int pmtID = PMTID[iHit] - 1;
        t_ToF.push_back( T[iHit] - GetToF(vertex, pmtID) );
    }

    if (doSort) {
        int sortedIndex[nHits];
        TMath::Sort(nHits, t_ToF.data(), sortedIndex, false);
        for (int iHit = 0; iHit < nHits; iHit++)
            t_ToF[iHit] = t_ToF[ sortedIndex[iHit] ];
    }

    return t_ToF;
}

float NTagEventInfo::MinimizeTRMS(std::vector<float>& T, std::vector<int>& PMTID, float rmsFitVertex[])
{
    float delta;
    bool doSort = true;
    int nHits = T.size();
    assert(nHits == PMTID.size());
<<<<<<< HEAD

=======
    
>>>>>>> 3890d4fd5abf222ebf035b152ea1377e60e735f9
    (DISTCUT > 200) ? delta = 100 : delta = DISTCUT / 2.;
    float tiskzmin[nHits];
    int cabizmin[nHits];

    int rMax, zMax;
    zMax = (int)(2*ZPINTK / (float)delta);
    rMax = (int)(2*RINTK / (float)delta);

    // main search position starts from tank center
    std::array<float, 3> vecR = {0., 0., 0.};
    std::array<float, 3> tmpVertex = {0., 0., 0.};      // temp vertex
    std::array<float, 3> srcVertex;                     // loop search vertex

    float minTRMS = 9999.;
    float tRMS;

    while (delta > 0.5) {
<<<<<<< HEAD

=======
    
>>>>>>> 3890d4fd5abf222ebf035b152ea1377e60e735f9
        for (float x = 0; x < rMax; x++) {
            srcVertex[0] = delta * (x - rMax/2.) + vecR[0];
            for (float y = 0; y < rMax; y++) {
                srcVertex[1] = delta * (y - rMax/2.) + vecR[1];
<<<<<<< HEAD

=======
                
>>>>>>> 3890d4fd5abf222ebf035b152ea1377e60e735f9
                if (TMath::Sqrt(srcVertex[0]*srcVertex[0] + srcVertex[1]*srcVertex[1]) > RINTK) continue;
                for (float z = 0; z < zMax; z++) {
                    srcVertex[2] = delta * (z - zMax/2.) + vecR[2];
                    if (srcVertex[2] > ZPINTK || srcVertex[2] < -ZPINTK) continue;
                    if (Norm(srcVertex[0] - vecR[0], srcVertex[1] - vecR[1], srcVertex[2] - vecR[2]) > DISTCUT) continue;
<<<<<<< HEAD

                    std::vector<float> t_ToF = GetToFSubtracted(t_ToF, T, PMTID, nHits, srcVertex, doSort);
                    tRMS = GetTRMS(t_ToF);

                    if (tRMS < minTRMS) {
                        minTRMS = tRMS;
                        tmpVertex = srcVertex;

=======

                    std::vector<float> t_ToF = GetToFSubtracted(t_ToF, T, PMTID, nHits, srcVertex, doSort);
                    tRMS = GetTRMS(t_ToF);
                    
                    if (tRMS < minTRMS) {
                        minTRMS = tRMS;
                        tmpVertex = srcVertex;
                        
>>>>>>> 3890d4fd5abf222ebf035b152ea1377e60e735f9
                        for (int i = 0; i < nHits; i++) {
                            tiskzmin[i] = t_ToF[i];
                        }
                    }
                }
            }
        }
        vecR = tmpVertex;
        delta = delta / 2.;
    }

    int index[1000];
    TMath::Sort(nHits, tiskzmin, index, false);

    rmsFitVertex[0] = vecR[0];
    rmsFitVertex[1] = vecR[1];
    rmsFitVertex[2] = vecR[2];

    return minTRMS;
}

std::array<float, 6> NTagEventInfo::GetBetaArray(std::vector<int> PMTID, int tID, int nHits)
{
    std::array<float, 6> beta = {0., 0., 0., 0., 0., 0};

    float uvx[nHits], uvy[nHits], uvz[nHits];	// direction vector from vertex to each hit PMT

    for (int i = 0; i < nHits; i++) {
        float distFromVertexToPMT;
        float vecFromVertexToPMT[3];
        vecFromVertexToPMT[0] = xyz[PMTID[tID+i]-1][0] - apvx;
        vecFromVertexToPMT[1] = xyz[PMTID[tID+i]-1][1] - apvy;
        vecFromVertexToPMT[2] = xyz[PMTID[tID+i]-1][2] - apvz;
        distFromVertexToPMT = Norm(vecFromVertexToPMT);
        uvx[i] = vecFromVertexToPMT[0] / distFromVertexToPMT;
        uvy[i] = vecFromVertexToPMT[1] / distFromVertexToPMT;
        uvz[i] = vecFromVertexToPMT[2] / distFromVertexToPMT;
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
<<<<<<< HEAD

=======
    
>>>>>>> 3890d4fd5abf222ebf035b152ea1377e60e735f9
    if (i < 0 || i > 5) {
        PrintMessage(Form("Incompatible i (%d) is passed to GetLegendreP.", i), pERROR);
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

int NTagEventInfo::GetNhitsFromStartIndex(std::vector<float>& T, int startIndex, float tWidth)
{
    int i     = startIndex;
    int nHits = T.size();

    while (1) {
        i++;
        if ((i > nHits-1) || (TMath::Abs((T[i] - T[startIndex])) > tWidth))
            break;
    }
    // Return number of hits within the time window
    return TMath::Abs(i - startIndex);
}

float NTagEventInfo::GetQhitsFromStartIndex(std::vector<float>& T, std::vector<float>& Q, int startIndex, float tWidth)
{
    int nHits       = Q.size();
    int searchIndex = startIndex;
    float sumQ      = 0.;
<<<<<<< HEAD

=======
    
>>>>>>> 3890d4fd5abf222ebf035b152ea1377e60e735f9
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
<<<<<<< HEAD

    for (int i = 0; i < 3; i++)
        vecFromVertexToPMT[i] = vertex[i] - xyz[pmtID][i];

    return GetDistance(xyz[pmtID], vertex) / C_WATER;
}

float NTagEventInfo::GetTRMS(std::vector<float>& T)
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

float NTagEventInfo::GetTRMSFromStartIndex(std::vector<float>& T, int startIndex, float tWidth)
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

=======

    for (int i = 0; i < 3; i++)
        vecFromVertexToPMT[i] = vertex[i] - xyz[pmtID][i];
        
    return GetDistance(xyz[pmtID], vertex) / C_WATER;
}

float NTagEventInfo::GetTRMS(std::vector<float>& T)
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

float NTagEventInfo::GetTRMSFromStartIndex(std::vector<float>& T, int startIndex, float tWidth)
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

>>>>>>> 3890d4fd5abf222ebf035b152ea1377e60e735f9
int NTagEventInfo::GetNhitsFromCenterTime(std::vector<float>& T, float centerTime, float tWidth)
{
    int NXX = 0;

    for (const auto& t: T) {
        if (t < centerTime - tWidth/2.) continue;
        if (t > centerTime + tWidth/2.) break;
        NXX++;
    }

    return NXX;
}

int NTagEventInfo::IsTrueCapture(int candidateID)
{
    float tRecon = ReconCaptureTime(candidateID);

    if (nscndprt >= SECMAXRNG) return -1;
    for (int iCapture = 0; iCapture < nCT; iCapture++) {
        if (fabs(vCaptureTime[iCapture] - tRecon) < TMATCHWINDOW ) return 1;
    }
    return 0;
}

int NTagEventInfo::IsTrueGdCapture(int candidateID)
{
    float tRecon = ReconCaptureTime(candidateID);

    if (nscndprt >= SECMAXRNG) return -1;
    for (int iCapture = 0; iCapture < nCT; iCapture++) {
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
<<<<<<< HEAD
    nCandidates = 0; trgofst = 0; timnsk = 0; qismsk = 0;
=======
    nCandidates = 0; trgofst = 0; timnsk = 0; qismsk = 0; 
>>>>>>> 3890d4fd5abf222ebf035b152ea1377e60e735f9
    nring = 0; nmue = 0; ndcy = 0; evis = 0;
    apvx = 0; apvy = 0; apvz = 0;
    towall = 0;
    N200M = 0;
    mctrue_nn = 0;
    ip0 = 0;
    nscnd = 0; nscndprt = 0;
    broken = 0;
    lasthit = 0; firsthit = 0; firstflz = 0;
    px = 0; py = 0; pz = 0;
    dirx = diry = dirz = 0;
    nCT = 0;
    nN = modene = numne = 0;
    pnu = 0;
    nvect = 0;
    bt = 1000000;
    T200M = -9999.;
    pos[0] = 0; pos[1] = 0; pos[2] = 0;

    vSortedPMTID.clear();
    vSortedT_ToF.clear();
    vUnsortedT_ToF.clear();
    vSortedQ.clear();

    for (int i = 0; i < APNMAXRG; i++) {
        vApip[i] = 0;
        vApamom[i] = 0; vApmome[i] = 0; vApmomm[i] = 0;
    }

    vTindex.clear();
    vN10.clear();
    vN10n.clear();
    vN50.clear();
    vN200.clear();
    vN1300.clear();
<<<<<<< HEAD

=======
    
>>>>>>> 3890d4fd5abf222ebf035b152ea1377e60e735f9
    for (int i = 0; i < MAXNP; i++) {
        vN10n[i] =  0; vN50[i] =  0; vN200[i] =  0; vN1300[i] = 0;
        vSumQ[i] =  0; vSpread[i] = 0; vTrms[i] = 0; vTrmsold[i] = 0; vTrms50[i] = 0;
        mintrms_3[i] = 0; mintrms_4[i] = 0; mintrms_5[i] = 0; mintrms_6[i] = 0;
        dt[i] = 0; vDtn[i] = 0;
        vNvx[i] = 0; vNvy[i] = 0; vNvz[i] = 0; vNwall[i] = 0;
        tmpVertex[0][i] = 0; tmpVertex[1][i] = 0; tmpVertex[2][i] = 0;
        vDoubleCount[i] = 0; vGoodn[i] = 0;
        vBenergy[i] = 0;
        vBvx[i] = 0; vBvy[i] = 0; tbsrcVertex[2][i] = 0; vBvt[i] = 0;
        vBwall[i] = 0; vBgood[i] = 0;
        vBpatlik[i] = 0; vBdirks[i] = 0; vBovaq[i] = 0;

        vBeta14_10[i] = 0; vBeta14_50[i] = 0;
        vBeta1_50[i] = 0; vBeta2_50[i] = 0; vBeta3_50[i] = 0; vBeta4_50[i] = 0; vBeta5_50[i] = 0;
        ratio[i] = 0; phirms[i] = 0; thetam[i] = 0; summedWeight[i] = 0; g2d2[i] = 0;
        Nback[i] = 0; Neff[i] = 0; Nc1[i] = 0; NhighQ[i] = 0;
        Nc[i] = 0; Ncluster[i] = 0; Nc8[i] = 0; Ncluster8[i] = 0;
        Nc7[i] = 0; Ncluster7[i] = 0; N12[i] = 0; N20[i] = 0; N300[i] = 0;
        npx[i] = 0; npy[i] = 0; npz[i] = 0;
        ndirx[i] = 0; ndiry[i] = 0; ndirz[i] = 0;

        vIsGdCapture[i] = 0; vIsTrueCapture[i] = 0;
        vTruth_vx[i] = 0; vTruth_vy[i] = 0; vTruth_vz[i] = 0; vTimeDiff[i] = 0;

        vTMVAoutput[i] = -9999.;

        for (int j = 0; j < 9; j++) Nlow[j][i] = 0;
    }

    for (int i = 0; i < kMaxCT; i++) {
        vNGam[i] = 0;
        vCaptureTime[i] = 0; vTotGamE[i] = 0;
        vIpne[i] = 0;

        for (int j = 0; j < 3; j++) {
            capPos[i][j] = 0;
        }
    }

    for (int i = 0; i < SECMAXRNG; i++) {
        vIprtscnd[i] = 0; vLmecscnd[i] = 0; vIprntprt[i] = 0;
        vWallscnd[i] = 0; vPabsscnd[i] = 0; vTscnd[i] = 0;
        vCaptureID[i] = 0;
        vIp[i] = 0;
        vPabs[i] = 0;

        for (int j = 0; j < 3; j++) {
            vtxscnd[i][j] = 0; pscnd[i][j] = 0; pin[i][j] = 0;
        }
    }
}

void NTagEventInfo::SaveSecondary(int secID)
{
    vIprtscnd.push_back(  secndprt_.iprtscnd[iSec]   ); // PID of secondaries
    vLmecscnd.push_back(  secndprt_.lmecscnd[iSec]   ); // creation process
    vIprntprt.push_back(  secndprt_.iprntprt[iSec]   ); // parent PID
    vVtxscndx.push_back(  secndprt_.vtxscnd[iSec][0] ); // creation vertex
    vVtxscndy.push_back(  secndprt_.vtxscnd[iSec][1] );
    vVtxscndz.push_back(  secndprt_.vtxscnd[iSec][2] );
    vWallscnd.push_back(  wallsk_(vtxscnd[nscnd])    ); // distance from wall to creation vertex
    vPscndx.push_back(    secndprt_.pscnd[iSec][0]   ); // momentum vector
    vPscndy.push_back(    secndprt_.pscnd[iSec][1]   );
    vPscndz.push_back(    secndprt_.pscnd[iSec][2]   );
    vPabsscnd.push_back(  Norm(pscnd[nscnd])         ); // momentum
    vTscnd.push_back(     secndprt_.vTscnd[iSec]     ); // time created
<<<<<<< HEAD
    vCaptureID.push_back( -1                         );
=======
    vCaptureID.push_back( -1                         );     
>>>>>>> 3890d4fd5abf222ebf035b152ea1377e60e735f9
}

void NTagEventInfo::SavePeakFromHit(int hitID)
{
    // Calculate betas
    float t0      = vSortedT_ToF[hitID];
    int   N10i    = GetNhitsFromStartIndex(vSortedPMTID, nqiskz, hitID, 10.);
    int   N200    = GetNhitsFromCenterTime(vSortedT_ToF, t0 + 5., 200.);
    auto  beta    = GetBetaArray(vSortedPMTID, hitID, N10i);
    float tEnd    = vSortedT_ToF[hitID+N10i-1];
    float sumQ    = GetQhitsFromStartIndex(hitID, 10.);
    float trmsold = GetTRMSFromStartIndex(vSortedT_ToF, hitID, 10.);
<<<<<<< HEAD

=======
    
>>>>>>> 3890d4fd5abf222ebf035b152ea1377e60e735f9
    // Save info to the member variables
    vNvx.push_back(       apvx                   );
    vNvy.push_back(       apvy                   );
    vNvz.push_back(       apvz                   );
    vN10.push_back(       N10i                   );
    N10n.push_back(       N10i                   );
    vN200.push_back(      N200                   );
    vSumQ.push_back(      sumQ                   );
    vDt.push_back(        (t0 + tEnd) / 2.       );
    vDtn.push_back(       (t0 + tEnd) / 2.       );
    vTindex.push_back(    hitID                  );
    vSpread.push_back(    tEnd - t0              );
    vTrmsold.push_back(   trmsold                );
    vBeta14_10.push_back( beta[1] + 4*beta[4]    );
<<<<<<< HEAD

=======
    
>>>>>>> 3890d4fd5abf222ebf035b152ea1377e60e735f9
    // Increment number of neutron candidates
    nCandidates++;
}

void NTagEventInfo::SetTMVAReader()
{
    reader->AddVariable("evis", 		&evis);
    reader->AddVariable("N10", 			&mva_N10);
    reader->AddVariable("N200", 		&mva_N200);
    reader->AddVariable("N50", 			&mva_N50);
    reader->AddVariable("dt", 			&mva_dt);
    reader->AddVariable("sumQ", 		&mva_sumQ);
    reader->AddVariable("spread", 		&mva_spread);
    reader->AddVariable("trmsold", 		&mva_trmsold);
    reader->AddVariable("beta1", 		&mva_beta1_50);
    reader->AddVariable("beta2", 		&mva_beta2_50);
    reader->AddVariable("beta3", 		&mva_beta3_50);
    reader->AddVariable("beta4", 		&mva_beta4_50);
    reader->AddVariable("beta5", 		&mva_beta5_50);
    reader->AddVariable("AP_Nfit:=sqrt((vx-nvx)*(vx-nvx)+(vy-nvy)*(vy-nvy)+(vz-nvz)*(vz-nvz))", &mva_AP_Nfit);
    reader->AddVariable("tbsenergy", 	&mva_tbsenergy);
    reader->AddVariable("tbswall", 		&mva_tbswall);
    reader->AddVariable("tbsgood", 		&mva_tbsgood);
    reader->AddVariable("tbsdirks", 	&mva_tbsdirks);
    reader->AddVariable("tbspatlik", 	&mva_tbspatlik);
    reader->AddVariable("tbsovaq", 		&mva_tbsovaq);
    reader->AddVariable("AP_BONSAI:=sqrt((vx-tbsvx)*(vx-tbsvx)+(vy-tbsvy)*(vy-tbsvy)+(vz-tbsvz)*(vz-tbsvz))", &mva_AP_BONSAI);
    reader->AddVariable("nwall", 		&mva_nwall);
    reader->AddVariable("trms40", 		&mva_trms50);
    reader->AddVariable("Nfit_BONSAI:=sqrt((nvx-tbsvx)*(nvx-tbsvx)+(nvy-tbsvy)*(nvy-tbsvy)+(nvz-tbsvz)*(nvz-tbsvz))", &mva_Nfit_BONSAI);

    reader->BookMVA("BDT method", "weights/BDT_Gd0.2p.xml");
}

void NTagEventInfo::PrintTag(unsigned int vType)
{
    switch (vType) {
        case pDEFAULT:
            std::cout << "[NTag] "; break;
        case pWARNING:
            std::cout << "\033[4;33m" << "[NTag WARNING] "; break;
        case pERROR:
            std::cerr << "\033[4;31m" << "[Error in NTag] "; break;
        case pDEBUG:
            std::cout << "\033[0;34m" << "[NTag DEBUG] "; break;
    }
}

void NTagEventInfo::PrintMessage(TString msg, unsigned int vType)
{
    if (vType <= fVerbosity) {
        PrintTag(vType);
        if (vType == pERROR) {
            std::cerr << "\033[m" << msg << std::endl;
            exit(1);
        }
        else std::cout << "\033[m" << msg << std::endl;
    }
}

void NTagEventInfo::PrintMessage(const char* msg, unsigned int vType)
{
    if (vType <= fVerbosity) {
        PrintTag(vType);
        if (vType == pERROR) std::cerr << "\033[m" << msg << std::endl;
        else std::cout << "\033[m" << msg << std::endl;
    }
}