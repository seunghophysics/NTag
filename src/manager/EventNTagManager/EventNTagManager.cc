#include "TFile.h"

#include "skroot.h"
#undef MAXPM
#undef MAXPMA
#undef MAXHWSK
#undef SECMAXRNG
#include "skparmC.h"
#include "sktqC.h"
#include "skheadC.h"
#include "apmringC.h"
#include "apmueC.h"
#include "apmsfitC.h"
#include "appatspC.h"
#include "apringspC.h"
#include "skroot_loweC.h"
#include "spliTChanOutC.h"
#include "fitqunoutC.h"
#include "geotnkC.h"
#include "neworkC.h"
#include "nbnkC.h"
#include "skonl/softtrg_cond.h"
#include "skbadcC.h"

#include "SKLibs.hh"
#include "SKIO.hh"
#include "GetStopMuVertex.hh"
#include "NTagBankIO.hh"
#include "Calculator.hh"
#include "NoiseManager.hh"
#include "EventNTagManager.hh"

EventNTagManager::EventNTagManager(Verbosity verbose)
: fOutDataFile(nullptr), fNoiseManager(nullptr),
  fIsBranchSet(false), fIsMC(true), fDoAutoRefRun(true), fFileFormat(mZBS)
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
    if (mode == mNONE) {
        fPromptVertex = TVector3(fSettings.GetFloat("vx"), fSettings.GetFloat("vy"), fSettings.GetFloat("vz"));
    }

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
        fEventVariables.Set("ring_dirx", apcommul_.apdir[0][0]);
        fEventVariables.Set("ring_diry", apcommul_.apdir[0][1]);
        fEventVariables.Set("ring_dirz", apcommul_.apdir[0][2]);

        // visible energy
        fEventVariables.Set("EVis", apcomene_.apevis);
    }

    else if (mode == mBONSAI && fFileFormat == mZBS) {
        lowbs3get_();
        fPromptVertex = TVector3(skroot_lowe_.bsvertex[0], skroot_lowe_.bsvertex[1], skroot_lowe_.bsvertex[2]);
        fEventVariables.Set("ring_dirx", skroot_lowe_.bsdir[0]);
        fEventVariables.Set("ring_diry", skroot_lowe_.bsdir[1]);
        fEventVariables.Set("ring_dirz", skroot_lowe_.bsdir[2]);
        fEventVariables.Set("BSgoodness", skroot_lowe_.bsgood[1]);
        fEventVariables.Set("BSdirks", skroot_lowe_.bsdirks);
        fEventVariables.Set("BSenergy", skroot_lowe_.bsenergy);
        fEventVariables.Set("BSn50", skroot_lowe_.bsn50);
        fEventVariables.Set("BSovaq", skroot_lowe_.bsgood[1]*skroot_lowe_.bsgood[1]-skroot_lowe_.bsdirks*skroot_lowe_.bsdirks);

        int isFromIDWall = 1;
        float vertex[3] = {skroot_lowe_.bsvertex[0], skroot_lowe_.bsvertex[1], skroot_lowe_.bsvertex[2]};
        float pointOnWall[3] = {0, 0, 0};
        fEventVariables.Set("EffWall", effwallf_(&isFromIDWall, vertex, skroot_lowe_.bsdir, pointOnWall));
    }

    else if (mode == mBONSAI && fFileFormat == mSKROOT) {
        int lun = 10;
        TreeManager* mgr = skroot_get_mgr(&lun);
        LoweInfo* LOWE = mgr->GetLOWE();
        mgr->GetEntry();
        fPromptVertex = TVector3(LOWE->bsvertex);
        fEventVariables.Set("Energy", LOWE->bsenergy);
    }

    else if (mode == mFITQUN) {
        int fqbank = 0;
        readfqzbsbank_(&fqbank);
        fPromptVertex = TVector3(fitqunmr_.fqmrpos[0][0]);

        // ring
        fEventVariables.Set("NRing", fitqunmr_.fqmrnring[0]);
        fEventVariables.Set("FirstRingType", fitqunmr_.fqmrpid[0][0]);
        fEventVariables.Set("FirstRingMom", fitqunmr_.fqmrmom[0][0]);
        fEventVariables.Set("ring_dirx", fitqunmr_.fqmrdir[0][0][0]);
        fEventVariables.Set("ring_diry", fitqunmr_.fqmrdir[0][0][1]);
        fEventVariables.Set("ring_dirz", fitqunmr_.fqmrdir[0][0][2]);

        // visible energy
        fEventVariables.Set("EVis", fitqunmr_.fqmreloss[0][0]);
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
        fPromptVertex = TVector3(skvect_.pos);
    }

    else if (mode == mSTMU) {
        fPromptVertex = GetStopMuVertex();
    }

    else {
        fMsg.Print("Invalid prompt vertex mode, setting vertex mode to NONE...", pWARNING);
        fPromptVertexMode = mNONE;
    }

    // vertex smearing
    //float dx = 2*RINTK, dy = 2*RINTK, dz = 2*ZPINTK;
    //float maxDist = 150.;
    //while (Norm(dx, dy, dz) > maxDist) {
    //  dx = gRandom->BreitWigner(0, PVXRES);
    //  dy = gRandom->BreitWigner(0, PVXRES);
    //  dz = gRandom->BreitWigner(0, PVXRES);
    //}
    float dx = gRandom->Uniform() > 0.5 ? gRandom->Exp(PVXRES) : -gRandom->Exp(PVXRES);
    float dy = gRandom->Uniform() > 0.5 ? gRandom->Exp(PVXRES) : -gRandom->Exp(PVXRES);
    float dz = gRandom->Uniform() > 0.5 ? gRandom->Exp(PVXRES) : -gRandom->Exp(PVXRES);

    auto biasDir= TVector3(gRandom->Uniform(), gRandom->Uniform(), gRandom->Uniform()).Unit();
    if (fPromptVertexMode == mAPFIT || fPromptVertexMode == mFITQUN)
        biasDir = TVector3(fEventVariables.GetFloat("ring_dirx"),
                           fEventVariables.GetFloat("ring_diry"),
                           fEventVariables.GetFloat("ring_dirz"));

    fPromptVertex = fPromptVertex + TVector3(dx, dy, dz) + PVXBIAS*biasDir;
}

void EventNTagManager::ReadVariables()
{
    // debug: read ntag bank
    //ReadNTAGBank();
    //std::cout << "Read following NTAG bank\n";
    //DumpNTAGBank();

    // run/event information
    fEventVariables.Set("RunNo", skhead_.nrunsk);
    fEventVariables.Set("SubrunNo", skhead_.nsubsk);
    fEventVariables.Set("EventNo", skhead_.nevsk);

    //fEventHits.DumpAllElements();
    int nhitac; odpc_2nd_s_(&nhitac);
    fEventVariables.Set("NHITAC", nhitac);

    // trigger information
    int trgtype = ((skhead_.idtgsk & 1<<29) ? tAFT : 
                  ((skhead_.idtgsk & 1<<28) ? tSHE : 
                  ((skhead_.idtgsk & 1<< 1) ?  tHE : tELSE)));
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

        // trgOffset in usec
        fEventVariables.Set("MCT0", SKIO::GetMCTriggerOffset(fFileFormat)*1e-3);

        // NEUT
        float posnu[3]{0., 0., 1e5+1}; nerdnebk_(posnu);
        if (posnu[2] < 1e5) {
            fSettings.Set("neut", true);
            auto nuMomVec = TVector3(nework_.pne[0]);
            auto nuDirVec = nuMomVec.Unit();
            fEventVariables.Set("NEUTMode", nework_.modene);
            fEventVariables.Set("NuType", nework_.ipne[0]);
            fEventVariables.Set("NuMom", nuMomVec.Mag());
            fEventVariables.Set("nu_dirx", nuDirVec.x());
            fEventVariables.Set("nu_diry", nuDirVec.y());
            fEventVariables.Set("nu_dirz", nuDirVec.z());
        }
    }
}

void EventNTagManager::ReadHits()
{
    fEventHits = PMTHitCluster(sktqz_);
    fEventHits.Sort();

    fEventODHits = PMTHitCluster(sktqaz_);
    fEventODHits.Sort();
}

void EventNTagManager::AddHits()
{
    /*
    float tOffset = 0.;
    PMTHit lastHit(std::numeric_limits<Float>::lowest(), 0, 0, 0);

    bool coincidenceFound = true;

    if (!fEventHits.IsEmpty()) {
        coincidenceFound = false;
        lastHit = fEventHits[fEventHits.GetSize()-1];
    }

    // getting time offset between SHE and AFT events:
    // look for an identical hit in the two events, with the same PMT ID and the deposit charge
    if (!coincidenceFound) {
        for (int iHit = 0; iHit < sktqz_.nqiskz; iHit++) {
            if (static_cast<unsigned int>(sktqz_.icabiz[iHit]) == lastHit.i()) {
                fMsg.Print(Form("Same PMT T: %3.2f ns Q: %3.9f pe, I: %d", sktqz_.tiskz[iHit], sktqz_.qiskz[iHit], sktqz_.icabiz[iHit]), pDEBUG);
            }
            // hit charges coincide within 5% (temp) or both are negative
            if (((fabs(sktqz_.qiskz[iHit]-lastHit.q()) < lastHit.q() * 5e-2) || (lastHit.q()<0 && sktqz_.qiskz[iHit]<0))
             && static_cast<unsigned int>(sktqz_.icabiz[iHit]) == lastHit.i() // PMT ID identical
             && fabs(lastHit.t() - sktqz_.tiskz[iHit] - 35000) < 500) {       // PMT timing offset within 500 ns from 35 usec
                tOffset = lastHit.t() - sktqz_.tiskz[iHit];
                coincidenceFound = true;
                fMsg.Print(Form("Coincidence found: t = %3.2f ns, (offset: %3.2f ns)", lastHit.t(), tOffset));
                break;
            }
        }
        if (!coincidenceFound) {
            fMsg.Print("Coincidence not found! Setting AFT tOffset to 35100 ns...", pWARNING);
            tOffset = 35100;
            fMsg.Print(Form("Last hit from the previous event: T: %3.2f ns Q: %3.9f pe, I: %d", lastHit.t(), lastHit.q(), lastHit.i()), pWARNING);
        }
    }

    //fMsg.Print(Form("TDiff using it0sk: %3.2f nsec", tdiff));
    */

    PMTHit lastHit(std::numeric_limits<Float>::lowest(), 0, 0, 0);
    PMTHit lastODHit(std::numeric_limits<Float>::lowest(), 0, 0, 0);
    if (!fEventHits.IsEmpty()) {
        lastHit = fEventHits.GetLastHit();
        lastODHit = fEventODHits.GetLastHit();
    }

    static int it0sk_prev = 0;
    float tOffset = fEventHits.IsEmpty() ? 0 : (skheadqb_.it0sk - it0sk_prev) / 1.92;
    it0sk_prev = skheadqb_.it0sk;

    fMsg.Print(Form("tOffset_it0sk: %3.2f ns", tOffset), pDEBUG);

    //fEventHits.Append(PMTHitCluster(sktqz_) + tOffset, true);
    //fEventODHits.Append(PMTHitCluster(sktqaz_) + tOffset, true);

    if (fEventHits.IsEmpty()) {
        fEventHits.Append(PMTHitCluster(sktqz_) + tOffset, true);
        fEventODHits.Append(PMTHitCluster(sktqaz_) + tOffset, true);
    }
    else {
        auto hitsToAdd = PMTHitCluster(sktqz_) + tOffset;
        auto odHitsToAdd = PMTHitCluster(sktqaz_) + tOffset;

        fEventHits.AppendByCoincidence(hitsToAdd);
        fEventODHits.AppendByCoincidence(odHitsToAdd);
    }

    fEventHits.Sort();
    fEventODHits.Sort();
}

void EventNTagManager::AddNoise()
{
    fNoiseManager->AddIDNoise(&fEventHits);

    // replace OD hits with dark noise from dummy trigger data
    fEventODHits.Clear();
    fNoiseManager->AddODNoise(&fEventODHits);
}

void EventNTagManager::ReadParticles()
{
    skgetv_();
    SKIO::SetSecondaryCommon(fFileFormat);
    fEventParticles.ReadCommonBlock(skvect_, secndprt_);

    float geantT0 = fEventVariables.GetFloat("MCT0", 0);
    fEventParticles.SetT0(geantT0);

    fEventTaggables.ReadParticleCluster(fEventParticles);
    fEventTaggables.SetPromptVertex(fPromptVertex);
}

void EventNTagManager::ReadEarlyCandidates()
{
    float parentPeakTime;
    int silence = 1;
    muechk_gate_(apcommul_.appos, parentPeakTime, silence);
    int bank = 0; apclrmue_(); apgetmue_(&bank);

    // check if mue bank exists
    if (apmue_.apmuever) {
        for (int iMuE=0; apmue_.apmuenhit[iMuE]>0; iMuE++) {
            float fitT = parentPeakTime*1e-3 + apmue_.apmuetime[iMuE] - 1;
            if (fitT < (T0TH-1000)*1e-3) {
                Candidate candidate;
                candidate.Set("FitT", fitT);
                candidate.Set("fvx", apmue_.apmuepos[iMuE][0]);
                candidate.Set("fvy", apmue_.apmuepos[iMuE][1]);
                candidate.Set("fvz", apmue_.apmuepos[iMuE][2]);
                candidate.Set("dirx", apmue_.apmuedir[iMuE][0]);
                candidate.Set("diry", apmue_.apmuedir[iMuE][1]);
                candidate.Set("dirz", apmue_.apmuedir[iMuE][2]);
                candidate.Set("DWall", wallsk_(apmue_.apmuepos[iMuE]));
                candidate.Set("NHits", apmue_.apmuenhit[iMuE]);
                candidate.Set("GateType", apmue_.apmuetype[iMuE]);
                candidate.Set("Goodness", apmue_.apmuegood[iMuE]);
                candidate.Set("TagClass", fTagger.Classify(candidate));
                fEventEarlyCandidates.Append(candidate);
            }
        }
    }
}

void EventNTagManager::ReadInfoFromCommon()
{
    ReadVariables();

    if (fIsMC) 
        ReadParticles();
    if (fSettings.GetBool("neut",false))
        ReadEarlyCandidates();
}

void EventNTagManager::ReadEventFromCommon()
{
    AddHits();
    if (fSettings.GetBool("add_noise", false)) {
        fEventHits.SetAsSignal(true);
        AddNoise();
    }
    ReadInfoFromCommon();
}

void EventNTagManager::SearchAndFill()
{
    PrepareEventHits();

    int nhitac = fEventVariables.GetInt("NHITAC");
    int nodhitmx = fSettings.GetInt("NODHITMX");
    if (nhitac > nodhitmx) {
        fMsg.Print(Form("%d OD hits in this event (allowed: NHITODMX = %d)...", nhitac, nodhitmx), pWARNING);
        fMsg.Print(Form("Skipping search for this event (EventNo: %d)", fEventVariables.GetInt("EventNo")), pWARNING);
    }
    else {
        SearchCandidates();
    }

    fEventVariables.Set("NCandidates", fEventCandidates.GetSize()+fEventEarlyCandidates.GetSize());

    FillNTagCommon();
    DumpEvent();
    FillTrees();
    if (fSettings.GetBool("write_bank")) {
        FillNTAGBank();
        //std::cout << "Filling following NTAG bank\n";
        //DumpNTAGBank();
    }
    if (fOutDataFile) {
        fEventHits.RemoveVertex();
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
        auto nnType = fSettings.GetString("NN_type");
        auto weightPath = fSettings.GetString("weight");
        if (nnType=="tmva") {
            if (weightPath=="default")
                weightPath = fSettings.GetString("delayed_vertex");
            fTMVAManager.InitializeReader(weightPath);
        }
        else if (nnType=="keras") {
            if (weightPath=="default") {
                weightPath = GetENV("NTAGLIBPATH")+Form("weights/keras/sk%d", SKIO::GetSKGeometry());
            }
            fKerasManager.LoadWeights(weightPath);
        }
        initialized = true;
    }

    if (fIsMC || fSettings.GetBool("force_flat"))
        ProcessFlatEvent();
    else
        ProcessDataEvent();
}

void EventNTagManager::ProcessDataEvent()
{
    static int prevEvTrg = tELSE;
    int thisEvTrg = ((skhead_.idtgsk & (1<<29)) ? tAFT : ((skhead_.idtgsk & (1<<28)) ? tSHE : tELSE));
    //fMsg.Print(Form("This evtrg: %d", thisEvTrg), pWARNING);

    // if current event is AFT, append TQ and fill output.
    if (thisEvTrg == tAFT) {
        if (prevEvTrg == tSHE) {
            //fMsg.Print("Appending AFT to previous SHE", pWARNING);
            fEventVariables.Set("TrgType", thisEvTrg);
            AddHits();
            SearchAndFill();
        }
        else thisEvTrg = tELSE;
    }

    // if previous event was SHE without following AFT,
    // just fill output because there's nothing to append.
    else if (!fEventHits.IsEmpty()) {
        //fMsg.Print("Processing previous SHE", pWARNING);
        SearchAndFill();
    }

    // if the current event is SHE,
    // save raw hit info and don't fill output.
    if (thisEvTrg == tSHE) {
        //fMsg.Print("Skipping current SHE", pWARNING);
        ReadEventFromCommon();
        //DumpEvent();
    }

    // if the current event is neither SHE nor AFT (e.g. HE, LE, etc.),
    // save raw hit info and fill output.
    if (thisEvTrg == tELSE) {
        //fMsg.Print("ELSE", pWARNING);
        ProcessFlatEvent();
    }

    prevEvTrg = thisEvTrg;
}

void EventNTagManager::ProcessFlatEvent()
{
    ReadEventFromCommon();
    SearchAndFill();
}

void EventNTagManager::SearchCandidates()
{
    int   iHitPrevious    = -1;
    int   NHitsNew        = 0;
    int   NHitsPrevious   = 0;
    int   N200Previous    = 0;
    Float t0Previous      = std::numeric_limits<Float>::min();

    int nEventHits = fEventVariables.GetInt("NAllHits");
    int nIDHitsMax = fSettings.GetInt("NIDHITMX", std::numeric_limits<int>::max());

    //fEventHits.DumpAllElements();

    if (nEventHits > nIDHitsMax) {
        fMsg.Print(Form("Skipping event with ID number of hits %ld > NIDHITMX %ld", nEventHits, nIDHitsMax), pWARNING);
    }
    else {
        // Loop over the saved TQ hit array from current event
        for (unsigned int iHit = 0; iHit < fEventHits.GetSize(); iHit++) {

            PMTHitCluster hitsInTCANWIDTH = fEventHits.Slice(iHit, TWIDTH);

            // If (ToF-subtracted) hit comes earlier than T0TH or later than T0MX, skip:
            Float firstHitTime = hitsInTCANWIDTH[0].t();
            if (firstHitTime < T0TH || firstHitTime > T0MX) continue;

            // Calculate NHitsNew:
            // number of hits within TWIDTH (ns) from the i-th hit
            int NHits_iHit = hitsInTCANWIDTH.GetSize();

            // Pass only if NHITSTH <= NHits_iHit <= NHITSMX:
            if (NHits_iHit < NHITSTH) continue;
            if (NHits_iHit > NHITSMX) {
                fMsg.Print(Form("Encountered a candidate with NHits=%d at T=%3.2f usec (>NHITSMX=%d), skipping...",
                                NHits_iHit, firstHitTime*1e-3, NHITSMX), pDEBUG);
            }

            // We've found a new peak.
            NHitsNew = NHits_iHit;
            Float t0New = firstHitTime;

            // Calculate N200
            PMTHitCluster hitsIn200ns = fEventHits.Slice(iHit, TWIDTH/2.-100, TWIDTH/2.+100);
            int N200New = hitsIn200ns.GetSize();

            // If peak t0 diff = t0New - t0Previous > TMINPEAKSEP, save the previous peak.
            // Also check if N200Previous is below N200 cut and if t0Previous is over t0 threshold
            if (t0New - t0Previous > TMINPEAKSEP) {
                if (iHitPrevious >= 0 && N200Previous < N200MX && t0Previous > T0TH) {
                    FindDelayedCandidate(iHitPrevious);
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
            FindDelayedCandidate(iHitPrevious);
    }
    if (!fEventEarlyCandidates.IsEmpty()) PruneCandidates();
    /*if (fIsMC)*/  MapTaggables();

    fEventEarlyCandidates.FillVectorMap();
    fEventCandidates.FillVectorMap();
}

void EventNTagManager::MapTaggables()
{
    Map(fEventTaggables, fEventEarlyCandidates, TMATCHWINDOW);
    Map(fEventTaggables, fEventCandidates, TMATCHWINDOW);
}

void EventNTagManager::ResetTaggableMapping(TaggableCluster& taggableCluster)
{
    for (auto& taggable: taggableCluster) {
        taggable.SetCandidateIndex("Early", -1);
        taggable.SetCandidateIndex("Delayed", -1);
        taggable.SetTaggedType(typeMissed);
    }
}

void EventNTagManager::ResetCandidateClass(CandidateCluster& candidateCluster)
{
    for (auto& candidate: candidateCluster) {
        candidate.Set("TagIndex", -1);
        candidate.Set("Label", lNoise);
        candidate.Set("TagClass", typeMissed);
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

    if (fSettings.GetBool("debug")) fMsg.SetVerbosity(pDEBUG);

    fSettings.Set("SKGEOMETRY", SKIO::GetSKGeometry());

    // NTag parameters
    float tMin = fSettings.GetFloat("TMIN");
    float tMax = fSettings.GetFloat("TMAX");
    T0TH = tMin * 1e3 + 1000; T0MX = tMax * 1e3 + 1000; // ns->us, add trigger time T=1000 ns
    fSettings.Get("TWIDTH", TWIDTH);
    fSettings.Get("TCANWIDTH", TCANWIDTH);
    fSettings.Get("TMINPEAKSEP", TMINPEAKSEP);
    fSettings.Get("TMATCHWINDOW", TMATCHWINDOW);
    fSettings.Get("TRBNWIDTH", TRBNWIDTH);
    fSettings.Get("NHITSTH", NHITSTH);
    fSettings.Get("NHITSMX", NHITSMX);
    fSettings.Get("MINNHITS", MINNHITS);
    fSettings.Get("MAXNHITS", MAXNHITS);
    fSettings.Get("N200TH", N200TH);
    fSettings.Get("N200MX", N200MX);
    fSettings.Get("PVXRES", PVXRES);
    fSettings.Get("PVXBIAS", PVXBIAS);

    // tagging conditions
    fTagger.SetECuts(fSettings.GetString("E_CUTS"));
    fTagger.SetNCuts(fSettings.GetString("N_CUTS"));

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
    else if (fDelayedVertexMode == mLOWFIT) {
        fBonsaiManager.UseLOWFIT(true, fSettings.GetInt("REFRUNNO"));
        fDelayedVertexManager = &fBonsaiManager;
    }
    else if (fPromptVertexMode == mNONE) {
        fMsg.Print("Prompt vertex mode is set to \"none\". TWIDTH > 100 ns and NHITSTH > 10 recommended.", pWARNING);
        fMsg.Print("The correct_tof option is set to false.", pWARNING);
        fSettings.Set("correct_tof", false);
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
    fSettings.Get("VTXMAXRADIUS", VTXMAXRADIUS);

    fTRMSFitManager.SetParameters(INITGRIDWIDTH, MINGRIDWIDTH, GRIDSHRINKRATE, VTXMAXRADIUS);
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
    else if (key == "lowfit")
        mode = mLOWFIT;
    else
        fMsg.Print("Vertex mode " + key + " is not a proper mode name!", pERROR);
}

void EventNTagManager::MakeTrees(TFile* outfile)
{
    if (outfile) outfile->cd();

    TTree* settingsTree = new TTree("settings", "settings");
    TTree* eventTree    = new TTree("event", "event");
    TTree* hitTree      = new TTree("hit", "hit");
    TTree* particleTree = new TTree("particle", "particle");
    TTree* taggableTree = new TTree("taggable", "taggable");
    TTree* nTree        = new TTree("ntag", "ntag");
    TTree* eTree        = new TTree("mue", "mue");

    if (outfile) {
        settingsTree->SetDirectory(outfile);
        eventTree->SetDirectory(outfile);
        hitTree->SetDirectory(outfile);
        particleTree->SetDirectory(outfile);
        taggableTree->SetDirectory(outfile);
        nTree->SetDirectory(outfile);
        eTree->SetDirectory(outfile);
    }

    fSettings.SetTree(settingsTree);
    fEventVariables.SetTree(eventTree);
    if (fSettings.GetBool("save_hits", true)) fEventHits.SetTree(hitTree);
    fEventParticles.SetTree(particleTree);
    fEventTaggables.SetTree(taggableTree);
    fEventCandidates.SetTree(nTree);
    fEventEarlyCandidates.SetTree(eTree);
}

void EventNTagManager::FillTrees()
{
    // set branch address for the first event
    if (!fIsBranchSet) {
        // make branches
        fSettings.MakeBranches();
        fEventVariables.MakeBranches();
        fEventHits.MakeBranches();
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
    fEventHits.FillTree(fSettings.GetString("save_hits")=="residual");
    fEventParticles.FillTree();
    fEventTaggables.FillTree();
    fEventEarlyCandidates.FillTree();
    fEventCandidates.FillTree();
}

void EventNTagManager::WriteTrees(bool doCloseFile)
{
    auto outFile = fEventCandidates.GetTree()->GetCurrentFile();
    outFile->cd();
    fSettings.WriteTree();
    fEventVariables.WriteTree();
    fEventHits.WriteTree();
    fEventParticles.WriteTree();
    fEventTaggables.WriteTree();
    fEventEarlyCandidates.WriteTree();
    fEventCandidates.WriteTree();
    if (doCloseFile) outFile->Close();
}

void EventNTagManager::ClearData()
{
    fEventVariables.Clear();
    fEventHits.Clear();
    fEventODHits.Clear();
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
                                           "Label", "TagIndex", "fvx", "fvy", "fvz", "DTaggable", "TagClass"});
    if (fSettings.GetBool("print", true)) {
        fEventCandidates.DumpAllElements(Split(fSettings.GetString("print"), ","));
    }
}

void EventNTagManager::CheckMC()
{
    if (skhead_.mdrnsk == 0)
        fIsMC = true;
    else
        fIsMC = false;
}

void EventNTagManager::ResetEventHitsVertex()
{
    if (fSettings.GetBool("correct_tof", true))
        fEventHits.SetVertex(fPromptVertex);
    else
        fEventHits.RemoveVertex();
}

void EventNTagManager::PrepareEventHits()
{
    // deadtime, burst noise
    fEventHits.ApplyDeadtime(900., true);
    if (TRBNWIDTH > 0) fEventHits.ApplyDeadtime(TRBNWIDTH, false);

    ResetEventHitsVertex();
    fEventODHits.RemoveNegativeHits();

    // fetch bad channels, dark rates
    FindReferenceRun();

    int nBadIDPMTs = 0; int nBadODPMTs = 0; 
    int nBadIDHits = 0; int nBadODHits = 0; 

    int allIDSize = fEventHits.SliceRange(T0TH, T0MX).GetSize();
    int allODSize = fEventODHits.SliceRange(T0TH, T0MX).GetSize();

    // remove bad tube hits
    if (TString(fSettings.GetString("SKOPTN")).Contains("25")) {
        fEventHits.RemoveBadChannels();
        fEventODHits.RemoveBadChannels();

        nBadIDPMTs = combad_.nbad;
        nBadODPMTs = combada_.nbada;
        int redIDSize = fEventHits.SliceRange(T0TH, T0MX).GetSize();
        int redODSize = fEventODHits.SliceRange(T0TH, T0MX).GetSize();
        nBadIDHits = allIDSize - redIDSize;
        nBadODHits = allODSize - redODSize;

        std::vector<std::string> badTypes;

        int skbadopt = fSettings.GetInt("SKBADOPT", -1);
        if (!skbadopt | skbadopt & (1<<0)) badTypes.push_back("bad");
        if (!skbadopt | skbadopt & (1<<1)) badTypes.push_back("dead1");
        if (!skbadopt | skbadopt & (1<<2)) badTypes.push_back("dead2");
        if (!skbadopt | skbadopt & (1<<3)) badTypes.push_back("noisy");
        if (!skbadopt | skbadopt & (1<<5)) badTypes.push_back("HK");

        std::cout << "\n";
        fMsg.Print(Form("BADSEL reference run: %d", fEventVariables.GetInt("RefRunNo")));
        fMsg.Print(Form("# of bad ID PMTs: %d ( ", nBadIDPMTs), pDEFAULT, false);
        for (auto const& btype: badTypes) std::cout << btype << " ";
        std::cout << ")\n";

        fMsg.Print(Form("# of bad OD PMTs: %d", nBadODPMTs));
        fMsg.Print(Form("Removed %d bad ID tube hits in search range: total %d -> %d hits", nBadIDHits, allIDSize, redIDSize));
        fMsg.Print(Form("Removed %d bad OD tube hits in search range: total %d -> %d hits", nBadODHits, allODSize, redODSize));
        std::cout << "\n";

        SKIO::ResetBadChannels();

        allIDSize = redIDSize;
        allODSize = redODSize;
    }

    fEventVariables.Set("NBadPMTs", nBadIDPMTs);
    fEventVariables.Set("NBadHits", nBadIDHits);
    fEventVariables.Set("NAllHits", allIDSize);
    fEventVariables.Set("NAllODHits", allODSize);

    // qismsk
    float qismsk = 0;
    float tGateMin = fSettings.GetFloat("TGATEMIN")*1e3 + 1000.;
    float tGateMax = fSettings.GetFloat("TGATEMAX")*1e3 + 1000.;
    for (auto const& hit: fEventHits) {
        float hitTime = hit.t();
        if (tGateMin < hitTime && hitTime < tGateMax) {
            qismsk += hit.q();
        }
    }

    // qismsk = skq_.qismsk;
    fEventVariables.Set("QISMSK", qismsk);

    // ODMaxN200
    auto odT = fEventODHits.GetProjection(HitFunc::T);
    int nBins = int((T0MX-T0TH)/200.);
    float remainderT = fmod(T0MX-T0TH, 200.);
    auto odHist = Histogram(odT, nBins, T0TH+remainderT/2., T0MX-remainderT/2.);
    int odMaxN200 = 0;
    for (auto const& pair: odHist) {
        if (pair.second > odMaxN200) odMaxN200 = pair.second;
    }
    fEventVariables.Set("ODMaxN200", odMaxN200);
}

//void EventNTagManager::SetToF(const TVector3& vertex)
//{
//    fEventHits.SetVertex(vertex);
//}
//
//void EventNTagManager::UnsetToF()
//{
//    fEventHits.RemoveVertex();
//}

void EventNTagManager::FindDelayedCandidate(unsigned int iHit)
{
    PMTHit firstHit = fEventHits[iHit];
    auto trgHits = fEventHits.Slice(iHit, TWIDTH);

    // set default values for delayed candidate properties
    TVector3 delayedVertex = fPromptVertex;

    Float delayedTime = firstHit.t() + TWIDTH/2.;
    float delayedGoodness = 0;

    bool doFit = true;

    // prompt mode: delayed vertex = prompt vertex
    if (fDelayedVertexMode == mPROMPT) {
        if (fPromptVertexMode != mNONE)
            delayedGoodness = fDelayedVertexManager->GetGoodness(trgHits, fPromptVertex, delayedTime);
        else
            fMsg.Print("MODE ERROR: Prompt vertex mode is NONE while delayed vertex mode is PROMPT!", pERROR);
    }

    // delayed mode: apply delayed vertex fit
    else {
        PMTHitCluster hitsForFit;

        // TRMS-fit
        if (fDelayedVertexMode == mTRMS)
            hitsForFit = fEventHits.Slice(iHit, (TWIDTH-TRMSTWIDTH)/2., (TWIDTH+TRMSTWIDTH)/2.) - firstHit.t() + 1000;

        // BONSAI
        else if (fDelayedVertexMode == mBONSAI || fDelayedVertexMode == mLOWFIT) {
            fEventHits.RemoveVertex();
            fEventHits.Sort();
            firstHit.UnsetToFAndDirection();
            unsigned int firstHitID = fEventHits.GetIndex(firstHit);
            Float tLeft  = fDelayedVertexMode == mLOWFIT ? -520 : -500;
            Float tRight = fDelayedVertexMode == mLOWFIT ?  780 : 1000;
            hitsForFit = fEventHits.Slice(firstHitID, TWIDTH/2.+tLeft, TWIDTH/2.+tRight) - firstHit.t() + 1000;

            // give up bonsai fit for N1300 larger than 2000
            auto nHitsForFit = hitsForFit.GetSize();
            if (nHitsForFit > 2000) {
                fMsg.Print(Form("A possible candidate at T=%3.2f us has N%d=%d that is larger than 2000,"
                                " giving up fit and setting the delayed vertex the same as the prompt...", firstHit.t()*1e-3, int(tRight-tLeft), nHitsForFit), pWARNING);
                doFit = false;
            }
        }

        if (doFit) {
            hitsForFit.Sort();
            fDelayedVertexManager->Fit(hitsForFit);
            delayedVertex   = fDelayedVertexManager->GetFitVertex();
            delayedTime     = fDelayedVertexManager->GetFitTime() + firstHit.t() - 1000;
            delayedGoodness = fDelayedVertexManager->GetFitGoodness();
        }
    }

    if (doFit || fSettings.GetBool("correct_tof")) {
        fEventHits.SetVertex(delayedVertex);
        firstHit.SetToFAndDirection(delayedVertex);
    }

    Float lastCandidateTime = fEventCandidates.GetSize() ? fEventCandidates.Last().Get("FitT")*1e3 + 1000 : std::numeric_limits<Float>::lowest();
    // fitted time should not be too far off from the first hit time
    // to prevent double counting of same hits
    if (fabs(delayedTime-firstHit.t()) < TMINPEAKSEP &&
        fabs(delayedTime-lastCandidateTime) > TMINPEAKSEP &&
        T0TH < delayedTime && delayedTime < T0MX) {
        //iHit = fEventHits.GetLowerBoundIndex(delayedTime);
        //unsigned int nHits = fEventHits.Slice(iHit, -TCANWIDTH/2., TCANWIDTH/2.).GetSize();
        // -0.03 is to ensure that hit at index == iHit is included in nHits
        unsigned int nHits = fEventHits.SliceRange(delayedTime, -TCANWIDTH/2.-0.03, TCANWIDTH/2.).GetSize();

        if (nHits >= MINNHITS && nHits <= MAXNHITS) {
            Candidate candidate(iHit);
            candidate.Set("FitT", (delayedTime-1000)*1e-3); // -1000 ns is to offset the trigger time T=1000 ns
            candidate.Set("FitGoodness", delayedGoodness);
            candidate.Set("BSenergy", fBonsaiManager.GetFitEnergy());
            candidate.Set("BSdirks", fBonsaiManager.GetFitDirKS());
            candidate.Set("BSovaq", fBonsaiManager.GetFitOvaQ());
            FindFeatures(candidate, delayedTime);
            fEventCandidates.Append(candidate);
        }
    }

    ResetEventHitsVertex();
}

void EventNTagManager::FindFeatures(Candidate& candidate, Float canTime)
{
    //unsigned int firstHitID = candidate.HitID();
    //float fitTime = candidate.Get("FitT")*1e3 + 1000;
    auto hitsInTCANWIDTH = fEventHits.SliceRange(canTime, -TCANWIDTH/2.-0.03, TCANWIDTH/2.);
    auto hitsIn30ns      = fEventHits.SliceRange(canTime,                -15,          +15);
    auto hitsIn50ns      = fEventHits.SliceRange(canTime,                -25,          +25);
    auto hitsIn200ns     = fEventHits.SliceRange(canTime,               -100,         +100);
    auto hitsIn1300ns    = fEventHits.SliceRange(canTime,               -520,         +780);
    auto hitsIn3000ns    = fEventHits.SliceRange(canTime,               -520,        +2480);

    //std::cout << "\n";
    //fMsg.Print(Form("Candidate found!"));
    //hitsInTCANWIDTH.DumpAllElements();

    //auto hitsInTCANWIDTH = fEventHits.Slice(firstHitID, TWIDTH);
    //auto hitsIn50ns   = fEventHits.Slice(firstHitID, TWIDTH/2.-25, TWIDTH/2.+ 25);
    //auto hitsIn200ns  = fEventHits.Slice(firstHitID, TWIDTH/2.-100, TWIDTH/2.+100);
    //auto hitsIn1300ns = fEventHits.Slice(firstHitID, TWIDTH/2.-520, TWIDTH/2.+780);

    //hitsInTCANWIDTH.FindHitProperties();
    //hitsInTCANWIDTH.DumpAllElements();
    //float deg = 3.141592/180.;
    //unsigned int nClusHits = hitsInTCANWIDTH.Slice(HitFunc::MinAngle, 0, 14.1*deg).GetSize();
    //unsigned int nBackHits = hitsInTCANWIDTH.Slice(HitFunc::DirAngle, 90*deg, 180*deg).GetSize();
    //unsigned int nLowHits = hitsInTCANWIDTH.Slice(HitFunc::Acceptance, 90, 180).GetSize();
    //candidate.Set("NClusHits", nClusHits);
    //candidate.Set("NBackHits", nBackHits);

    // Delayed vertex
    auto delayedVertex = fEventHits.GetVertex();
    candidate.Set("fvx", delayedVertex.x());
    candidate.Set("fvy", delayedVertex.y());
    candidate.Set("fvz", delayedVertex.z());

    // Number of hits
    candidate.Set("NHits", hitsInTCANWIDTH.GetSize());
    candidate.Set("N30",   hitsIn30ns.GetSize());
    candidate.Set("N50",   hitsIn50ns.GetSize());
    candidate.Set("N200",  hitsIn200ns.GetSize());
    candidate.Set("N1300", hitsIn1300ns.GetSize());
    candidate.Set("N3000", hitsIn3000ns.GetSize());
    candidate.Set("NResHits", hitsIn200ns.GetSize()-hitsInTCANWIDTH.GetSize());

    // Time
    //float fitT = hitsInTCANWIDTH.Find(HitFunc::T, Calc::Mean) * 1e-3;
    //candidate.Set("FitT", candidate.Time());
    candidate.Set("TRMS", hitsInTCANWIDTH.Find(HitFunc::T, Calc::RMS));

    // Charge
    candidate.Set("QSum", hitsInTCANWIDTH.Find(HitFunc::Q, Calc::Sum));

    // Beta's
    auto beta = hitsInTCANWIDTH.GetBetaArray();
    candidate.Set("Beta1", beta[1]);
    candidate.Set("Beta2", beta[2]);
    candidate.Set("Beta3", beta[3]);
    candidate.Set("Beta4", beta[4]);
    candidate.Set("Beta5", beta[5]);

    // DWall
    auto dirVec = hitsInTCANWIDTH[HitFunc::Dir];

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
    auto openingAngleStats = hitsInTCANWIDTH.GetOpeningAngleStats();
    candidate.Set("OpeningAngleMean",  openingAngleStats.mean);
    candidate.Set("OpeningAngleStdev", openingAngleStats.stdev);
    candidate.Set("OpeningAngleSkew",  openingAngleStats.skewness);

    candidate.Set("DPrompt", fPromptVertexMode==mNONE && fPromptVertex==TVector3() ?
                             -1 : (fPromptVertex-delayedVertex).Mag());

    candidate.Set("SignalRatio", hitsInTCANWIDTH.GetSignalRatio());
    candidate.Set("NBurst", hitsInTCANWIDTH.GetNBurst());
    candidate.Set("BurstRatio", hitsInTCANWIDTH.GetBurstRatio());
    candidate.Set("DarkLikelihood", hitsInTCANWIDTH.GetDarkLikelihood());
    candidate.Set("NNoisyPMT", hitsInTCANWIDTH.GetNNoisyPMT());
    candidate.Set("NoisyPMTRatio", hitsInTCANWIDTH.GetNoisyPMTRatio());

    auto nnType = fSettings.GetString("NN_type");
    float tagOut = nnType=="tmva" ? fTMVAManager.GetTMVAOutput(candidate) :
                   nnType=="keras" ?  fKerasManager.GetOutput(candidate) : 0;

    candidate.Set("TagOut", tagOut);
    candidate.Set("TagClass", fTagger.Classify(candidate));
}

void EventNTagManager::Map(TaggableCluster& taggableCluster, CandidateCluster& candidateCluster, Float tMatchWindow)
{
    std::string key = candidateCluster.GetName();

    for (unsigned int iCandidate=0; iCandidate<candidateCluster.GetSize(); iCandidate++) {

        auto& candidate = candidateCluster[iCandidate];
        candidate.Set("TagIndex", -1);
        candidate.Set("DTaggable", -1);

        // default label: noise
        TrueLabel label = lNoise;
        //bool hasMatchingN = false;
        //bool hasMatchingE = false;
        //bool hasMatchingG = false;

        std::vector<int> taggableIndexList;
        std::vector<float> matchTimeList;

        //taggableIndexList.clear();
        //matchTimeList.clear();
        for (unsigned int iTaggable=0; iTaggable<taggableCluster.GetSize(); iTaggable++) {
            auto& taggable = taggableCluster[iTaggable];
            float tDiff = fabs(taggable.Time() - candidate["FitT"]);
            if (tDiff*1e3 < tMatchWindow) {
                matchTimeList.push_back(tDiff);
                taggableIndexList.push_back(iTaggable);
            }
        }

        if (!matchTimeList.empty()) {

            // closest taggable is matched to the given candidate
            int iMinMatchTimeCapture = taggableIndexList[GetMinIndex(matchTimeList)];
            auto& taggable = taggableCluster[iMinMatchTimeCapture];

            // candidate label determined by taggable type
            if (taggable.Type() == typeN) {
                //hasMatchingN = true;
                if (taggable.Energy() > 6.) label = lnGd;
                else                        label = lnH;
            }
            else if (taggable.Type() == typeE) {
                //hasMatchingE = true;
                label = lDecayE;
            }
            else if (taggable.Type() == typeG) {
                //hasMatchingG = true;
                label = lGamma;
            }

            // set candidate tagindex as the index of the closest taggable
            candidate.Set("TagIndex", iMinMatchTimeCapture);
            TVector3 fitVertex = TVector3(candidate.Get("fvx"), candidate.Get("fvy"), candidate.Get("fvz"));
            float dTaggable = (taggable.Vertex() - fitVertex).Mag();
            candidate.Set("DTaggable", fitVertex==TVector3()? -1: dTaggable);

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
                auto& givenCandidate = candidateCluster[iCandidate];
                auto& savedCandidate = candidateCluster[taggable.GetCandidateIndex(key)];
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

        //if (hasMatchingE && hasMatchingN)
        //    label = lUndefined;

        candidate.Set("Label", label);
    }
}

void EventNTagManager::FindReferenceRun()
{
    int refRunNo = fSettings.GetInt("REFRUNNO", 0);

    // refrunno == 0: auto-determine refrunno
    if (!refRunNo) {
        refRunNo = skhead_.mdrnsk ? skhead_.nrunsk :
                   (skhead_.nrunsk != 999999 ? skhead_.nrunsk :
                   ((fNoiseManager!=nullptr) ? fNoiseManager->GetCurrentRun() : 0)); 

        if (!refRunNo)
            fMsg.Print("Unable to determine reference run number for the input MC, " 
                       "specify reference run number with -REFRUNNO option.", pERROR);
    }

    int readStatus = SKIO::SetBadChannels(refRunNo, 0);

    // if bad channel list not found, search for closest noise run
    if (!readStatus) {
        int newRefRunNo = refRunNo;
        int step = 1; int sign = 1;
        while (!readStatus && step<100) {
            newRefRunNo -= sign*step;
            readStatus = SKIO::SetBadChannels(newRefRunNo, 0);
            sign *= -1; step += 1;
        }
        if (step>=100) {
            fMsg.Print(Form("Unable to fetch bad channel list for run %d", refRunNo), pERROR);
        }
        else {
            fMsg.Print(Form("Unable to fetch bad channel list for run %d, " 
                            "fetching from the closest noise run %d...", refRunNo, newRefRunNo), pWARNING);
        }
        refRunNo = newRefRunNo;
        SKIO::SetBadChannels(refRunNo, 0);
    }
    SKIO::SetBadChannels(refRunNo);

    fEventVariables.Set("RefRunNo", refRunNo);
}

void EventNTagManager::SetTaggedType(Taggable& taggable, Candidate& candidate)
{
    TaggableType tagClass = static_cast<TaggableType>((int)(candidate.Get("TagClass", -1)+0.5f));
    TaggableType tagType = taggable.TaggedType();

    // if candidate tag class undefined
    if (tagClass < 0);
    //     candidate.Set("TagClass", fTagger.Classify(candidate));
    // if candidate tag class defined
    else if (tagType != typeMissed && tagType != tagClass)
        taggable.SetTaggedType(typeMixed);
    else
        taggable.SetTaggedType(tagClass);
}

void EventNTagManager::PruneCandidates()
{
    std::vector<int> duplicateCandidateList;
    for (unsigned int iCandidate=0; iCandidate<fEventEarlyCandidates.GetSize(); iCandidate++) {
        auto& candidate = fEventEarlyCandidates[iCandidate];
        for (auto& delayed: fEventCandidates) {
            if (delayed["TagClass"] == typeE &&
                fabs(delayed["FitT"] - candidate["FitT"])*1e3 < 2*TMATCHWINDOW) {
                duplicateCandidateList.push_back(iCandidate); break;
            }
        }
    }

    for (auto& duplicateIndex: duplicateCandidateList)
        fEventEarlyCandidates.Erase(duplicateIndex);
}

void EventNTagManager::FillNTagCommon()
{
    int nCandidates = fEventCandidates.GetSize();
    int nTrueE = 0, nTaggedE = 0, nTrueN = 0, nTaggedN = 0;

    // count true
    if (fIsMC) {
        for (auto const& taggable: fEventTaggables) {
            if      (taggable.Type() == typeE) nTrueE++;
            else if (taggable.Type() == typeN) nTrueN++;
        }
        fEventVariables.Set("NTrueE", nTrueE);
        fEventVariables.Set("NTrueN", nTrueN);
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
        ntag_.n10[i] = candidate["NHits"];
        ntag_.ntime[i] = candidate["FitT"] * 1e3;
        ntag_.mctruth_neutron[i] = ntag_.np > MAXNP ? -1 : (label == lnH || label == lnGd ? 1 : 0);
        ntag_.goodness[i] = candidate["TagOut"];
        //ntag_.tag[i] = isTaggedOrNot;
        ntag_.tag[i] = candidate["TagClass"];
        i++;
    }

    // fill ntag bank: event variables
    ntag_.np = nCandidates;
    ntag_.trgtype = nTrueE; // temporarily save nTrueE in ntag_.trgtype for now
    ntag_.n200m = nTaggedE; // temporarily save nTaggedE ntag_.n200m for now
    ntag_.mctruth_nn = nTrueN;
    ntag_.nn = nTaggedN;

    // set event variables
    fEventVariables.Set("NTaggedE", nTaggedE);
    fEventVariables.Set("NTaggedN", nTaggedN);
}