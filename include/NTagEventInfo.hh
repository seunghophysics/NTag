#ifndef NTAGEVENTINFO_HH
#define NTAGEVENTINFO_HH 1

#include <array>
#include <vector>
#include <ctime>

#include <TString.h>
#include <TMVA/Reader.h>

#include "NTagMessage.hh"
#include "NTagTMVA.hh"

enum VertexMode {mAPFIT, mBONSAI, mCUSTOM, mTRUE, mMUON};

/********************************************************
 *
 * \brief Steering class of NTag.
 *
 * It looks for hit peaks from the raw TQ info and
 * searches for neutron capture candidates.
 * TMVA is used to give classifier output to each 
 * candidate.
 *
 ********************************************************/

class NTagEventInfo
{
    public:
        /** Constructor */
        NTagEventInfo(Verbosity verbose /**< verbosity */);
        /** Destructor */
        virtual ~NTagEventInfo();

        // Functions to set variables

            /* All types */
            virtual void         SetEventHeader();      /**< Sets event header */
            virtual void         SetPromptVertex();     /**< Sets prompt vertex */
            virtual void         SetAPFitInfo();        /**< Sets APFitInfo */
            virtual void         SetLowFitInfo();       /**< Sets LowFit Info */
            virtual void         AppendRawHitInfo();    /**< Appends raw hit info */
            virtual void         SetToFSubtractedTQ();  /**< Sets ToF subtracted TQ */

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
            int                  IsCapture(int candidateID, bool bSave=false);
            int                  IsGdCapture(int candidateID);
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
        inline void          SetVertexMode(VertexMode m) { fVertexMode = m; }
        inline void          UseTMVA(bool b) { useTMVA = b; }
        inline void          SetCustomVertex(float x, float y, float z)
                                            { customvx = x; customvy = y; customvz = z; fVertexMode = mCUSTOM; }
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
        float       TMATCHWINDOW;           // used in function IsCapture
        float       TMINPEAKSEP;            // minimum peak separation in time
        float       ODHITMX;                // OD total charge threshold

        // Custom prompt vertex
        float       customvx, customvy, customvz;

    protected:
        NTagMessage  msg;
        Verbosity fVerbosity;
        bool         bData, useTMVA;
        int          fVertexMode;

        /************************************************************************************************/
        // Data/fit event info
        /************************************************************************************************/
        /**/
        /**/    // SK data variables
        /**/    int                 runNo, subrunNo, eventNo, trgType, nhitac, nqiskz;
        /**/    float               trgOffset, qismsk;
        /**/    std::vector<int>    vCABIZ;
        /**/    std::vector<float>  vTISKZ, vQISKZ;
        /**/    std::vector<int>    vSortedPMTID;
        /**/    std::vector<float>  vSortedT_ToF, vUnsortedT_ToF, vSortedQ;
        /**/
        /**/    // Prompt-peak fit variables (extractable from both APFit and BONSAI)
        /**/    float               pvx, pvy, pvz, dWall;
        /**/    float               evis;
        /**/
        /**/    // APFit variables
        /**/    int                 apNRings, apNMuE, apNDecays;
        /**/    std::vector<int>    vAPRingPID;
        /**/    std::vector<float>  vAPMom, vAPMomE, vAPMomMu;
        /**/
        /**/        // Variables for neutron capture candidates
        /**/        int                 nCandidates, maxN200;
        /**/        float               maxT200, firstHitTime_ToF;
        /**/        std::vector<int>    vFirstHitID, vN10n, vN1300;
        /**/        std::vector<float>  vTRMS10n, vTRMS50;
        /**/        std::vector<float>  vReconCTn, vNvx, vNvy, vNvz;
        /**/
        /**/        // BONSAI variables
        /**/        std::vector<float>  vBSvx, vBSvy, vBSvz, vBSReconCT;
        /**/
        /**/        // Beta variables
        /**/        std::vector<float>  vBeta14_10, vBeta14_50;
        /**/
        /**/    // TMVA output
        /**/    std::vector<float>  vTMVAOutput;
        /**/
        /************************************************************************************************/

        /************************************************************************************************/
        // MC truth event info
        /************************************************************************************************/
        /**/
        /**/    // Variables for true neutron capture
        /**/    int                 nTrueCaptures;
        /**/    std::vector<int>    vNGamma, vCandidateID;
        /**/    std::vector<float>  vTrueCT, vCapVX, vCapVY, vCapVZ, vTotGammaE;
        /**/
        /**/    // Variables for neutron capture candidates
        /**/    std::vector<int>    vIsGdCapture, vIsCapture, vDoubleCount;
        /**/    std::vector<float>  vTrueCapVX, vTrueCapVY, vTruth_vz, vCTDiff;
        /**/
        /**/    // Variables from secondaries
        /**/    int                 nSavedSec, nAllSec;
        /**/    std::vector<int> 	vSecPID, vSecIntID, vParentPID, vCapID;
        /**/    std::vector<float> 	vSecVX, vSecVY, vSecVZ, vSecPX, vSecPY, vSecPZ;
        /**/    std::vector<float> 	vSecDWall, vSecMom, vSecT;
        /**/
        /**/    // Variables for neutrino interaction
        /**/    int    	            nNInNeutVec, neutIntMode, nVecInNeut;
        /**/    float  	            neutIntMom;
        /**/    std::vector<int>    vNeutVecPID;
        /**/
        /**/    // Variables from primary stack
        /**/    int                 nVec;
        /**/    float               vecx, vecy, vecz;
        /**/    std::vector<int>    vVecPID;
        /**/    std::vector<float>  vVecPX, vVecPY, vVecPZ, vVecMom;
        /**/
        /************************************************************************************************/
};

#endif