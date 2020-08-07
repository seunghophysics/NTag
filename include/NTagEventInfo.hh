#ifndef NTAGEVENTINFO_HH
#define NTAGEVENTINFO_HH 1

#define SECMAXRNG (4000)

#include <array>
#include <vector>
#include <ctime>

#include <TString.h>
#include <TMVA/Reader.h>

#include "apscndryC.h"
#include "apmringC.h"

#include "loweroot.h"

enum {pDEFAULT, pWARNING, pERROR, pDEBUG};

class NTagEventInfo
{
    public:
        NTagEventInfo();
        virtual ~NTagEventInfo();

        // Event handling
        virtual void         SetEventHeader();
        virtual void         SetAPFitInfo();
        virtual void         SetLowFitInfo();
        virtual void         SetToFSubtractedTQ();
        virtual void         SetMCInfo();
        virtual void         SearchCaptureCandidates();
        virtual void         SetTrueCaptureInfo();
        virtual void         GetTMVAoutput();

        // Calculator
        inline float         Norm(float vec[3]);
        inline float         Norm(float x, float y, float z);
        float                GetDistance(const float vec1[3], float vec2[3]);
        float                GetLegendreP(int i, float& x); // Legendre polynomials for betas
        std::array<float, 6> GetBetaArray(std::vector<int>& PMTID, int tID, int nHits);

        // Variable-related functions

            /* functions on capture candidates */
            inline float         ReconCaptureTime(int candidateID);
            int                  IsTrueCapture(int candidateID);
            int                  IsTrueGdCapture(int candidateID);
            float                TrueCaptureTime(int candidateID);
            std::array<float, 3> TrueCaptureVertex(int candidateID);

            /* functions on hits */
            int                  GetNhitsFromStartIndex(std::vector<float>& T, int startIndex, float tWidth);
            int                  GetNhitsFromCenterTime(std::vector<float>& T, float centerTime, float searchTWidth);
            float                GetQhitsFromStartIndex(std::vector<float>& T, std::vector<float>& Q, int startIndex, float tWidth);
            float                GetToF(float vertex[3], int pmtID);
            float                GetTRMS(std::vector<float>& T);
            float                GetTRMSFromStartIndex(std::vector<float>& T, int startIndex, float tWidth);
            float                MinimizeTRMS(std::vector<float>& T, std::vector<int>& PMTID, float fitVertex[3]);
            std::vector<float>   GetToFSubtracted(std::vector<float>& T, std::vector<int>& PMTID,
                                                  float vertex[3], bool doSort=false);

        // Member variable control
        virtual void         Clear();
        virtual void         SaveSecondary(int secID);
        virtual void         SavePeakFromHit(int hitID);

        // Set tag conditions
        virtual void         SetTMVAReader();
        inline void          SetN10Limits(int low, int high) { N10TH = low; N10MX = high; }
        inline void          SetN200Max(int max) { N200MX = max; }
        inline void          SetT0Threshold(float th) { T0TH = th; }
        inline void          SetDistanceCut(float cut) { DISTCUT = cut; }
        inline void          SetTMatchWindow(float t) { TMATCHWINDOW = t; }
        inline void          SetTPeakSeparation(float t) { TMINPEAKSEP = t; }

        // Message
        virtual void         PrintTag(unsigned int);
        virtual void         PrintMessage(TString, unsigned int vType=pDEFAULT);
        virtual void         PrintMessage(const char*, unsigned int vType=pDEFAULT);
        virtual float        Timer(TString, std::clock_t tStart);

    private:
        const float (*xyz)[3];              // PMT positions
        const float C_WATER;                // Speed-of-light in water [cm/ns]

        // Tag conditions
        int         N10TH, N10MX, N200MX;   // N_hits cut
        float       T0TH;                   // T0 threshold
        float       DISTCUT;
        float       TMATCHWINDOW;           // used in function IsTrueCapture
        float       TMINPEAKSEP;            // minimum peak separation in time

        TMVA::Reader* reader;

    protected:
        unsigned int fVerbosity;
        bool bData;

        /************************************************************************************************/
        // Data/fit event info
        /************************************************************************************************/
        /**/
        /**/    // SK data variables
        /**/    int                 nrun, nsub, nev, trgtype, nhitac, nqiskz;
        /**/    float               trgofst, qismsk;
        /**/    std::vector<int>    vSortedPMTID;
        /**/    std::vector<float>  vSortedT_ToF, vUnsortedT_ToF, vSortedQ;
        /**/
        /**/    /* M.Harada */
        /**/    /* vx, vy, vz, towall should be same for both ATMPD and LOWE,*/
        /**/    /* because for using same Ntag algorithm.*/
        /**/    // Fit variables
        /**/    float               pvx, pvy, pvz, towall;
        /**/
        /**/    // APFit variables
        /**/    int                 nring, nmue, ndcy;
        /**/    float               evis;
        /**/    std::vector<int>    vApip;
        /**/    std::vector<float>  vApamom, vApmome, vApmomm;
        /**/
        /**/    // LowFit variables
        /**/    //float               bsenergy;
        /**/
        /**/        // Variables for neutron capture candidates
        /**/        int                 nCandidates, N200M;
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
        /**/    std::vector<float>  vTMVAoutput;
        /**/
        /************************************************************************************************/

        /************************************************************************************************/
        // MC truth event info
        /************************************************************************************************/
        /**/
        /**/    // Variables for true neutron capture
        /**/    int                 nTrueCaptures;
        /**/    std::vector<int>    vNGam;
        /**/    std::vector<float>  vCaptureTime, vCapPosx, vCapPosy, vCapPosz, vTotGamE;
        /**/
        /**/    // Variables for neutron capture candidates
        /**/    std::vector<int>    vIsGdCapture, vIsTrueCapture;
        /**/    std::vector<float>  vTruth_vx, vTruth_vy, vTruth_vz, vTimeDiff;
        /**/
        /**/    // Variables from secondaries
        /**/    int                 nSavedSec, nscndprt;
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
        /**/    float               truevx, truevy, truevz;
        /**/    std::vector<int>    vIp;
        /**/    std::vector<float>  vPinx, vPiny, vPinz, vPabs;
        /**/
        /************************************************************************************************/
};

typedef NTagEventInfo Res_t;

#endif
