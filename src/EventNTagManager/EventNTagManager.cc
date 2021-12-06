#include "TFile.h"

#include "skroot.h"
#undef MAXPM
#undef MAXPMA
#undef MAXHWSK
#include "skparmC.h"
#include "sktqC.h"
#include "skheadC.h"
#include "apmringC.h"
#include "apmueC.h"
#include "apmsfitC.h"
#include "appatspC.h"
#include "apringspC.h"
#include "spliTChanOutC.h"
#include "fitqunoutC.h"
#include "geotnkC.h"
#include "neworkC.h"
#include "nbnkC.h"
#include "skonl/softtrg_cond.h"

#include "SKLibs.hh"
#include "SKIO.hh"
#include "GetStopMuVertex.hh"
#include "NTagBankIO.hh"
#include "Calculator.hh"
#include "NoiseManager.hh"
#include "EventNTagManager.hh"

EventNTagManager::EventNTagManager(Verbosity verbose)
: fOutDataFile(nullptr), fIsBranchSet(false), fIsMC(true), fFileFormat(mZBS)
{
    fMsg = Printer("NTagManager", verbose);

    fSettings = Store("Settings");
    fSettings.Initialize(GetENV("NTAGLIBPATH")+"/NTagConfig");
    ApplySettings();

    fEventVariables = Store("Variables");
    fEventCandidates = CandidateCluster("Delayed");
    fEventEarlyCandidates = CandidateCluster("Early");

    fEventCandidates.RegisterFeatureNames(gNTagFeatures);
    fEventEarlyCandidates.RegisterFeatureNames(gMuechkFeatures);

    auto handler = new TInterruptHandler(this);
    handler->Add();

    fTRMSFitManager = TRMSFitManager(verbose);
    fBonsaiManager  = BonsaiManager(verbose);
    fBonsaiManager.Initialize();
}

EventNTagManager::~EventNTagManager() {}

void EventNTagManager::ReadPromptVertex(VertexMode mode)
{
    if (mode == mNONE);

    else if (mode == mAPFIT) {
        int bank = 0; aprstbnk_(&bank);
        fPromptVertex = TVector3(apcommul_.appos);
        
        // ring
        fEventVariables.Set("NRing", apcommul_.apnring);
        fEventVariables.Set("FirstRingType", apcommul_.apip[0]);
        if      (apcommul_.apip[0] == ELECTRON)
            fEventVariables.Set("FirstRingMom", appatsp2_.apmsamom[0][1]);
        else if (apcommul_.apip[0] == MUON)
            fEventVariables.Set("FirstRingMom", appatsp2_.apmsamom[0][2]);
            
        // visible energy
        fEventVariables.Set("EVis", apcomene_.apevis);
    }

    else if (mode == mBONSAI) {
        int lun = 10;
        TreeManager* mgr = skroot_get_mgr(&lun);
        LoweInfo* LOWE = mgr->GetLOWE();
        mgr->GetEntry();
        fPromptVertex = TVector3(LOWE->bsvertex);
        
        // visible energy
        fEventVariables.Set("EVis", LOWE->bsenergy);
    }
    
    else if (mode == mFITQUN) {
        int fqbank = 0;
        readfqzbsbank_(&fqbank);
        fPromptVertex = TVector3(fitqunmr_.fqmrpos[0][0]);
    }

    else if (mode == mCUSTOM) {
        float vx, vy, vz;
        if (fSettings.Get("vx", vx) && fSettings.Get("vy", vy) && fSettings.Get("vz", vz)) {
            fPromptVertex = TVector3(vx, vy, vz);
        }
        else
            fMsg.Print("Custom prompt vertex not fully specified! "
                       "Use -vx, -vy, -vz commands to specify the custom prompt vertex.", pERROR);
    }

    else if (mode == mTRUE) {
        skgetv_();
        float dx = 2*RINTK, dy = 2*RINTK, dz = 2*ZPINTK;
        float maxDist = 150.;
        while (Norm(dx, dy, dz) > maxDist) {
            dx = gRandom->BreitWigner(0, PVXRES);
            dy = gRandom->BreitWigner(0, PVXRES);
            dz = gRandom->BreitWigner(0, PVXRES);
        }
        fPromptVertex = TVector3(skvect_.pos) + TVector3(dx, dy, dz);
    }

    else if (mode == mSTMU) {
        fPromptVertex = GetStopMuVertex();
    }

    else {
        fMsg.Print("Invalid prompt vertex mode, setting vertex mode to NONE...", pWARNING);
        fPromptVertexMode = mNONE;
    }
}

void EventNTagManager::ReadVariables()
{
    // run/event information
    fEventVariables.Set("RunNo", skhead_.nrunsk);
    fEventVariables.Set("SubrunNo", skhead_.nsubsk);
    fEventVariables.Set("EventNo", skhead_.nevsk);

    // hit information
    float qismsk = 0;
    for (int iHit = 0; iHit < sktqz_.nqiskz; iHit++) {
        float hitTime = sktqz_.tiskz[iHit];
        if (479.2 < hitTime && hitTime < 1779.2) {
            qismsk += sktqz_.qiskz[iHit];
        }
    }
    // qismsk = skq_.qismsk;
    int nhitac; odpc_2nd_s_(&nhitac);
    fEventVariables.Set("QISMSK", qismsk);
    fEventVariables.Set("NHITAC", nhitac);

    // trigger information
    int trgtype = skhead_.idtgsk & 1<<29 ? tAFT : skhead_.idtgsk & 1<<28 ? tSHE : tELSE;
    static double prevEvTime = 0;
    double globalTime =  (skhead_.nt48sk[0] * std::pow(2, 32)
                        + skhead_.nt48sk[1] * std::pow(2, 16)
                        + skhead_.nt48sk[2]) * 20 * 1e-6;      // [ms]
    double tDiff = globalTime - prevEvTime;
    fEventVariables.Set("TrgType", trgtype);
    fEventVariables.Set("TDiff", tDiff);
    prevEvTime = globalTime;

    // reconstructed information
    // prompt vertex
    ReadPromptVertex(fPromptVertexMode);
    fEventVariables.Set("pvx", fPromptVertex.x());
    fEventVariables.Set("pvy", fPromptVertex.y());
    fEventVariables.Set("pvz", fPromptVertex.z());
    // dwall
    fEventVariables.Set("DWall", GetDWall(fPromptVertex));

    if (fIsMC) {
        // vector information (true event generation vertex)
        skgetv_();
        fEventVariables.Set("vecvx", skvect_.pos[0]);
        fEventVariables.Set("vecvy", skvect_.pos[1]);
        fEventVariables.Set("vecvz", skvect_.pos[2]);
        fEventVariables.Set("VtxRes", (TVector3(skvect_.pos)-fPromptVertex).Mag());
        
        // trgOffset
        fEventVariables.Set("MCT0", SKIO::GetMCTriggerOffset(fFileFormat));

        // NEUT
        if (fSettings.GetBool("neut")) {
            float posnu[3]; nerdnebk_(posnu);
            fEventVariables.Set("NEUTMode", nework_.modene);
            fEventVariables.Set("NuType", nework_.ipne[0]);
            fEventVariables.Set("NuMom", TVector3(nework_.pne[0]).Mag());
        }
    }
}

void EventNTagManager::ReadHits()
{
    fEventHits = PMTHitCluster(sktqz_);
    fEventHits.Sort();
}

void EventNTagManager::AddHits()
{
    float tOffset = 0.;
    PMTHit lastHit;

    bool  coincidenceFound = true;

    if (!fEventHits.IsEmpty()) {
        coincidenceFound = false;
        lastHit = fEventHits[fEventHits.GetSize()-1];
    }

    if (!coincidenceFound) {
        for (int iHit = 0; iHit < sktqz_.nqiskz; iHit++) {
            if (sktqz_.qiskz[iHit] == lastHit.q() && static_cast<unsigned int>(sktqz_.icabiz[iHit]) == lastHit.i()) {
                tOffset = lastHit.t() - sktqz_.tiskz[iHit];
                coincidenceFound = true;
                fMsg.Print(Form("Coincidence found: t = %f ns, (offset: %f ns)", lastHit.t(), tOffset));
                break;
            }
        }
    }

    fEventHits.Append(PMTHitCluster(sktqz_) + tOffset, true);
    fEventHits.Sort();
}

void EventNTagManager::AddNoise()
{
    fNoiseManager->AddNoise(&fEventHits);
}

void EventNTagManager::ReadParticles()
{
    skgetv_();
    SKIO::SetSecondaryCommon(fFileFormat);
    fEventParticles.ReadCommonBlock(skvect_, secndprt_);

    float geantT0 = fEventVariables.GetFloat("MCT0", 0);
    fEventParticles.SetT0(geantT0*1e-3);

    fEventTaggables.ReadParticleCluster(fEventParticles);
    fEventTaggables.SetPromptVertex(fPromptVertex);
}

void EventNTagManager::ReadEarlyCandidates()
{
    float parentPeakTime;
    int silence = 1;
    muechk_gate_(apcommul_.appos, parentPeakTime, silence);
    int bank = 0; apclrmue_(); apgetmue_(&bank);

    for (int iMuE=0; apmue_.apmuenhit[iMuE]>0; iMuE++) {
        float fitT = parentPeakTime*1e-3 + apmue_.apmuetime[iMuE] - 1;
            Candidate candidate;
            candidate.Set("FitT", fitT);
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
            candidate.Set("TagClass", fTagger->Classify(candidate));
            fEventEarlyCandidates.Append(candidate);
    }
}

void EventNTagManager::ReadInfoFromCommon()
{
    ReadVariables();

    if (fIsMC) ReadParticles();
    if (fSettings.GetBool("muechk")) ReadEarlyCandidates();
}

void EventNTagManager::ReadEventFromCommon()
{
    ReadInfoFromCommon();
    AddHits();
    if (fSettings.GetBool("add_noise")) {
        fEventHits.SetAsSignal(true);
        AddNoise();
    }
}

void EventNTagManager::SearchAndFill()
{
    int nhitac = fEventVariables.GetInt("NHITAC");
    int nodhitmx = fSettings.GetInt("NODHITMX");
    if (nhitac > nodhitmx) {
        fMsg.Print(Form("%d OD hits in this event (allowed: NHITODMX = %d)...", nhitac, nodhitmx), pWARNING);
        fMsg.Print(Form("Skipping search for this event (EventNo: %d)", fEventVariables.GetInt("EventNo")), pWARNING);
    }
    else {
        SearchCandidates();
    }

    DumpEvent();
    FillTrees();
    if (fSettings.GetBool("write_bank"))
        FillNTAGBank();
    if (fOutDataFile) {
        fOutDataFile->FillTQREAL(fEventHits);
        fOutDataFile->Write();
    }
        
    ClearData();
}

void EventNTagManager::ProcessEvent()
{
    static bool initialized = false;
    
    if (!initialized) {
        CheckMC();
        if (fSettings.GetBool("tmva")) 
            fTMVATagger.Initialize();
        initialized = true;
    }

    if (fIsMC || fSettings.GetBool("force_flat"))
        ProcessFlatEvent();
    else
        ProcessDataEvent();
}

void EventNTagManager::ProcessDataEvent()
{
    auto thisEvTrg = skhead_.idtgsk & (1<<29) ? tAFT : (skhead_.idtgsk & (1<<28) ? tSHE : tELSE);
    fMsg.Print(Form("This evtrg: %d", thisEvTrg), pWARNING);

    // if current event is AFT, append TQ and fill output.
    if (thisEvTrg == tAFT) {
        fMsg.Print("Appending AFT to previous SHE", pWARNING);
        fEventVariables.Set("TrgType", tAFT);
        AddHits();
        SearchAndFill();
    }

    // if previous event was SHE without following AFT,
    // just fill output because there's nothing to append.
    else if (!fEventHits.IsEmpty()) {
        fMsg.Print("Processing previous SHE", pWARNING);
        SearchAndFill();
    }

    // if the current event is SHE,
    // save raw hit info and don't fill output.
    if (thisEvTrg == tSHE) {
        fMsg.Print("Skipping current SHE", pWARNING);
        ReadEventFromCommon();
        DumpEvent();
    }

    // if the current event is neither SHE nor AFT (e.g. HE, LE, etc.),
    // save raw hit info and fill output.
    if (thisEvTrg == tELSE) {
        fMsg.Print("ELSE", pWARNING);
        ProcessFlatEvent();
    }
}

void EventNTagManager::ProcessFlatEvent()
{
    ReadEventFromCommon();
    SearchAndFill();
}

void EventNTagManager::SearchCandidates()
{
    float pmtDeadTime; fSettings.Get("TRBNWIDTH", pmtDeadTime);
    if (pmtDeadTime) fEventHits.ApplyDeadtime(pmtDeadTime);

    // subtract tof
    if (fPromptVertexMode != mNONE) SetToF(fPromptVertex);
    else                            UnsetToF();

    int   iHitPrevious    = -1;
    int   NHitsNew        = 0;
    int   NHitsPrevious   = 0;
    int   N200Previous    = 0;
    float t0Previous      = std::numeric_limits<float>::min();

    unsigned long nEventHits = fEventHits.GetSize();

    // Loop over the saved TQ hit array from current event
    for (unsigned int iHit = 0; iHit < nEventHits; iHit++) {

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
            if (iHitPrevious >= 0 && N200Previous < N200MX && t0Previous > T0TH)
                FindDelayedCandidate(iHitPrevious);
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
        FindDelayedCandidate(iHitPrevious);

    if (fSettings.GetBool("tag_e")) PruneCandidates();
    /*if (fIsMC)*/  MapTaggables();

    fEventEarlyCandidates.FillVectorMap();
    fEventCandidates.FillVectorMap();
}

void EventNTagManager::MapTaggables()
{
    MapCandidateClusters(fEventEarlyCandidates);
    MapCandidateClusters(fEventCandidates);
}

void EventNTagManager::ResetTaggableMapping()
{
    // taggables
    for (auto& taggable: fEventTaggables) {
        taggable.SetCandidateIndex("Early", -1);
        taggable.SetCandidateIndex("Delayed", -1);
        taggable.SetTaggedType(typeMissed);
    }

    // muechk candidates
    for (auto& candidate: fEventEarlyCandidates) {
        candidate.Set("TagIndex", -1);
        candidate.Set("Label", lNoise);
        candidate.Set("TagClass", fTagger->Classify(candidate));
    }

    // ntag candidates
    for (auto& candidate: fEventCandidates) {
        candidate.Set("TagIndex", -1);
        candidate.Set("Label", lNoise);
        candidate.Set("TagClass", fTagger->Classify(candidate));
    }
}

void EventNTagManager::ApplySettings()
{
    TString inFilePath;
    fSettings.Get("in", inFilePath);

    if (inFilePath.EndsWith(".root"))
        fFileFormat = mSKROOT;
    else
        fFileFormat = mZBS;
    
    // NTag parameters
    float tMin = fSettings.GetFloat("TMIN");
    float tMax = fSettings.GetFloat("TMAX");
    T0TH = tMin * 1e3 + 1000; T0MX = tMax * 1e3 + 1000; // ns->us, add trigger time T=1000 ns
    fSettings.Get("TWIDTH", TWIDTH);
    fSettings.Get("TMINPEAKSEP", TMINPEAKSEP);
    fSettings.Get("TMATCHWINDOW", TMATCHWINDOW);
    fSettings.Get("NHITSTH", NHITSTH);
    fSettings.Get("NHITSMX", NHITSMX);
    fSettings.Get("N200TH", N200TH);
    fSettings.Get("N200MX", N200MX);
    fSettings.Get("PVXRES", PVXRES);

    // vertex mode
    std::string promptVertexMode, delayedVertexMode;
    fSettings.Get("prompt_vertex", promptVertexMode);
    fSettings.Get("delayed_vertex", delayedVertexMode);

    SetVertexMode(fPromptVertexMode, promptVertexMode);
    SetVertexMode(fDelayedVertexMode, delayedVertexMode);

    if (fDelayedVertexMode == mTRMS)
        fDelayedVertexManager = &fTRMSFitManager;
    else if (fDelayedVertexMode == mBONSAI)
        fDelayedVertexManager = &fBonsaiManager;
    else if (fPromptVertexMode == mNONE) {
        fMsg.Print("Prompt vertex mode is set to \"none\". TWIDTH > 100 ns and NHITSTH > 10 recommended.", pWARNING);
        if (fDelayedVertexMode == mPROMPT) {
        fMsg.Print("Delayed vertex mode is \"prompt\", but no prompt vertex is specified.", pWARNING);
        fMsg.Print("Setting delayed vertex mode as \"bonsai\"...", pWARNING);
        fDelayedVertexMode = mBONSAI;
        fDelayedVertexManager = &fBonsaiManager;
        }
    }
    else if (fDelayedVertexMode != mPROMPT){
        fMsg.Print("Delayed vertex mode should be one of \"trms\", \"bonsai\", or \"prompt\".", pWARNING);
        fMsg.Print("Setting delayed vertex mode as \"prompt\"...", pWARNING);
        fDelayedVertexMode = mPROMPT;
    }

    fSettings.Get("TRMSTWIDTH", TRMSTWIDTH);
    fSettings.Get("INITGRIDWIDTH", INITGRIDWIDTH);
    fSettings.Get("MINGRIDWIDTH", MINGRIDWIDTH);
    fSettings.Get("GRIDSHRINKRATE", GRIDSHRINKRATE);
    fSettings.Get("VTXSRCRANGE", VTXSRCRANGE);

    fTRMSFitManager.SetParameters(INITGRIDWIDTH, MINGRIDWIDTH, GRIDSHRINKRATE, VTXSRCRANGE);

    fSettings.Get("E_N50CUT", E_N50CUT);
    fSettings.Get("E_TIMECUT", E_TIMECUT);
    fSettings.Get("N_OUTCUT", N_OUTCUT);
    
    if (fSettings.GetBool("tag_e")) 
        fTMVATagger.SetECut(tMin, E_N50CUT, E_TIMECUT);
    fTMVATagger.SetNCut(N_OUTCUT);
    
    if (fSettings.GetBool("tmva")) {
        fTagger = &fTMVATagger;
    }
    else {
        fTagger = &fVoidTagger;
    }
}

void EventNTagManager::ReadArguments(const ArgParser& argParser)
{
    fSettings.ReadArguments(argParser);
    
    for (auto const& pair: fSettings.GetMap()) {
        auto key = pair.first;
        if (FindIndex(gCmdOptions, key)<0) {
            fMsg.Print(key + " is not a valid option name. Skipping...", pWARNING);
            fSettings.RemoveKey(key);
        }
    }
}

void EventNTagManager::SetVertexMode(VertexMode& mode, std::string key)
{
    if (key == "none")
        mode = mNONE;
    else if (key == "apfit")
        mode = mAPFIT;
    else if (key == "bonsai")
        mode = mBONSAI;
    else if (key == "fitqun")
        mode = mFITQUN;
    else if (key == "custom")
        mode = mCUSTOM;
    else if (key == "true")
        mode = mTRUE;
    else if (key == "stmu")
        mode = mSTMU;
    else if (key == "trms")
        mode = mTRMS;
    else if (key == "prompt")
        mode = mPROMPT;
}

void EventNTagManager::MakeTrees(TFile* outfile)
{
    if (outfile) outfile->cd();

    TTree* settingsTree = new TTree("settings", "settings");
    TTree* eventTree    = new TTree("event", "event");
    TTree* particleTree = new TTree("particle", "particle");
    TTree* taggableTree = new TTree("taggable", "taggable");
    TTree* nTree        = new TTree("ntag", "ntag");
    TTree* eTree        = new TTree("muechk", "muechk");
    
    if (outfile) {
        settingsTree->SetDirectory(outfile);
        eventTree->SetDirectory(outfile);
        particleTree->SetDirectory(outfile);
        taggableTree->SetDirectory(outfile);
        nTree->SetDirectory(outfile);
        eTree->SetDirectory(outfile);
    }
    
    fSettings.SetTree(settingsTree);
    fEventVariables.SetTree(eventTree);
    fEventParticles.SetTree(particleTree);
    fEventTaggables.SetTree(taggableTree);
    fEventCandidates.SetTree(nTree);
    fEventEarlyCandidates.SetTree(eTree);
}

void EventNTagManager::FillTrees()
{
    FillNTagCommon();

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
    if (fSettings.GetBool("muechk")) fEventEarlyCandidates.WriteTree();
    fEventCandidates.WriteTree();
    if (doCloseFile) fEventCandidates.GetTree()->GetCurrentFile()->Close();
}

void EventNTagManager::ClearData()
{
    fEventVariables.Clear();
    fEventHits.Clear();
    fEventParticles.Clear();
    fEventTaggables.Clear();
    fEventCandidates.Clear();
    fEventEarlyCandidates.Clear();
}

void EventNTagManager::DumpEvent()
{
    fEventVariables.Print();
    fEventParticles.DumpAllElements();
    fEventTaggables.DumpAllElements();
    fEventEarlyCandidates.DumpAllElements({"FitT", "NHits", "DWall", "Goodness",
                                           "Label", "TagIndex", "TagClass"});
    fEventCandidates.DumpAllElements({"FitT",
                                      "NHits",
                                      "dvx",
                                      "dvy",
                                      "dvz",
                                      "DPrompt",
                                      "DWall",
                                      "MeanDirAngleMean",
                                      "SignalRatio",
                                      "TagOut",
                                      "Label",
                                      "TagIndex",
                                      "TagClass"});
}

void EventNTagManager::CheckMC()
{
    if (skhead_.nrunsk == 999999)
        fIsMC = true;
    else
        fIsMC = false;
}

void EventNTagManager::SetToF(const TVector3& vertex)
{
    fEventHits.SetVertex(vertex);
}

void EventNTagManager::UnsetToF()
{
    fEventHits.RemoveVertex();
}

void EventNTagManager::FindDelayedCandidate(unsigned int iHit)
{
    PMTHit firstHit = fEventHits[iHit];
    TVector3 delayedVertex = fPromptVertex;
    float delayedTime = firstHit.t();

    // delayed vertex = prompt vertex
    if (fDelayedVertexMode == mPROMPT) {
        delayedVertex = fPromptVertex;
        delayedTime = fEventHits.Slice(iHit, TWIDTH).Find(HitFunc::T, Calc::Mean);
    }
    // delayed vertex fit
    else {
        PMTHitCluster hitsForFit;
        bool doFit = true;

        if (fDelayedVertexMode == mTRMS)
            hitsForFit = fEventHits.Slice(iHit, (TWIDTH-TRMSTWIDTH)/2., (TWIDTH+TRMSTWIDTH)/2.) - firstHit.t() + 1000;

        else if (fDelayedVertexMode == mBONSAI) {
            fEventHits.RemoveVertex();
            fEventHits.Sort();
            firstHit.UnsetToFAndDirection();
            unsigned int firstHitID = fEventHits.GetIndex(firstHit);
            hitsForFit = fEventHits.Slice(firstHitID, TWIDTH/2.-520, TWIDTH/2.+780) - firstHit.t() + 1000;
            
            // give up bonsai fit for N1300 larger than 2000
            if (hitsForFit.GetSize() > 2000) {
                fMsg.Print(Form("A possible candidate at T=%3.2f us has N1300 larger than 2000," 
                                " giving up fit and setting the delayed vertex the same as the prompt...", firstHit.t()*1e-3), pWARNING);
                doFit = false;
            }
        }
        
        if (doFit) {
            hitsForFit.Sort();
            fDelayedVertexManager->Fit(hitsForFit);
            delayedVertex = fDelayedVertexManager->GetFitVertex();
            delayedTime   = fDelayedVertexManager->GetFitTime() + firstHit.t() - 1000;
        }
    }
    fEventHits.SetVertex(delayedVertex);
    firstHit.SetToFAndDirection(delayedVertex);

    // fitted time should not be too far off from the first hit time
    // to prevent double counting of same hits
    if (fabs(delayedTime-firstHit.t()) < TMINPEAKSEP) {
        iHit = fEventHits.GetLowerBoundIndex(delayedTime);
        unsigned int nHits = fEventHits.Slice(iHit, -TWIDTH/2., TWIDTH/2.).GetSize();
        
        // NHits > 4 to prevent NaN in angle variables
        if (nHits > 4) {
            Candidate candidate(iHit, (delayedTime-1000)*1e-3); // -1000 ns is to offset the trigger time T=1000 ns
            FindFeatures(candidate);
            fEventCandidates.Append(candidate);
        }
    }

    SetToF(fPromptVertex);
}

void EventNTagManager::FindFeatures(Candidate& candidate)
{
    unsigned int firstHitID = candidate.HitID();
    auto hitsInTWIDTH = fEventHits.Slice(firstHitID, -TWIDTH/2., TWIDTH/2.);
    auto hitsIn50ns   = fEventHits.Slice(firstHitID, -25, 25);
    auto hitsIn200ns  = fEventHits.Slice(firstHitID, -100, +100);
    auto hitsIn1300ns = fEventHits.Slice(firstHitID, -520, +780);
    //auto hitsInTWIDTH = fEventHits.Slice(firstHitID, TWIDTH);
    //auto hitsIn50ns   = fEventHits.Slice(firstHitID, TWIDTH/2.-25, TWIDTH/2.+ 25);
    //auto hitsIn200ns  = fEventHits.Slice(firstHitID, TWIDTH/2.-100, TWIDTH/2.+100);
    //auto hitsIn1300ns = fEventHits.Slice(firstHitID, TWIDTH/2.-520, TWIDTH/2.+780);
    
    //hitsInTWIDTH.FindHitProperties();
    //hitsInTWIDTH.DumpAllElements();
    //float deg = 3.141592/180.;
    //unsigned int nClusHits = hitsInTWIDTH.Slice(HitFunc::MinAngle, 0, 14.1*deg).GetSize();
    //unsigned int nBackHits = hitsInTWIDTH.Slice(HitFunc::DirAngle, 90*deg, 180*deg).GetSize();
    //unsigned int nLowHits = hitsInTWIDTH.Slice(HitFunc::Acceptance, 90, 180).GetSize();
    //candidate.Set("NClusHits", nClusHits);
    //candidate.Set("NBackHits", nBackHits);

    // Delayed vertex
    auto delayedVertex = fEventHits.GetVertex();
    candidate.Set("dvx", delayedVertex.x());
    candidate.Set("dvy", delayedVertex.y());
    candidate.Set("dvz", delayedVertex.z());

    // Number of hits
    candidate.Set("NHits", hitsInTWIDTH.GetSize());
    candidate.Set("N50",   hitsIn50ns.GetSize());
    candidate.Set("N200",  hitsIn200ns.GetSize());
    candidate.Set("N1300", hitsIn1300ns.GetSize());

    // Time
    //float fitT = hitsInTWIDTH.Find(HitFunc::T, Calc::Mean) * 1e-3;
    candidate.Set("FitT", candidate.Time());
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
    auto dirVec = hitsInTWIDTH[HitFunc::Dir];

    auto meanDir = GetMean(dirVec).Unit();
    candidate.Set("DWall", GetDWall(delayedVertex));
    candidate.Set("DWallMeanDir", GetDWallInDirection(delayedVertex, meanDir));

    // Mean angle formed by all hits and the mean hit direction
    std::vector<float> angles;
    for (auto const& dir: dirVec)
        angles.push_back((180/M_PI)*meanDir.Angle(dir));

    candidate.Set("MeanDirAngleMean", GetMean(angles));
    candidate.Set("MeanDirAngleRMS", GetRMS(angles));

    // Opening angle stats
    auto openingAngleStats = hitsInTWIDTH.GetOpeningAngleStats();
    candidate.Set("OpeningAngleMean",  openingAngleStats.mean);
    candidate.Set("OpeningAngleStdev", openingAngleStats.stdev);
    candidate.Set("OpeningAngleSkew",  openingAngleStats.skewness);

    candidate.Set("DPrompt", (fPromptVertex-delayedVertex).Mag());
    
    candidate.Set("SignalRatio", hitsInTWIDTH.GetSignalRatio());

    float likelihood = fTagger->GetLikelihood(candidate);
    candidate.Set("TagOut", likelihood);
    candidate.Set("TagClass", fTagger->Classify(candidate));
}

void EventNTagManager::MapCandidateClusters(CandidateCluster& candidateCluster)
{
    std::string key = candidateCluster.GetName();

    for (unsigned int iCandidate=0; iCandidate<candidateCluster.GetSize(); iCandidate++) {

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
        for (unsigned int iTaggable=0; iTaggable<fEventTaggables.GetSize(); iTaggable++) {
            auto& taggable = fEventTaggables[iTaggable];
            float tDiff = fabs(taggable.Time() - candidate["FitT"]);
            if (tDiff*1e3 < TMATCHWINDOW) {
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

void EventNTagManager::SetTaggedType(Taggable& taggable, Candidate& candidate)
{
    TaggableType tagClass = static_cast<TaggableType>((int)(candidate.Get("TagClass", -1)+0.5f));
    TaggableType tagType = taggable.TaggedType();
    // if candidate tag class undefined
    if (tagClass < 0)
        candidate.Set("TagClass", fTagger->Classify(candidate));
    // if candidate tag class defined
    else if (tagType != typeMissed && tagType != tagClass)
        taggable.SetTaggedType(typeEN);
    else
        taggable.SetTaggedType(tagClass);
}

void EventNTagManager::PruneCandidates()
{
    std::vector<int> duplicateCandidateList;
    for (unsigned int iCandidate=0; iCandidate<fEventCandidates.GetSize(); iCandidate++) {
        auto& candidate = fEventCandidates[iCandidate];
        for (auto& early: fEventEarlyCandidates) {
            if (early["TagClass"] == typeE &&
                fabs(early["FitT"] - candidate["FitT"])*1e3 < 2*TMATCHWINDOW) {
                duplicateCandidateList.push_back(iCandidate); break;
            }
        }
    }

    for (auto& duplicateIndex: duplicateCandidateList)
        fEventCandidates.Erase(duplicateIndex);
}

void EventNTagManager::FillNTagCommon()
{
    int nTrueE = 0, nTaggedE = 0, nTrueN = 0, nTaggedN = 0;

    // count true
    for (auto const& taggable: fEventTaggables) {
        if      (taggable.Type() == typeE) nTrueE++;
        else if (taggable.Type() == typeN) nTrueN++;
    }

    // muechk: count tagged
    for (auto const& candidate: fEventEarlyCandidates) {
        if (candidate["TagClass"] == typeE)
            nTaggedE++;
        else if (candidate["TagClass"] == typeN)
            nTaggedN++;
    }

    // ntag: count tagged
    int i = 0;
    for (auto const& candidate: fEventCandidates) {
        int label = candidate["Label"];
        int isTaggedOrNot = 0;

        if (candidate["TagClass"] == typeE) {
            nTaggedE++;
        }
        else if (candidate["TagClass"] == typeN) {
            isTaggedOrNot = 1;
            nTaggedN++;
        }

        // fill ntag bank: candidates
        ntag_.ntime[i] = candidate["FitT"] * 1e3;
        ntag_.mctruth_neutron[i] = ntag_.np > MAXNP ? -1 : (label == lnH || label == lnGd ? 1 : 0);
        ntag_.goodness[i] = candidate["TagOut"];
        ntag_.tag[i] = isTaggedOrNot;
        i++;
    }

    // fill ntag bank: event variables
    ntag_.np = fEventCandidates.GetSize();
    ntag_.trgtype = nTrueE; // temporarily save nTrueE in ntag_.trgtype for now
    ntag_.n200m = nTaggedE; // temporarily save nTaggedE ntag_.n200m for now
    ntag_.mctruth_nn = nTrueN;
    ntag_.nn = nTaggedN;

    // set event variables
    fEventVariables.Set("NTrueE", nTrueE);
    fEventVariables.Set("NTaggedE", nTaggedE);
    fEventVariables.Set("NTrueN", nTrueN);
    fEventVariables.Set("NTaggedN", nTaggedN);
}