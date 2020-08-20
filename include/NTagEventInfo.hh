#ifndef NTAGEVENTINFO_HH
#define NTAGEVENTINFO_HH 1

#define SECMAXRNG (4000)

#include <array>

#include <TString.h>
#include <TMVA/Reader.h>

#include "apscndryC.h"
#include "apmringC.h"
#include "skparmC.h"

#define MAXNP (500)
#define kMaxCT (4000)

enum {vDefault, vWarning, vError, vDebug};

class NTagEventInfo
{
    public:
        NTagEventInfo();
        virtual ~NTagEventInfo();

        // Event handling
        virtual void SetEventHeader();
        virtual void SetAPFitInfo();
        virtual void SetToFSubtractedTQ();
        virtual void SetMCInfo();
        virtual void SearchCaptureCandidates();
        virtual void SetTrueCaptureInfo();
        virtual void GetTMVAoutput();

        // Calculator
        inline float Norm(float vec[3]);
        inline float Norm(float x, float y, float z);
        float GetDistance(const float vec1[3], float vec2[3]);
        inline float ReconCaptureTime(int candidateID);
        float TrueCaptureTime(int candidateID);
        std::array<float, 3> TrueCaptureVertex(int candidateID);
        float SubtractToF(float t_ToF[], float T[], int PMTID[], int nHits, float vertex[3], bool doSort=false);
        float MinimizeTRMS(float T[], int PMTID[], int nHits, float fitVertex[3]);
        float GetLegendreP(int i, float& x); // Legendre polynomials for beta calculation
        std::array<float, 6> GetBetaArray(int PMTID[], int tID, int n10);

        int GetNhitsFromStartIndex(float T[], int nHits, int startIndex, float tWidth);
        float GetQhitsFromStartIndex(int startIndex, float tWidth);
        float GetTRMSFromStartIndex(float T[], int startIndex, float tWidth);
        int GetNhitsFromCenterTime(float centerTime, float searchTWidth);

        int IsTrueCapture(int candidateID);
        int IsTrueGdCapture(int candidateID);
        virtual void Clear();

        virtual void SetTMVAReader();
        inline void SetN10Limits(int low, int high) { N10TH = low; N10MX = high; }
        inline void SetN200Max(int max) { N200MX = max; }
        inline void SetT0Threshold(float th) { T0TH = th; }
        inline void SetDistanceCut(float cut) { distanceCut = cut; }
        inline void SetTMatchWindow(float t) { tMatchWindow = t; }

        // Message
        virtual void PrintTag(unsigned int);
        virtual void PrintMessage(TString, unsigned int vType=vDefault);
        virtual void PrintMessage(const char*, unsigned int vType=vDefault);

    private:
        const float (*xyz)[3];        // PMT positions
        const float C_WATER;          // Speed-of-light in water [cm/ns]

        int N10TH, N10MX, N200MX;     // N_hits cut
        float T0TH;                   // T0 threshold
        float distanceCut;
        float tMatchWindow;           // used in function IsTrueCapture

        TMVA::Reader* reader;

    protected:
        unsigned int fVerbosity;
        bool bData;

        /******************************************************************************************/
        // Data/fit info
        /******************************************************************************************/
        /**/
        /**/    // SK data variables
        /**/    int     nrun, nsub, nev, trgtype, nhitac;
        /**/    float   trgofst, timnsk, qismsk;
        /**/    int     nqiskz, sortedPMTID[30*MAXPM];
        /**/    float   sortedT_ToF[30*MAXPM], unsortedT_ToF[30*MAXPM], sortedQ[30*MAXPM];
        /**/
        /**/    // APFit variables
        /**/    int     nring, nmue, ndcy;
        /**/    float   evis, vx, vy, vz, towall;
        /**/    int     apip[APNMAXRG];
        /**/    float   apamom[APNMAXRG], amome[APNMAXRG], amomm[APNMAXRG];
        /**/
        /**/        // Variables for neutron capture candidates
        /**/        int     np, N200M;
        /**/        int     tindex[MAXNP];
        /**/        int     N10[MAXNP], N10n[MAXNP], N50[MAXNP], N200[MAXNP], N1300[MAXNP];
        /**/        float   T200M;
        /**/        float   sumQ[MAXNP], spread[MAXNP], trms[MAXNP], trmsold[MAXNP], trms50[MAXNP];
        /**/        float   mintrms_3[MAXNP], mintrms_4[MAXNP], mintrms_5[MAXNP], mintrms_6[MAXNP];
        /**/        float   dt[MAXNP], dtn[MAXNP];
        /**/        float   nvx[MAXNP], nvy[MAXNP], nvz[MAXNP], nwall[MAXNP];
        /**/        float   tvx[MAXNP], tvy[MAXNP], tvz[MAXNP]; 
        /**/        int     doubleCount[MAXNP], goodn[MAXNP];
        /**/
        /**/        // BONSAI variables
        /**/        float   tbsenergy[MAXNP], tbsenergy2[MAXNP];
        /**/        float   tbsvx[MAXNP], tbsvy[MAXNP], tbsvz[MAXNP], tbsvt[MAXNP];
        /**/        float   tbswall[MAXNP], tbsgood[MAXNP];
        /**/        float   tbspatlik[MAXNP], tbsdirks[MAXNP], tbsovaq[MAXNP];
        /**/        float   bsenergy[MAXNP], bsenergy2[MAXNP], 
        /**/                bsvertex0[MAXNP], bsvertex1[MAXNP], bsvertex2[MAXNP];
        /**/        float   bsgood[MAXNP];
        /**/
        /**/        // Beta variables
        /**/        float   beta14_10[MAXNP], beta14_50[MAXNP];
        /**/        float   beta1_50[MAXNP], beta2_50[MAXNP], beta3_50[MAXNP], beta4_50[MAXNP], beta5_50[MAXNP];
        /**/
        /**/    // probably obsolete or not needed
        /**/    int     mctrue_nn, ip0, broken;
        /**/    float   lasthit, firsthit, firstflz, bt;
        /**/    float   ratio[MAXNP], phirms[MAXNP], thetam[MAXNP], summedWeight[MAXNP], g2d2[MAXNP];
        /**/    int     Nback[MAXNP], Neff[MAXNP], Nc1[MAXNP], NhighQ[MAXNP], Nlow[9][MAXNP], 
        /**/            Nc[MAXNP], Ncluster[MAXNP], Nc8[MAXNP], Ncluster8[MAXNP], 
        /**/            Nc7[MAXNP], Ncluster7[MAXNP], N12[MAXNP], N20[MAXNP], N300[MAXNP];
        /**/		
        /**/    float 	px, py, pz, npx[MAXNP], npy[MAXNP], npz[MAXNP],
        /**/    		dirx, diry, dirz, ndirx[MAXNP], ndiry[MAXNP], ndirz[MAXNP];
        /**/
        /**/    // TMVA inputs and output
        /**/    float   mva_N10, mva_N50, mva_N200;
        /**/    float 	mva_evis, mva_dt, mva_sumQ, mva_spread, mva_trmsold, mva_nwall, mva_trms50;
        /**/    float	mva_beta1_50, mva_beta2_50, mva_beta3_50, mva_beta4_50, mva_beta5_50;
        /**/    float 	mva_tbsenergy, mva_tbswall, mva_tbsgood, mva_tbsdirks, mva_tbspatlik, mva_tbsovaq;
        /**/    float 	mva_AP_Nfit, mva_AP_BONSAI, mva_Nfit_BONSAI;
        /**/    float   TMVAoutput[MAXNP];
        /**/
        /******************************************************************************************/

        /******************************************************************************************/
        // MC truth info
        /******************************************************************************************/
        /**/
        /**/    // Variables for true neutron capture
        /**/    int     nCT, nGam[kMaxCT];
        /**/    float   captureTime[kMaxCT], capPos[kMaxCT][3], totGamEn[kMaxCT];
        /**/
        /**/    // Variables for neutron capture candidates
        /**/    int     nGd[MAXNP], realneutron[MAXNP];
        /**/    float   truth_vx[MAXNP], truth_vy[MAXNP], truth_vz[MAXNP], timeRes[MAXNP];
        /**/
        /**/    // Variables from secondaries
        /**/    int     nscnd, nscndprt; 
        /**/    int 	iprtscnd[SECMAXRNG], lmecscnd[SECMAXRNG], iprntprt[SECMAXRNG];
        /**/    float 	vtxscnd[SECMAXRNG][3], pscnd[SECMAXRNG][3];
        /**/    float 	wallscnd[SECMAXRNG], pabsscnd[SECMAXRNG], tscnd[SECMAXRNG];
        /**/    int 	capId[SECMAXRNG];
        /**/
        /**/    // Variables for neutrino interaction
        /**/    int    	nN, modene, numne, ipne[kMaxCT];
        /**/    float  	pnu;
        /**/
        /**/    // Variables from primary stack
        /**/    int   	nvect, ip[SECMAXRNG];
        /**/    float  	pos[3], pin[SECMAXRNG][3], pabs[SECMAXRNG];
        /**/
        /******************************************************************************************/
};

typedef NTagEventInfo Res_t;

#endif
