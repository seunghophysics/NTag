
#include <math.h>

#include <geotnkC.h>

#include <NTagEventInfo.hh>

NTagEventInfo::NTagEventInfo() {}
NTagEventInfo::~NTagEventInfo() {}

float NTagEventInfo::GetDWall(float& x, float& y, float& z)
{
	float r2 = x*x + y*y;
    float r = sqrt(r2);
    float d_wall = RINTK - r;
    float z1 = ZPINTK - z;
    float z2 = z - ZMINTK;

    if(z1 < d_wall) d_wall = z1;
    if(z2 < d_wall) d_wall = z2;
    return d_wall;
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