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
N10TH(5), N10MX(50), N200MX(140), T0TH(2.), distanceCut(4000.), tMatchWindow(40.),
fVerbosity(vDefault), bData(false)
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
    PrintMessage(Form("RUN %d SUBRUN %d EVENT %d", nrun, nsub, nev), vDebug);
    
    // Get number of OD hits
    odpc_2nd_s_(&nhitac);
    PrintMessage(Form("Number of OD hits: %d", nhitac), vDebug);

    trginfo_(&trgofst);
    qismsk = skq_.qismsk;
    PrintMessage(Form("Trigger offset: %f", trgofst), vDebug);
    PrintMessage(Form("Total charge in ID: %f", qismsk), vDebug);
}

void NTagEventInfo::SetAPFitInfo()
{
    // Get apcommul bank
    int bank = 0;
    aprstbnk_(&bank);

    // Get APFit vertex
    vx = apcommul_.appos[0];
    vy = apcommul_.appos[1];
    vz = apcommul_.appos[2];
    
    float tmp_v[3] = {vx, vy, vz};
    towall = wallsk_(tmp_v);
    
    PrintMessage(Form("APFit vertex: %f, %f, %f", vx, vy, vz), vDebug);
    PrintMessage(Form("d_wall: %f", towall), vDebug);

    // E_vis
    evis = apcomene_.apevis;
    PrintMessage(Form("e_vis: %f", evis), vDebug);

    // AP ring information
    nring = apcommul_.apnring;
    for(int iRing = 0; iRing < nring; iRing++){
        apip[iRing] = apcommul_.apip[iRing];          // PID
        apamom[iRing] = apcommul_.apamom[iRing];      // Reconstructed momentum
        amome[iRing] = appatsp2_.apmsamom[iRing][1];  // e-like momentum
        amomm[iRing] = appatsp2_.apmsamom[iRing][2];  // mu-like momentum
    }
    PrintMessage(Form("APFit number of rings: %d", nring), vDebug);
    
    // mu-e check
    nmue = apmue_.apnmue; ndcy = 0;
    for(int iMuE = 0; iMuE < nmue; iMuE++){
        if(iMuE == 10) break;
        if(apmue_.apmuetype[iMuE] == 1 || apmue_.apmuetype[iMuE] == 4) ndcy++;
    }
}

void NTagEventInfo::SetToFSubtractedTQ()
{
    nqiskz = sktqz_.nqiskz;
    int sortedIndex[nqiskz], pmtID;
    
    PrintMessage(Form("NQISKZ: %d", nqiskz), vDebug);
    
    // Subtract TOF from PMT hit time
    float apfitVertex[3] = {vx, vy, vz};
    SubtractToF(unsortedT_ToF, sktqz_.tiskz, sktqz_.icabiz, nqiskz, apfitVertex);

    // Sort: first hit first
    TMath::Sort(nqiskz, unsortedT_ToF, sortedIndex, false);

    // Save hit info, sorted in (T - ToF)
    for (int iHit = 0; iHit < nqiskz; iHit++){
        sortedPMTID[iHit] = sktqz_.icabiz[ sortedIndex[iHit] ];
        sortedT_ToF[iHit] = unsortedT_ToF[ sortedIndex[iHit] ];
        sortedQ[iHit] = sktqz_.qiskz[ sortedIndex[iHit] ];
        //PrintMessage(Form("iHit: %d    sortedT: %f", iHit, sortedT_ToF[iHit]), vDebug);
    }
}

void NTagEventInfo::SetMCInfo()
{
    // Read SKVECT (primaries)
    skgetv_();
    nvect = skvect_.nvect;                  // number of primaries
    pos[0] = skvect_.pos[0];                // initial vertex of primaries
    pos[1] = skvect_.pos[1];
    pos[2] = skvect_.pos[2];

    for(int i = 0; i < nvect; i++){
        ip[i] = skvect_.ip[i];              // PID of primaries
        pin[i][0] = skvect_.pin[i][0];      // momentum vector of primaries
        pin[i][1] = skvect_.pin[i][1];
        pin[i][2] = skvect_.pin[i][2];
        pabs[i] = skvect_.pabs[i];          // momentum of primaries
    }

    // Read neutrino interaction vector
    float posnu[3];
    nerdnebk_(posnu);

    modene = nework_.modene;                // neutrino interaction mode
    numne = nework_.numne;                  // number of particles in vector
    nN = 0;                                 // number of neutrons
    pnu = Norm(nework_.pne[0]);

    for(int i = 0; i < numne; i++){
        ipne[i] = nework_.ipne[i];          // check particle code
        if(ipne[i] == 2112 && i >= 3) nN++; // count neutrons
    }
    PrintMessage(Form("Number of neutrons in primary stack: %d", nN), vDebug);

    // initialize number of n captures
    nCT = 0;
    nscnd = 0;

    // ?
    float ZBLST = 5.30;
    float dr = RINTK - ZBLST;
    float dz = 0.5 * HIINTK - ZBLST;

    // Read secondary bank
    apflscndprt_();
    int nSecNeutron = 0;
    nscndprt = secndprt_.nscndprt;
    
    for(int iSec = 0; iSec < secndprt_.nscndprt; iSec++){

        iprtscnd[nscnd] = secndprt_.iprtscnd[iSec];              // PID of secondaries

        // Save secondary info if PID is either deuteron, gamma, or neutron
        // (no capture electrons?)
        if(iprtscnd[nscnd] == 100045 || iprtscnd[nscnd] == 22|| iprtscnd[nscnd] == 2112){
            lmecscnd[nscnd] = secndprt_.lmecscnd[iSec];          // creation process
            iprntprt[nscnd] = secndprt_.iprntprt[iSec];          // parent PID
            vtxscnd[nscnd][0] = secndprt_.vtxscnd[iSec][0];      // creation vertex
            vtxscnd[nscnd][1] = secndprt_.vtxscnd[iSec][1];
            vtxscnd[nscnd][2] = secndprt_.vtxscnd[iSec][2];
            wallscnd[nscnd] = wallsk_(vtxscnd[nscnd]);           // distance from wall to creation vertex
            pscnd[nscnd][0] = secndprt_.pscnd[iSec][0];          // momentum vector
            pscnd[nscnd][1] = secndprt_.pscnd[iSec][1];
            pscnd[nscnd][2] = secndprt_.pscnd[iSec][2];
            pabsscnd[nscnd] = Norm(pscnd[nscnd]);                // momentum
            tscnd[nscnd] = secndprt_.tscnd[iSec];                // time created
            capId[nscnd] = -1;                                   // initialize capture index
                                                                 // (-1: not from n capture)
            
            float vtxR2 = vtxscnd[nscnd][0]*vtxscnd[nscnd][0] + vtxscnd[nscnd][1]*vtxscnd[nscnd][1]; 
            
            int inPMT;
            inpmt_(vtxscnd[nscnd], inPMT);

            // Save all neutrons
            if(iprtscnd[nscnd] == 2112){
                nSecNeutron++;
                PrintMessage(Form("Secondary neutron (#%d): [t = %f ns] [p = %f MeV/c]",
                             nSecNeutron, tscnd[nscnd]*1e-3, pabsscnd[iSec]), vDebug);
                nscnd += 1;
            }

            // Check if the capture is within ID volume
            else if(vtxR2 < dr*dr && fabs(vtxscnd[nscnd][2]) < dz && inPMT == 0){
            
                // particle produced by n-capture
                if(lmecscnd[nscnd] == 18){
                
                    bool isNewCapture = true;
                    
                    for(int iCheckedCT = 0; iCheckedCT < nCT; iCheckedCT++){
                    
                        // If this capture is already saved:
                        if(fabs((double)(tscnd[nscnd] - captureTime[iCheckedCT])) < 1.e-7) {
                            
                            isNewCapture = false;
                            
                            // Add capture product gammas to the pre-existing stack
                            if(iprtscnd[nscnd] == 22){
                                PrintMessage(Form("Gamma from already saved capture... capId %d", iCheckedCT), vDebug);
                                nGam[iCheckedCT] += 1;
                                totGamEn[iCheckedCT] += pabsscnd[nscnd];
                                capId[nscnd] = iCheckedCT;
                            }
                        }
                    }
                  
                    // If this capture product is new,
                    // save it as a new element in each array
                    if(isNewCapture){
                        captureTime[nCT] = tscnd[nscnd];
                        capPos[nCT][0] = vtxscnd[nscnd][0];
                        capPos[nCT][1] = vtxscnd[nscnd][1];
                        capPos[nCT][2] = vtxscnd[nscnd][2];
                        
                        // Add capture product gamma to the new elements
                        if(iprtscnd[nscnd] == 22){
                            PrintMessage(Form("Gamma from new capture... capId %d", nCT), vDebug);
                            nGam[nCT] = 1;
                            totGamEn[nCT] = pabsscnd[nscnd];
                            capId[nscnd] = nCT;
                        }
                        
                        else { nGam[nCT] = 0; totGamEn[nCT] = 0.; }
                        // Increment number of neutron captures
                        nCT += 1;
                    }
                }
            // Save deuterons and gammas within ID volume
            nscnd += 1;
            }
        }
    }

    for(int i = 0; i < nCT; i++){
        PrintMessage(Form("CaptureID %d: [t: %f us] [Gamma E: %f MeV]",
                          i, captureTime[i]*1e-3, totGamEn[i]), vDebug);
    }
    PrintMessage(Form("Number of secondary neutrons saved in bank: %d", nSecNeutron), vDebug);
    PrintMessage(Form("Number of captures: %d", nCT), vDebug);
}

void NTagEventInfo::SearchCaptureCandidates()
{
    bool 	isLastPeak
    float	t0Previous = 0.;
    int    	N10i = 0;
    
    // Loop over the saved TQ hit array from current event
    for(int iHit = 0; iHit < nqiskz; iHit++){

        // Save time of first hit
        if(firsthit == 0.) firsthit = sortedT_ToF[iHit];
        
        // Calculate N10i:
        // number of hits in 10 ns window from the i-th hit
        N10i = GetNhitsFromStartIndex(sortedT_ToF, nqiskz, iHit, 10.);

        // If N10TH <= N10i <= N10MX:
        if((N10i < N10TH) || (N10i > N10MX)) continue;
        // We've found a new peak.
        
        // t0 of new peak
        float t0New = sortedT_ToF[iHit];
        
        // Save maximum N200 and its t0
        float tmpN200 = GetNhitsFromCenterTime(t0New + 5., 200.);
        if(t0New > 2.e4 && tmpN200 > N200M){
            N200M = tmpN200;
            T200M = t0New;
          }

        // Skip the first peak in event
        if(isFirstPeak){
            isFirstPeak = false;
            t0Previous = t0New;
            continue;
        }

        // If peak t0 diff = t0_new - t0_previous > 20 ns, save the previous peak.
        // Also check if N200 is below N200 cut and if t0 is over t0 threshold
        if(t0New - t0Previous > 50. && tmpN200 < N200MX && t0New*1.e-3 > T0TH){

            // Set t0Previous for the next peak
            t0Previous = t0New;

            // Calculate betas
            auto beta = GetBetaArray(sortedPMTID, iHit, N10i);

            float tEndNew = sortedT_ToF[iHit+N10i-1];
            // Save info to the member variables
            nvx[np]			= vx;
            nvy[np] 		= vy;
            nvz[np] 		= vz;
            N10[np]    		= N10i;
            N10n[np]   		= N10i;
            N200[np]   		= tmpN200;
            sumQ[np]   		= GetQhitsFromStartIndex(iHit, 10.);
            dt[np]     		= (t0New + tEndNew) / 2.;
            dtn[np]    		= (t0New + tEndNew) / 2.;
            tindex[np] 		= iHit;
            spread[np] 		= tEndNew - t0New;
            trmsold[np] 	= GetTRMSFromStartIndex(sortedT_ToF, iHit, 10.);
            beta14_10[np] 	= beta[1] + 4*beta[4];

            // Increment number of neutron candidates
            np++;

            if(np >= MAXNP-1){
                PrintMessage(Form("Run %d Subrun %d Event %d", nrun, nsub, nev), vWarning);
                PrintMessage(Form("Number of neutron candidates reached limit (MAXNP=%d)", MAXNP), vWarning);
                PrintMessage(Form("Skipping remaining neutrons..."), vWarning);
                break;
            }
        }
    }

    // Select hits within 50 ns around each capture candidate
    // to calculate beta and feed BONSAI

    int n50hits, index50[N200MX];
    int n1300hits, index1300[1000];

    int cabiz50[1000], nindex[1000];
    float tiskz50[1000], qiskz50[1000];
    int cabiz1300[1000];
    float tiskz1300[1000], qiskz1300[1000];

    // Loop over all found capture candidates
    for(int iCapture = 0; iCapture < np; iCapture++){
        n50hits = 0;
        n1300hits = 0;

        // Loop over all hits in event
        for(int iHit = 0; iHit < nqiskz; iHit++){

            // Count N50 and save hit indices in sortedT_ToF
            if(fabs(unsortedT_ToF[iHit] - dtn[iCapture]) < 25.) {
                  index50[n50hits] = iHit;
                  n50hits++;
            }

            // Count N1300 and save hit indices in sortedT_ToF
            if(unsortedT_ToF[iHit] > dtn[iCapture] - 520.8 && unsortedT_ToF[iHit] <  dtn[iCapture] + 779.2) {
                if(n1300hits < 1000){
                    index1300[n1300hits] = iHit;
                    n1300hits++;
                }
            }
        }

        for(int iHit50 = 0; iHit50 < n50hits; iHit50++){
            cabiz50[iHit50] = sktqz_.icabiz[ index50[iHit50] ];
            tiskz50[iHit50] = sktqz_.tiskz[ index50[iHit50] ];
            qiskz50[iHit50] = sktqz_.qiskz[ index50[iHit50] ];
        }

        for(int iHit1300 = 0; iHit1300 < n1300hits; iHit1300++){
            cabiz1300[iHit1300] = sktqz_.icabiz[ index1300[iHit1300] ];
            tiskz1300[iHit1300] = sktqz_.tiskz[ index1300[iHit1300] ];
            qiskz1300[iHit1300] = sktqz_.qiskz[ index1300[iHit1300] ];
        }

        // Calculate betas for N50 hits
        auto beta_50 = GetBetaArray(cabiz50, 0, n50hits);

        // Save N50, N1300, and betas for N50 hits
        N50[iCapture]   	= n50hits;
        N1300[iCapture]   	= n1300hits;
        beta1_50[iCapture]  = beta_50[1];
        beta2_50[iCapture]  = beta_50[2];
        beta3_50[iCapture]  = beta_50[3];
        beta4_50[iCapture]  = beta_50[4];
        beta5_50[iCapture]  = beta_50[5];
        beta14_50[iCapture] = beta_50[1] + 4*beta_50[4];

        // BONSAI fit to each capture candidate
        float tmptbsenergy, tmptbsvx, tmptbsvy, tmptbsvz, tmptbsvt, tmptbsgood, tmptbsdirks, tmptbspatlik, tmptbsovaq;
        float time0 = dtn[iCapture];

        bonsai_fit_(&time0, tiskz1300, qiskz1300, cabiz1300, &n1300hits, &tmptbsenergy, &tmptbsvx, &tmptbsvy, &tmptbsvz,
        &tmptbsvt, &tmptbsgood, &tmptbsdirks, &tmptbspatlik, &tmptbsovaq);

        float tbsvertex[3] = {tmptbsvx, };
        tbsvertex[0] = tmptbsvx; tbsvertex[1] = tmptbsvy; tbsvertex[2] = tmptbsvz;

        // Save BONSAI fit results
        tbsenergy[iCapture] = tmptbsenergy;
        tbsvx[iCapture] 	= tmptbsvx;
        tbsvy[iCapture] 	= tmptbsvy;
        tbsvz[iCapture] 	= tmptbsvz;
        tbsvt[iCapture] 	= tmptbsvt;
        tbswall[iCapture] 	= wallsk_(tbsvertex);
        tbsgood[iCapture] 	= tmptbsgood;
        tbsdirks[iCapture] 	= tmptbsdirks;
        tbspatlik[iCapture] = tmptbspatlik;
        tbsovaq[iCapture] 	= tmptbsovaq;

        float nv[3];	// vertex to fit by minimizing tRMS
        float minTRMS = MinimizeTRMS(tiskz50, cabiz50, n50hits, nv);

        nvx[iCapture]       = nv[0];
        nvy[iCapture]       = nv[1];
        nvz[iCapture]       = nv[2];
        nwall[iCapture]     = wallsk_(nv);
        trms50[iCapture]    = minTRMS;

        SubtractToF(tiskz50, tiskz50, cabiz50, n50hits, nv, true);

        int N10in, tmpN10n = 0;
        float t0n = 0.;

        // Search for a new best N10 (N10n) from these new ToF corrected hits
        int n10index = 0;
        for(int iHit50 = 0; iHit50 < n50hits; iHit50++){
            N10in = GetNhitsFromStartIndex(tiskz50, n50hits, iHit50, 10.);
            if(N10in > tmpN10n){
                tmpN10n = N10in; n10index = iHit50;
                t0n = (tiskz50[iHit50] + tiskz50[iHit50+tmpN10n-1]) / 2.;
            }
        }

        trms[iCapture] = GetTRMSFromStartIndex(tiskz50, n10index, 10.);
        N10n[iCapture] = tmpN10n;
        dtn[iCapture]  = t0n;
    }

    if(!bData){
        SetTrueCaptureInfo();
    }
}

void NTagEventInfo::SetTrueCaptureInfo()
{
    int maxTrueCapture = np;
    int checkedTrueCaptureTime[maxTrueCapture];
    int nCheckedCaptures = 0;

    for(int iCapture = 0; iCapture < np; iCapture++){

        // Check if a candidate is a true neutron capture (MC only)
        realneutron[iCapture] = IsTrueCapture(iCapture);

        // if a candidate is actually a true neutron capture
        if(realneutron[iCapture] == 1){

            doubleCount[iCapture] = 0;
            //time diff between true and reconstructed capture time
            timeRes[iCapture] = ReconCaptureTime(iCapture) - TrueCaptureTime(iCapture);

            bool newCaptureFound = true;

            for(int iCheck = 0; iCheck < nCheckedCaptures; iCheck++){
                if(fabs(TrueCaptureTime(iCapture) - checkedTrueCaptureTime[iCheck]) < 1.e-3){
                    newCaptureFound = false; break;
                }
            }

            if(newCaptureFound){
                checkedTrueCaptureTime[nCheckedCaptures] = TrueCaptureTime(iCapture);
                nCheckedCaptures++;
            }
            // Check whether we should save two candidates a single true capture
            else {
                if (fabs(timeRes[iCapture]) < fabs(timeRes[iCapture-1])){
                    doubleCount[iCapture-1] = 1;realneutron[iCapture-1] = 0;
                }
                else{
                    doubleCount[iCapture] = 1;realneutron[iCapture] = 0;
                }
            }
            auto tmpTruthV = TrueCaptureVertex(iCapture);
            truth_vx[iCapture] = tmpTruthV[0];
            truth_vy[iCapture] = tmpTruthV[1];
            truth_vz[iCapture] = tmpTruthV[2];

            // Check whether capture is on Gd or H
            nGd[iCapture] = IsTrueGdCapture(iCapture);
        }
    }
}

void NTagEventInfo::GetTMVAoutput()
{
    for(int iCapture = 0; iCapture < np; iCapture++){

        // Check conditions for non-dummy TMVA output:
        // * Number of OD hits < 16
        // * N10 >= 7
        // * 0 < Reconstructed capture time < 200 us
        if(!(nhitac < 16)){
            TMVAoutput[iCapture] = -9999; continue;
        }
        if(!(N10[iCapture] >= 7 && 0 < dt[iCapture] && dt[iCapture] < 2.e5)){
            TMVAoutput[iCapture] = -9999; continue;
        }

        mva_N10 		= N10[iCapture];
        mva_N200 		= N200[iCapture];
        mva_N50 		= N50[iCapture];
        mva_dt 			= dt[iCapture];
        mva_sumQ 		= sumQ[iCapture];
        mva_spread 		= spread[iCapture];
        mva_trmsold 	= trmsold[iCapture];
        mva_beta1_50 	= beta1_50[iCapture];
        mva_beta2_50 	= beta2_50[iCapture];
        mva_beta3_50 	= beta3_50[iCapture];
        mva_beta4_50 	= beta4_50[iCapture];
        mva_beta5_50 	= beta5_50[iCapture];
        mva_tbsenergy 	= tbsenergy[iCapture];
        mva_tbswall 	= tbswall[iCapture];
        mva_tbsgood 	= tbsgood[iCapture];
        mva_tbsdirks 	= tbsdirks[iCapture];
        mva_tbspatlik 	= tbspatlik[iCapture];
        mva_tbsovaq 	= tbsovaq[iCapture];
        mva_nwall 		= nwall[iCapture];
        mva_trms50 		= trms50[iCapture];

        mva_AP_BONSAI 	= Norm(vx - tbsvx[iCapture], 
                               vy - tbsvy[iCapture], 
                               vz - tbsvz[iCapture]);
        mva_AP_Nfit 	= Norm(vx - nvx[iCapture], 
                               vy - nvy[iCapture], 
                               vz - nvz[iCapture]);
        mva_Nfit_BONSAI = Norm(nvx[iCapture] - tbsvx[iCapture],
                               nvy[iCapture] - tbsvy[iCapture],
                               nvz[iCapture] - tbsvz[iCapture]);

        TMVAoutput[iCapture] = reader->EvaluateMVA("BDT method");
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

    for(int i = 0; i < 3; i++)
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

    if(nscndprt >= SECMAXRNG) return -1;
    for(int iCapture = 0; iCapture < nCT; iCapture++){
        if(fabs(captureTime[iCapture] - tRecon) < tMatchWindow)
            return captureTime[iCapture];
    }
    if(!IsTrueCapture(candidateID))
        PrintMessage("A false neutron signal is passsed to TrueCaptureTime!", vError);

    return 0.;
}

std::array<float, 3> NTagEventInfo::TrueCaptureVertex(int candidateID)
{
    std::array<float, 3> trueCaptureVertex = {0., 0., 0.};
    float tRecon = ReconCaptureTime(candidateID);

    for(int iCapture = 0; iCapture < nCT; iCapture++){
        if(fabs(captureTime[iCapture] - tRecon) < tMatchWindow){
            for(int k = 0; k < 3; k++)
                trueCaptureVertex[k] = capPos[iCapture][k];
        }
    }
    if(!IsTrueCapture(candidateID))
        PrintMessage("A false neutron signal is passsed to TrueCaptureVertex!", vError);

    return trueCaptureVertex;
}

float NTagEventInfo::SubtractToF(float t_ToF[], float T[], int PMTID[], int nHits, float vertex[3], bool doSort)
{
    // Subtract TOF from PMT hit time
    for(int iHit = 0; iHit < nHits; iHit++){
        int pmtID = PMTID[iHit] - 1;

        float vecFromVertexToPMT[3];

        for(int i = 0; i < 3; i++)
            vecFromVertexToPMT[i] = vertex[i] - xyz[pmtID][i];

        float ToF = GetDistance(xyz[pmtID], vertex) / C_WATER;

        t_ToF[iHit] = T[iHit] - ToF;
    }

    float tMean, tVar;
    tMean = tVar = 0.;

    for(int iHit = 0; iHit < nHits; iHit++)
        tMean += t_ToF[iHit]/nHits;
    for(int iHit = 0; iHit < nHits; iHit++)
        tVar += (t_ToF[iHit]-tMean)*(t_ToF[iHit]-tMean) / nHits;

    if(doSort){
        int sortedIndex[nHits];
        TMath::Sort(nHits, t_ToF, sortedIndex, false);
        for(int iHit = 0; iHit < nHits; iHit++)
            t_ToF[iHit] = t_ToF[ sortedIndex[iHit] ];
    }

    return sqrt(tVar);
}

float NTagEventInfo::MinimizeTRMS(float T[], int PMTID[], int nHits, float rmsFitVertex[])
{
    float delta;
    bool doSort = true;
    (distanceCut > 200) ? delta = 100 : delta = distanceCut / 2.;
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

    while(delta > 0.5){
    
        for(float x = 0; x < rMax; x++){
            srcVertex[0] = delta * (x - rMax/2.) + vecR[0];
            for(float y = 0; y < rMax; y++){
                srcVertex[1] = delta * (y - rMax/2.) + vecR[1];
                
                if(TMath::Sqrt(srcVertex[0]*srcVertex[0] + srcVertex[1]*srcVertex[1]) > RINTK) continue;
                for(float z = 0; z < zMax; z++){
                    srcVertex[2] = delta * (z - zMax/2.) + vecR[2];
                    if(srcVertex[2] > ZPINTK || srcVertex[2] < -ZPINTK) continue;
                    if(Norm(srcVertex[0] - vecR[0], srcVertex[1] - vecR[1], srcVertex[2] - vecR[2]) > distanceCut) continue;

                    float t_ToF[nHits];
                    tRMS = SubtractToF(t_ToF, T, PMTID, nHits, srcVertex, doSort);

                    if(tRMS < minTRMS){
                        minTRMS = tRMS;
                        tmpVertex = srcVertex;
                        
                        for(int i = 0; i < nHits; i++){
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

std::array<float, 6> NTagEventInfo::GetBetaArray(int PMTID[], int tID, int nHits)
{
    std::array<float, 6> beta = {0., 0., 0., 0., 0., 0};

    float uvx[nHits], uvy[nHits], uvz[nHits];	// direction vector from vertex to each hit PMT

    for(int i = 0; i < nHits; i++){
        float distFromVertexToPMT;
        float vecFromVertexToPMT[3];
        vecFromVertexToPMT[0] = xyz[PMTID[tID+i]-1][0] - vx;
        vecFromVertexToPMT[1] = xyz[PMTID[tID+i]-1][1] - vy;
        vecFromVertexToPMT[2] = xyz[PMTID[tID+i]-1][2] - vz;
        distFromVertexToPMT = Norm(vecFromVertexToPMT);
        uvx[i] = vecFromVertexToPMT[0] / distFromVertexToPMT;
        uvy[i] = vecFromVertexToPMT[1] / distFromVertexToPMT;
        uvz[i] = vecFromVertexToPMT[2] / distFromVertexToPMT;
    }

    for(int i = 0; i < nHits-1; i++){
        for(int j = i+1; j < nHits; j++){
            // cosine angle between two consecutive uv vectors
            float cosTheta = uvx[i]*uvx[j] + uvy[i]*uvy[j] + uvz[i]*uvz[j];
            for(int k = 1; k <= 5; k++)
                beta[k] += GetLegendreP(k, cosTheta);
        }
    }

    for(int k = 1; k <= 5; k++)
        beta[k] = 2.*beta[k] / float(nHits) / float(nHits-1);

    // Return calculated beta array
    return beta;
}

float NTagEventInfo::GetLegendreP(int i, float& x)
{
    float result = 0.;
    
    if(i < 0 || i > 5){
        PrintMessage(Form("Incompatible i (%d) is passed to GetLegendreP.", i), vError);
    }
    switch(i){
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

int NTagEventInfo::GetNhitsFromStartIndex(float T[], int nHits, int startIndex, float tWidth)
{
    int i = startIndex;

    while(1){
        i++;
        if((i > nHits-1) || (TMath::Abs((T[i] - T[startIndex])) > tWidth))
            break;
    }
    // Return number of hits within the time window
    return TMath::Abs(i - startIndex);
}

float NTagEventInfo::GetQhitsFromStartIndex(int startIndex, float tWidth)
{
    int i = startIndex;
    float sumQ = 0.;
    while(1){
        sumQ += sortedQ[i];
        i++;
        if((i > nqiskz -1) || (TMath::Abs((sortedT_ToF[i] - sortedT_ToF[startIndex])) > tWidth))
            break;
    }
    // Return total hit charge within the time window
    return sumQ;
}

float NTagEventInfo::GetTRMSFromStartIndex(float T[], int startIndex, float tWidth)
{
    int nHits = sizeof(T) / sizeof(T[0]);
    int i = startIndex;
    std::vector<float> tList;

    while(1){
        tList.push_back(T[i]);
        i++;
        if((i > nHits -1) || (TMath::Abs((T[i] - T[startIndex])) > tWidth))
            break;
    }

    float tMean = 0.;
    float tVar = 0;
    int nEntry = tList.size();

    for(const auto& t: tList) tMean += t/nEntry;
    for(const auto& t: tList) tVar += (t-tMean)*(t-tMean) / nEntry;

    return sqrt(tVar);
}

int NTagEventInfo::GetNhitsFromCenterTime(float centerTime, float tWidth)
{
    int nHits = 0;

    for (int i = 0; i < nqiskz; i++){
        if(sortedT_ToF[i] < centerTime - tWidth/2.) continue;
        if(sortedT_ToF[i] > centerTime + tWidth/2.) break;
        nHits++;
    }

    return nHits;
}

int NTagEventInfo::IsTrueCapture(int candidateID)
{
    float tRecon = ReconCaptureTime(candidateID);

    if(nscndprt >= SECMAXRNG) return -1;
    for(int iCapture = 0; iCapture < nCT; iCapture++){
        if(fabs(captureTime[iCapture] - tRecon) < tMatchWindow ) return 1;
    }
    return 0;
}

int NTagEventInfo::IsTrueGdCapture(int candidateID)
{
    float tRecon = ReconCaptureTime(candidateID);

    if(nscndprt >= SECMAXRNG) return -1;
    for(int iCapture = 0; iCapture < nCT; iCapture++){
        if(fabs(captureTime[iCapture] - tRecon) < tMatchWindow ){
            if(totGamEn[iCapture] > 6.) return 1;
            else return 0;
        }
    }

    return -9999;
}

void NTagEventInfo::Clear()
{   
    nrun = 0; nsub = 0; nev = 0; trgtype = 0; nhitac = 0; nqiskz = 0;
    np = 0; trgofst = 0; timnsk = 0; qismsk = 0; 
    nring = 0; nmue = 0; ndcy = 0; evis = 0;
    vx = 0; vy = 0; vz = 0;
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

    for(int i = 0; i < 30*MAXNP; i++){
        sortedPMTID[i] = 0;
        sortedT_ToF[i] = 0; sortedQ[i] = 0; unsortedT_ToF[i] = 0;
    }

    for(int i = 0; i < APNMAXRG; i++){
        apip[i] = 0;
        apamom[i] = 0; amome[i] = 0; amomm[i] = 0;
    }

    for(int i = 0; i < MAXNP; i++){
        tindex[i] = 0;
        N10[i] =  0; N10n[i] =  0; N50[i] =  0; N200[i] =  0; N1300[i] = 0;
        sumQ[i] =  0; spread[i] = 0; trms[i] = 0; trmsold[i] = 0; trms50[i] = 0;
        mintrms_3[i] = 0; mintrms_4[i] = 0; mintrms_5[i] = 0; mintrms_6[i] = 0;
        dt[i] = 0; dtn[i] = 0;
        nvx[i] = 0; nvy[i] = 0; nvz[i] = 0; nwall[i] = 0;
        tmpVertex[0][i] = 0; tmpVertex[1][i] = 0; tmpVertex[2][i] = 0;
        doubleCount[i] = 0; goodn[i] = 0;
        tbsenergy[i] = 0; tbsenergy2[i] = 0;
        tbsvx[i] = 0; tbsvy[i] = 0; tbsrcVertex[2][i] = 0; tbsvt[i] = 0;
        tbswall[i] = 0; tbsgood[i] = 0;
        tbspatlik[i] = 0; tbsdirks[i] = 0; tbsovaq[i] = 0;
        bsenergy[i] = 0; bsenergy2[i] = 0;
        bsvertex0[i] = 0; bsvertex1[i] = 0; bsvertex2[i] = 0;
        bsgood[i] = 0;

        beta14_10[i] = 0; beta14_50[i] = 0;
        beta1_50[i] = 0; beta2_50[i] = 0; beta3_50[i] = 0; beta4_50[i] = 0; beta5_50[i] = 0;
        ratio[i] = 0; phirms[i] = 0; thetam[i] = 0; summedWeight[i] = 0; g2d2[i] = 0;
        Nback[i] = 0; Neff[i] = 0; Nc1[i] = 0; NhighQ[i] = 0;
        Nc[i] = 0; Ncluster[i] = 0; Nc8[i] = 0; Ncluster8[i] = 0;
        Nc7[i] = 0; Ncluster7[i] = 0; N12[i] = 0; N20[i] = 0; N300[i] = 0;
        npx[i] = 0; npy[i] = 0; npz[i] = 0;
        ndirx[i] = 0; ndiry[i] = 0; ndirz[i] = 0;

        nGd[i] = 0; realneutron[i] = 0;
        truth_vx[i] = 0; truth_vy[i] = 0; truth_vz[i] = 0; timeRes[i] = 0;

        TMVAoutput[i] = -9999.;

        for(int j = 0; j < 9; j++) Nlow[j][i] = 0;
    }

    for(int i = 0; i < kMaxCT; i++){
        nGam[i] = 0;
        captureTime[i] = 0; totGamEn[i] = 0;
        ipne[i] = 0;

        for(int j = 0; j < 3; j++){
            capPos[i][j] = 0;
        }
    }

    for(int i = 0; i < SECMAXRNG; i++){
        iprtscnd[i] = 0; lmecscnd[i] = 0; iprntprt[i] = 0;
        wallscnd[i] = 0; pabsscnd[i] = 0; tscnd[i] = 0;
        capId[i] = 0;
        ip[i] = 0;
        pabs[i] = 0;

        for(int j = 0; j < 3; j++){
            vtxscnd[i][j] = 0; pscnd[i][j] = 0; pin[i][j] = 0;
        }
    }
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
    switch(vType){
        case vDefault:
            std::cout << "[NTag] "; break;
        case vWarning:
            std::cout << "\033[4;33m" << "[NTag WARNING] "; break;
        case vError:
            std::cerr << "\033[4;31m" << "[Error in NTag] "; break;
        case vDebug:
            std::cout << "\033[0;34m" << "[NTag DEBUG] "; break;
    }
}

void NTagEventInfo::PrintMessage(TString msg, unsigned int vType)
{
    if(vType <= fVerbosity){
        PrintTag(vType);
        if(vType == vError){
            std::cerr << "\033[m" << msg << std::endl;
            exit(1);
        }
        else std::cout << "\033[m" << msg << std::endl;
    }
}

void NTagEventInfo::PrintMessage(const char* msg, unsigned int vType)
{
    if(vType <= fVerbosity){
        PrintTag(vType);
        if(vType == vError) std::cerr << "\033[m" << msg << std::endl;
        else std::cout << "\033[m" << msg << std::endl;
    }
}