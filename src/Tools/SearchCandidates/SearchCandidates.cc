#include "Candidate.hh"

#include "SearchCandidates.hh"

bool SearchCandidates::Initialize()
{
    // read ntag options
    if (!sharedData->ntagInfo.Get("T0TH", T0TH)) T0TH = 2000;
    if (!sharedData->ntagInfo.Get("T0MX", T0MX)) T0MX = 535000;
    if (!sharedData->ntagInfo.Get("TWIDTH", TWIDTH)) TWIDTH = 14;
    if (!sharedData->ntagInfo.Get("TMINPEAKSEP", TMINPEAKSEP)) TMINPEAKSEP = 14;
    if (!sharedData->ntagInfo.Get("NHITSTH", NHITSTH)) NHITSTH = 7;
    if (!sharedData->ntagInfo.Get("NHITSMX", NHITSMX)) NHITSMX = 70;
    if (!sharedData->ntagInfo.Get("N200TH", N200TH)) N200TH = 0;
    if (!sharedData->ntagInfo.Get("N200MX", N200MX)) N200MX = 200;

    return true;
}

bool SearchCandidates::CheckSafety()
{
    // eventPMTHits and prompt vertex must be filled
    if (!sharedData->eventPMTHits.IsEmpty()
        && sharedData->eventVariables.Get("prompt_vertex", promptVertex)) {
        safeToExecute = true;
    }
    else {
        Log("EventPMTHits is empty, or prompt vertex is not set.", pWARNING);
        safeToExecute = false;
    }
    return safeToExecute;
}

bool SearchCandidates::Execute()
{
    sharedData->eventCandidates.Clear();
    
    //Log("Dumping hits");
    //sharedData->eventPMTHits.DumpAllElements();

    int   iHitPrevious    = 0;
    int   NHitsNew        = 0;
    int   NHitsPrevious   = 0;
    int   N200Previous    = 0;
    float t0Previous      = -1e6;

    PMTHitCluster* eventHits = &(sharedData->eventPMTHits);
    unsigned long nEventHits = eventHits->GetSize();

    // Loop over the saved TQ hit array from current event
    for (int iHit = 0; iHit < nEventHits; iHit++) {

        PMTHitCluster hitsInTWIDTH = eventHits->Slice(iHit, TWIDTH);

        // If (ToF-subtracted) hit comes earlier than T0TH or later than T0MX, skip:
        float firstHitTime = hitsInTWIDTH[0].t();
        if (firstHitTime < T0TH || firstHitTime > T0MX) continue;

        // Calculate NHitsNew:
        // number of hits within TWIDTH (ns) from the i-th hit
        int NHits_iHit = hitsInTWIDTH.GetSize();

        // Pass only if NHITSTH <= NHits_iHit <= NHITSMX:
        if (NHits_iHit < NHITSTH || NHits_iHit > NHITSMX) continue;

        // We've found a new peak.
        NHitsNew = NHits_iHit;
        float t0New = firstHitTime;

        // Calculate N200
        PMTHitCluster hitsIn200ns = eventHits->Slice(iHit, TWIDTH/2.-100, TWIDTH/2.+100);
        int N200New = hitsIn200ns.GetSize();

        // If peak t0 diff = t0New - t0Previous > TMINPEAKSEP, save the previous peak.
        // Also check if N200Previous is below N200 cut and if t0Previous is over t0 threshold
        if (t0New - t0Previous > TMINPEAKSEP) {
            if (N200Previous < N200MX && t0Previous > T0TH) {
                Candidate candidate(iHitPrevious);
                sharedData->eventCandidates.Append(candidate);
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
    if (NHitsPrevious >= NHITSTH) {
        Candidate candidate(iHitPrevious);
        sharedData->eventCandidates.Append(candidate);
    }

    return true;
}

bool SearchCandidates::Finalize()
{
    return true;
}