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
    truthTree->Branch("nCT", &nCT, "nCT/I");
    truthTree->Branch("captureTime", vCaptureTime, "captureTime[nCT]/F");
    truthTree->Branch("capPos", capPos, "capPos[nCT][3]/F");
    truthTree->Branch("nGam", vNGam, "nGam[nCT]/I");
    truthTree->Branch("totGamEn", vTotGamE, "totGamEn[nCT]/F");
    truthTree->Branch("nscnd", &nscnd, "nscnd/I");
    truthTree->Branch("iprtscnd", vIprtscnd, "iprtscnd[nscnd]/I");
    truthTree->Branch("lmecscnd", vLmecscnd, "lmecscnd[nscnd]/I");
    truthTree->Branch("iprntprt", vIprntprt, "iprntprt[nscnd]/I");
    truthTree->Branch("vtxscnd", vtxscnd , "vtxscnd[nscnd][3]/F");
    truthTree->Branch("wallscnd", vWallscnd , "wallscnd[nscnd]/F");
    truthTree->Branch("tscnd", vTscnd, "tscnd[nscnd]/F");
    truthTree->Branch("pscnd", pscnd , "pscnd[nscnd][3]/F");
    truthTree->Branch("pabsscnd", vPabsscnd, "pabsscnd[nscnd]/F");
    truthTree->Branch("capId", vCaptureID, "capId[nscnd]/I");
    truthTree->Branch("modene", &modene, "modene/I");
    truthTree->Branch("numne", &numne, "numne/I");
    truthTree->Branch("ipne", vIpne, "ipne[numne]/I");
    truthTree->Branch("nN", &nN, "nN/I");
    truthTree->Branch("pnu", &pnu, "pnu/F");
    truthTree->Branch("nvect", &nvect, "nvect/I");
    truthTree->Branch("pos", pos, "pos[3]/F");
    truthTree->Branch("ip", ip, "ip[nvect]/I");
    truthTree->Branch("pin", pin , "pin[nvect][3]/F");
    truthTree->Branch("pabs", &vPabs, "pabs[nvect]/I");
}

void NTagAnalysis::CreateBranchesToNTvarTree()
{
    ntvarTree->Branch("np", &nCandidates, "np/I");
    ntvarTree->Branch("nrun", &nrun, "nrun/I");
    ntvarTree->Branch("nsub", &nsub, "nsub/I");
    ntvarTree->Branch("nev", &nev, "nev/I");
    ntvarTree->Branch("broken", &broken, "broken/I");
    ntvarTree->Branch("trgtype", &trgtype, "trgtype/I");
    ntvarTree->Branch("lasthit", &lasthit, "lasthit/F");
    ntvarTree->Branch("trgofst", &trgofst, "trgofst/F");
    ntvarTree->Branch("firsthit", &firsthit, "firsthit/F");
    ntvarTree->Branch("firstflz", &firstflz, "firstflz/F");
    ntvarTree->Branch("timnsk", &timnsk, "timnsk/F");
    ntvarTree->Branch("bt", &bt, "bt/F");
    ntvarTree->Branch("N200M", &N200M, "N200M/I");
    ntvarTree->Branch("T200M", &T200M, "T200M/F");
    ntvarTree->Branch("vx", &apvx, "vx/F");
    ntvarTree->Branch("vy", &apvy, "vy/F");
    ntvarTree->Branch("vz", &apvz, "vz/F");
    ntvarTree->Branch("nvx", vNvx, "nvx[np]/F");
    ntvarTree->Branch("nvy", vNvy, "nvy[np]/F");
    ntvarTree->Branch("nvz", vNvz, "nvz[np]/F");
    ntvarTree->Branch("nwall", vNwall, "nwall[np]/F");
    ntvarTree->Branch("tvx", tvx, "tvx[np]/F");
    ntvarTree->Branch("tvy", tvy, "tvy[np]/F");
    ntvarTree->Branch("tvz", tvz, "tvz[np]/F");
    ntvarTree->Branch("N10", vN10, "N10[np]/I");
    ntvarTree->Branch("wall", &towall, "wall/F");
    ntvarTree->Branch("N10n", vN10n, "N10n[np]/I");
    ntvarTree->Branch("Nc", Nc, "Nc[np]/I");
    ntvarTree->Branch("Ncluster", Ncluster, "Ncluster[np]/I");
    ntvarTree->Branch("Nc8", Nc8, "Nc8[np]/I");
    ntvarTree->Branch("Ncluster8", Ncluster8, "Ncluster8[np]/I");
    ntvarTree->Branch("Nc7", Nc7, "Nc7[np]/I");
    ntvarTree->Branch("Ncluster7", Ncluster7, "Ncluster7[np]/I");
    ntvarTree->Branch("Nback", Nback, "Nback[np]/I");
    ntvarTree->Branch("N300", N300, "N300[np]/I");
    ntvarTree->Branch("N200", vN200, "N200[np]/I");
    ntvarTree->Branch("N50", vN50, "N50[np]/I");
    ntvarTree->Branch("N1300", vN1300, "N1300[np]/I");
    ntvarTree->Branch("N12", N12, "N12[np]/I");
    ntvarTree->Branch("N20", N20, "N20[np]/I");
    ntvarTree->Branch("trms", vTrms, "trms[np]/F");
    ntvarTree->Branch("trmsold", vTrmsold, "trmsold[np]/F");
    ntvarTree->Branch("trms40", vTrms50, "trms40[np]/F");
    ntvarTree->Branch("mintrms_3", mintrms_3, "mintrms_3[np]/F");
    ntvarTree->Branch("mintrms_4", mintrms_4, "mintrms_4[np]/F");
    ntvarTree->Branch("mintrms_5", mintrms_5, "mintrms_5[np]/F");
    ntvarTree->Branch("mintrms_6", mintrms_6, "mintrms_6[np]/F");
    ntvarTree->Branch("phirms", phirms, "phirms[np]/F");
    ntvarTree->Branch("thetam", thetam, "thetam[np]/F");
    ntvarTree->Branch("dt", vDt, "dt[np]/F");
    ntvarTree->Branch("dtn", vDtn, "dtn[np]/F");
    ntvarTree->Branch("spread", vSpread, "spread[np]/F");
    ntvarTree->Branch("Neff", Neff, "Neff[np]/I");
    ntvarTree->Branch("ratio", ratio, "ratio[np]/F");
    ntvarTree->Branch("Nc1", Nc1, "Nc1[np]/I");
    ntvarTree->Branch("NhighQ", NhighQ, "NhighQ[np]/I");
    ntvarTree->Branch("Nlow1", Nlow[0], "Nlow1[np]/I");
    ntvarTree->Branch("Nlow2", Nlow[1], "Nlow2[np]/I");
    ntvarTree->Branch("Nlow3", Nlow[2], "Nlow3[np]/I");
    ntvarTree->Branch("Nlow4", Nlow[3], "Nlow4[np]/I");
    ntvarTree->Branch("Nlow5", Nlow[4], "Nlow5[np]/I");
    ntvarTree->Branch("Nlow6", Nlow[5], "Nlow6[np]/I");
    ntvarTree->Branch("Nlow7", Nlow[6], "Nlow7[np]/I");
    ntvarTree->Branch("Nlow8", Nlow[7], "Nlow8[np]/I");
    ntvarTree->Branch("Nlow9", Nlow[8], "Nlow9[np]/I");
    ntvarTree->Branch("tbsenergy", vBenergy, "tbsenergy[np]/F");
    ntvarTree->Branch("tbsvx", vBvx, "tbsvx[np]/F");
    ntvarTree->Branch("tbsvy", vBvy, "tbsvy[np]/F");
    ntvarTree->Branch("tbsvz", vBvz, "tbsvz[np]/F");
    ntvarTree->Branch("tbsvt", vBvt, "tbsvt[np]/F");
    ntvarTree->Branch("tbswall", vBwall, "tbswall[np]/F");
    ntvarTree->Branch("tbsgood", vBgood, "tbsgood[np]/F");
    ntvarTree->Branch("tbsdirks", vBdirks, "tbsdirks[np]/F");
    ntvarTree->Branch("tbspatlik", vBpatlik, "tbspatlik[np]/F");
    ntvarTree->Branch("tbsovaq", vBovaq, "tbsovaq[np]/F");
    ntvarTree->Branch("g2d2", g2d2, "g2d2[np]/F");
    ntvarTree->Branch("goodn", vGoodn, "goodn[np]/I");
    ntvarTree->Branch("beta14", vBeta14_10, "beta14[np]/F");
    ntvarTree->Branch("beta14_40", vBeta14_50, "beta14_40[np]/F");
    ntvarTree->Branch("beta1", vBeta1_50, "beta1[np]/F");
    ntvarTree->Branch("beta2", vBeta2_50, "beta2[np]/F");
    ntvarTree->Branch("beta3", vBeta3_50, "beta3[np]/F");
    ntvarTree->Branch("beta4", vBeta4_50, "beta4[np]/F");
    ntvarTree->Branch("beta5", vBeta5_50, "beta5[np]/F");
    ntvarTree->Branch("px", &px, "px/F");
    ntvarTree->Branch("py", &py, "py/F");
    ntvarTree->Branch("pz", &pz, "pz/F");
    ntvarTree->Branch("npx", npx, "npx[np]/F");
    ntvarTree->Branch("npy", npy, "npy[np]/F");
    ntvarTree->Branch("npz", npz, "npz[np]/F");
    ntvarTree->Branch("dirx", &dirx, "dirx/F");
    ntvarTree->Branch("diry", &diry, "diry/F");
    ntvarTree->Branch("dirz", &dirz, "dirz/F");
    ntvarTree->Branch("ndirx", ndirx, "ndirx[np]/F");
    ntvarTree->Branch("ndiry", ndiry, "ndiry[np]/F");
    ntvarTree->Branch("ndirz", ndirz, "ndirz[np]/F");
    ntvarTree->Branch("summedWeight", summedWeight, "summedWeight[np]/F");
    ntvarTree->Branch("ip0", &ip0, "ip0/I");
    ntvarTree->Branch("nring", &nring, "nring/I");
    ntvarTree->Branch("evis", &evis, "evis/F");
    ntvarTree->Branch("nhitac", &nhitac, "nhitac/I");
    ntvarTree->Branch("ndcy", &ndcy, "ndcy/I");
    ntvarTree->Branch("qismsk", &qismsk, "qismsk/F");
    ntvarTree->Branch("apnmue", &nmue, "apnmue/I");
    ntvarTree->Branch("apnring", &nring, "apnring/I");
    ntvarTree->Branch("apip", vApip, "apip[apnring]/I");
    ntvarTree->Branch("apamom", vApamom, "apamom[apnring]/F");
    ntvarTree->Branch("amome", vApmome, "amome[apnring]/F");
    ntvarTree->Branch("amomm", vApmomm, "amomm[apnring]/F");
    ntvarTree->Branch("sumQ", vSumQ, "sumQ[np]/F");
    ntvarTree->Branch("TMVAoutput", vTMVAoutput, "TMVAoutput[np]/F");
    
    if (!bData) {
        ntvarTree->Branch("nGd", vIsGdCapture, "nGd[np]/I");
        ntvarTree->Branch("timeRes", vTimeDiff, "timeRes[np]/F");
        ntvarTree->Branch("doubleCount", vDoubleCount, "doubleCount[np]/I");
        ntvarTree->Branch("mctrue_nn", &mctrue_nn, "mctrue_nn/I");
        ntvarTree->Branch("realneutron",vIsTrueCapture, "realneutron[np]/I");
        ntvarTree->Branch("truth_vx", vTruth_vx, "truth_vx[np]/F");
        ntvarTree->Branch("truth_vy", vTruth_vy, "truth_vy[np]/F");
        ntvarTree->Branch("truth_vz", vTruth_vz, "truth_vz[np]/F");
    }
}