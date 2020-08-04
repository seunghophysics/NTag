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

enum {pDEFAULT, pWARNING, pERROR, pDEBUG};

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
        virtual void PrintMessage(TString, unsigned int vType=pDEFAULT);
        virtual void PrintMessage(const char*, unsigned int vType=pDEFAULT);

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
        // Data/fit event info
        /******************************************************************************************/
        /**/
        /**/    // SK data variables
        /**/    int                 nrun, nsub, nev, trgtype, nhitac;
        /**/    float               trgofst, timnsk, qismsk;
        /**/    int                 nqiskz;
        /**/    std::vector<int>    vSortedPMTID;
        /**/    std::vector<float>  vSortedT_ToF, vUnsortedT_ToF, vSortedQ;
        /**/
        /**/    // APFit variables
        /**/    int                 nring, nmue, ndcy;
        /**/    float               evis, apvx, apvy, apvz, towall;
        /**/    std::vector<int>    vApip;
        /**/    std::vector<float>  vApamom, vApmome, vApmomm;
        /**/
        /**/        // Variables for neutron capture candidates
        /**/        int                 np, N200M;
        /**/        float               T200M, firsthit;
        /**/        std::vector<int>    vTindex, vN10, vN10n, vN50, vN200, vN1300;
        /**/        std::vector<float>  vSumQ, vSpread, vTrms, vTrmsold, vTrms50;
        /**/        std::vector<float>  vDt, vDtn, vNvx, vNvy, vNvz, vNwall;
        /**/        std::vector<float>  vDoubleCount;
        /**/
        /**/        // BONSAI variables
        /**/        std::vector<float>  vBenergy, vBvx, vBvy, vBvz, vBvt;
        /**/        std::vector<float>  vBwall, vBgood, vBpatlik, vBdirks, vBovaq;
        /**/
        /**/        // Beta variables
        /**/        std::vector<float>  vBeta14_10, vBeta14_50;
        /**/        std::vector<float>  vBeta1_50, vBeta2_50, vBeta3_50, vBeta4_50, vBeta5_50;
        /**/
        /**/    // TMVA inputs and output
        /**/    float               mva_N10, mva_N50, mva_N200;
        /**/    float               mva_dt, mva_trmsold, mva_trms50;
        /**/    float               mva_nwall, mva_evis, mva_sumQ, mva_spread;
        /**/    float               mva_beta1_50, mva_beta2_50, mva_beta3_50, mva_beta4_50, mva_beta5_50;
        /**/    float               mva_tbsenergy, mva_tbswall, mva_tbsgood;
        /**/    float               mva_tbsdirks, mva_tbspatlik, mva_tbsovaq;
        /**/    float               mva_AP_Nfit, mva_AP_BONSAI, mva_Nfit_BONSAI;
        /**/    std::vector<float>  TMVAoutput;
        /**/
        /******************************************************************************************/

        /******************************************************************************************/
        // MC truth event info
        /******************************************************************************************/
        /**/
        /**/    // Variables for true neutron capture
        /**/    int                 nCT;
        /**/    std::vector<int>    vNGam;
        /**/    std::vector<float>  vCaptureTime, vCapPosx, vCapPosy, vCapPosz, vTotGamE;
        /**/
        /**/    // Variables for neutron capture candidates
        /**/    std::vector<int>    vIsGdCapture, vIsTrueCapture;
        /**/    std::vector<float>  vTruth_vx, vTruth_vy, vTruth_vz, vTimeDiff;
        /**/
        /**/    // Variables from secondaries
        /**/    int                 nscnd, nscndprt;
        /**/    std::vector<int> 	vIprtscnd, vLmecscnd, vIprntprt, vCaptureID;
        /**/    std::vector<float> 	vVtxscndx, vVtxscndy, vVtxscndz, vPscndx, vPscndy, vPscndz;
        /**/    std::vector<float> 	vWallscnd, vPabsscnd, vTscnd;
        /**/
        /**/    // Variables for neutrino interaction
        /**/    int    	            nN, modene, numne;
        /**/    float  	            pnu;
        /**/    std::vector<int>    vIpne;
        /**/
        /**/    // Variables from primary stack
        /**/    int                 nvect;
        /**/    std::vector<int>    vIp;
        /**/    std::vector<float>  vPosx, vPosy, vPosz, vPinx, vPiny, vPinz, vPabs;
        /**/
        /******************************************************************************************/
};

typedef NTagEventInfo Res_t;

#endif