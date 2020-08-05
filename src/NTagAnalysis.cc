#include <math.h>
#include <stdlib.h>

#include <TFile.h>

#include <skheadC.h>
#include <skvectC.h>

#include <SKLibs.hh>
#include "NTagAnalysis.hh"

NTagAnalysis::NTagAnalysis(const char* fileName, bool useData, unsigned int verbose)
: lun(10)
{
    bData = useData;
    fVerbosity = verbose;

    SetN10Limits(5, 50);
    SetN200Max(140);
    SetT0Threshold(2.);		 // [us]
    SetDistanceCut(4000.);	 // [cm]
    SetTMatchWindow(40.);	 // [ns]
    SetTPeakSeparation(50.); // [us]

    ntvarTree = new TTree("ntvar", "ntag variables");
    CreateBranchesToNTvarTree();

    if (!bData) {
        truthTree = new TTree("truth", "true variables");
        CreateBranchesToTruthTree();
    }

    OpenFile(fileName);
    ReadFile();
}

NTagAnalysis::~NTagAnalysis() {}

void NTagAnalysis::OpenFile(const char* fileName)
{
    kzinit_();

    // Set rflist and open file
    int ipt = 1;
    int openError;

    set_rflist_(&lun, fileName, "LOCAL", "", "RED", "", "", "recl=5670 status=old", "", "",
                strlen(fileName),5,0,3,0,0,20,0,0);
    skopenf_(&lun, &ipt, "Z", &openError);

    if (openError) {
        std::cerr << "[NTagAnalysis]: File open error." << std::endl;
        exit(1);
    }

    // Set SK options and SK geometry
    const char* skoptn = "31,30,26,25"; skoptn_(skoptn, strlen(skoptn));
    skheadg_.sk_geometry = 4; geoset_();

    // Initialize BONSAI
    bonsai_ini_();
}

void NTagAnalysis::ReadFile()
{
    // Read data event-by-event
    int readStatus;
    int eventID = 0;
    bool bEOF = false;

    while (!bEOF) {
        Clear();
        readStatus = skread_(&lun);
        switch (readStatus) {
            case 0: // event read
                eventID++;
                std::cout << "\n" << std::endl;
                PrintMessage("###########################", pDEBUG);
                PrintMessage(Form("Event ID: %d", eventID), pDEBUG);
                PrintMessage("###########################", pDEBUG);
                int inPMT;
                if (!bData) {
                    skgetv_();
                    inpmt_(skvect_.pos, inPMT);
                    if (inPMT) {
                        PrintMessage(
                            Form("True vertex is in PMT. Skipping event %d...",
                                 eventID), pDEBUG);
                    break;
                    }
                }
                ReadEvent();
                break;
            case 1: // read-error
                break;
            case 2: // end of input
                PrintMessage(Form("Reached the end of input. Closing file..."), pDEFAULT);
                skclosef_(&lun);
                WriteOutput();
                bEOF = true;
                break;
        }
    }
}

void NTagAnalysis::ReadEvent()
{
    SetEventHeader();
    SetAPFitInfo();
    SetToFSubtractedTQ();
    SearchCaptureCandidates();
    GetTMVAoutput();

    if (!bData) {
        SetMCInfo();
    }

    ntvarTree->Fill();
    if (!bData) truthTree->Fill();
}

void NTagAnalysis::WriteOutput()
{
    TFile* file = new TFile("out/nTagOutput.root", "recreate");
    ntvarTree->Write();
    if (!bData) truthTree->Write();
    file->Close();

    bonsai_end_();
}

void NTagAnalysis::CreateBranchesToTruthTree()
{
    truthTree->Branch("nCT", &nCT);
    truthTree->Branch("captureTime", &vCaptureTime);
    truthTree->Branch("capPosx", &vCapPosx);
    truthTree->Branch("capPosy", &vCapPosy);
    truthTree->Branch("capPosz", &vCapPosz);
    truthTree->Branch("nGam", &vNGam);
    truthTree->Branch("totGamEn", &vTotGamE);
    truthTree->Branch("nscnd", &nscnd);
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

void NTagAnalysis::CreateBranchesToNTvarTree()
{
    ntvarTree->Branch("np", &nCandidates);
    ntvarTree->Branch("nrun", &nrun);
    ntvarTree->Branch("nsub", &nsub);
    ntvarTree->Branch("nev", &nev);
    ntvarTree->Branch("trgtype", &trgtype);
    ntvarTree->Branch("trgofst", &trgofst);
    ntvarTree->Branch("firsthit", &firsthit);
    ntvarTree->Branch("N200M", &N200M);
    ntvarTree->Branch("T200M", &T200M);
    ntvarTree->Branch("vx", &apvx);
    ntvarTree->Branch("vy", &apvy);
    ntvarTree->Branch("vz", &apvz);
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