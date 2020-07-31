#ifndef NTAGEVENTINFO_HH
#define NTAGEVENTINFO_HH 1

#define MAXNP (500)
#define kMAXCT (4000)
#define MAXNSCNDPRT (4000)

class NTagEventInfo
{
	public:
		NTagEventInfo(bool isData);
		virtual ~NTagEventInfo();

		virtual void Clear();

	protected:

        bool bData;

		// Data/fit info

			// SK data variables
			Int_t	nrun, nsub, nev, trgtype, nhitac;
			Float_t	trgofst, timnsk;

			// APFit variables
			Int_t   nring;
  			Float_t evis, vx, vy, vz, towall;

			// Variables for neutron capture candidates
    			Int_t   np;
				Int_t	tindex[MAXNP];
    			Int_t   N10[MAXNP], N10n[MAXNP], N50[MAXNP], N200[MAXNP], N200M, N1300[MAXNP];
    			Float_t T200M;
    			Float_t spread[MAXNP], trms[MAXNP], trmsold[MAXNP], trms40[MAXNP];
    			Float_t mintrms_3[MAXNP], mintrms_4[MAXNP], mintrms_5[MAXNP], mintrms_6[MAXNP];
    			Float_t dt[MAXNP], dtn[MAXNP];
				Float_t nvx[MAXNP], nvy[MAXNP], nvz[MAXNP], nwall[MAXNP];
				Float_t tvx[MAXNP], tvy[MAXNP], tvz[MAXNP]; 
				Int_t   doubleCount[MAXNP], goodn[MAXNP];
				Int_t	n40index[MAXNP];

				// BONSAI variables
				Float_t	tbsenergy[MAXNP], tbsenergy2[MAXNP];
				Float_t tbsvx[MAXNP], tbsvy[MAXNP], tbsvz[MAXNP], tbsvt[MAXNP];
				Float_t tbswall[MAXNP], tbsgood[MAXNP];
				Float_t tbspatlik[MAXNP], tbsdirks[MAXNP], tbsovaq[MAXNP];
				Float_t	bsenergy[MAXNP], bsenergy2[MAXNP], bsvertex0[MAXNP], bsvertex1[MAXNP], bsvertex2[MAXNP];
				Float_t bsgood[MAXNP];

				// Beta variables
    			Float_t beta14[MAXNP], beta14_40[MAXNP];
    			Float_t beta1[MAXNP], beta2[MAXNP], beta3[MAXNP], beta4[MAXNP], beta5[MAXNP];

			// probably obsolete or not needed
  			Int_t 	mctrue_nn, ip0, nscndprt, broken;
			Float_t	lasthit, firsthit, firstflz, bt;
    		Float_t ratio[MAXNP], phirms[MAXNP], thetam[MAXNP], summedWeight[MAXNP], g2d2[MAXNP];
    		Int_t   Nback[MAXNP], Neff[MAXNP], Nc1[MAXNP], NhighQ[MAXNP], Nlow[9][MAXNP], 
					Nc[MAXNP], Ncluster[MAXNP], Nc8[MAXNP], Ncluster8[MAXNP], Nc7[MAXNP], Ncluster7[MAXNP],
					N12[MAXNP], N20[MAXNP], N300[MAXNP];
    		Float_t px, py, pz, npx[MAXNP], npy[MAXNP], npz[MAXNP],
					dirx, diry, dirz, ndirx[MAXNP], ndiry[MAXNP], ndirz[MAXNP];

		// MC truth-related variables
    	Int_t   realneutron[MAXNP];

		// SK data variables
            Float_t qismsk;
            Int_t   cabiz3[30*MAXPM];
            Float_t tiskz3[30*MAXPM], qiskz3[30*MAXPM];

            // APFit variables
            Int_t   nmue, ndcy;
            Int_t   apip[APNMAXRG];
            Float_t apamom[APNMAXRG], amome[APNMAXRG], amomm[APNMAXRG];

            // Variables for neutron capture candidates
            Float_t sumQ[MAXNP];

        // MC truth info

            // Variables for true neutron capture
            Int_t nCT;
            Float_t captureTime[kMaxCT];
            Float_t capPos[kMaxCT][3];
            Int_t nGam[kMaxCT];
            Float_t totGamEn[kMaxCT];

            // Variables for neutron capture candidates
            Int_t nGd[MAXNP];
			Float_t truth_vx[MAXNP], truth_vy[MAXNP], truth_vz[MAXNP], timeRes[MAXNP];

            // TMVA output
    	    Float_t TMVAoutput[MAXNP];

            // Variables from secondaries
            Int_t iprtscnd[MAXNSCNDPRT], lmecscnd[MAXNSCNDPRT], iprntprt[MAXNSCNDPRT];
            Float_t vtxscnd[MAXNSCNDPRT][3], pscnd[MAXNSCNDPRT][3];
            Float_t wallscnd[MAXNSCNDPRT], pabsscnd[MAXNSCNDPRT], tscnd[MAXNSCNDPRT];
            Int_t capId[MAXNSCNDPRT];

            // Variables for neutrino interaction
            Int_t    modene, numne, ipne[kMaxCT];
            Int_t    nN;
            Float_t  pnu;

            // Variables from primary stack
            Int_t   nvect, ip[MAXNSCNDPRT];
            Float_t  pos[3], pin[MAXNSCNDPRT][3], pabs[MAXNSCNDPRT];
};

typedef NTagEventInfo Res_t;

#endif