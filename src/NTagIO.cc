#include <csignal>

#include <TFile.h>

#include <skheadC.h>
#include <skvectC.h>

#include <NTagIO.hh>
#include <SKLibs.hh>

NTagIO* NTagIO::instance;

NTagIO::NTagIO(const char* inFileName, const char* outFileName, bool useData, unsigned int verbose)
: NTagEventInfo(verbose), fInFileName(inFileName), fOutFileName(outFileName), nProcessedEvents(0), lun(10)
{
    instance = this;

    bData = useData;
    fVerbosity = verbose;

    ntvarTree = new TTree("ntvar", "ntag variables");
    CreateBranchesToNTvarTree();

    if (!bData) {
        truthTree = new TTree("truth", "true variables");
        CreateBranchesToTruthTree();
    }
}

NTagIO::~NTagIO()
{ 
    WriteOutput(); 
    delete ntvarTree;
    delete truthTree;
}

void NTagIO::Initialize()
{
    SetN10Limits(5, 50);
    SetN200Max(140);
    SetT0Limits(2., 600.);   // [us]
    SetDistanceCut(4000.);   // [cm]
    SetTMatchWindow(40.);    // [ns]
    SetTPeakSeparation(50.); // [us]
    SetMaxODHitThreshold(16);

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

    // MC-only truth info
    SetMCInfo();

    // Prompt-peak info
    SetEventHeader();
    SetFitInfo();

    // Hit info (all hits)
    AppendRawHitInfo();
    SetToFSubtractedTQ();

    // Tagging starts here!
    SearchCaptureCandidates();
    GetTMVAoutput();

    // DONT'T FORGET TO FILL!
    ntvarTree->Fill();
    truthTree->Fill();
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
    else if (!vTISKZ.empty()) {
        msg.Print("Saving SHE without AFT...", pDEBUG);
        SetToFSubtractedTQ();

        // Tagging starts here!
        SearchCaptureCandidates();
        GetTMVAoutput();

        ntvarTree->Fill();
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

    // Prompt-peak info
    SetEventHeader();
    SetFitInfo();

    // Hit info (SHE: close-to-prompt hits only)
    AppendRawHitInfo();
}

void NTagIO::ReadAFTEvent()
{
    // Append hit info (AFT: delayed hits after prompt)
    AppendRawHitInfo();
    SetToFSubtractedTQ();

    // Tagging starts here!
    SearchCaptureCandidates();
    GetTMVAoutput();

    // DONT'T FORGET TO FILL!
    // (ntvar only for data)
    ntvarTree->Fill();

    // DONT'T FORGET TO CLEAR!
    Clear();
}

void NTagIO::WriteOutput()
{
    TFile* file = new TFile(fOutFileName, "recreate");
    ntvarTree->Write();
    if (!bData) truthTree->Write();
    file->Close();
}

void NTagIO::DoWhenInterrupted()
{
    WriteOutput();
    msg.Print(Form("Interrupted by SIGINT. Events up to #%d are saved at: %s.", nProcessedEvents, fOutFileName), pWARNING);
    exit(1);
}

void NTagIO::SIGINTHandler(int sigNo)
{
    instance->DoWhenInterrupted();
}

void NTagIO::CreateBranchesToTruthTree()
{
    truthTree->Branch("nCT", &nTrueCaptures);
    truthTree->Branch("captureTime", &vCaptureTime);
    truthTree->Branch("capPosx", &vCapPosx);
    truthTree->Branch("capPosy", &vCapPosy);
    truthTree->Branch("capPosz", &vCapPosz);
    truthTree->Branch("nGam", &vNGam);
    truthTree->Branch("totGamEn", &vTotGamE);
    truthTree->Branch("trgofst", &trgofst);
    truthTree->Branch("nscnd", &nSavedSec);
    truthTree->Branch("iprtscnd", &vIprtscnd);
    truthTree->Branch("lmecscnd", &vLmecscnd);
    truthTree->Branch("iprntprt", &vIprntprt);
    truthTree->Branch("vtxscndx", &vVtxscndx);
    truthTree->Branch("vtxscndy", &vVtxscndy);
    truthTree->Branch("vtxscndz", &vVtxscndz);
    truthTree->Branch("wallscnd", &vWallscnd);
    truthTree->Branch("tscnd", &vTscnd);
    truthTree->Branch("pscndx", &vPscndx);
    truthTree->Branch("pscndy", &vPscndy);
    truthTree->Branch("pscndz", &vPscndz);
    truthTree->Branch("pabsscnd", &vPabsscnd);
    truthTree->Branch("capId", &vCaptureID);
    truthTree->Branch("modene", &modene);
    truthTree->Branch("numne", &numne);
    truthTree->Branch("ipne", &vIpne);
    truthTree->Branch("nN", &nN);
    truthTree->Branch("pnu", &pnu);
    truthTree->Branch("nvect", &nvect);
    truthTree->Branch("truevx", &truevx);
    truthTree->Branch("truevy", &truevy);
    truthTree->Branch("truevz", &truevz);
    truthTree->Branch("ip", &vIp);
    truthTree->Branch("pinx", &vPinx);
    truthTree->Branch("piny", &vPiny);
    truthTree->Branch("pinz", &vPinz);
    truthTree->Branch("pabs", &vPabs);
}

void NTagIO::CreateBranchesToNTvarTree()
{
    ntvarTree->Branch("np", &nCandidates);
    ntvarTree->Branch("nrun", &nrun);
    ntvarTree->Branch("nsub", &nsub);
    ntvarTree->Branch("nev", &nev);
    ntvarTree->Branch("trgtype", &trgtype);
    ntvarTree->Branch("firsthit", &firsthit);
    ntvarTree->Branch("N200M", &N200M);
    ntvarTree->Branch("T200M", &T200M);
    ntvarTree->Branch("vx", &pvx);
    ntvarTree->Branch("vy", &pvy);
    ntvarTree->Branch("vz", &pvz);
    ntvarTree->Branch("nvx", &vNvx);
    ntvarTree->Branch("nvy", &vNvy);
    ntvarTree->Branch("nvz", &vNvz);
    ntvarTree->Branch("nwall", &vNwall);
    ntvarTree->Branch("N10", &vN10);
    ntvarTree->Branch("wall", &towall);
    ntvarTree->Branch("N10n", &vN10n);
    ntvarTree->Branch("N200", &vN200);
    ntvarTree->Branch("N50", &vN50);
    ntvarTree->Branch("N1300", &vN1300);
    ntvarTree->Branch("trms", &vTrms);
    ntvarTree->Branch("trmsold", &vTrmsold);
    ntvarTree->Branch("trms40", &vTrms50);
    ntvarTree->Branch("dt", &vDt);
    ntvarTree->Branch("dtn", &vDtn);
    ntvarTree->Branch("spread", &vSpread);
    ntvarTree->Branch("tbsenergy", &vBenergy);
    ntvarTree->Branch("tbsvx", &vBvx);
    ntvarTree->Branch("tbsvy", &vBvy);
    ntvarTree->Branch("tbsvz", &vBvz);
    ntvarTree->Branch("tbsvt", &vBvt);
    ntvarTree->Branch("tbswall", &vBwall);
    ntvarTree->Branch("tbsgood", &vBgood);
    ntvarTree->Branch("tbsdirks", &vBdirks);
    ntvarTree->Branch("tbspatlik", &vBpatlik);
    ntvarTree->Branch("tbsovaq", &vBovaq);
    ntvarTree->Branch("beta14", &vBeta14_10);
    ntvarTree->Branch("beta14_40", &vBeta14_50);
    ntvarTree->Branch("beta1", &vBeta1_50);
    ntvarTree->Branch("beta2", &vBeta2_50);
    ntvarTree->Branch("beta3", &vBeta3_50);
    ntvarTree->Branch("beta4", &vBeta4_50);
    ntvarTree->Branch("beta5", &vBeta5_50);
    ntvarTree->Branch("nring", &nring);
    ntvarTree->Branch("evis", &evis);
    ntvarTree->Branch("nhitac", &nhitac);
    ntvarTree->Branch("ndcy", &ndcy);
    ntvarTree->Branch("qismsk", &qismsk);
    ntvarTree->Branch("apnmue", &nmue);
    ntvarTree->Branch("apnring", &nring);
    ntvarTree->Branch("apip", &vApip);
    ntvarTree->Branch("apamom", &vApamom);
    ntvarTree->Branch("amome", &vApmome);
    ntvarTree->Branch("amomm", &vApmomm);
    ntvarTree->Branch("sumQ", &vSumQ);
    ntvarTree->Branch("prompt_bonsai", &vPrompt_BONSAI);
    ntvarTree->Branch("prompt_nfit", &vPrompt_NFit);
    ntvarTree->Branch("bonsai_nfit", &vBONSAI_NFit);
    ntvarTree->Branch("TMVAoutput", &vTMVAoutput);

    if (!bData) {
        ntvarTree->Branch("nGd", &vIsGdCapture);
        ntvarTree->Branch("timeRes", &vTimeDiff);
        ntvarTree->Branch("doubleCount", &vDoubleCount);
        ntvarTree->Branch("realneutron", &vIsTrueCapture);
        ntvarTree->Branch("truth_vx", &vTruth_vx);
        ntvarTree->Branch("truth_vy", &vTruth_vy);
        ntvarTree->Branch("truth_vz", &vTruth_vz);
    }
}

void NTagIO::CheckMC()
{
    if (skhead_.nrunsk != 999999) {
        bData = true;
        msg.Print(Form("Reading event #%d from data...", nProcessedEvents+1));
    }
    else msg.Print(Form("Reading event #%d from MC...", nProcessedEvents+1));
}