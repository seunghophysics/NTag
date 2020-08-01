#include <math.h>
#include <iostream>

#include <TMath.h>

#include <skparmC.h>
#include <apmringC.h>
#include <apmueC.h>
#include <appatspC.h>
#include <geotnkC.h>
#include <skheadC.h>
#include <sktqC.h>
#include <geopmtC.h>
#include <skvectC.h>
#include <neworkC.h>

#include <NTagEventInfo.hh>
#include <SKLibs.hh>

NTagEventInfo::NTagEventInfo()
:xyz(geopmt_.xyzpm), C_WATER(21.5833) {}
NTagEventInfo::~NTagEventInfo() {}

void NTagEventInfo::SetEventHeader()
{
    nrun = skhead_.nrunsk;
    nsub = skhead_.nsubsk;
    nev = skhead_.nevsk;

	// Get number of OD hits
    odpc_2nd_s_(&nhitac);

    trginfo_(&trgofst);
    qismsk = skq_.qismsk;
}

void NTagEventInfo::SetAPFitInfo()
{
    // Get apcommul bank
	int bank = 0;
    aprstbnk_(&bank);

	// Get APFit vertex
	vx = apcommul_.appos[0];
	vy = apcommul_.appos[1];
	vz = apcommul_.appos[2];
	float tmp_v[3]; tmp_v[0] = vx; tmp_v[1] = vy; tmp_v[2] = vz;
    towall = wallsk_(tmp_v);

    // E_vis
    evis = apcomene_.apevis;

    // AP ring information
    nring = apcommul_.apnring;
    for (int i=0; i < nring; i++) {
      apip[i] = apcommul_.apip[i];          // PID
      apamom[i] = apcommul_.apamom[i];      // Reconstructed momentum
      amome[i] = appatsp2_.apmsamom[i][1];  // e-like momentum
      amomm[i] = appatsp2_.apmsamom[i][2];  // mu-like momentum
    }

    // mu-e check
    nmue = apmue_.apnmue; ndcy = 0;
    for (int i = 0; i < nmue; i++) {
      if (i == 10) break;
      if (apmue_.apmuetype[i] == 1 || apmue_.apmuetype[i] == 4) ndcy++;
    }
}

void NTagEventInfo::SetToFSubtractedTQ()
{
    float tmpHitTime[sktqz_.nqiskz], ToF;
    int   sortedIndex[sktqz_.nqiskz], pmtID;

	// Subtract TOF from PMT hit time
    for(int i = 0; i < sktqz_.nqiskz; i++){
		pmtID = sktqz_.icabiz[i] - 1;
        tmpHitTime[i] = sktqz_.tiskz[i];

		float vecVtxFromPMT[3];

		vecVtxFromPMT[0] = vx - xyz[pmtID][0];
		vecVtxFromPMT[1] = vy - xyz[pmtID][1];
		vecVtxFromPMT[2] = vz - xyz[pmtID][2];

        ToF = Norm(vecVtxFromPMT) / C_WATER;

        tmpHitTime[i] -= ToF;
    }

	// Sort: first hit first
    TMath::Sort(sktqz_.nqiskz, tmpHitTime, sortedIndex, kFALSE);

    for (int i = 0; i < sktqz_.nqiskz; i++){
        cabiz3[i] = sktqz_.icabiz[ sortedIndex[i] ];
        tiskz3[i] = tmpHitTime[ sortedIndex[i] ];
        qiskz3[i] = sktqz_.qiskz[ sortedIndex[i] ];	
    } 
}

void NTagEventInfo::SetTruthInfo()
{ 
  	// Read SKVECT (primaries)
  	skgetv_();
  	nvect = skvect_.nvect;					// number of primaries
  	pos[0] = skvect_.pos[0]; 				// initial vertex of primaries
	pos[1] = skvect_.pos[1]; 
	pos[2] = skvect_.pos[2];

  	for(int i = 0; i < nvect; i++){
  	  	ip[i] = skvect_.ip[i];				// PID of primaries
  	  	pin[i][0] = skvect_.pin[i][0];		// momentum vector of primaries
		pin[i][1] = skvect_.pin[i][1];		
		pin[i][2] = skvect_.pin[i][2];
  	  	pabs[i] = skvect_.pabs[i];			// momentum of primaries
  	}

  	// Read neutrino interaction vector
  	float posnu[3];
  	nerdnebk_(posnu);

  	modene = nework_.modene;				// neutrino interaction mode
  	numne = nework_.numne;					// number of particles in vector
  	nN = 0; 								// number of neutrons
	pnu = Norm(nework_.pne[0]);

  	for(int i = 0; i < numne; i++){
  	  ipne[i] = nework_.ipne[i];			// check particle code
  	  if(ipne[i] == 2112 && i >= 3) nN++;	// count neutrons
  	}

	PrintMessage(Form("modene: %d", modene), vDebug);
	PrintMessage(Form("pnu: %f", pnu), vDebug);
	PrintMessage(Form("nN: %d", nN), vDebug);
	
  	// Get bank containing reconstructed vertex information

  	nCT = 0;

  	float ZBLST = 5.30;
  	float dr = RINTK - ZBLST;
  	float dz = 0.5 * HIINTK - ZBLST;

	PrintMessage(Form("Reading aflscndprt..."), vDebug);
  	// Read secondary bank
  	apflscndprt_();
	PrintMessage(Form("aflscndprt done, nscndprt: %d", secndprt_.nscndprt), vDebug);
  	for(int i = 0; i < secndprt_.nscndprt; i++) {
  	  	iprtscnd[i] = secndprt_.iprtscnd[i];				// PID of secondaries
		PrintMessage(Form("secondary PID: %d", secndprt_.iprtscnd[i]), vDebug);
		PrintMessage(Form("secondary mom: %f", secndprt_.pscnd[i][0]), vDebug);
		PrintMessage(Form("secondary time: %f", secndprt_.tscnd[i]), vDebug);
	  	// save secondary info if PID is:
  	  	//                deutron                  gamma               neutron
  	  	if(iprtscnd[i] == 100045 || iprtscnd[i] == 22|| iprtscnd[i] == 2112){
  	    	lmecscnd[i] = secndprt_.lmecscnd[i];			// creation process
  	    	iprntprt[i] = secndprt_.iprntprt[i];			// parent PID
  	    	vtxscnd[i][0] = secndprt_.vtxscnd[i][0];		// creation vertex
  	    	vtxscnd[i][1] = secndprt_.vtxscnd[i][1];
  	    	vtxscnd[i][2] = secndprt_.vtxscnd[i][2];
  	    	wallscnd[i] = wallsk_(vtxscnd[i]);				// distance from wall to creation vertex
  	    	pscnd[i][0] = secndprt_.pscnd[i][0];			// momentum vector
  	    	pscnd[i][1] = secndprt_.pscnd[i][1];
  	    	pscnd[i][2] = secndprt_.pscnd[i][2];
  	    	pabsscnd[i] = Norm(pscnd[i]);					// momentum
  	    	tscnd[i] = secndprt_.tscnd[i];					// time created
  	    	capId[i] = -1;									// initialize capture index (-1: not from n capture) 
  	    	
			int inPMT;
  	    	inpmt_(vtxscnd[i], inPMT);
			
			// record all neutron produced
  	    	if(iprtscnd[i] == 2112) { i += 1; PrintMessage(Form("Now i is %d", i), vDebug); }
			
  	    	// record only deutrons and gammas produced inside ID by capture
  	    	else if(Norm(vtxscnd[i]) < dr*dr && fabs(vtxscnd[i][2]) < dz && !inPMT) {
  	      		if(lmecscnd[i] == 18){		//particle produced by n-capture
  	        		bool isNewCapture = true;
  	       			 for(int j = 0; j < nCT; j++){
						// judge whether this is a new n-capture
  	          			if(fabs((double)(tscnd[i]-captureTime[j])) < 1.e-7) {
  	            			isNewCapture = false;
							// count gammas produced in capture
  	            			if(iprtscnd[i] == 22){
								  nGam[j] += 1;
								  totGamEn[j] += pabsscnd[i];
								  capId[i] = j;}
  	          			}
  	        		}
					// save new capture
  	        		if(isNewCapture){
  	          			captureTime[nCT] = tscnd[i];
  	          			capPos[nCT][0] = vtxscnd[i][0];
						capPos[nCT][1] = vtxscnd[i][1];
						capPos[nCT][2] = vtxscnd[i][2];
						//count gammas produced in capture
  	          			if(iprtscnd[i] == 22){
								nGam[nCT] = 1; 
								totGamEn[nCT] = pabsscnd[i]; 
								capId[i] = nCT; 
						}
  	          			else { nGam[nCT] = 0; totGamEn[nCT] = 0.; }
  	          			nCT += 1;
  	        		}
  	      		}
  	    	}
  	  	}
  	}
}

void NTagEventInfo::Clear()
{	
	nrun = nsub = nev = trgtype = nhitac = 0;
	trgofst = timnsk = qismsk = 0.;
	nring = nmue = ndcy = 0;
  	evis = vx = vy = vz = towall = 0.;
    N200M = 0;
	mctrue_nn = ip0 = nscndprt = broken = 0;
	lasthit = firsthit = firstflz = 0.;
	px = py = pz = 0.;
	dirx = diry = dirz = 0.;
	nCT = 0;
	nN = modene = numne = 0;
	pnu = 0.;
	nvect = 0;
	bt = 1000000;
    T200M = -9999.;
	pos[0] = pos[1] = pos[2] = 0.;

	for(int i = 0; i < 30*MAXNP; i++){
		cabiz3[i] = 0;
		tiskz3[i] = qiskz3[i] = 0.;
	}

	for(int i = 0; i < APNMAXRG; i++){
		apip[i] = 0;
		apamom[i] = amome[i] = amomm[i] = 0.;
	}

    for(int i = 0; i < MAXNP; i++){
		tindex[i] = n40index[i] = 0;
		N10[i] =  N10n[i] =  N50[i] =  N200[i] =  N1300[i] = 0;
		sumQ[i] =  spread[i] =  trms[i] =  trmsold[i] =  trms40[i] = 0.;
		mintrms_3[i] =  mintrms_4[i] =  mintrms_5[i] =  mintrms_6[i] = 0.;
		dt[i] =  dtn[i] = 0.;
		nvx[i] =  nvy[i] =  nvz[i] =  nwall[i] = 0.;
		tvx[i] =  tvy[i] =  tvz[i] = 0.;
		doubleCount[i] =  goodn[i] = 0.;
		tbsenergy[i] =  tbsenergy2[i] = 0.;
		tbsvx[i] =  tbsvy[i] =  tbsvz[i] =  tbsvt[i] = 0.;
		tbswall[i] =  tbsgood[i] = 0.;
		tbspatlik[i] =  tbsdirks[i] =  tbsovaq[i] = 0.;
		bsenergy[i] =  bsenergy2[i] = 0.;
		bsvertex0[i] =  bsvertex1[i] =  bsvertex2[i] = 0.;
		bsgood[i] = 0.;

		beta14[i] =  beta14_40[i] = 0.;
		beta1[i] =  beta2[i] =  beta3[i] =  beta4[i] =  beta5[i] = 0.;
		ratio[i] =  phirms[i] =  thetam[i] =  summedWeight[i] =  g2d2[i] = 0.;
		Nback[i] =  Neff[i] =  Nc1[i] =  NhighQ[i] = 0;
		Nc[i] =  Ncluster[i] =  Nc8[i] =  Ncluster8[i] = 0;
		Nc7[i] =  Ncluster7[i] =  N12[i] =  N20[i] =  N300[i] = 0;
		npx[i] =  npy[i] =  npz[i] = 0.;
		ndirx[i] =  ndiry[i] =  ndirz[i] = 0.;

		nGd[i] =  realneutron[i] = 0;
		truth_vx[i] =  truth_vy[i] =  truth_vz[i] =  timeRes[i] = 0.;

		TMVAoutput[i] = -9999.;

		for(int j = 0; j < 9; j++) Nlow[j][i] = 0;
    }

	for(int i = 0; i < kMaxCT; i++){
		nGam[i] = 0;
		captureTime[i] = totGamEn[i] = 0.;
		ipne[i] = 0;

		for(int j = 0; j < 3; j++){
			capPos[i][j] = 0.;
		}
	}

	for(int i = 0; i < SECMAXRNG; i++){
		iprtscnd[i] = lmecscnd[i] = iprntprt[i] = 0;
		wallscnd[i] = pabsscnd[i] = tscnd[i] = 0.;
		capId[i] = 0;
		ip[i] = 0;
		pabs[i] = 0.;

		for(int j = 0; j < 3; j++){
			vtxscnd[i][j] = pscnd[i][j] = pin[i][j] = 0.;
		}
	}
}

float NTagEventInfo::Norm(float vec[3])
{
	return sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
}

void NTagEventInfo::PrintTag(unsigned int vType){
	switch(vType){
		case vDefault:
			std::cout << "[NTag] ";
			break;
		case vError:
			std::cerr << "\033[1;31m" << "[Error in NTag] ";
			break;
		case vDebug:
			std::cout << "\033[0;34m" << "[NTag DEBUG] ";
			break;
	}
}

void NTagEventInfo::PrintMessage(TString msg, unsigned int vType)
{
    if(vType <= fVerbosity){
		PrintTag(vType);
		if(vType == vError) std::cerr << "\033[m" << msg << std::endl;
		else std::cout << "\033[m" << msg << std::endl;
	}
}