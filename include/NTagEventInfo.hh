#ifndef NTAGEVENTINFO_HH
#define NTAGEVENTINFO_HH 1

#include <array>
#include <vector>
#include <ctime>

#include <TString.h>
#include <TMVA/Reader.h>

#include "NTagMessage.hh"
#include "NTagTMVA.hh"

class NTagEventInfo
{
    public:
        NTagEventInfo(unsigned int verbose);
        virtual ~NTagEventInfo();

        // Functions to set variables

            /* All types */
            virtual void         SetEventHeader();
            virtual void         SetAPFitInfo();
            virtual void         SetLowFitInfo();
            virtual void         AppendRawHitInfo();
            virtual void         SetToFSubtractedTQ();

            /* MC-only */
            virtual void         SetMCInfo();
            virtual void         ReadSecondaries();
            virtual void         SetTrueCaptureInfo();

        // Tagging
        virtual void         SearchCaptureCandidates();
        virtual void         GetTMVAoutput();

        // Calculator
        inline float         Norm(float vec[3]);
        inline float         Norm(float x, float y, float z);
        float                GetDistance(const float vec1[3], float vec2[3]);
        float                GetLegendreP(int i, float& x); // legendre polynomials for betas
        std::array<float, 6> GetBetaArray(const std::vector<int>& PMTID, int tStartIndex, int nHits);

        // Variable-related functions

            /* functions on capture candidates */
            inline float         ReconCaptureTime(int candidateID);
            int                  IsTrueCapture(int candidateID, bool bSave=false);
            int                  IsTrueGdCapture(int candidateID);
            float                TrueCaptureTime(int candidateID);
            std::array<float, 3> TrueCaptureVertex(int candidateID);

            /* functions on hits */
            void                 SortToFSubtractedTQ();
            int                  GetNhitsFromStartIndex(const std::vector<float>& T, int startIndex, float tWidth);
            int                  GetNhitsFromCenterTime(const std::vector<float>& T, float centerTime, float searchTWidth);
            float                GetQhitsFromStartIndex(const std::vector<float>& T, const std::vector<float>& Q,
                                                        int startIndex, float tWidth);
            float                GetToF(float vertex[3], int pmtID);
            float                GetTRMS(const std::vector<float>& T);
            float                GetTRMSFromStartIndex(const std::vector<float>& T, int startIndex, float tWidth);
            float                MinimizeTRMS(const std::vector<float>& T, const std::vector<int>& PMTID, float fitVertex[3]);
            std::vector<float>   GetToFSubtracted(const std::vector<float>& T, const std::vector<int>& PMTID,
                                                  float vertex[3], bool doSort=false);

        // Member variable control
        virtual void         Clear();
        virtual void         ClearRawHitInfo();
        virtual void         SaveSecondary(int secID);
        virtual void         SavePeakFromHit(int hitID);

        // Set tag conditions
        inline void          SetN10Limits(int low, int high) { N10TH = low; N10MX = high; }
        inline void          SetN200Max(int max) { N200MX = max; }
        inline void          SetT0Limits(float low, float high) { T0TH = low; T0MX = high; }
        inline void          SetDistanceCut(float cut) { VTXSRCRANGE = cut; }
        inline void          SetTMatchWindow(float t) { TMATCHWINDOW = t; }
        inline void          SetTPeakSeparation(float t) { TMINPEAKSEP = t; }
        inline void          SetMaxODHitThreshold(float q) { ODHITMX = q; }
        inline void          SetCustomVertex(float x, float y, float z)
                                            { customvx = x; customvy = y; customvz = z; bCustomVertex = true; }
        // TMVA tools
        // All input variables to TMVA are here!
        NTagTMVA    TMVATools;

    private:
        const float (*PMTXYZ)[3];           // PMT positions
        const float C_WATER;                // speed-of-light in water [cm/ns]

        // Tag conditions
        int         N10TH, N10MX, N200MX;   // N_hits cut
        float       VTXSRCRANGE;            // vertex search range in MinimizeTRMS
        float       T0TH, T0MX;             // T0 threshold
        float       TMATCHWINDOW;           // used in function IsTrueCapture
        float       TMINPEAKSEP;            // minimum peak separation in time
        float       ODHITMX;                // OD total charge threshold

        // Custom prompt vertex
        float       customvx, customvy, customvz;

    protected:
        NTagMessage  msg;
        unsigned int fVerbosity;
        bool         bData, bCustomVertex;

        /************************************************************************************************/
        // Data/fit event info
        /************************************************************************************************/
        /**/
        /**/    // SK data variables
        /**/    int                 nrun, nsub, nev, trgtype, nhitac, nqiskz;
        /**/    float               trgofst, qismsk;
        /**/    std::vector<int>    vCABIZ;
        /**/    std::vector<float>  vTISKZ, vQISKZ;
        /**/    std::vector<int>    vSortedPMTID;
        /**/    std::vector<float>  vSortedT_ToF, vUnsortedT_ToF, vSortedQ;
        /**/
        /**/    // Prompt-peak fit variables (extractable from both APFit and BONSAI)
        /**/    float               pvx, pvy, pvz, towall;
        /**/    float               evis;
        /**/
        /**/    // APFit variables
        /**/    int                 nring, nmue, ndcy;
        /**/    std::vector<int>    vApip;
        /**/    std::vector<float>  vApamom, vApmome, vApmomm;
        /**/
        /**/        // Variables for neutron capture candidates
        /**/        int                 nCandidates, N200M;
        /**/        float               T200M, firsthit;
        /**/        std::vector<int>    vTindex, vN10n, vN1300;
        /**/        std::vector<float>  vTrms, vTrms50;
        /**/        std::vector<float>  vDtn, vNvx, vNvy, vNvz;
        /**/
        /**/        // BONSAI variables
        /**/        std::vector<float>  vBvx, vBvy, vBvz, vBvt;
        /**/
        /**/        // Beta variables
        /**/        std::vector<float>  vBeta14_10, vBeta14_50;
        /**/
        /**/    // TMVA output
        /**/    std::vector<float>  vTMVAoutput;
        /**/
        /************************************************************************************************/

        /************************************************************************************************/
        // MC truth event info
        /************************************************************************************************/
        /**/
        /**/    // Variables for true neutron capture
        /**/    int                 nTrueCaptures;
        /**/    std::vector<int>    vNGam, vCandidateID;
        /**/    std::vector<float>  vCaptureTime, vCapPosx, vCapPosy, vCapPosz, vTotGamE;
        /**/
        /**/    // Variables for neutron capture candidates
        /**/    std::vector<int>    vIsGdCapture, vIsTrueCapture, vDoubleCount;
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

#endif