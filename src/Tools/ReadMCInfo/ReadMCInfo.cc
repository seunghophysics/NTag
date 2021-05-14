#include <cmath>

#include <skroot.h>
#undef MAXHWSK
#undef MAXPM
#undef MAXPMA
#include "skvectC.h"
#include "geotnkC.h"
#include "skheadC.h"

#include "apscndryC.h"

#include "SKLibs.hh"

#include "ReadMCInfo.hh"

bool ReadMCInfo::Initialize()
{
    TString inFilePath;
    sharedData->ntagInfo.Get("in_file_path", inFilePath);
    inputIsSKROOT = inFilePath.EndsWith(".root");
    return true;
}

bool ReadMCInfo::CheckSafety()
{
    if (skhead_.nrunsk == 999999) {
        safeToExecute = true;
    }
    else
        safeToExecute = false;
    return safeToExecute;
}

bool ReadMCInfo::Execute()
{
    sharedData->eventPrimaries.Clear();
    sharedData->eventSecondaries.Clear();
    sharedData->eventTrueCaptures.Clear();

    float trgOffset;
    trginfo_(&trgOffset);
    sharedData->eventVariables.Set("TrgOffset", trgOffset);

    // Primaries
    skgetv_();

    for (int iVec = 0; iVec < skvect_.nvect; iVec++) {
        Particle primary(skvect_.ip[iVec], trgOffset, TVector3(skvect_.pos), TVector3(skvect_.pin[iVec]));
        sharedData->eventPrimaries.Append(primary);
    }

    // NEUT

    // Read secondary bank
    if (inputIsSKROOT) {
        int lun = 10;

        TreeManager* mgr  = skroot_get_mgr(&lun);
        SecondaryInfo* SECONDARY = mgr->GetSECONDARY();
        mgr->GetEntry();

        secndprt_.nscndprt = SECONDARY->nscndprt;

        std::copy(std::begin(SECONDARY->iprtscnd), std::end(SECONDARY->iprtscnd), std::begin(secndprt_.iprtscnd));
        std::copy(std::begin(SECONDARY->iprntprt), std::end(SECONDARY->iprntprt), std::begin(secndprt_.iprntprt));
        std::copy(std::begin(SECONDARY->lmecscnd), std::end(SECONDARY->lmecscnd), std::begin(secndprt_.lmecscnd));
        std::copy(std::begin(SECONDARY->tscnd), std::end(SECONDARY->tscnd), std::begin(secndprt_.tscnd));

        std::copy(&SECONDARY->vtxscnd[0][0], &SECONDARY->vtxscnd[0][0] + 3*SECMAXRNG, &secndprt_.vtxscnd[0][0]);
        std::copy(&SECONDARY->pscnd[0][0], &SECONDARY->pscnd[0][0] + 3*SECMAXRNG, &secndprt_.pscnd[0][0]);
    }
    else {
        apflscndprt_();
    }

    int nAllSec = secndprt_.nscndprt;

    // Loop over all secondaries in secondary common block
    for (int iSec = 0; iSec < nAllSec; iSec++) {

        Particle secondary(secndprt_.iprtscnd[iSec],
                           secndprt_.tscnd[iSec] + trgOffset,
                           TVector3(secndprt_.vtxscnd[iSec]),
                           TVector3(secndprt_.pscnd[iSec]),
                           secndprt_.lmecscnd[iSec]);

        // neutrons
        if (secondary.PID() == 2112)
            sharedData->eventSecondaries.Append(secondary);

        // deuteron, gamma, electrons
        else if (secondary.PID() == 100045 ||
                 secondary.PID() == 22 ||
                 // electrons over Cherenkov threshold momentum, from interaction other than multiple scattering
                 (fabs(secondary.PID()) == 11 && secondary.Momentum().Mag() > 0.579 && secondary.IntID() != 2)) {

            int inPMT; inpmt_(secndprt_.vtxscnd[iSec], inPMT);

            // Check if the capture is within ID volume
            if (secondary.Vertex().Perp() < RINTK && fabs(secondary.Vertex().z()) < ZPINTK && !inPMT) {

                // Save secondary (deuteron, gamma, electrons)
                sharedData->eventSecondaries.Append(secondary);

                bool isNewCapture = true;

                // particle produced by n-capture
                if (secondary.IntID() == 18) {

                    // Check saved capture stack
                    auto nCaptures = sharedData->eventTrueCaptures.GetSize();
                    for (int iCapture = 0; iCapture < nCaptures; iCapture++) {
                        TrueCapture* capture = &(sharedData->eventTrueCaptures.At(iCapture));
                        if (fabs((double)(secondary.Time()-capture->Time()))<1.e-7) {
                            isNewCapture = false;
                            if (secondary.PID() == 22) {
                                capture->Append(secondary);
                            }
                        }
                    }

                    if (isNewCapture) {
                        TrueCapture capture;
                        if (secondary.PID() == 22 && secondary.Momentum().Mag()>0) 
                            capture.Append(secondary);
                        sharedData->eventTrueCaptures.Append(capture);
                    }
                }
            }
        }
    }
    Log("True capture information:");
    sharedData->eventTrueCaptures.Sort();
    sharedData->eventTrueCaptures.DumpAllElements();

    return true;
}

bool ReadMCInfo::Finalize()
{
    return true;
}