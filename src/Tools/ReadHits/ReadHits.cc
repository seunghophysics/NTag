#include "ReadHits.hh"

bool ReadHits::Initialize()
{
    return true;
}

bool ReadHits::Execute()
{
    float tOffset = 0.;
    PMTHit lastHit{0, 0, 0};

    bool coincidenceFound = true;
    TQI* rawTQI = m_data->eventTQI;

    if (!rawTQI->IsEmpty()) {
        coincidenceFound = false;
        PMTHit lastHit = rawTQI->GetLastHit();
    }
    
    for (int iHit = 0; iHit < sktqz_.nqiskz; iHit++) {

        if (!coincidenceFound && sktqz_.qiskz[iHit] == lastHit.q() && sktqz_.icabiz[iHit] == lastHit.i()) {
            tOffset = lastHit.t() - sktqz_.tiskz[iHit];
            coincidenceFound = true;
            Log(Form("Coincidence found: t = %f ns, (offset: %f ns)", lastHit.t(), tOffset), 2, m_verbose);
        }

        PMTHit hit{ /*T*/ sktqz_.tiskz[iHit] + tOffset,
                    /*Q*/ sktqz_.qiskz[iHit],
                    /*I*/ sktqz_.icabiz[iHit]
                  };

        // Use hits that are in-gate and within MAXPM only
        if (sktqz_.ihtiflz[iHit] & (1<<1) && hit.i() <= MAXPM) {

            nTotalHits++;

            //if (fabs(hit.t() - vPMTHitTime[hit.i()]) < rbnDeadTime*1.e3) {
            //    nRemovedHits++;
            //    continue;
            //}

            //vPMTHitTime[hit.i()] = hit.t();

            /*
            if (vSIGT) {
                bool isSignal = false;
                nTotalSigHits = vSIGT->size();
                // Look for matching hits between sig+bkg TQ and sig TQ
                for (int iSigHit = 0; iSigHit < nTotalSigHits; iSigHit++) {
                    // If both hit time and PMT ID match, then the current hit iHit is from signal
                    if (fabs(hit.T - vSIGT->at(iSigHit)) < 1e-3
                        && hit.I == vSIGI->at(iSigHit)) {
                        isSignal = true;
                    }
                }
                if (isSignal) { hit.S = true; nFoundSigHits++; }
            }
            */
            
            rawTQI->AppendHit(hit);
        }
    }

    nqiskz = rawTQI->GetNHits();
    
    m_data->CStore.Set("NTotalHits", nTotalHits);
    m_data->CStore.Set("NRemovedHits", nRemovedHits);
    m_data->CStore.Set("nqiskz", nqiskz);
    
    //rawTQI->DumpAllHits();

    return true;
}

bool ReadHits::Finalize()
{
    return true;
}