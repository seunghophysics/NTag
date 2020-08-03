#include <math.h>
#include <stdlib.h>

#include <TFile.h>

#include <skheadC.h>

#include <SKLibs.hh>
#include "NTagAnalysis.hh"

NTagAnalysis::NTagAnalysis(const char* fileName, bool useData, unsigned int verbose)
: lun(10)
{
    bData = useData;
    fVerbosity = verbose;

    SetN10Limits(5, 50);
    SetN200Max(140);
    SetT0Threshold(1.);		// [us]
    SetDistanceCut(4000.);	// [cm]
    SetTMatchWindow(40.);	// [ns]

    
    ntvarTree = new TTree("ntvar", "ntag variables");
    CreateBranchesToNTvarTree();

    if(!bData){
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

    if(openError){
        std::cerr << "[NTagAnalysis]: File open error." << std::endl;
        exit(1);
    }

    // Set SK options and SK geometry
    const char* skoptn = "31,30,26,25"; skoptn_(skoptn, strlen(skoptn));
    skheadg_.sk_geometry = 5; geoset_();
}

void NTagAnalysis::ReadFile()
{
    // Read data event-by-event
    int readStatus;
    int eventID = 0;

    while(1){
        readStatus = skread_(&lun);
        switch(readStatus){
            case 0: // event read
                eventID++;
                std::cout << "\n" << std::endl;
                PrintMessage("###########################", vDebug);
                PrintMessage(Form("Event ID: %d", eventID), vDebug);
                PrintMessage("###########################", vDebug);
                ReadEvent();
                break;
            case 1: // read-error
                break;
            case 2: // end of input
                PrintMessage(Form("Reached the end of input. Closing file..."), vDefault);
                skclosef_(&lun);
                WriteOutput();
                break;
        }
    }
}

void NTagAnalysis::ReadEvent()
{
    Clear();
    SetEventHeader();
    SetAPFitInfo();
    SetToFSubtractedTQ();
    SearchCaptureCandidates();

    if(!bData){
        SetMCInfo();
    }

    ntvarTree->Fill();
    if(!bData) truthTree->Fill();
}

void NTagAnalysis::WriteOutput()
{
    TFile* file = new TFile("nTagOutput.root", "recreate");
    ntvarTree->Write();
    if(!bData) truthTree->Write();
    file->Close();
}

void NTagAnalysis::CreateBranchesToTruthTree()
{
    truthTree->Branch("nCT", &nCT, "nCT/I");
    truthTree->Branch("captureTime", captureTime, "captureTime[nCT]/F");
    truthTree->Branch("capPos", capPos, "capPos[nCT][3]/F");
    truthTree->Branch("nGam", nGam, "nGam[nCT]/I");
    truthTree->Branch("totGamEn", totGamEn, "totGamEn[nCT]/F");
    truthTree->Branch("nscnd", &nscndprt, "nscnd/I");
    truthTree->Branch("iprtscnd", iprtscnd, "iprtscnd[nscnd]/I");
    truthTree->Branch("lmecscnd", lmecscnd, "lmecscnd[nscnd]/I");
    truthTree->Branch("iprntprt", iprntprt, "iprntprt[nscnd]/I");
    truthTree->Branch("vtxscnd", vtxscnd , "vtxscnd[nscnd][3]/F");
    truthTree->Branch("wallscnd", wallscnd , "wallscnd[nscnd]/F");
    truthTree->Branch("tscnd", tscnd, "tscnd[nscnd]/F");
    truthTree->Branch("pscnd", pscnd , "pscnd[nscnd][3]/F");
    truthTree->Branch("pabsscnd", pabsscnd, "pabsscnd[nscnd]/F");
    truthTree->Branch("capId", capId, "capId[nscnd]/I");
    truthTree->Branch("modene", &modene, "modene/I");
    truthTree->Branch("numne", &numne, "numne/I");
    truthTree->Branch("ipne", ipne, "ipne[numne]/I");
    truthTree->Branch("nN", &nN, "nN/I");
    truthTree->Branch("pnu", &pnu, "pnu/F");
    truthTree->Branch("nvect", &nvect, "nvect/I");
    truthTree->Branch("pos", pos, "pos[3]/F");
    truthTree->Branch("ip", ip, "ip[nvect]/I");
    truthTree->Branch("pin", pin , "pin[nvect][3]/F");
    truthTree->Branch("pabs", &pabs, "pabs[nvect]/I");
}

void NTagAnalysis::CreateBranchesToNTvarTree()
{
    ntvarTree->Branch("np", &np, "np/I");
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
    ntvarTree->Branch("vx", &vx, "vx/F");
    ntvarTree->Branch("vy", &vy, "vy/F");
    ntvarTree->Branch("vz", &vz, "vz/F");
    ntvarTree->Branch("nvx",	nvx, "nvx[np]/F");
    ntvarTree->Branch("nvy",	nvy, "nvy[np]/F");
    ntvarTree->Branch("nvz",	nvz, "nvz[np]/F");
    ntvarTree->Branch("nwall", nwall, "nwall[np]/F");
    ntvarTree->Branch("tvx",	tvx, "tvx[np]/F");
    ntvarTree->Branch("tvy",	tvy, "tvy[np]/F");
    ntvarTree->Branch("tvz",	tvz, "tvz[np]/F");
    ntvarTree->Branch("N10", N10, "N10[np]/I");
    ntvarTree->Branch("wall", &towall, "wall/F");
    ntvarTree->Branch("N10n", N10n, "N10n[np]/I");
    ntvarTree->Branch("Nc", Nc, "Nc[np]/I");
    ntvarTree->Branch("Ncluster", Ncluster, "Ncluster[np]/I");
    ntvarTree->Branch("Nc8", Nc8, "Nc8[np]/I");
    ntvarTree->Branch("Ncluster8", Ncluster8, "Ncluster8[np]/I");
    ntvarTree->Branch("Nc7", Nc7, "Nc7[np]/I");
    ntvarTree->Branch("Ncluster7", Ncluster7, "Ncluster7[np]/I");
    ntvarTree->Branch("Nback", Nback, "Nback[np]/I");
    ntvarTree->Branch("N300", N300, "N300[np]/I");
    ntvarTree->Branch("N200", N200, "N200[np]/I");
    ntvarTree->Branch("N50", N50, "N50[np]/I");
    ntvarTree->Branch("N1300", N1300, "N1300[np]/I");
    ntvarTree->Branch("N12", N12, "N12[np]/I");
    ntvarTree->Branch("N20", N20, "N20[np]/I");
    ntvarTree->Branch("trms", trms, "trms[np]/F");
    ntvarTree->Branch("trmsold", trmsold, "trmsold[np]/F");
    ntvarTree->Branch("trms40", trms50, "trms40[np]/F");
    ntvarTree->Branch("mintrms_3", mintrms_3, "mintrms_3[np]/F");
    ntvarTree->Branch("mintrms_4", mintrms_4, "mintrms_4[np]/F");
    ntvarTree->Branch("mintrms_5", mintrms_5, "mintrms_5[np]/F");
    ntvarTree->Branch("mintrms_6", mintrms_6, "mintrms_6[np]/F");
    ntvarTree->Branch("phirms", phirms, "phirms[np]/F");
    ntvarTree->Branch("thetam", thetam, "thetam[np]/F");
    ntvarTree->Branch("dt", dt, "dt[np]/F");
    ntvarTree->Branch("dtn", dtn, "dtn[np]/F");
    ntvarTree->Branch("spread", spread, "spread[np]/F");
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
    ntvarTree->Branch("bsenergy", bsenergy, "bsenergy[np]/F");
    ntvarTree->Branch("bsvertex0", bsvertex0, "bsvertex0[np]/F");
    ntvarTree->Branch("bsvertex1", bsvertex1, "bsvertex1[np]/F");
    ntvarTree->Branch("bsvertex2", bsvertex2, "bsvertex2[np]/F");
    ntvarTree->Branch("bsgood", bsgood, "bsgood[np]/F");
    ntvarTree->Branch("tbsenergy", tbsenergy, "tbsenergy[np]/F");
    ntvarTree->Branch("tbsenergy2", tbsenergy2, "tbsenergy2[np]/F");
    ntvarTree->Branch("tbsvx", tbsvx, "tbsvx[np]/F");
    ntvarTree->Branch("tbsvy", tbsvy, "tbsvy[np]/F");
    ntvarTree->Branch("tbsvz", tbsvz, "tbsvz[np]/F");
    ntvarTree->Branch("tbsvt", tbsvt, "tbsvt[np]/F");
    ntvarTree->Branch("tbswall",	tbswall, "tbswall[np]/F");
    ntvarTree->Branch("tbsgood",	tbsgood, "tbsgood[np]/F");
    ntvarTree->Branch("tbsdirks", tbsdirks, "tbsdirks[np]/F");
    ntvarTree->Branch("tbspatlik", tbspatlik, "tbspatlik[np]/F");
    ntvarTree->Branch("tbsovaq",	tbsovaq, "tbsovaq[np]/F");
    ntvarTree->Branch("g2d2", g2d2, "g2d2[np]/F");
    ntvarTree->Branch("goodn", goodn, "goodn[np]/I");
    ntvarTree->Branch("beta14", beta14_10, "beta14[np]/F");
    ntvarTree->Branch("beta14_40", beta14_50, "beta14_40[np]/F");
    ntvarTree->Branch("beta1", beta1_50, "beta1[np]/F");
    ntvarTree->Branch("beta2", beta2_50, "beta2[np]/F");
    ntvarTree->Branch("beta3", beta3_50, "beta3[np]/F");
    ntvarTree->Branch("beta4", beta4_50, "beta4[np]/F");
    ntvarTree->Branch("beta5", beta5_50, "beta5[np]/F");
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
    ntvarTree->Branch("qismsk", &qismsk, "qismsk/I");
    ntvarTree->Branch("apnmue", &nmue, "apnmue/I");
    ntvarTree->Branch("apnring", &nring, "apnring/I");
    ntvarTree->Branch("apip", apip, "apip[apnring]/I");
    ntvarTree->Branch("apamom", apamom, "apamom[apnring]/F");
    ntvarTree->Branch("amome", amome, "amome[apnring]/F");
    ntvarTree->Branch("amomm", amomm, "amomm[apnring]/F");
    ntvarTree->Branch("sumQ", sumQ, "sumQ[np]/F");
    ntvarTree->Branch("TMVAoutput", TMVAoutput, "TMVAoutput[np]/F");
    
    if(!bData){
        ntvarTree->Branch("nGd", nGd, "nGd[np]/I");
        ntvarTree->Branch("timeRes",	timeRes, "timeRes[np]/F");
        ntvarTree->Branch("doubleCount",	doubleCount, "doubleCount[np]/I");
        ntvarTree->Branch("mctrue_nn", &mctrue_nn, "mctrue_nn/I");
        ntvarTree->Branch("realneutron",realneutron, "realneutron[np]/I");
        ntvarTree->Branch("truth_vx", truth_vx, "truth_vx[np]/F");
        ntvarTree->Branch("truth_vy", truth_vy, "truth_vy[np]/F");
        ntvarTree->Branch("truth_vz", truth_vz, "truth_vz[np]/F");
    }
}