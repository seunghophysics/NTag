#include <csignal>

#include <TFile.h>

#include <skheadC.h>
#include <skvectC.h>

#include "NTagPath.hh"
#include "NTagIO.hh"
#include "SKLibs.hh"

NTagIO* NTagIO::instance;

NTagIO::NTagIO(const char* inFileName, const char* outFileName, Verbosity verbose)
: NTagEventInfo(verbose), fInFileName(inFileName), fOutFileName(outFileName), lun(10)
{
    instance = this;

    bData = false;
    fVerbosity = verbose;

    outFile = new TFile(fOutFileName, "recreate");
    
    ntvarTree = new TTree("ntvar", "NTag variables");
    CreateBranchesToNtvarTree();

    truthTree = new TTree("truth", "True variables");
    CreateBranchesToTruthTree();
    
    rawtqTree = new TTree("rawtq", "Raw TQ");
    CreateBranchesToRawTQTree();
    
    restqTree = new TTree("restq", "Residual TQ");
    CreateBranchesToResTQTree();
    
    fSigTQFile = NULL; fSigTQTree = NULL;
}

NTagIO::~NTagIO()
{
    //if (ntvarTree) delete ntvarTree;
    //if (truthTree) delete truthTree;
    //if (restqTree) delete restqTree;
}

void NTagIO::Initialize()
{
    SKInitialize();
    OpenFile();
}

void NTagIO::SKInitialize()
{
    // Set SK options and SK geometry
    const char* skoptn = "31,30,26,25"; skoptn_(skoptn, strlen(skoptn));
    skheadg_.sk_geometry = 5; geoset_();

    // Initialize BONSAI
    kzinit_();
    bonsai_ini_();
}

void NTagIO::ReadFile()
{
    if (useTMVA)
        TMVATools.InstantiateReader();
    
    // SIGINT handler
    struct sigaction sigHandler;
    sigHandler.sa_handler = NTagIO::SIGINTHandler;
    sigemptyset(&sigHandler.sa_mask);
    sigHandler.sa_flags = 0;

    sigaction(SIGINT, &sigHandler, NULL);

    // Read data event-by-event
    int readStatus;
    bool bEOF = false;
    auto startTime = std::clock();

    while (!bEOF) {

        readStatus = skread_(&lun);
        CheckMC();

        switch (readStatus) {
            case 0: // event read
                std::cout << "\n" << std::endl;
                msg.Print("###########################", pDEBUG);
                msg.Print(Form("RUN %d EVENT %d", skhead_.nrunsk, skhead_.nevsk), pDEBUG);
                msg.Print(Form("Process No. %d", nProcessedEvents+1), pDEBUG);
                msg.Print("###########################", pDEBUG);

                // If MC
                if (!bData) {
                    int inPMT;
                    skgetv_();
                    inpmt_(skvect_.pos, inPMT);

                    // Skip event with vertex in PMT
                    if (inPMT) {
                        msg.Print(
                            Form("True vertex is in PMT. Skipping event %d...",
                                 nProcessedEvents+1), pDEBUG);
                        break;
                    }
                }

                ReadEvent();
                nProcessedEvents++;

                break;

            case 1: // read-error
                msg.Print("FILE READ ERROR OCCURED!", pERROR);
                break;

            case 2: // end of input
                msg.Print(Form("Reached the end of input. Closing file..."), pDEFAULT);
                CloseFile();
                bEOF = true;

                msg.Print(Form("Number of processed events: %d", nProcessedEvents), pDEFAULT);
                msg.Timer("Reading this file", startTime, pDEFAULT);
                break;
        }
    }
}

void NTagIO::ReadEvent()
{
    if (bData) ReadDataEvent(); // Data
    else       ReadMCEvent();   // MC
}

void NTagIO::ReadMCEvent()
{
    // DONT'T FORGET TO CLEAR!
    Clear();

    // Prompt-peak info
    SetEventHeader();
    SetPromptVertex();
    SetFitInfo();

    // MC-only truth info
    SetMCInfo();
    
    // Hit info (all hits)
    AppendRawHitInfo();
    SetToFSubtractedTQ();

    // Tagging starts here!
    SearchCaptureCandidates();
    GetTMVAOutput();

    // DONT'T FORGET TO FILL!
    FillTrees();
}

void NTagIO::ReadDataEvent()
{
    // If current event is AFT, append TQ and fill output.
    if (skhead_.idtgsk & 1<<29) {
        msg.Print("Saving SHE+AFT...", pDEBUG);
        ReadAFTEvent();
    }

    // If previous event was SHE without following AFT,
    // just fill output because there's nothing to append.
    else if (!IsRawHitVectorEmpty()) {
        msg.Print("Saving SHE without AFT...", pDEBUG);
        SetToFSubtractedTQ();

        // Tagging starts here!
        SearchCaptureCandidates();
        GetTMVAOutput();

        FillTrees();
        
        // DONT'T FORGET TO CLEAR!
        Clear();
    }

    // If current event is SHE,
    // save raw hit info and don't fill output.
    if (skhead_.idtgsk & 1<<28) {
        msg.Print("Reading SHE...", pDEBUG);
        ReadSHEEvent();
    }
}

void NTagIO::ReadSHEEvent()
{
    // DONT'T FORGET TO CLEAR!
    Clear();
    trgType = 1;

    // Prompt-peak info
    SetEventHeader();
    SetPromptVertex();
    SetFitInfo();

    // Hit info (SHE: close-to-prompt hits only)
    AppendRawHitInfo();
}

void NTagIO::ReadAFTEvent()
{
    trgType = 2;
    
    // Append hit info (AFT: delayed hits after prompt)
    AppendRawHitInfo();
    SetToFSubtractedTQ();

    // Tagging starts here!
    SearchCaptureCandidates();
    GetTMVAOutput();

    // DONT'T FORGET TO FILL!
    FillTrees();

    // DONT'T FORGET TO CLEAR!
    Clear();
}

void NTagIO::WriteOutput()
{
    outFile->cd();
    
    ntvarTree->Write();
    if (!bData) truthTree->Write();
    if (saveTQ) restqTree->AutoSave();
    outFile->Close();
    
    //bonsai_end_();
}

void NTagIO::DoWhenInterrupted()
{
    WriteOutput();
    msg.Print(Form("Interrupted by SIGINT. Events up to #%d are saved at: %s", nProcessedEvents, fOutFileName), pWARNING);
    exit(1);
}

void NTagIO::SIGINTHandler(int sigNo)
{
    instance->DoWhenInterrupted();
}

void NTagIO::CreateBranchesToTruthTree()
{
    truthTree->Branch("NTrueCaptures", &nTrueCaptures);
    truthTree->Branch("CandidateID", &vCandidateID);
    truthTree->Branch("TrueCT", &vTrueCT);
    truthTree->Branch("capvx", &vCapVX);
    truthTree->Branch("capvy", &vCapVY);
    truthTree->Branch("capvz", &vCapVZ);
    truthTree->Branch("NGamma", &vNGamma);
    truthTree->Branch("TotGammaE", &vTotGammaE);
    truthTree->Branch("TrgOffset", &trgOffset);
    truthTree->Branch("NSavedSec", &nSavedSec);
    truthTree->Branch("SecPID", &vSecPID);
    truthTree->Branch("SecIntID", &vSecIntID);
    truthTree->Branch("ParentPID", &vParentPID);
    truthTree->Branch("secvx", &vSecVX);
    truthTree->Branch("secvy", &vSecVY);
    truthTree->Branch("secvz", &vSecVZ);
    truthTree->Branch("SecDWall", &vSecDWall);
    truthTree->Branch("SecT", &vSecT);
    truthTree->Branch("secpx", &vSecPX);
    truthTree->Branch("secpy", &vSecPY);
    truthTree->Branch("secpz", &vSecPZ);
    truthTree->Branch("SecMom", &vSecMom);
    truthTree->Branch("SecCaptureID", &vCapID);
    truthTree->Branch("NeutIntMode", &neutIntMode);
    truthTree->Branch("NVecInNeut", &nVecInNeut);
    truthTree->Branch("NeutVecPID", &vNeutVecPID);
    truthTree->Branch("NnInNeutVec", &nNInNeutVec);
    truthTree->Branch("NeutIntMom", &neutIntMom);
    truthTree->Branch("NVec", &nVec);
    truthTree->Branch("vecx", &vecx);
    truthTree->Branch("vecy", &vecy);
    truthTree->Branch("vecz", &vecz);
    truthTree->Branch("VecPID", &vVecPID);
    truthTree->Branch("vecpx", &vVecPX);
    truthTree->Branch("vecpy", &vVecPY);
    truthTree->Branch("vecpz", &vVecPZ);
    truthTree->Branch("VecMom", &vVecMom);
}

void NTagIO::CreateBranchesToNtvarTree()
{
    ntvarTree->Branch("NCandidates", &nCandidates);
    ntvarTree->Branch("RunNo", &runNo);
    ntvarTree->Branch("SubrunNo", &subrunNo);
    ntvarTree->Branch("EventNo", &eventNo);
    ntvarTree->Branch("TrgType", &trgType);
    ntvarTree->Branch("FirstHitTime_ToF", &firstHitTime_ToF);
    ntvarTree->Branch("MaxN200", &maxN200);
    ntvarTree->Branch("MaxN200Time", &maxN200Time);
    ntvarTree->Branch("pvx", &pvx);
    ntvarTree->Branch("pvy", &pvy);
    ntvarTree->Branch("pvz", &pvz);
    ntvarTree->Branch("nvx", &vNvx);
    ntvarTree->Branch("nvy", &vNvy);
    ntvarTree->Branch("nvz", &vNvz);
    ntvarTree->Branch("DWall", &dWall);
    ntvarTree->Branch("N10n", &vN10n);
    ntvarTree->Branch("N1300", &vN1300);
    ntvarTree->Branch("TRMS10n", &vTRMS10n);
    ntvarTree->Branch("ReconCTn", &vReconCTn);
    ntvarTree->Branch("bsvx", &vBSvx);
    ntvarTree->Branch("bsvy", &vBSvy);
    ntvarTree->Branch("bsvz", &vBSvz);
    ntvarTree->Branch("BSReconCT", &vBSReconCT);
    ntvarTree->Branch("Beta14_10", &vBeta14_10);
    ntvarTree->Branch("Beta14_50", &vBeta14_50);
    ntvarTree->Branch("APNrings", &apNRings);
    ntvarTree->Branch("EVis", &evis);
    ntvarTree->Branch("NHITAC", &nhitac);
    ntvarTree->Branch("APNDecays", &apNDecays);
    ntvarTree->Branch("QISMSK", &qismsk);
    ntvarTree->Branch("APNMuE", &apNMuE);
    ntvarTree->Branch("APNRings", &apNRings);
    ntvarTree->Branch("APRingPID", &vAPRingPID);
    ntvarTree->Branch("APMom", &vAPMom);
    ntvarTree->Branch("APMomE", &vAPMomE);
    ntvarTree->Branch("APMomMu", &vAPMomMu);
    ntvarTree->Branch("TMVAOutput", &vTMVAOutput);
    
    //vHitRawTimes = 0;
    //vHitResTimes = 0;
    //vHitCableIDs = 0;
    //vHitSigFlags = 0;
    
    ntvarTree->Branch("HitRawTimes", "vector<vector<float>>", &vHitRawTimes);
    ntvarTree->Branch("HitResTimes", "vector<vector<float>>", &vHitResTimes);
    ntvarTree->Branch("HitCableIDs", "vector<vector<int>>", &vHitCableIDs);
    ntvarTree->Branch("HitSigFlags", "vector<vector<int>>", &vHitSigFlags);

    // Make branches from TMVAVariables class
    TMVATools.fVariables.MakeBranchesToTree(ntvarTree);

    if (!bData) {
        ntvarTree->Branch("IsGdCapture", &vIsGdCapture);
        ntvarTree->Branch("CTDiff", &vCTDiff);
        ntvarTree->Branch("DoubleCount", &vDoubleCount);
        ntvarTree->Branch("IsCapture", &vIsCapture);
        ntvarTree->Branch("truecapvx", &vTrueCapVX);
        ntvarTree->Branch("truecapvy", &vTrueCapVY);
        ntvarTree->Branch("truecapvz", &vTrueCapVZ);
    }
}

void NTagIO::CreateBranchesToRawTQTree()
{
    rawtqTree->Branch("T", &vTISKZ);
    rawtqTree->Branch("Q", &vQISKZ);
    rawtqTree->Branch("I", &vCABIZ);
    rawtqTree->Branch("IsSignal", &vISIGZ);
}

void NTagIO::CreateBranchesToResTQTree()
{
    restqTree->Branch("T", &vSortedT_ToF);
    restqTree->Branch("Q", &vSortedQ);
    restqTree->Branch("I", &vSortedPMTID);
    restqTree->Branch("IsSignal", &vSortedSigFlag);
}

void NTagIO::FillTrees()
{
    ntvarTree->Fill();
    if (!bData) truthTree->Fill();
    if (saveTQ) restqTree->Fill();
}

void NTagIO::SetSignalTQ(const char* fSigTQName)
{
    fSigTQFile = TFile::Open(fSigTQName);
    fSigTQTree = (TTree*)fSigTQFile->Get("rawtq");
    
    vSIGT = 0; vSIGI = 0;
    fSigTQTree->SetBranchAddress("T", &vSIGT);
    fSigTQTree->SetBranchAddress("I", &vSIGI);
}

void NTagIO::CheckMC()
{
    if (skhead_.nrunsk != 999999) {
        bData = true;
        msg.Print(Form("Reading event #%d from data...", nProcessedEvents+1));
    }
    else {
        bData = false;
        msg.Print(Form("Reading event #%d from MC...", nProcessedEvents+1));
    }
}