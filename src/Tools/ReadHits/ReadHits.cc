#include <skheadC.h>
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
        Log("The SK common block sktqz is empty.", pWARNING);
        //Log("Aborting the program...", pERROR);
        safeToExecute = false;
    }
    return safeToExecute;
}

bool ReadHits::Execute()
{
    SetTriggerType();
    
    float tOffset = 0.;
    PMTHit lastHit(0, 0, 0);
    bool coincidenceFound = true;
    PMTHitCluster appendedHits;
    
    PMTHitCluster* eventHits = &(sharedData->eventPMTHits);

    // if not AFT
    if (currentEvTrigType != tAFT) {
    
        // if previous event was unprocessed SHE
        if (prevEvTrigType == tSHE && !isPrevEvProcessed) {
            // deal with the previous event hits first and come back!
            //prevEvTrigType = currentEvTrigType;
            isPrevEvProcessed = true;
            Log("Process previous SHE first and return to the current event later...");
            throw ePROCESSANDRETURN;
        }
        // else, clear the previous event hits 
        // and start getting hits from the current event
        else {  
            Log("Clear event hits");
            eventHits->Clear();
        }
    }
    // if AFT, prepare to append with timing offset
    else {
        coincidenceFound = false;
        lastHit = eventHits->GetLastHit();
    }

    int iHit = 0;
    if (!coincidenceFound) {
        for (iHit = 0; iHit < sktqz_.nqiskz; iHit++) {
            // timing offset search for AFT
            if (sktqz_.qiskz[iHit] == lastHit.q() && sktqz_.icabiz[iHit] == lastHit.i()) {
                tOffset = lastHit.t() - sktqz_.tiskz[iHit];
                coincidenceFound = true;
                Log(Form("coinciding hit: t: %f ns q: %f i: %d", sktqz_.tiskz[iHit], sktqz_.qiskz[iHit], sktqz_.icabiz[iHit]));
                Log(Form("Coincidence found: t = %f ns, (offset: %f ns)", lastHit.t(), tOffset));
                break;
            }
        }
    }
    
    Log("Before appending hits");
    //eventHits->DumpAllElements();

    Log("Appending hits");
    for (iHit = iHit; iHit < sktqz_.nqiskz; iHit++) {

        PMTHit hit{ /*T*/ sktqz_.tiskz[iHit] + tOffset,
                    /*Q*/ sktqz_.qiskz[iHit],
                    /*I*/ sktqz_.icabiz[iHit]
                  };

        // Use in-gate hits only
        if (sktqz_.ihtiflz[iHit] & (1<<1)) {
            //hit.Dump();
            eventHits->Append(hit);
        }
    }
    std::cout << "\n";
    Log("Finalized hits");
    
    eventHits->Sort();
    //eventHits->DumpAllElements();
    
    if (currentEvTrigType == tSHE) {
        Log("Finished saving hits for SHE. Wait for AFT...");
        prevEvTrigType = currentEvTrigType;
        isPrevEvProcessed = false;
        throw eSKIPEVENT;
    }

    prevEvTrigType = currentEvTrigType;
    return true;
}

bool ReadHits::Finalize()
{
    return true;
}

void ReadHits::SetTriggerType()
{
    // SHE
    if (skhead_.idtgsk & 1<<28)
        currentEvTrigType = tSHE;
    // AFT
    else if (skhead_.idtgsk & 1<<29)
        currentEvTrigType = tAFT;
    // Else
    else
        currentEvTrigType = tELSE;
    
    sharedData->eventVariables.Set("trigger_type", currentEvTrigType);
    Log(Form("Previous trigger type: %d Current trigger type: %d", prevEvTrigType, currentEvTrigType));
}