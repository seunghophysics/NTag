#include "TFile.h"

#include "skparmC.h"
#include "sktqC.h"
#include "skheadC.h"
#include "apmringC.h"
#include "apmueC.h"
#include "appatspC.h"
#include "neworkC.h"
#include "nbnkC.h"

#include "SKLibs.hh"
#include "EventNTagManager.hh"
#include "Calculator.hh"

EventNTagManager::EventNTagManager(Verbosity verbose)
: fIsBranchSet(false)
{
    fMsg = Printer("NTagManager", verbose);
    
    fSettings = Store("Settings");
    fSettings.Initialize("/disk02/usr6/han/phd/utillib/src/EventNTagManager/NTagConfig");
    ApplySettings();
    
    fEventVariables = Store("Variables");
    fEventCandidates = CandidateCluster("Delayed");
    fEventEarlyCandidates = CandidateCluster("Early");
    
    std::vector<std::string> featureList = {"NHits", "N50", "N200", "N1300", "ReconCT", "TRMS", "QSum", 
                                            "Beta1", "Beta2", "Beta3", "Beta4", "Beta5", 
                                            "AngleMean", "AngleSkew", "AngleStdev", "Label",
                                            "DWall", "DWallMeanDir", "ThetaMeanDir", "DWall_n", "prompt_nfit", 
                                            "TMVAOutput", "TagIndex", "TagClass"};
    fEventCandidates.RegisterFeatureNames(featureList);
    
    std::vector<std::string> earlyFeatureList = {"ReconCT", "x", "y", "z", "DWall", "dirx", "diry", "dirz", 
                                                 "NHits", "GateType", "Goodness", "Label", "TagIndex", "TagClass"};
    fEventEarlyCandidates.RegisterFeatureNames(earlyFeatureList);

    InitializeTMVA();
    
    auto handler = new TInterruptHandler(this);
    handler->Add();
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
        fPromptVertex = TVector3(apcommul_.appos);
        fEventVariables.Set("pvx", apcommul_.appos[0]);
        fEventVariables.Set("pvy", apcommul_.appos[1]);
        fEventVariables.Set("pvz", apcommul_.appos[2]);
        // dwall
        fEventVariables.Set("DWall", wallsk_(apcommul_.appos));
        // ring
            // nring
            fEventVariables.Set("NRing", apcommul_.apnring);
            fEventVariables.Set("RingType", apcommul_.apip[0]);
            fEventVariables.Set("RingEMom", appatsp2_.apmsamom[0][1]);
            fEventVariables.Set("RingMuMom", appatsp2_.apmsamom[0][2]);
            // most energetic ring pid
            // most energetic ring mom
            
    // mc information
        float posnu[3]; nerdnebk_(posnu);
        fEventVariables.Set("NEUTMode", nework_.modene);
        fEventVariables.Set("NeutrinoType", nework_.ipne[0]);
        fEventVariables.Set("NeutrinoMom", TVector3(nework_.pne[0]).Mag());

    ReadEarlyCandidates();
}

void EventNTagManager::ReadHits()
{
    fEventHits = PMTHitCluster(sktqz_);
}

void EventNTagManager::ReadParticles()
{
    skgetv_(); apflscndprt_();
    fEventParticles.ReadCommonBlock(skvect_, secndprt_);
    
    float geantT0; trginfo_(&geantT0);
    fEventParticles.SetT0(geantT0);

    fEventTaggables.ReadParticleCluster(fEventParticles);
}

void EventNTagManager::ReadEarlyCandidates()
{
    float parentPeakTime;
    int silence = 1;
    muechk_gate_(apcommul_.appos, parentPeakTime, silence);
    int bank = 0; apclrmue_(); apgetmue_(&bank);

    for (int iMuE=0; apmue_.apmuenhit[iMuE]>0; iMuE++) {
        float reconCT = parentPeakTime*1e-3 + apmue_.apmuetime[iMuE];
            Candidate candidate;
            int label = reconCT < T0TH*1e-3 ? typeE : typeMissed;
            candidate.Set("ReconCT", reconCT);
            candidate.Set("x", apmue_.apmuepos[iMuE][0]);
            candidate.Set("y", apmue_.apmuepos[iMuE][1]);
            candidate.Set("z", apmue_.apmuepos[iMuE][2]);
            candidate.Set("dirx", apmue_.apmuedir[iMuE][0]);
            candidate.Set("diry", apmue_.apmuedir[iMuE][1]);
            candidate.Set("dirz", apmue_.apmuedir[iMuE][2]);
            candidate.Set("DWall", wallsk_(apmue_.apmuepos[iMuE]));
            candidate.Set("NHits", apmue_.apmuenhit[iMuE]);
            candidate.Set("GateType", apmue_.apmuetype[iMuE]);
            candidate.Set("Goodness", apmue_.apmuegood[iMuE]);
            candidate.Set("TagClass", typeE);
            fEventEarlyCandidates.Append(candidate);
    }
    
    //fEventEarlyCandidates.Sort();
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
    fEventTaggables.Clear();
    //fEventNCaptures.Clear();
    fEventCandidates.Clear();
    fEventEarlyCandidates.Clear();
}

void EventNTagManager::ApplySettings()
{
    fSettings.Get("T0TH", T0TH);
    fSettings.Get("T0MX", T0MX);
    fSettings.Get("TWIDTH", TWIDTH);
    fSettings.Get("TMINPEAKSEP", TMINPEAKSEP);
    fSettings.Get("TMATCHWINDOW", TMATCHWINDOW);
    fSettings.Get("NHITSTH", NHITSTH);
    fSettings.Get("NHITSMX", NHITSMX);
    fSettings.Get("N200TH", N200TH);
    fSettings.Get("N200MX", N200MX);
    
    fSettings.Get("INITGRIDWIDTH", INITGRIDWIDTH);
    fSettings.Get("MINGRIDWIDTH", MINGRIDWIDTH);
    fSettings.Get("GRIDSHRINKRATE", GRIDSHRINKRATE);
    fSettings.Get("VTXSRCRANGE", VTXSRCRANGE);
}

void EventNTagManager::InitializeTMVA()
{
    std::vector<std::string> featureList = {"AngleMean", "AngleSkew", "AngleStdev",
                                            "Beta1", "Beta2", "Beta3", "Beta4", "Beta5", 
                                            "DWall", "DWallMeanDir", "DWall_n", "N200", "NHits", "TRMS", 
                                            "ThetaMeanDir", "prompt_nfit"};

    for (auto const& feature: featureList) {
        fFeatureContainer[feature] = 0;
        fTMVAReader.AddVariable(feature, &(fFeatureContainer[feature]));
    }

    fTMVAReader.AddSpectator("Label", &(fCandidateLabel));
    //fTMVAReader.BookMVA("MLP", "/disk02/usr6/han/weights/TMVAClassification_MLP.weights.xml");
    fTMVAReader.BookMVA("MLP", "/disk02/usr6/han/phd/ntag/test_weight.xml");
    //sharedData->eventCandidates.RegisterFeatureName("TMVAOutput");
}

void EventNTagManager::SearchCandidates()
{
    float pmtDeadTime; fSettings.Get("rbn_deadtime", pmtDeadTime);
    fEventHits.ApplyDeadtime(pmtDeadTime);

    // subtract tof
    SubtractToF();

    int   iHitPrevious    = 0;
    int   NHitsNew        = 0;
    int   NHitsPrevious   = 0;
    int   N200Previous    = 0;
    float t0Previous      = std::numeric_limits<float>::min();

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
    
    // if MC
    PruneCandidates();
    MapTaggables();
    fEventTaggables.SetPromptVertex(fPromptVertex);
    
    // TMVA output
    //for (auto& candidate: fEventCandidates)
    //    candidate.Set("TMVAOutput", GetTMVAOutput(candidate));
    
    fEventEarlyCandidates.FillVectorMap();
    fEventCandidates.FillVectorMap();
    
    FillNTagCommon();
}

void EventNTagManager::SubtractToF()
{
    fEventHits.SetVertex(fPromptVertex);
}

void EventNTagManager::FindFeatures(Candidate& candidate)
{
    int firstHitID = candidate.HitID();
    //PMTHitCluster hitsIn30ns = fEventHits.Slice(candidate.HitID(), TWIDTH/2. -15, TWIDTH/2. +15);
    //PMTHitCluster hitsIn50ns = fEventHits.Slice(candidate.HitID(), TWIDTH/2. -25, TWIDTH/2. +25);
    auto hitsInTWIDTH = fEventHits.Slice(firstHitID, TWIDTH);
    auto hitsIn50ns   = fEventHits.Slice(firstHitID, TWIDTH/2.- 25, TWIDTH/2.+ 25);
    auto hitsIn200ns  = fEventHits.Slice(firstHitID, TWIDTH/2.-100, TWIDTH/2.+100);
    auto hitsIn1300ns = fEventHits.Slice(firstHitID, TWIDTH/2.-520, TWIDTH/2.+780);

    // TRMS-fit (BONSAI?)
    auto trmsFitVertex = hitsIn50ns.FindTRMSMinimizingVertex(/* TRMS-fit options */
                                                             INITGRIDWIDTH, 
                                                             MINGRIDWIDTH, 
                                                             GRIDSHRINKRATE, 
                                                             VTXSRCRANGE);
                                                             
    //hitsIn50ns.SetVertex(trmsFitVertex);
    //fEventHits.SetVertex(trmsFitVertex);
    //float tempT = hitsIn50ns[GetMaxNHitsIndex(hitsIn50ns)].t();
    //unsigned int trmsFitFirstHitID = fEventHits.GetIndex(tempT);


    //candidate.SetHitID(trmsFitFirstHitID);

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
    auto dirVec = hitsInTWIDTH[HitFunc::Dir];
    auto meanDir = GetMean(dirVec).Unit();
    candidate.Set("DWall", dWall);
    candidate.Set("DWallMeanDir", GetDWallInDirection(fPromptVertex, meanDir));
    
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

    candidate.Set("DWall_n", GetDWall(trmsFitVertex));
    candidate.Set("prompt_nfit", (fPromptVertex-trmsFitVertex).Mag());
    
    float tmvaOut = GetTMVAOutput(candidate);
    candidate.Set("TMVAOutput", tmvaOut);

    int tagClass = 0;
    if ((candidate.Get("N50") > 50) && reconCT < 20) {
        tagClass = typeE;
    }
    else if (tmvaOut > 0.7) {
        tagClass = typeN;
    }
    else
        tagClass = typeMissed;
    candidate.Set("TagClass", tagClass);
    
    //fEventHits.SetVertex(fPromptVertex);
}

float EventNTagManager::GetTMVAOutput(Candidate& candidate)
{
    // get features from candidate and fill feature container
    for (auto const& pair: fFeatureContainer) {
        float value = candidate[pair.first];
        fFeatureContainer[pair.first] = value;
    }

    // get spectator
    //fCandidateLabel = candidate["Label"];

    return fTMVAReader.EvaluateMVA("MLP");
}

void EventNTagManager::MapTaggables()
{
    MapCandidateClusters(fEventEarlyCandidates);
    MapCandidateClusters(fEventCandidates);
}

void EventNTagManager::MapCandidateClusters(CandidateCluster& candidateCluster)
{
    std::string key = candidateCluster.GetName();

    for (int iCandidate=0; iCandidate<candidateCluster.GetSize(); iCandidate++) {
        
        auto& candidate = candidateCluster[iCandidate];
        candidate.Set("TagIndex", -1);
        
        // default label: noise
        TrueLabel label = lNoise;
        bool hasMatchingN = false;
        bool hasMatchingE = false;

        std::vector<int> taggableIndexList;
        std::vector<float> matchTimeList;
            
        taggableIndexList.clear();
        matchTimeList.clear();
        for (int iTaggable=0; iTaggable<fEventTaggables.GetSize(); iTaggable++) {
            auto& taggable = fEventTaggables[iTaggable];
            float tDiff = fabs(taggable.Time() - candidate["ReconCT"]*1e3);
            if (tDiff < TMATCHWINDOW) {
                matchTimeList.push_back(tDiff);
                taggableIndexList.push_back(iTaggable);
            }
        }

        if (!matchTimeList.empty()) {

            // closest taggable is matched to the given candidate
            int iMinMatchTimeCapture = taggableIndexList[GetMinIndex(matchTimeList)];
            auto& taggable = fEventTaggables[iMinMatchTimeCapture];

            // candidate label determined by taggable type
            if (taggable.Type() == typeN) {
                hasMatchingN = true;
                if (taggable.Energy() > 6.) label = lnGd;
                else                        label = lnH;
            }
            else if (taggable.Type() == typeE) {
                hasMatchingE = true;
                label = lDecayE;
            }
            
            // set candidate tagindex as the index of the closest taggable
            candidate.Set("TagIndex", iMinMatchTimeCapture);
            
            // set two taggable candidate indices: 
            // one from early (muechk) and another from delayed (ntag) candidates
            // if the taggable has no previously saved candidate index
            if (taggable.GetCandidateIndex(key) == -1) {
                SetTaggedType(taggable, candidate);
                taggable.SetCandidateIndex(key, iCandidate);
            }
            // if the taggable has previously saved candidate index,
            // save the candidate with more hits
            else {
                auto& givenCandidate = fEventCandidates[iCandidate];
                auto& savedCandidate = fEventCandidates[taggable.GetCandidateIndex(key)];
                int givenNHits = givenCandidate["NHits"];
                int savedNHits = savedCandidate["NHits"];
                if (givenNHits > savedNHits) {
                    SetTaggedType(taggable, candidate);
                    taggable.SetCandidateIndex(key, iCandidate);
                    savedCandidate.Set("Label", lRemnant);
                }
                else {
                    label = lRemnant;
                }
            }
        }

        if (hasMatchingE && hasMatchingN)
            label = lUndefined;
        
        candidate.Set("Label", label);
    }
}

void EventNTagManager::PruneCandidates()
{
    std::vector<int> duplicateCandidateList;
    for (int iCandidate=0; iCandidate<fEventCandidates.GetSize(); iCandidate++) {
        auto& candidate = fEventCandidates[iCandidate];
        for (auto& early: fEventEarlyCandidates) {
            if (fabs(early["ReconCT"] - candidate["ReconCT"])*1e3 < 2*TMATCHWINDOW) {
                duplicateCandidateList.push_back(iCandidate); break;
            }
        }
    }
    
    for (auto& duplicateIndex: duplicateCandidateList)
        fEventCandidates.Erase(duplicateIndex);
}

int EventNTagManager::GetMaxNHitsIndex(PMTHitCluster& hitCluster)
{
    int maxID = 0;
    int maxNHits = 0;

    for (int i=0; i<hitCluster.GetSize(); i++) {
        
        int nHits = hitCluster.Slice(i, TWIDTH).GetSize();
        if (nHits > maxNHits) {
            maxID = i;
            maxNHits = nHits;
        }
    }
    
    return maxID;
}

void EventNTagManager::SetTaggedType(Taggable& taggable, const Candidate& candidate)
{
    float time = candidate["ReconCT"];
    if (time < T0TH*1e-3) {
        taggable.SetTaggedType(typeE);
    }
    else if (candidate.Get("N50", 0) > 50 && time < 20) {
        taggable.SetTaggedType(typeE);
    }
    else if (candidate.Get("TMVAOutput", 0) > 0.7) {
        taggable.SetTaggedType(typeN);
    }
    else {
        taggable.SetTaggedType(typeMissed);
    }
}

void EventNTagManager::DumpEvent()
{
    fEventVariables.Print();
    fEventParticles.DumpAllElements();
    fEventTaggables.DumpAllElements();
    fEventEarlyCandidates.DumpAllElements();
    fEventCandidates.DumpAllElements({"ReconCT", 
                                      "NHits", 
                                      "DWall_n", 
                                      "ThetaMeanDir", 
                                      "TMVAOutput", 
                                      "Label", 
                                      "TagIndex",
                                      "TagClass"});
}

void EventNTagManager::FillTrees()
{
    // set branch address for the first event
    if (!fIsBranchSet) {
        
        // make branches
        fSettings.MakeBranches();
        fEventVariables.MakeBranches();
        fEventParticles.MakeBranches();
        fEventTaggables.MakeBranches();
        fEventEarlyCandidates.MakeBranches();
        fEventCandidates.MakeBranches();
        
        // settings should be filled only once
        fSettings.FillTree();
        fIsBranchSet = true;
    }

    // fill trees
    fEventVariables.FillTree();
    fEventParticles.FillTree();
    fEventTaggables.FillTree();
    fEventEarlyCandidates.FillTree();
    fEventCandidates.FillTree();
}

void EventNTagManager::WriteTrees(bool doCloseFile)
{
    fSettings.WriteTree();
    fEventVariables.WriteTree();
    fEventParticles.WriteTree();
    fEventTaggables.WriteTree();
    fEventEarlyCandidates.WriteTree();
    fEventCandidates.WriteTree();
    if (doCloseFile) fEventCandidates.GetTree()->GetCurrentFile()->Close();
}

void EventNTagManager::FillNTagCommon()
{ 
    int i = 0;
    bool isELike;
    int nTaggedN = 0, nTaggedE = fEventEarlyCandidates.GetSize();
    for (auto const& candidate: fEventCandidates) {
        int n50 = candidate["N50"];
        float time = candidate["ReconCT"];
        int label = candidate["Label"];
        float nLikelihood = candidate["TMVAOutput"];
        isELike = (n50 > 50) & (time < 20);
        ntag_.mctruth_neutron[i] = ntag_.np > MAXNP ? -1 : (label == lnH || label == lnGd ? 1 : 0);
        ntag_.goodness[i] = nLikelihood;
        ntag_.tag[i] = (nLikelihood > 0.7) & (!isELike);
        ntag_.ntime[i] = time*1e3;
        i++;
        if (isELike) nTaggedE++;
        else if (ntag_.tag[i]) nTaggedN++; 
    }
    
    int nTrueN = 0;
    for (auto const& taggable: fEventTaggables) {
        if (taggable.Type() == typeN) {
            nTrueN++;
        }
    }

    ntag_.np = fEventCandidates.GetSize();
    ntag_.nn = nTaggedN;
    ntag_.mctruth_nn = nTrueN;
    ntag_.n200m = nTaggedE; // temporarily save the number of revised nmue to n200m for now
}