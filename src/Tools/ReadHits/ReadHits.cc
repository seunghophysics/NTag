#include <skparmC.h>
#include <sktqC.h>

#include "ReadHits.hh"

bool ReadHits::Initialize()
{
    return true;
}

bool ReadHits::CheckSafety()
{
    // Check if sktqz is filled
    if (sktqz_.nqiskz) {
        safeToExecute = true;
    }
    else {
        Log("The SK common block sktqz is either empty or without any hit.", pWARNING);
        Log("Make sure sktqz is properly filled with skread.", pWARNING);
        Log("Aborting the program...", pERROR);
        safeToExecute = false;
    }
    return safeToExecute;
}

bool ReadHits::Execute()
{
    float tOffset = 0.;
    PMTHit lastHit{0, 0, 0};

    bool coincidenceFound = true;
    PMTHitCluster* eventHits = &(sharedData->eventPMTHits);
    eventHits->Clear();

    if (!eventHits->IsEmpty()) {
        coincidenceFound = false;
        PMTHit lastHit = eventHits->GetLastHit();
    }

    for (int iHit = 0; iHit < sktqz_.nqiskz; iHit++) {

        if (!coincidenceFound && sktqz_.qiskz[iHit] == lastHit.q() && sktqz_.icabiz[iHit] == lastHit.i()) {
            tOffset = lastHit.t() - sktqz_.tiskz[iHit];
            coincidenceFound = true;
            Log(Form("Coincidence found: t = %f ns, (offset: %f ns)", lastHit.t(), tOffset), pDEBUG);
        }

        PMTHit hit{ /*T*/ sktqz_.tiskz[iHit] + tOffset,
                    /*Q*/ sktqz_.qiskz[iHit],
                    /*I*/ sktqz_.icabiz[iHit]
                  };

        // Use hits that are in-gate and within MAXPM only
        if (sktqz_.ihtiflz[iHit] & (1<<1) && hit.i() <= MAXPM) {
            eventHits->Append(hit);
        }
    }

    return true;
}

bool ReadHits::Finalize()
{
    return true;
}