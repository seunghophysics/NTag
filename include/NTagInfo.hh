#ifndef NTAGINFO_HH
#define NTAGINFO_HH 1

#include <TTree.h>
#define MAXNP (500)

class NTagInfo
{
	public:
		NTagInfo() {}
		~NTagInfo() {}

		void Clear();
		void SetBranchAddressOfTree(TTree*);
		void CreateBranchesToTree(TTree*);

		// SK data variables
		Int_t	nrun, nsub, nev, trgtype, nhitac;
		Float_t	trgofst, timnsk;

		// APFit variables
		Int_t 	apnmue, ndcy, nring;
  		Float_t evis, vx, vy, vz, wall;

		// Variables for neutron capture candidates
    		Int_t   np;
    		Float_t sumQ[MAXNP];
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
		
			// MC truth-related variables
    		Int_t   realneutron[MAXNP], nGd[MAXNP], timeRes[MAXNP];
			Float_t truth_vx[MAXNP], truth_vy[MAXNP], truth_vz[MAXNP];

			// TMVA output
    		Float_t TMVAoutput[MAXNP];

		// probably obsolete or not needed
		Float_t	lasthit, firsthit, firstflz;
    	Int_t   Neff[MAXNP];
    	Float_t ratio[MAXNP];
    	Int_t   Nc1[MAXNP];
    	Int_t   NhighQ[MAXNP];
    	Int_t   Nlow[9][MAXNP];
  		Int_t 	mctrue_nn, ip0, nscndprt;
		Float_t g2d2[MAXNP];
    	Int_t   Nback[MAXNP], Nc[MAXNP], Ncluster[MAXNP], Nc8[MAXNP], Ncluster8[MAXNP], Nc7[MAXNP], Ncluster7[MAXNP];
		Float_t phirms[MAXNP], thetam[MAXNP];
		Int_t	broken;
    	Float_t bt;
		Int_t 	N12[MAXNP], N20[MAXNP], N300[MAXNP];
    	Float_t px, py, pz, npx[MAXNP], npy[MAXNP], npz[MAXNP];
    	Float_t dirx, diry, dirz, ndirx[MAXNP], ndiry[MAXNP], ndirz[MAXNP];
  		Float_t summedWeight[MAXNP];

		// aliases
		void setBranch(TTree* tree) { SetBranchAddressOfTree(tree); }
		void createTree(TTree* tree) { CreateBranchesToTree(tree); }
};

typedef NTagInfo Res_t;

#endif
