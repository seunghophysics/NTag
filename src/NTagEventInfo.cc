#include <math.h>

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
#include <apflscndprtC.h>

#include <NTagEventInfo.hh>
#include <SKIOLib.hh>

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

        ToF = sqrt((vx - xyz[pmtID][0]) * (vx - xyz[pmtID][0])
		   		 + (vy - xyz[pmtID][1]) * (vy - xyz[pmtID][1])
		   		 + (vz - xyz[pmtID][2]) * (vz - xyz[pmtID][2])) / C_WATER;

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
	// Read NEUT vector
  	float posnu[3]; 
  	nerdnebk_(posnu);
	  
  	// Read SKVECT vector
  	skgetv_();
  	// Read secondary bank
  	apflscndprt_();

  	float ZBLST = 5.30;

  	float dr = RINTK-ZBLST;
  	float dz = 0.5*HIINTK-ZBLST;

  	// Read primaries
  	nvect = skvect_.nvect;
  	pos[0] = skvect_.pos[0]; 
	pos[1] = skvect_.pos[1]; 
	pos[2] = skvect_.pos[2];

  	for(int i = 0; i < nvect; i++){
  	  	ip[i] = skvect_.ip[i];
  	  	pin[i][0] = skvect_.pin[i][0];
		pin[i][1] = skvect_.pin[i][1];
		pin[i][2] = skvect_.pin[i][2];
  	  	pabs[i] = skvect_.pabs[i];
  	}

  	//event loop to read neutrino interaction
  	modene=nework_.modene;
  	numne=nework_.numne;
  	nN=0;
  	pnu=sqrt(nework_.pne[0][0]*nework_.pne[0][0]+nework_.pne[0][1]*nework_.pne[0][1]+nework_.pne[0][2]*nework_.pne[0][2]);
  	for (int i=0;i<numne;i++) {
  	  ipne[i]=nework_.ipne[i];
  	  // count neutron in input vector
  	  if (ipne[i]==2112&&i>=3) nN++;
  	}

  	//get bank containing reconstructed vertex information

  	nCT = 0;

  	//event loop to read seconadries
  	for(int i=0;i<secndprc_.nscndprtc;i++) {
  	  iprtscnd[i] = secndprc_.iprtscndc[i];
  	  //record only deutrons, gammas and neutrons
  	  if (iprtscnd[i]==100045||iprtscnd[i]==22||iprtscnd[i]==2112) {
  	    lmecscnd[i]=secndprc_.lmecscndc[i];
  	    iprntprt[i]=secndprc_.iprntprtc[i];
  	    vtxscnd[i][0]=secndprc_.vtxscndc[i][0];
  	    vtxscnd[i][1]=secndprc_.vtxscndc[i][1];
  	    vtxscnd[i][2]=secndprc_.vtxscndc[i][2];
  	    wallscnd[i]=wallsk_(vtxscnd[i]);
  	    pscnd[i][0]=secndprc_.pscndc[i][0];
  	    pscnd[i][1]=secndprc_.pscndc[i][1];
  	    pscnd[i][2]=secndprc_.pscndc[i][2];
  	    pabsscnd[i]=sqrt(pscnd[i][0]*pscnd[i][0]+pscnd[i][1]*pscnd[i][1]+pscnd[i][2]*pscnd[i][2]);
  	    tscnd[i]=secndprc_.tscndc[i];
  	    capId[i]=-1;
  	    int pmtn;
  	    inpmt_(vtxscnd[i],pmtn);
  	    if (iprtscnd[i]==2112) {i+=1;} //record all neutron produced
  	    //record only deutrons and gammas produced inside ID by capture
  	    else if (vtxscnd[i][0]*vtxscnd[i][0]+vtxscnd[i][1]*vtxscnd[i][1]<dr*dr&&fabs(vtxscnd[i][2])<dz&&pmtn==0) {
  	      if (lmecscnd[i]==18) {//particle produced by n-capture
  	        bool newTime = true;
  	        for (int j=0;j<nCT;j++) {
  	          if (fabs((double)(tscnd[i]-captureTime[j]))<1.e-7) {//judge whether this is a new n-capture
  	            newTime=false;
  	            if(iprtscnd[i]==22) {nGam[j]+=1;totGamEn[j]+=pabsscnd[i];capId[i]=j;}//count gammas produced in capture
  	          }
  	        }
  	        if (newTime) { //record new capture event
  	          captureTime[nCT]=tscnd[i];
  	          capPos[nCT][0]=vtxscnd[i][0];capPos[nCT][1]=vtxscnd[i][1];capPos[nCT][2]=vtxscnd[i][2];
  	          if (iprtscnd[i]==22) {nGam[nCT]=1;totGamEn[nCT]=pabsscnd[i];capId[i]=nCT;} //count gammas produced in capture
  	          else {nGam[nCT]=0;totGamEn[nCT]=0.;}
  	          nCT+=1;
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

	for(int i = 0; i < MAXNSCNDPRT; i++){
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