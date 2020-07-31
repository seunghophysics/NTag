#include "NTagAnalysis.hh"

NTagAnalysis::NTagAnalysis()
: bData(false)
{
    ntvarTree = new TTree("ntvar",  "ntag variables");
    CreateBranchesToTree(ntvarTree, false);

    if(!bData){
        truthTree = new TTree("truth",  "true variables");
        CreateBranchesToTree(truthTree, true);
    }

    reader = new TMVA::Reader( "!Color:!Silent" );

    OpenFile(fileName);
	ReadFile();
}
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
    
    while(1){
	    readStatus = skread_(&lun);
	    switch (readStatus){
	        case 0: // event read
                ReadEvent();
                break;
            case 1: // read-error
                break;
            case 2: // end of input
                std::cout << "Reached the end of input. Closing file..." << std::endl;
                skclosef_(&lun);
                break;
        }
    }
}

void NTagAnalysis::ReadEvent()
{
	Clear();
}

void NTagAnalysis::SetEventHeader()
{
	// get apcommul bank
	int bank = 0;
    aprstbnk_(&bank);

	// get APFit ring vertex
	vx = apcommul_.appos[0];
	vy = apcommul_.appos[1];
	vz = apcommul_.appos[2];

    std::cout << "Vertex: " << vx << " " << vx << " " << vx << std::endl;

	// get number of OD hits
    odpc_2nd_s_(&nhitac);

    towall = GetDWall(vx, vy, vz);
    evis = apcomene_.apevis;
    apnring = apcommul_.apnring;
    for (Int_t i=0;i<apnring;i++) {
      apip[i]=apcommul_.apip[i];
      apamom[i]=apcommul_.apamom[i];
      amome[i]=appatsp2_.apmsamom[i][1];
      amomm[i]=appatsp2_.apmsamom[i][2];
    }
    trginfo_(&trgofst);
    qismsk = skq_.qismsk;
    apnmue = apmue_.apnmue;
    ndcy = 0;
    for (Int_t i=0;i<apnmue;i++) {
      if (i==10) break;
      if (apmue_.apmuetype[i]==1 || apmue_.apmuetype[i]==4) ndcy++;
    }
    res_n.nrun = skhead_.nrunsk;
    res_n.nsub = skhead_.nsubsk;
    res_n.nev = skhead_.nevsk;
    towall = wallsk(pvx) ; 
    res_n.wall = wall;
    res_n.apnmue = apnmue;
    res_n.ndcy = ndcy;
    res_n.nhitac = nhitac;
    res_n.evis = evis;
    res_n.vx = pvx[0];
    res_n.vy = pvx[1];
    res_n.vz = pvx[2];
    res_n.trgofst=trgofst;
}

float NTagAnalysis::GetDWall(float& x, float& y, float& z)
{
	Double_t r2 = x*x + y*y;
    Double_t r = sqrt(r2);
    Double_t d_wall = RINTK - r;
    Double_t z1 = ZPINTK - z;
    Double_t z2 = z - ZMINTK;

    if(z1 < d_wall) d_wall = z1;
    if(z2 < d_wall) d_wall = z2;
    return d_wall;
}


void NTagAnalysis::Clear()
{
    NTagAnalysis::Clear();
    for(int i = 0; i < MAXNP; i++) TMVAoutput[i] = -9999;
}

void NTagAnalysis::CreateBranchesToTree(TTree* tree,  bool forTruth)
{
    if(forTruth){
        tree->Branch("nCT", &nCT, "nCT/I");
        tree->Branch("captureTime", captureTime, "captureTime[nCT]/F");
        tree->Branch("capPos", capPos, "capPos[nCT][3]/F");
        tree->Branch("nGam", nGam, "nGam[nCT]/I");
        tree->Branch("totGamEn", totGamEn, "totGamEn[nCT]/F");
        tree->Branch("nscnd", &nscndprt, "nscnd/I");
        tree->Branch("iprtscnd", iprtscnd, "iprtscnd[nscnd]/I");
        tree->Branch("lmecscnd", lmecscnd, "lmecscnd[nscnd]/I");
        tree->Branch("iprntprt", iprntprt, "iprntprt[nscnd]/I");
        tree->Branch("vtxscnd", vtxscnd , "vtxscnd[nscnd][3]/F");
        tree->Branch("wallscnd", wallscnd , "wallscnd[nscnd]/F");
        tree->Branch("tscnd", tscnd, "tscnd[nscnd]/F");
        tree->Branch("pscnd", pscnd , "pscnd[nscnd][3]/F");
        tree->Branch("pabsscnd", pabsscnd, "pabsscnd[nscnd]/F");
        tree->Branch("capId", capId, "capId[nscnd]/I");
        tree->Branch("modene", &modene, "modene/I");
        tree->Branch("numne", &numne, "numne/I");
        tree->Branch("ipne", ipne, "ipne[numne]/I");
        tree->Branch("nN", &nN, "nN/I");
        tree->Branch("pnu", &pnu, "pnu/F");
        tree->Branch("nvect", &nvect, "nvect/I");
        tree->Branch("pos", pos, "pos[3]/F");
        tree->Branch("ip", ip, "ip[nvect]/I");
        tree->Branch("pin", pin , "pin[nvect][3]/F");
        tree->Branch("pabs", &pabs, "pabs[nvect]/I");
        tree->Branch("np", &np, "np/I");
	    tree->Branch("nGd", nGd, "nGd[np]/I");
        tree->Branch("timeRes",	timeRes, "timeRes[np]/F");
        tree->Branch("doubleCount",	doubleCount, "doubleCount[np]/I");
        //tree->Branch("truth_vx", truth_vx, "truth_vx[np]/F");
	    //tree->Branch("truth_vy", truth_vy, "truth_vy[np]/F");
	    //tree->Branch("truth_vz", truth_vz, "truth_vz[np]/F");
    }
    else{
        tree->Branch("nhitac", &nhitac, "nhitac/I");
        tree->Branch("ndcy", &ndcy, "ndcy/I");
        tree->Branch("qismsk", &qismsk, "qismsk/I");
        tree->Branch("apnmue", &nmue, "apnmue/I");
        tree->Branch("apnring", &nring, "apnring/I");
        tree->Branch("apip", apip, "apip[apnring]/I");
        tree->Branch("apamom", apamom, "apamom[apnring]/F");
        tree->Branch("amome", amome, "amome[apnring]/F");
        tree->Branch("amomm", amomm, "amomm[apnring]/F");
        tree->Branch("TMVAoutput", TMVAoutput, "TMVAoutput[np]/F");
        tree->Branch("sumQ", sumQ, "sumQ[np]/F");
    }
}