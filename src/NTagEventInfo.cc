#include <math.h>
#include <cassert>
#include <iostream>
#include <numeric>

#include <TMath.h>
#include <TRandom.h>

// Size limit of secondary tree/bank
#define MAXNSCNDPRT (4000)

#include <skroot.h>
#undef MAXHWSK
#include <apmringC.h>
#include <apmueC.h>
#include <apmsfitC.h>
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

NTagEventInfo::NTagEventInfo(Verbosity verbose):
TWIDTH(NTagDefault::TWIDTH),
NHITSTH(NTagDefault::NHITSTH), NHITSMX(NTagDefault::NHITSMX),
N200MX(NTagDefault::N200MX),
T0TH(NTagDefault::T0TH), T0MX(NTagDefault::T0MX),
TRBNWIDTH(NTagDefault::TRBNWIDTH),
TMATCHWINDOW(NTagDefault::TMATCHWINDOW),
TMINPEAKSEP(NTagDefault::TMINPEAKSEP),
ODHITMX(NTagDefault::ODHITMX),
VTXSRCRANGE(NTagDefault::VTXSRCRANGE),
MINGRIDWIDTH(NTagDefault::MINGRIDWIDTH),
PVXRES(NTagDefault::PVXRES),
customvx(0.), customvy(0.), customvz(0.),
fVerbosity(verbose),
bData(false), bUseTMVA(true), bSaveTQ(false), bForceMC(false), bUseResidual(true), bUseNeutFit(true)
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

    // Number of OD hits
    odpc_2nd_s_(&nhitac);

    // Read trigger offset
    if (!bData) {
        msg.PrintBlock("Reading trigger information...", pSUBEVENT, pDEFAULT, false);
        trginfo_(&trgOffset);
    }
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
            float dx = 2*RINTK, dy = 2*RINTK, dz = 2*ZPINTK;
            float maxd = 150.;
            while (Norm(dx, dy, dz) > maxd) {
                dx = gRandom->BreitWigner(0, PVXRES);
                dy = gRandom->BreitWigner(0, PVXRES);
                dz = gRandom->BreitWigner(0, PVXRES);
            }
            pvx = skvect_.pos[0] + dx;
            pvy = skvect_.pos[1] + dy;
            pvz = skvect_.pos[2] + dz; break; }
        case mSTMU: {
        	msg.Print("Calculating muon stoping point...", pDEFAULT);
            float stmpos[3], stmdir[3], stmgood, qent, stpoint[3];
			stmfit_(stmpos, stmdir, stmgood, qent);
			apcommul_.apnring = 1; apcommul_.apip[0] = 13;
			for(int i=0; i<3; i++){
				apcommul_.appos[i] = stmpos[i];
				apcommul_.apdir[0][i] = stmdir[i];
				apcommul_.apangcer[0] = 42.;
			}
			int ta = 0, tb = 0, tc = 0, td = 1;
			skheadg_.sk_geometry = 5; geoset_();
			sparisep_(ta,tb,tc,td); //rtot -> amom
			//MS fit
			pffitres_.pffitflag = 1; ta = 3;
			pfdodirfit_(ta);
			for(int i=0; i<3; i++) 
				apcommul_.apdir[0][i] = pffitres_.pfdir[2][0][i];
			apcommul_.apnring = 1; apcommul_.apip[0] = 13;
			ta = 0, tb = 0, tc = 0, td = 1;
			sparisep_(ta,tb,tc,td); //rtot -> amom
			sppang_(apcommul_.apip[0], apcommul_.apamom[0], apcommul_.apangcer[0]);
			appatsp_.approb[0][1] = -100.;
			appatsp_.approb[0][2] = 0.;
			spfinalsep_();
			for(int i=0; i<3; i++) 
				stmdir[i] = apcommul_.apdir[0][i];

        	msg.Print("Stop mu fit finished", pDEFAULT);
        	msg.Print(Form("stmpos : (%lf, %lf, %lf)",stmpos[0],stmpos[1],stmpos[2]), pDEFAULT);
        	msg.Print(Form("stmdir : (%lf, %lf, %lf)",stmdir[0],stmdir[1],stmdir[2]), pDEFAULT);
        	msg.Print(Form("amom : %lf",appatsp2_.apmsamom[0][2]), pDEFAULT);
			//muon range in function of momentum from PDG2020 
			float momenta[] = {0., 339.6, 1301., 2103., 3604., 4604., 5605., 7105., 8105., 9105.,
								10110., 12110., 14110., 17110., 20110.};
			float ranges[] = {0., 103.9, 567.8, 935.3, 1595., 2023., 2443., 3064., 3472., 3877.,
								4279., 5075., 5862., 7030., 8183.};
			int mombin=0;
			float range=0;
			for(int k=1; k<15; k++){
				if(momenta[k] > appatsp2_.apmsamom[0][2]){
					mombin = k;
					break;
				}
				if(k==14){
					mombin = 999;
					range = appatsp2_.apmsamom[0][2]/2.3;
				}
			}
			if(mombin != 999)
				range = ranges[mombin-1]+(appatsp2_.apmsamom[0][2]-momenta[mombin-1])*
							(ranges[mombin]-ranges[mombin-1])/(momenta[mombin]-momenta[mombin-1]);

			for(int k=0; k<200; k++){
				//shorten range if stpoint is out of ID
				for(int i=0;i<3; i++) stpoint[i] = stmpos[i]+stmdir[i]*range;
				if(stpoint[0]*stpoint[0]+stpoint[1]*stpoint[1]<1690.*1690.
									&& stpoint[2]<1810.&&stpoint[2]>-1810.)break;
				range = range*0.93;
			}
        	msg.Print(Form("stpoint : (%lf, %lf, %lf)",stpoint[0],stpoint[1],stpoint[2]), pDEFAULT);
			pvx = stpoint[0];
			pvy = stpoint[1];
			pvz = stpoint[2]; break; }
    }

    float tmp_v[3] = {pvx, pvy, pvz};
    dWall = wallsk_(tmp_v);
}

void NTagEventInfo::SetAPFitInfo()
{
    // E_vis
    evis = apcomene_.apevis;

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
}

void NTagEventInfo::SetTDiff()
{
    // Calculate time difference from previous event to current event [ms]
    if (preRawTrigTime[0] < 0)
         tDiff = 0;
    else tDiff = (skhead_.nt48sk[0] - preRawTrigTime[0]) * std::pow(2, 32)
               + (skhead_.nt48sk[1] - preRawTrigTime[1]) * std::pow(2, 16)
               + (skhead_.nt48sk[2] - preRawTrigTime[2]);
    tDiff *= 20.; tDiff /= 1.e6; // [ms]
    
    for (int i = 0; i < 3; i++) preRawTrigTime[i] = skhead_.nt48sk[i];
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

            nTotalHits++;

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
                nTotalSigHits = vSIGT->size();
                // Look for matching hits between sig+bkg TQ and sig TQ
                for (int iSigHit = 0; iSigHit < nTotalSigHits; iSigHit++) {
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

    nqiskz = static_cast<int>(vTISKZ.size());
}

void NTagEventInfo::SetToFSubtractedTQ()
{
    // Subtract ToF from raw PMT hit time
    if (bUseResidual) {
        float fitVertex[3] = {pvx, pvy, pvz};
        vUnsortedT_ToF = GetToFSubtracted(vTISKZ, vCABIZ, fitVertex, false);
    }
    else
        vUnsortedT_ToF = vTISKZ;

    SortToFSubtractedTQ();
}

void NTagEventInfo::DumpEventVariables()
{
    msg.PrintBlock(Form("Event #%d summary", nProcessedEvents), pSUBEVENT, pDEFAULT, false);

    // Event header
    msg.Print("\033[1;36m* Event header\033[m");
    msg.Print("\033[4mRun       Subrun    Event     Evis (MeV)\033[0m");
    msg.Print("", pDEFAULT, false);
    std::cout << std::left << std::setw(10) << runNo;
    std::cout << std::left << std::setw(10) << subrunNo;
    std::cout << std::left << std::setw(10) << eventNo;
    std::cout << std::left << std::setw(10) << evis;
    std::cout << std::endl;
    msg.Print("");
    msg.Print("\033[4mQISMSK (p.e.)       OD Hits             \033[0m");
    msg.Print("", pDEFAULT, false);
    std::cout << std::left << std::setw(20) << qismsk;
    std::cout << std::left << std::setw(20) << nhitac;
    std::cout << std::endl;
    msg.Print("");

    // Trigger information
    msg.Print("\033[1;36m* Trigger\033[m");
    msg.Print("\033[4mTrgType     TrgTime (ns)   TDiff (ms)   \033[0m");
    msg.Print("", pDEFAULT, false);
    std::cout << std::left << std::setw(12);
    if      (trgType == 1) std::cout << "SHE-only";
    else if (trgType == 2) std::cout << "SHE+AFT";
    else if (trgType == 3) std::cout << "No-SHE";
    else                   std::cout << "MC";
    std::cout << std::left << std::setw(15) << trgOffset;
    std::cout << std::left << std::setw(13) << tDiff;
    std::cout << std::endl;
    msg.Print("");

    // Hit information
    msg.Print("\033[1;36m* Hits\033[m");
    msg.Print("\033[4mTotal hits          Signal hits         \033[0m");
    msg.Print("", pDEFAULT, false);
    std::cout << std::left << std::setw(20) << nTotalHits;
    std::cout << std::left << std::setw(20);
    if (vSIGT) std::cout << vSIGT->size();
    else       std::cout << "-";
    std::cout << std::endl;
    msg.Print("");

    // RBN reduction information
    int nFoundHits = nTotalHits - nRemovedHits;
    msg.Print(Form("\033[1;36m* RBN reduction (Deadtime: %d us)\033[m", (int)TRBNWIDTH));
    msg.Print("\033[4mSurvived hits       Survived signal     \033[0m");
    msg.Print("", pDEFAULT, false);
    std::cout << std::left << std::setw(20)
              << Form("%d (%d%%)", nFoundHits, (int)(100*nFoundHits/(nTotalHits+1.e-3)));
    std::cout << std::left << std::setw(20);
    if (vSIGT) {
        std::cout << Form("%d (%d%%)", nFoundSigHits, (int)(100*nFoundSigHits/(vSIGT->size()+1.e-3)));
    }
    else std::cout << "-";
    std::cout << std::endl;
    msg.Print("");

    // Prompt vertex
    msg.Print("\033[1;36m* Prompt vertex (neutron search vertex)\033[m");
    msg.Print("\033[4mX (cm)    Y (cm)    Z (cm)    dWall (cm)\033[0m");
    msg.Print("", pDEFAULT, false);
    std::cout << std::left << std::setw(10) << pvx;
    std::cout << std::left << std::setw(10) << pvy;
    std::cout << std::left << std::setw(10) << pvz;
    std::cout << std::left << std::setw(10) << dWall;
    std::cout << std::endl;
    msg.Print("");

    // APFit information

    // NEUT information

    if (!bData) {
        // Primary information (MC)
        msg.Print("\033[1;36m* (MC) Primary vectors\033[m");
        msg.Print("\033[4mID  PID   Mom. (MeV/c)  dWall (cm)      \033[0m");
        msg.Print("", pDEFAULT, false);
        for (unsigned int iVec = 0; iVec < vVecPID.size(); iVec++) {
            std::cout << std::left << std::setw(4) << iVec;
            std::cout << std::left << std::setw(6);
            if (vVecPID[iVec] == 13) std::cout << "n";
            else                     std::cout << vVecPID[iVec];
            std::cout << std::left << std::setw(14) << std::setprecision(4) << vVecMom[iVec];
            float vecV[3] = {vVecPX[iVec], vVecPY[iVec], vVecPZ[iVec]};
            std::cout << std::left << std::setw(17) << wallsk_(vecV);
            std::cout << std::setprecision(6) << std::endl;
        }
        msg.Print("");

        if (fVerbosity > pDEFAULT) {
            // Secondary information (MC)
            msg.Print("\033[1;36m* (MC) Secondaries\033[m");
            msg.Print("\033[4mID  PID   IntID   ParentPID Mom. (MeV/c)\033[0m");
            for (unsigned int iSec = 0; iSec < vSecPID.size(); iSec++) {
                msg.Print("", pDEFAULT, false);
                std::cout << std::left << std::setw(4) << iSec;
                std::cout << std::left << std::setw(6) << GetParticleName(vSecPID[iSec]);
                std::cout << std::left << std::setw(8) << GetInteractionName(vSecIntID[iSec]);
                std::cout << std::left << std::setw(10) << GetParticleName(vParentPID[iSec]);
                std::cout << std::left << std::setw(13) << std::setprecision(3) << vSecMom[iSec];
                std::cout << std::setprecision(6) << std::endl;
            }
            msg.Print("");
        }

        // True capture information (MC)
        msg.Print("\033[1;36m* (MC) True captures\033[m");
        msg.Print("\033[4mID  Time (us)  E (MeV)  TravelDist. (cm)\033[0m");
        for (unsigned int iCap = 0; iCap < vTrueCT.size(); iCap++) {
            msg.Print("", pDEFAULT, false);
            std::cout << std::left << std::setw(4) << iCap;
            std::cout << std::left << std::setw(11) << (int)(vTrueCT[iCap]*1.e-3);
            std::cout << std::left << std::setw(9) << std::setprecision(3)
                      << vTotGammaE[iCap];
            std::cout << std::left << std::setw(16) << Norm(pvx - vCapVX[iCap],
                                                            pvy - vCapVY[iCap],
                                                            pvz - vCapVZ[iCap]);
            std::cout << std::setprecision(6) << std::endl;
        }
        msg.Print("");
    }

    // Neutron capture candidate information
    msg.Print("\033[1;36m* Found neutron capture candidates\033[m");
    msg.Print(Form("\033[4mID  T (us)  N    N_n   Type  Out    \033[0m"));
    for (auto& candidate: vCandidates) {
        msg.Print("", pDEFAULT, false);
        std::cout << std::left << std::setw(4) << candidate.candidateID;
        std::cout << std::left << std::setw(8) << (int)(candidate.fVarMap["ReconCT"]*1.e-3);
        std::cout << std::left << std::setw(5) << candidate.iVarMap["NHits"];
        std::cout << std::left << std::setw(6) << candidate.iVarMap["NHits_n"];
        std::cout << std::left << std::setw(6);
        if (bData) std::cout << "-";
        else if (candidate.iVarMap["CaptureType"] == 0) std::cout << "Bkg";
        else if (candidate.iVarMap["CaptureType"] == 1) std::cout << "H";
        else if (candidate.iVarMap["CaptureType"] == 2) std::cout << "Gd";
        std::cout << std::left << std::setw(11);
        if (bUseTMVA) std::cout << std::setprecision(3) << candidate.fVarMap["TMVAOutput"];
        else          std::cout << "-";
        std::cout << std::endl;
    }
}

void NTagEventInfo::SetMCInfo()
{
    // Read SKVECT (primaries)
    msg.PrintBlock("Reading MC vectors...", pSUBEVENT, pDEFAULT, false);
    skgetv_();
    nVec = skvect_.nvect;   // number of primaries
    vecx = skvect_.pos[0];  // initial vertex of primaries
    vecy = skvect_.pos[1];
    vecz = skvect_.pos[2];

    for (int iVec = 0; iVec < nVec; iVec++) {
        vVecPID.push_back( skvect_.ip[iVec]     );  // PID of primaries
        vVecPX. push_back( skvect_.pin[iVec][0] );  // momentum vector of primaries
        vVecPY. push_back( skvect_.pin[iVec][1] );
        vVecPZ. push_back( skvect_.pin[iVec][2] );
        vVecMom.push_back( skvect_.pabs[iVec]   );  // momentum of primaries
    }

    // Read neutrino interaction vector
    msg.PrintBlock("Reading NEUT vectors...", pSUBEVENT, pDEFAULT, false);
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
}

void NTagEventInfo::ReadSecondaries()
{
    apflscndprt_();
}

void NTagEventInfo::SearchCaptureCandidates()
{
    int   iHitPrevious    = 0;
    int   NHitsNew        = 0;
    int   NHitsPrevious   = 0;
    int   N200Previous    = 0;
    float t0Previous      = -1e6;

    // Loop over the saved TQ hit array from current event
    for (int iHit = 0; iHit < nqiskz; iHit++) {

        // the Hit timing w/o TOF is larger than limit, or less smaller than t0
        if (vSortedT_ToF[iHit]*1.e-3 < T0TH || vSortedT_ToF[iHit]*1.e-3 > T0MX) continue;

        // Save time of first hit
        if (firstHitTime_ToF == 0.) firstHitTime_ToF = vSortedT_ToF[iHit];

        // Calculate NHitsNew:
        // number of hits in 10(or so) ns window from the i-th hit
        int NHits_iHit = GetNhitsFromStartIndex(vSortedT_ToF, iHit, TWIDTH);

        // Pass only if NHITSTH <= NHits_iHit <= NHITSMX:
        if ((NHits_iHit < NHITSTH) || (NHits_iHit > NHITSMX)) continue;

        // We've found a new peak.
        NHitsNew = NHits_iHit;
        float t0New = vSortedT_ToF[iHit];

        // Save maximum N200 and its t0
        float N200New = GetNhitsFromCenterTime(vSortedT_ToF, t0New + TWIDTH/2, 200.);
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
            // Reset NHitsPrevious,
            // if peaks are separated enough
            NHitsPrevious = 0;
        }

        // If NHits is not greater than previous, skip
        if ( NHitsNew <= NHitsPrevious ) continue;

        iHitPrevious  = iHit;
        t0Previous    = t0New;
        NHitsPrevious = NHitsNew;
        N200Previous  = N200New;
    }
    // Save the last peak
    if (NHitsPrevious >= NHITSTH)
        SavePeakFromHit(iHitPrevious);
}

void NTagEventInfo::SavePeakFromHit(int hitID)
{
    // Initialize candidate
    vCandidates.push_back(NTagCandidate(vCandidates.size(), this));

    // Containers for hit info
    float tWidth = TWIDTH;
    std::vector<float> resTVec = GetVectorFromStartIndex(vSortedT_ToF, hitID, tWidth);
    int nHits = resTVec.size();

    std::vector<float> rawTVec = SliceVector(vTISKZ, hitID, nHits, reverseIndex.data());
    std::vector<float> pmtQVec = SliceVector(vSortedQ, hitID, nHits);
    std::vector<int>   cabIVec = SliceVector(vSortedPMTID, hitID, nHits);
    std::vector<int>   sigFVec;

    if (!vSortedSigFlag.empty()) sigFVec = SliceVector(vSortedSigFlag, hitID, nHits);

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
    }
}

void NTagEventInfo::InitializeCandidateVariableVectors()
{
    msg.PrintBlock("Initializing feature variables...", pSUBEVENT, pDEBUG, false);
    for (auto const& pair: vCandidates[0].iVarMap) {
        msg.Print(Form("Initializing variable %s...", pair.first.c_str()), pDEBUG);
        iCandidateVarMap[pair.first] = new std::vector<int>();
    }
    for (auto const& pair: vCandidates[0].fVarMap) {
        msg.Print(Form("Initializing variable %s...", pair.first.c_str()), pDEBUG);
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

    nTotalHits = 0; nTotalSigHits = 0; nFoundSigHits = 0; nRemovedHits = 0;

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
}
