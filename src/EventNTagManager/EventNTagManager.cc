#include "skparmC.h"
#include "sktqC.h"
#include "skheadC.h"
#include "apmringC.h"

#include "SKLibs.hh"
#include "EventNTagManager.hh"
#include "Calculator.hh"

EventNTagManager::EventNTagManager(Verbosity verbose) 
{
    fMsg = Printer("NTagManager", verbose);
    
    fSettings.Initialize("/disk02/usr6/han/phd/utillib/src/EventNTagManager/NTagConfig");
    ApplySettings();
}

EventNTagManager::~EventNTagManager() {}

void EventNTagManager::ReadVariables()
{
    // basic information
    fEventVariables.Set("RunNo", skhead_.nrunsk);
    fEventVariables.Set("SubrunNo", skhead_.nsubsk);
    fEventVariables.Set("EventNo", skhead_.nevsk);
    
    // hit information
    int nhitac; odpc_2nd_s_(&nhitac);
    int trgtype = skhead_.idtgsk & 1<<28 ? tSHE : skhead_.idtgsk & 1<<29 ? tAFT : tELSE;
    fEventVariables.Set("QISMSK", skq_.qismsk);
    fEventVariables.Set("NHITAC", nhitac);
    fEventVariables.Set("TrgType", trgtype);
        
    // reconstructed information
    int bank = 0; aprstbnk_(&bank);
        // evis
        fEventVariables.Set("EVis", apcomene_.apevis);
        // prompt vertex
        //fEventVariables.Set("pvx", apcommul_.appos[0]);
        //fEventVariables.Set("pvy", apcommul_.appos[1]);
        //fEventVariables.Set("pvz", apcommul_.appos[2]);
        fEventVariables.Set("PromptVertex", TVector3(apcommul_.appos));
        // dwall
        fEventVariables.Set("DWall", wallsk_(apcommul_.appos));
        // ring
            // nring
            fEventVariables.Set("NRing", apcommul_.apnring);
            // most energetic ring pid
            // most energetic ring mom
        //// muechk
        //    // nmue
        //    // etime
        //    // epos
            
    // mc information
        // ipnu
        // pnu
}

void EventNTagManager::ReadHits()
{
    fEventHits = PMTHitCluster(sktqz_);
}

void EventNTagManager::ReadParticles()
{
    skgetv_(); apflscndprt_();
    fEventParticles = ParticleCluster(skvect_, secndprt_);
    
    float geantT0; trginfo_(&geantT0);
    fEventParticles.SetT0(geantT0);
    
    fEventNCaptures = NCaptureCluster(fEventParticles);
    fEventDecayEs = DecayECluster(fEventParticles);
}

void EventNTagManager::ReadEventFromCommon()
{
    ClearData();

    ReadVariables();
    ReadHits();
    
    // if MC
    ReadParticles();
}

void EventNTagManager::ClearData()
{
    fEventVariables.Clear();
    fEventHits.Clear();
    fEventParticles.Clear();
    fEventDecayEs.Clear();
    fEventNCaptures.Clear();
    fEventCandidates.Clear();
}

void EventNTagManager::ApplySettings()
{
    fSettings.Get("T0TH", T0TH);
    fSettings.Get("T0MX", T0MX);
    fSettings.Get("TWIDTH", TWIDTH);
    fSettings.Get("TMINPEAKSEP", TMINPEAKSEP);
    fSettings.Get("NHITSTH", NHITSTH);
    fSettings.Get("NHITSMX", NHITSMX);
    fSettings.Get("N200TH", N200TH);
    fSettings.Get("N200MX", N200MX);
    
    fSettings.Get("INITGRIDWIDTH", INITGRIDWIDTH);
    fSettings.Get("MINGRIDWIDTH", MINGRIDWIDTH);
    fSettings.Get("GRIDSHRINKRATE", GRIDSHRINKRATE);
    fSettings.Get("VTXSRCRANGE", VTXSRCRANGE);
}

void EventNTagManager::SearchCandidates()
{
    // subtract tof
    SubtractToF();

    int   iHitPrevious    = 0;
    int   NHitsNew        = 0;
    int   NHitsPrevious   = 0;
    int   N200Previous    = 0;
    float t0Previous      = -1e6;

    unsigned long nEventHits = fEventHits.GetSize();

    // Loop over the saved TQ hit array from current event
    for (int iHit = 0; iHit < nEventHits; iHit++) {

        PMTHitCluster hitsInTWIDTH = fEventHits.Slice(iHit, TWIDTH);

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
        PMTHitCluster hitsIn200ns = fEventHits.Slice(iHit, TWIDTH/2.-100, TWIDTH/2.+100);
        int N200New = hitsIn200ns.GetSize();

        // If peak t0 diff = t0New - t0Previous > TMINPEAKSEP, save the previous peak.
        // Also check if N200Previous is below N200 cut and if t0Previous is over t0 threshold
        if (t0New - t0Previous > TMINPEAKSEP) {
            if (N200Previous < N200MX && t0Previous > T0TH) {
                Candidate candidate(iHitPrevious);
                FindFeatures(candidate);
                fEventCandidates.Append(candidate);
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
        FindFeatures(candidate);
        fEventCandidates.Append(candidate);
    }
}

void EventNTagManager::SubtractToF()
{
    TVector3 promptVertex; fEventVariables.Get("PromptVertex", promptVertex);
    //fEventHits.SetVertex(promptVertex);
    fEventHits.Sort();
}

void EventNTagManager::FindFeatures(Candidate& candidate)
{
    int firstHitID = candidate.HitID();

    PMTHitCluster hitsInTWIDTH = fEventHits.Slice(firstHitID, TWIDTH);
    PMTHitCluster hitsIn50ns   = fEventHits.Slice(firstHitID, TWIDTH/2.- 50, TWIDTH/2.+ 50);
    PMTHitCluster hitsIn200ns  = fEventHits.Slice(firstHitID, TWIDTH/2.-100, TWIDTH/2.+100);
    PMTHitCluster hitsIn1300ns = fEventHits.Slice(firstHitID, TWIDTH/2.-520, TWIDTH/2.+780);
    
    // Number of hits
    candidate.Set("NHits", hitsInTWIDTH.GetSize());
    candidate.Set("N50",   hitsIn50ns.GetSize());
    candidate.Set("N200",  hitsIn200ns.GetSize());
    candidate.Set("N1300", hitsIn1300ns.GetSize());
    
    // Time
    float reconCT = hitsInTWIDTH.Find(HitFunc::T, Calc::Mean) * 1e-3;
    candidate.Set("ReconCT", reconCT);
    candidate.Set("TRMS", hitsInTWIDTH.Find(HitFunc::T, Calc::RMS));
    
    // Charge
    candidate.Set("QSum", hitsInTWIDTH.Find(HitFunc::Q, Calc::Sum));
    
    // Beta's
    auto beta = hitsInTWIDTH.GetBetaArray();
    candidate.Set("Beta1", beta[1]);
    candidate.Set("Beta2", beta[2]);
    candidate.Set("Beta3", beta[3]);
    candidate.Set("Beta4", beta[4]);
    candidate.Set("Beta5", beta[5]);
    
    // DWall
    float dWall; fEventVariables.Get("DWall", dWall);
    TVector3 promptVertex; fEventVariables.Get("PromptVertex", promptVertex);
    auto dirVec = hitsInTWIDTH[HitFunc::Dir];
    auto meanDir = GetMean(dirVec).Unit();
    candidate.Set("DWall", dWall);
    candidate.Set("DWallMeanDir", GetDWallInDirection(promptVertex, meanDir));
    
    // Mean angle formed by all hits and the mean hit direction
    std::vector<float> angles;
    for (auto const& dir: dirVec) {
        angles.push_back((180/M_PI)*meanDir.Angle(dir));
    }
    
    float meanAngleWithMeanDirection = GetMean(angles);
    candidate.Set("ThetaMeanDir", meanAngleWithMeanDirection);
    
    // Opening angle stats
    auto openingAngleStats = hitsInTWIDTH.GetOpeningAngleStats();
    candidate.Set("AngleMean",  openingAngleStats.mean);
    candidate.Set("AngleStdev", openingAngleStats.stdev);
    candidate.Set("AngleSkew",  openingAngleStats.skewness);
    
    // TRMS-fit
    auto trmsFitVertex = hitsInTWIDTH.FindTRMSMinimizingVertex(/* TRMS-fit options */
                                                               INITGRIDWIDTH, MINGRIDWIDTH, GRIDSHRINKRATE, VTXSRCRANGE);

    candidate.Set("DWall_n", GetDWall(trmsFitVertex));
    candidate.Set("prompt_nfit", (promptVertex-trmsFitVertex).Mag());

    //int passDecayECut = 0;
    //if ((candidate.Get("N50") > 50) && reconCT < 20) {
    //    passDecayECut = 1;
    //}
    //candidate.Set("decay_e_like", passDecayECut);
}