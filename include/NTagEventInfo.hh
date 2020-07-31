#ifndef NTAGEVENTINFO_HH
#define NTAGEVENTINFO_HH 1

#define MAXNP (500)
#define kMaxCT (4000)
#define MAXNSCNDPRT (4000)

#include "skparmC.h"

class NTagEventInfo
{
	public:
		NTagEventInfo();
		virtual ~NTagEventInfo();

		// Calculations
		float GetDWall(float& x, float& y, float& z);

		virtual void Clear();

	protected:

		/******************************************************************************************/
		// Data/fit info
		/******************************************************************************************/
		/**/
		/**/	// SK data variables
		/**/	int		nrun, nsub, nev, trgtype, nhitac;
		/**/	float	trgofst, timnsk, qismsk;
        /**/    int   	cabiz3[30*MAXPM];
		/**/	float 	tiskz3[30*MAXPM], qiskz3[30*MAXPM];
		/**/
		/**/	// APFit variables
		/**/	int   	nring, nmue, ndcy;
  		/**/	float 	evis, vx, vy, vz, towall;
        /**/    int   	apip[APNMAXRG];
        /**/    float 	apamom[APNMAXRG], amome[APNMAXRG], amomm[APNMAXRG];
		/**/
		/**/	// Variables for neutron capture candidates
    	/**/		int   	np, N200M;
		/**/		int		tindex[MAXNP], n40index[MAXNP];
    	/**/		int   	N10[MAXNP], N10n[MAXNP], N50[MAXNP], N200[MAXNP], N1300[MAXNP];
    	/**/		float 	T200M;
        /**/    	float 	sumQ[MAXNP], spread[MAXNP], trms[MAXNP], trmsold[MAXNP], trms40[MAXNP];
    	/**/		float 	mintrms_3[MAXNP], mintrms_4[MAXNP], mintrms_5[MAXNP], mintrms_6[MAXNP];
    	/**/		float 	dt[MAXNP], dtn[MAXNP];
		/**/		float 	nvx[MAXNP], nvy[MAXNP], nvz[MAXNP], nwall[MAXNP];
		/**/		float 	tvx[MAXNP], tvy[MAXNP], tvz[MAXNP]; 
		/**/		int   	doubleCount[MAXNP], goodn[MAXNP];
		/**/
		/**/		// BONSAI variables
		/**/		float	tbsenergy[MAXNP], tbsenergy2[MAXNP];
		/**/		float 	tbsvx[MAXNP], tbsvy[MAXNP], tbsvz[MAXNP], tbsvt[MAXNP];
		/**/		float 	tbswall[MAXNP], tbsgood[MAXNP];
		/**/		float 	tbspatlik[MAXNP], tbsdirks[MAXNP], tbsovaq[MAXNP];
		/**/		float	bsenergy[MAXNP], bsenergy2[MAXNP], 
		/**/				bsvertex0[MAXNP], bsvertex1[MAXNP], bsvertex2[MAXNP];
		/**/		float 	bsgood[MAXNP];
		/**/
		/**/		// Beta variables
    	/**/		float 	beta14[MAXNP], beta14_40[MAXNP];
    	/**/		float 	beta1[MAXNP], beta2[MAXNP], beta3[MAXNP], beta4[MAXNP], beta5[MAXNP];
		/**/
		/**/	// probably obsolete or not needed
  		/**/	int 	mctrue_nn, ip0, broken;
		/**/	float	lasthit, firsthit, firstflz, bt;
    	/**/	float 	ratio[MAXNP], phirms[MAXNP], thetam[MAXNP], summedWeight[MAXNP], g2d2[MAXNP];
    	/**/	int   	Nback[MAXNP], Neff[MAXNP], Nc1[MAXNP], NhighQ[MAXNP], Nlow[9][MAXNP], 
		/**/			Nc[MAXNP], Ncluster[MAXNP], Nc8[MAXNP], Ncluster8[MAXNP], 
		/**/			Nc7[MAXNP], Ncluster7[MAXNP], N12[MAXNP], N20[MAXNP], N300[MAXNP];
		/**/			
    	/**/	float 	px, py, pz, npx[MAXNP], npy[MAXNP], npz[MAXNP],
		/**/			dirx, diry, dirz, ndirx[MAXNP], ndiry[MAXNP], ndirz[MAXNP];
		/******************************************************************************************/

		/******************************************************************************************/
        // MC truth info
		/******************************************************************************************/
		/**/
        /**/    // Variables for true neutron capture
        /**/    int 	nCT, nGam[kMaxCT];
        /**/    float 	captureTime[kMaxCT], capPos[kMaxCT][3], totGamEn[kMaxCT];
		/**/
        /**/    // Variables for neutron capture candidates
        /**/    int 	nGd[MAXNP], realneutron[MAXNP];
		/**/	float 	truth_vx[MAXNP], truth_vy[MAXNP], truth_vz[MAXNP], timeRes[MAXNP];
		/**/
        /**/    // Variables from secondaries
		/**/	int		nscndprt; 
        /**/    int 	iprtscnd[MAXNSCNDPRT], lmecscnd[MAXNSCNDPRT], iprntprt[MAXNSCNDPRT];
        /**/    float 	vtxscnd[MAXNSCNDPRT][3], pscnd[MAXNSCNDPRT][3];
        /**/    float 	wallscnd[MAXNSCNDPRT], pabsscnd[MAXNSCNDPRT], tscnd[MAXNSCNDPRT];
        /**/    int 	capId[MAXNSCNDPRT];
		/**/
        /**/    // Variables for neutrino interaction
        /**/    int    	nN, modene, numne, ipne[kMaxCT];
        /**/    float  	pnu;
		/**/
        /**/    // Variables from primary stack
        /**/    int   	nvect, ip[MAXNSCNDPRT];
        /**/    float  	pos[3], pin[MAXNSCNDPRT][3], pabs[MAXNSCNDPRT];
		/**/
        /**/    // TMVA output
    	/**/    float 	TMVAoutput[MAXNP];
		/******************************************************************************************/
};

typedef NTagEventInfo Res_t;

#endif