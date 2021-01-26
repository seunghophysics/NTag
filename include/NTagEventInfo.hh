/*******************************************
*
* @file NTagEventInfo.hh
*
* @brief Defines NTagEventInfo.
*
********************************************/

#ifndef NTAGEVENTINFO_HH
#define NTAGEVENTINFO_HH 1

#include <array>
#include <vector>
#include <ctime>
#include <cmath>

#include <TString.h>
#include <TMVA/Reader.h>

#undef MAXPM
#undef MAXPMA

#include <skparmC.h>
#include <geopmtC.h>

#include "NTagMessage.hh"
#include "NTagTMVA.hh"
#include "NTagTMVAVariables.hh"
#include "NTagCandidate.hh"

/******************************************
*
* @brief Vertex mode for NTagEventInfo
*
* Each option sets the type of prompt
* (neutron creation) vertex to be used
* in capture candidate search.
* This affects the behavior of the member
* function NTagEventInfo::SetPromptVertex.
*
* The ToF to each hit PMT is based on
* the prompt vertex specified by this mode.
*
* Vertex mode can be set using
* NTagEventInfo::SetVertexMode.
*
* @see NTagEventInfo::SetPromptVertex
* @see NTagEventInfo::SetVertexMode
*
*******************************************/
enum VertexMode
{
    mAPFIT,  ///< Use APFit vertex. Default in class NTagZBS. Generally applies to all ATMPD data with APFit applied.
    mBONSAI, ///< Use BONSAI vertex. Default in class NTagROOT. Generally applies to all LOWE data with BONSAI applied.
    mCUSTOM, ///< Use custom prompt vertex. This mode is automatically set if NTagEventInfo::SetCustomVertex is called.
             ///< Use this option for Am/Be calibration data if you're not using BONSAI.
    mTRUE,   ///< Use true prompt vertex.
             ///< Only applicable if the input is an MC file with the \c skvect common filled.
             ///< This feature fully supports all files generated using \b SKDETSIM.
    mSTMU    ///< Use the vertex where a stopping muon has stopped inside the tank. Not supported yet.
};

/******************************************
* @brief Constants used in NTag.
*******************************************/
namespace NTagConstant{
    constexpr float (*PMTXYZ)[3] = geopmt_.xyzpm; /*!< An array of PMT coordinates.
                                                       Index 0 for x, 1 for y, 2 for z-coordinates. [cm] */
    constexpr float C_WATER = 21.5833; ///< The speed of light in pure water. [cm/ns]
}

/******************************************
* @brief Default parameters used in NTag.
*******************************************/
namespace NTagDefault{
    constexpr float TWIDTH       = 14.;   ///< Default value for NTagEventInfo::TWIDTH. (ns)
    constexpr int   NHITSTH      = 7;     ///< Default value for NTagEventInfo::NHITSTH.
    constexpr int   NHITSMX      = 70;    ///< Default value for NTagEventInfo::NHITSMX.
    constexpr int   N200MX       = 200;   ///< Default value for NTagEventInfo::N200MX.
    constexpr float T0TH         = 5.;    ///< Default value for NTagEventInfo::T0TH. (us)
    constexpr float T0MX         = 535.;  ///< Default value for NTagEventInfo::T0MX. (us)
    constexpr float VTXSRCRANGE  = 5000.; ///< Default value for NTagEventInfo::VTXSRCRANGE. (cm)
    constexpr float MINGRIDWIDTH = 20;    ///< Default value for NTagEventInfo::MINGRIDWIDTH. (cm)
    constexpr float TMATCHWINDOW = 50.;   ///< Default value for NTagEventInfo::TMATCHWINDOW. (ns)
    constexpr float TMINPEAKSEP  = 60.;   ///< Default value for NTagEventInfo::TMINPEAKSEP. (ns)
    constexpr int   ODHITMX      = 16;    ///< Default value for NTagEventInfo::ODHITMX.
    constexpr float TRBNWIDTH    = 0.;    ///< Default value for NTagEventInfo::TRBNWIDTH. (us)
    constexpr float PVXRES       = 7.;    ///< Default value for NTagEventInfo::PVXRES. (cm)
}

/**********************************************************
 * @brief The container of raw TQ hit information,
 * event variables, and manipulating function library.
 *
 * @details NTagEventInfo has two purposes:
 * - Containing event variables such as
 * #qismsk and candidate capture variables such as #vNHitsn
 * - Providing a set of manipulating functions that are
 * used in event handling
 *
 * Most of the functions have appropriate names that
 * are self-explanatory, so please refer to each member
 * function to get ideas on what this class has to offer.
 *
 * NTagEventInfo::SearchCaptureCandidates is the core
 * utility provided by this class as the capture candidates
 * are sought right in this function. If the search
 * algorithm for candidates has to change by any reason,
 * NTagEventInfo::SearchCaptureCandidates should be the
 * first place to have a look. When a peak in the residual
 * PMT hit times (or ToF-subtracted hit times) satisfying
 * primary selection cut (NHits &ge NHITSTH) is found, the
 * hit information within 10 ns are fed into a NTagCandidate
 * class via NTagEventInfo::SavePeakFromHit, and all feature
 * variables of that candidate is calcuated within the class
 * NTagCandidate.
 *
 * Since this class is merely a container of member
 * event variables with a bunch of manipulating functions,
 * this class by itself cannot read any file
 * nor process events without help of its subclass NTagIO.
 * NTagIO, as a subclass of NTagEventInfo, uses the
 * inherited member functions to process each event.
 * Refer to NTagIO::ReadFile,
 * which uses the member functions provided by
 * NTagEventInfo in a specified order in
 * NTagIO::ReadMCEvent for MC events and
 * NTagIO::ReadDataEvent, NTagIO::ReadSHEEvent,
 * and NTagIO::ReadAFTEvent for data events, for the
 * event-wise instructions applied to the input file.
 ********************************************************/
class NTagEventInfo
{
    public:
        /**
         * @brief Constructor of class NTagEventInfo.
         * @details Default search settings for capture candidates, i.e., the range of NHits and T0,
         * are set in this constructor. You can always change the settings using the setter functions
         * provided with this class, but only if you use them before NTagIO::ReadFile is called.
         * Cuts in calculating TMVA classifier output can also be set using NTagTMVA::SetReaderCutRange method.
         * @param verbose The verbosity of NTagEventInfo and all of its daughter classes.
         */
        NTagEventInfo(Verbosity verbose);
        /**
         * @brief Destructor of class NTagEventInfo.
         */
        virtual ~NTagEventInfo();



        //////////////////////////////////////
        // Functions to set event variables //
        //////////////////////////////////////


            /***********/
            /* Data/MC */
            /***********/

            /**
             * @brief Saves basic event information to member variables.
             * @details Saved variables: #runNo, #subrunNo, #eventNo, #qismsk, #nhitac, #trgOffset
             */
            virtual void SetEventHeader();

            /**
             * @brief Saves prompt vertex (neutron creation vertex).
             * @details Saved variables: #pvx, #pvy, #pvz
             */
            virtual void SetPromptVertex();

            /**
             * @brief Saves variables generated by APFit.
             * @details Saved variables: #evis, #apNRings, #vAPRingPID,
             * #vAPMom, #vAPMomE, #vAPMomMu, #apNMuE, #apNDecays
             */
            virtual void SetAPFitInfo();

            /**
             * @brief Saves prompt-related variables generated by BONSAI.
             * @details Saved variables: #evis
             */
            virtual void SetLowFitInfo();
            
            /**
             * @brief Saves #tDiff, the time passed since the previous trigger.
             */
            virtual void SetTDiff();

            /**
             * @brief Extracts TQ hit arrays from input file and append it to the raw hit vectors.
             * @details Saved variables: #vTISKZ, #vQISKZ, #vCABIZ
             */
            virtual void AppendRawHitInfo();

            /**
             * @brief Subtracts ToF from each raw hit time in #vTISKZ and sort.
             * @details Saved variables: #vUnsortedT_ToF, #vSortedT_ToF, #vSortedPMTID, #vSortedQ
             */
            virtual void SetToFSubtractedTQ();

            /**
             * @brief Dump event variables and statistics.
             */
            virtual void DumpEventVariables();


            /***********/
            /* MC-only */
            /***********/

            /**
             * @brief Saves true variables extractable only from MC.
             * @details Saved variables: #nVec, #vecx, #vecy, #vecz, #vVecPID, #vVecPX, #vVecPY, #vVecPZ, #vVecMom,
             * #neutIntMode, #nVecInNeut, #nNInNeutVec, #neutIntMom,
             * #nSavedSec, #nTrueCaptures, #vSecPID, #vSecIntID, #vParentPID,
             * #vSecVX, #vSecVY, #vSecVZ, #vSecDWall, #vSecPX, #vSecPY, #vSecPZ, #vSecMom, #vSecT, #vCapID
             * #vTrueCT, #vCapVX, #vCapVY, #vCapVZ, #vNGamma, #vTotGammaE, #vCapID
             */
            virtual void SetMCInfo();

            /**
             * @brief Reads secondary bank from input file and fills \c secndprt common. Called inside
             * NTagEventInfo::SetMCInfo.
             * @see: NTagROOT::ReadSecondaries
             */
            virtual void ReadSecondaries();



        ///////////////////////////////////////////////
        // Functions for primary candidate selection //
        ///////////////////////////////////////////////

        /**
         * @brief The main search function for candidate selection before applying neural network.
         * @details NTagEventInfo::SavePeakFromHit is called to save peaks that match
         * the primary selection conditions as neutron capture candidates.
         * See the source code for the details.
         * @see: <a href="https://kmcvs.icrr.u-tokyo.ac.jp/svn/rep/skdoc/atmpd/publish/neutron2013/technote/
         * tn_neutron2.pdf">Tristan's ntag technote</a> for the description of Neut-fit.
         */
        virtual void SearchCaptureCandidates();

        /**
         * @brief Saves the peak from the input index of the sorted ToF-subtracted hit-time vector #vSortedT_ToF.
         * @param hitID The index of the first hit of the peak to save.
         * @details Saved variables: #vFirstHitID, #vBeta14_10, #nCandidates
         */
        virtual void SavePeakFromHit(int hitID);

        /**
         * @brief Function for setting candidate variables.
         * @details Extract candidate variables from the candidate vector #vCandidates.
         * @see NTagEventInfo::ExtractCandidateVariables
         */
        virtual void SetCandidateVariables();

        /**
         * @brief Initialize STL maps #iCandidateVarMap and #fCandidateVarMap with feature variable names
         * declared in NTagCandidate::SetVariables.
         */
        void InitializeCandidateVariableVectors();
        /**
         * @brief Extract variable values from each candidates in #vCandidates and save those in
         * #iCandidateVarMap and #fCandidateVarMap.
         */
        void ExtractCandidateVariables();
        /**
         * @brief Dump all saved candidates' hit information and feature variables.
         */
        void DumpCandidateVariables();


        //////////////////////////////////
        // Functions for hit processing //
        //////////////////////////////////


        /**
         * @brief Gets ToF from a vertex specified by \p vertex to a PMT with cable ID \p pmtID.
         * @param vertex A size-3 \c float array of x, y, and z-coordinates of some point in the SK coordinate system.
         * @param pmtID Cable ID of a PMT.
         * @return The time-of-flight (ToF) of a photon from the given vertex to the specified PMT.
         */
        float GetToF(float vertex[3], int pmtID);

        /**
         * @brief Gets the ToF-subtracted version of an input hit-time vector \p T.
         * @param T A vector of PMT hit times. [ns]
         * @param PMTID A vector of PMT cable IDs corresponding to each hit in \p T.
         * @param vertex A size-3 array of vertex coordinates to calculate ToF from.
         * @param doSort If \c true, the returned vector is sorted in ascending order.
         * @return The ToF-subtracted version of an input hit-time vector \p T. If \p doSort is set to \c true,
         * the returned vector is sorted in ascending order.
         * @note The input hit-time vector must not have ToF subtracted as ToF will be subtracted inside this function.
         */
        std::vector<float> GetToFSubtracted(const std::vector<float>& T, const std::vector<int>& PMTID,
                                              float vertex[3], bool doSort=false);

        /**
         * @brief Sort ToF-subtracted hit vector #vUnsortedT_ToF.
         * @details Saved variables: #vSortedT_ToF, #vSortedQ, #vSortedPMTID.
         */
        void SortToFSubtractedTQ();



        /////////////////////////////
        // Member variable control //
        /////////////////////////////


        /**
         * @brief Clears all member vectors and sets all member variables to a default value (which is mostly 0).
         */
        virtual void Clear();

        /**
         * @brief Saves the secondary of the given index. Called inside NTagEventInfo::SetMCInfo.
         * @param secID The index of the secondary particle saved in the \c secndprt common block.
         * @details Saved variables: #vSecPID, #vSecIntID, #vParentPID, #vSecVX, #vSecVY, #vSecVZ,
         * #vSecDWall, #vSecPX, #vSecPY, #vSecPZ, #vSecMom, #vSecT, #vCapID, #nSavedSec
         */
        virtual void SaveSecondary(int secID);

        /**
         * @brief Checks if the raw hit vector #vTISKZ is empty.
         * @details This function is used as a flag for an SHE event.
         * If this function is \c false, the previous event must have been an SHE event.
         * @return \c true if #vTISKZ is empty, otherwise \c false.
         */
        bool IsRawHitVectorEmpty() { return vTISKZ.empty(); }



        ////////////////////////
        // Set tag conditions //
        ////////////////////////

        /**
         * @brief Set width of NHits.
         * @param w Time width of TWIDTH. [ns]
         */
        inline void SetNHitsWidth(float w) { TWIDTH = w; }

        /**
         * @brief Set limits #NHITSTH and #NHITSMX for NHits.
         * @param low Lower limit for NHits.
         * @param high Upper limit for NHits.
         */
        inline void SetNHitsLimits(int low, int high=NTagDefault::NHITSMX) { NHITSTH = low; NHITSMX = high; }

        /**
         * @brief Set upper limit N200MX for N200.
         * @param max Upper limit for N200.
         */
        inline void SetN200Max(int max) { N200MX = max; }

        /**
         * @brief Set limits #T0TH and #T0MX for T0.
         * @param low Lower limit for T0. [ns]
         * @param high Upper limit for T0. [ns]
         * @note Both #T0TH and #T0MX should be in the form of global recorded hit time.
         * Please take into account that tthe rigger offset is ~1,000 ns in this format.
         */
        inline void SetT0Limits(float low, float high=NTagDefault::T0MX) { T0TH = low; T0MX = high; }

        /**
         * @brief Set vertex search range #VTXSRCRANGE in NTagEventInfo::MinimizeTRMS.
         * Use this function to cut T0 of the capture candidates.
         * @param cut Vertex search range to be used in NTagEventInfo::MinimizeTRMS
         */
        inline void SetDistanceCut(float cut) { VTXSRCRANGE = cut; }

        /**
         * @brief Set vertex search range #MINGRIDWIDTH in NTagEventInfo::MinimizeTRMS.
         * @param w Vertex search grid width [cm] to be used in NTagCandidate::MinimizeTRMS.
         */
        inline void SetMinGridWidth(float w) { MINGRIDWIDTH = w; }

        /**
         * @brief Set the width #TMATCHWINDOW of the time window used in true-to-reconstructed capture mapping.
         * @param t Width of the time window for capture mapping. [ns]
         * @see NTagCandidate::SetTrueInfo
         */
        inline void SetTMatchWindow(float t) { TMATCHWINDOW = t; }

        /**
         * @brief Set the minimum peak separation #TMINPEAKSEP.
         * @param t Width of minimum peak separation. [ns]
         * @see NTagEventInfo::SearchCaptureCandidates
         */
        inline void SetTPeakSeparation(float t) { TMINPEAKSEP = t; }

        /**
         * @brief Set the upper limit #ODHITMX for the number of OD hits.
         * @param q Upper limit for the number of OD hits.
         * @note The parameter #ODHITMX is not used at the moment.
         */
        inline void SetMaxODHitThreshold(float q) { ODHITMX = q; }

        /**
         * @brief Set the dead time width #TRBNWIDTH for RBN reduction.
         * @param t The dead time width for RBN reduction.
         * @see <a href="http://www-sk.icrr.u-tokyo.ac.jp/sk/_pdf/articles/2019/mochizuki-main.pdf">
         * Mochizuki-san's Master's thesis</a> for the details of RBN (i.e., repetitive burst noise).
         */
        inline void SetTRBNWidth(float t) { TRBNWIDTH = t; }

        /**
         * @brief Sets #VertexMode #fVertexMode.
         * @param m Vertex mode to use in NTagEventInfo.
         * @see #VertexMode
         */
        inline void SetVertexMode(VertexMode m) { fVertexMode = m; }

        /**
         * @brief Sets NTagEventInfo::PVXRES.
         * @param s Prompt vertex resolution. [cm]
         */
        inline void SetVertexResolution(float s) { PVXRES = s; }

        /**
         * @brief Chooses whether to use TMVA or not. Sets #bUseTMVA.
         * @param b If \c true, TMVA is used to produce classifier output. (#vTMVAOutput)
         */
        inline void UseTMVA(bool b) { bUseTMVA = b; }

        /**
         * @brief Sets custom vertex to start searching for candidates. Sets #customvx, #customvy, #customvz.
         * @param x X coordinate in SK coordinate system [cm]
         * @param y Y coordinate in SK coordinate system [cm]
         * @param z Z coordinate in SK coordinate system [cm]
         * @see #VertexMode
         */
        inline void SetCustomVertex(float x, float y, float z)
                                   { customvx = x; customvy = y; customvz = z; fVertexMode = mCUSTOM; }

        /**
         * @brief Choose whether to save residual TQ vectors (#vSortedT_ToF, #vSortedQ, #vSortedPMTID) or not. Sets #bSaveTQ.
         * @param b If \c true, #NTagIO::restqTree is written to the output file filled with residual TQ vectors.
         */
        inline void SetSaveTQFlagAs(bool b) { bSaveTQ = b; }

        /**
         * @brief Set \c true to force MC mode for data files. Useful for dummy data files with no trigger separation.
         * @param b If \c true, NTagIO::ReadMCEvent will be called instead of NTagIO::ReadDataEvent.
         * @see NTagIO::ReadEvent()
         */
        inline void ForceMCMode(bool b) { bForceMC = b; };

        /**
         * @brief Set \c false to not subtract ToF from each PMT hit times. Raw hit times will replace #vUnsortedT_ToF.
         * @param b If \c false, neutron candidates will be searched for from raw hit times rather than residual.
         */
        inline void UseResidual(bool b) { bUseResidual = b; }

        /**
         * @brief Set \c false to not go through Neut-fit for all candidates, which sometimes takes ages to complete.
         * @param b If \c false, NTag will not use Neut-fit and therefore no related variables will be saved.
         * @details Especially useful when finding primary selection efficiency.
         * @see NTagCandidate::SetVariables
         */
        inline void UseNeutFit(bool b) { bUseNeutFit = b; }

        // TMVA tools
        /// All input variables to TMVA are controlled by this class!
        NTagTMVA    TMVATools;

    private:

        // Tag conditions
        float       TWIDTH;       ///< Width of NHits. (By default it's 10 ns.) @see NTagEventInfo::SetNHitsWidth
        int         NHITSTH,      ///< Lower limit for NHits. @see NTagEventInfo::SetNHitsLimits
                    NHITSMX,      ///< Upper limit for NHits. @see NTagEventInfo::SetNHitsLimits
                    N200MX;       ///< Upper limit for N200. @see NTagEventInfo::SetN200Max
        float       T0TH,         ///< Lower limit for T0. @see: NTagEventInfo::SetT0Limits
                    T0MX;         ///< Upper limit for T0. @see: NTagEventInfo::SetT0Limits
        float       TRBNWIDTH;    ///< The width of the dead time to apply to all PMTs to reduce RBN.
        float       TMATCHWINDOW; ///< Width of the true-reconstructed capture time matching window. [ns]
                                  ///< @see: NTagEventInfo::SetTMatchWindow
        float       TMINPEAKSEP;  ///< Minimum candidate peak separation. [ns] @see: NTagEventInfo::SetTPeakSeparation
        float       ODHITMX;      ///< Threshold on the number of OD hits. Not used at the moment.
        float       VTXSRCRANGE;  ///< Vertex search range in NTagCandidate::MinimizeTRMS. @see NTagCandidate::SetDistanceCut
        float       MINGRIDWIDTH;   ///< Vertex search grid width in NTagCandidate::MinimizeTRMS.
        float       PVXRES;       ///< Prompt vertex resolution. (&Gamma of Breit-Wigner distribution) [cm]

        // Prompt-vertex-related
        float       customvx,     ///< X coordinate of a custom prompt vertex
                    customvy,     ///< Y coordinate of a custom prompt vertex
                    customvz;     ///< Z coordinate of a custom prompt vertex
        VertexMode  fVertexMode;  ///< #VertexMode of class NTagInfo and all inheriting classes.


    protected:
        NTagMessage msg;        ///< NTag Message printer.
        Verbosity   fVerbosity; ///< Verbosity.

        /** # of processed events */
        int nProcessedEvents;

        /** Raw trigger time (`skhead_.nt48sk`) */
        int preRawTrigTime[3];

        // Signal TQ source
        TFile* fSigTQFile;
        TTree* fSigTQTree;

        // Raw TQ hit vectors
        std::vector<int>    vCABIZ; ///< A vector of PMT cable IDs of all recorded hits from an event.
                                    ///< Forms a triplet with #vTISKZ and #vQISKZ.
        std::vector<float>  vTISKZ, ///< A vector of hit times [ns] of all recorded hits from an event.
                                    ///< Forms a triplet with #vCABIZ and #vQISKZ.
                            vQISKZ; ///< A vector of deposited charge [p.e.] of all recorded hits from an event.
                                    ///< Forms a triplet with #vCABIZ and #vTISKZ.
        std::vector<int>    vISIGZ; ///< A vector of signal flags (0: bkg, 1: sig) of all recorded hits from an event.
                                    ///< If #fSigTQFile is not \c NULL, it is saved in NTagEventInfo::AppendRawHitInfo.
        std::vector<float>* vSIGT;  ///< A vector to save signal hit times from #fSigTQTree temporarily. Not included in output.
        std::vector<int>*   vSIGI;  ///< A vector to save signal hit PMT IDs from #fSigTQTree temporarily. Not included in output.

        std::array<float, MAXPM+1> vPMTHitTime; ///< An array to save hit times for each PMT. Used for RBN reduction.

        // Processed TQ hit vectors
        std::vector<int>    vSortedPMTID;   ///< A vector of PMT cable IDs corresponding to each hit
                                            ///< sorted by ToF-subtracted hit time in ascending order.
                                            ///< Forms a triplet with #vSortedT_ToF and #vSortedQ.
        std::vector<float>  vSortedT_ToF,   ///< A vector of ToF-subtracted hit times [ns] sorted in ascending order.
                                            ///< Forms a triplet with #vSortedPMTID and #vSortedQ.
                            vUnsortedT_ToF, /*!< A vector of unsorted ToF-subtracted hit times [ns],
                                                 in the order as in #vTISKZ. */
                            vSortedQ;       ///< A vector of deposited charge [p.e.] corresponding to each hit
                                            ///< sorted by ToF-subtracted hit time in ascending order.
                                            ///< Forms a triplet with #vSortedT_ToF and #vSortedPMTID.
        std::vector<int>    vSortedSigFlag; ///< A vector of signal flags (0: bkg, 1: sig) corresponding to each hit
                                            ///< in #vSortedT_ToF.
        std::vector<int> reverseIndex;      ///< Inverse map from indices of vSortedT_ToF to indices of vTISKZ.

        // event processing options
        bool        bData,          /*!< Set \c true for data events, \c false for MC events.
                                         Automatically determined by the run number at NTagIO::CheckMC. */
                    bUseTMVA,       /*!< Set \c true if using TMVA, otherwise \c false.
                                         Can be set to \c false from command line with option `-noMVA`. */
                    bSaveTQ,        /*!< Set \c true if saving the ToF-subtracted TQ vectors, otherwise \c false.
                                         Can be set to \c true from command line with option `-saveTQ`. */
                    bForceMC,       /*!< Set \c true if forcing MC mode, otherwise \c false.
                                         Can be set to \c true from command line with option `-forceMC`. */
                    bUseResidual,   /*!< Set \c false if not using ToF-subtracted hit times, otherwise \c false.
                                         Can be set to \c false from command line with option `-noTOF`. */
                    bUseNeutFit;    /*!< Set \c false if not using Neut-fit and MVA, otherwise \c false.
                                         Can be set to \c false from command line with option `-noFit`. */
        bool candidateVariablesInitialized; /*!< A flag to check if #iCandidateVarMap and #fCandidateVarMap
                                                 are initialized. */



        /************************************************************************************************/
        /* Event information variables                                                                  */
        /************************************************************************************************/


        /////////////////////////
        // Data/fit event info //
        /////////////////////////

        // SK data variables
        int    runNo,     ///< Run # of an event.
               subrunNo,  ///< Subrun # of an event.
               eventNo,   ///< Event # of an event.
               nhitac,    ///< Number of OD hits within 1.3 us around the main trigger of an event.
               nqiskz,    ///< Number of all hits recorded in #vTISKZ.
               trgType;   ///< Trigger type. MC: 0, SHE: 1, SHE+AFT: 2, No-SHE: 3
        float  trgOffset, ///< Trigger offset of an event. Default set to 1,000 [ns].
               tDiff,     ///< Time difference from the current event to the previous event. [ms]
               qismsk;    /*!< Total p.e. deposited in ID within 1.3 us around
                               the main trigger of an event. */

        // Prompt vertex variables
        float  pvx,       ///< X position of a prompt-peak vertex (estimated neutron creation point).
               pvy,       ///< Y position of a prompt-peak vertex (estimated neutron creation point).
               pvz,       ///< Z position of a prompt-peak vertex (estimated neutron creation point).
               dWall,     ///< Distance from the prompt vertex to the tank wall.
               evis;      ///< Visible energy (the amount of energy if the peak was from an electron).

        // APFit variables
        int                 apNRings,   ///< Number of rings found by APFit.
                            apNMuE,     ///< Number of mu-e decays found by APFit.
                            apNDecays;  ///< Number of decay electrons?
        std::vector<int>    vAPRingPID; ///< Vector of PID corresponding to each ring found by APFit. [Size: #apNRings]
        std::vector<float>  vAPMom,     /*!< Vector of momentum corresponding to each ring found by APFit.
                                             [Size: #apNRings] */
                            vAPMomE,    /*!< Vector of e-like momentum corresponding to each ring found by APFit.
                                             [Size: #apNRings] */
                            vAPMomMu;   /*!< Vector of mu-like momentum corresponding to each ring found by APFit.
                                             [Size: #apNRings] */

        // Variables for neutron capture candidates
        int                 nCandidates,      ///< Number of total found neutron capture candidates.
                            maxN200;          ///< Maximum N200 found in neutron capture candidates.
        float               maxN200Time,      ///< First hit time of the candidate with maximum N200
                            firstHitTime_ToF; /*!< The earliest hit time in an event, subtracted by the ToF
                                                   from the prompt vertex. */
        std::vector<int>    vFirstHitID;      ///< Vector of all indices of the earliest hit in each candidate.
                                              ///< The indices are based off #vSortedT_ToF.

        std::vector<std::vector<float>> *vHitRawTimes, ///< Vector of residual hit times. [Size: #nCandidates]
                                        *vHitResTimes; ///< Vector of residual hit times. [Size: #nCandidates]
        std::vector<std::vector<int>>   *vHitCableIDs, ///< Vector of hit cable IDs. [Size: #nCandidates]
                                        *vHitSigFlags; ///< Vector of signal flags. (0: bkg, 1: sig) [Size: #nCandidates]

        int nTotalHits;    ///< Number of total hits, including unrecorded hits.
        int nTotalSigHits; ///< Number of total signal hits, including unrecorded signal hits.
        int nFoundSigHits; ///< Number of registered signal hits.
        int nRemovedHits;  ///< Number of removed hits due to RBN reduction.



        /////////////////////////
        // MC truth event info //
        /////////////////////////

        // Variables for true neutron capture
        int                 nTrueCaptures; ///< Number of true neutron captures in MC.
        std::vector<int>    vNGamma,       /*!< Vector of the total numbers of emitted gammas in true capture.
                                                [Size: #nTrueCaptures] */
                            vCandidateID;  ///< Vector of matching capture candidate IDs. [Size: #nTrueCaptures]
        std::vector<float>  vTrueCT,       ///< Vector of true capture times. [ns] [Size: #nTrueCaptures]
                            vCapVX,        /*!< Vector of X coordinates of true capture vertices. [cm]
                                                [Size: #nTrueCaptures] */
                            vCapVY,        /*!< Vector of Y coordinates of true capture vertices. [cm]
                                                [Size: #nTrueCaptures] */
                            vCapVZ,        /*!< Vector of Z coordinates of true capture vertices. [cm]
                                                [Size: #nTrueCaptures] */
                            vTotGammaE;    ///< Vector of the total emitted gamma energies. [MeV] [Size: #nTrueCaptures]

        // Variables from secondaries
        int                 nSavedSec,  ///< Number of saved secondaries.
                            nAllSec;    ///< Number of all secondaries in the input file.
        std::vector<int>    vSecPID,    ///< Vector of saved secondary PIDs. [Size: #nSavedSec]
                            vSecIntID,  ///< Vector of saved secondary interaction IDs. [Size: #nSavedSec]
                            vParentPID, ///< Vector of saved parent PIDs. [Size: #nSavedSec]
                            vCapID;     /*!< Vector of true capture indices that generates the secondary.
                                            -1 is pushed back if the secondary is not from a neutron capture.
                                            [Size: #nSavedSec] */
        std::vector<float>  vSecVX,     ///< Vector of X coordinates of secondary vertices. [cm] [Size: #nSavedSec]
                            vSecVY,     ///< Vector of Y coordinates of secondary vertices. [cm] [Size: #nSavedSec]
                            vSecVZ,     ///< Vector of Z coordinates of secondary vertices. [cm] [Size: #nSavedSec]
                            vSecPX,     /*!< Vector of X-direction initial momenta of secondaries. [MeV/c]
                                             [Size: #nSavedSec] */
                            vSecPY,     /*!< Vector of Y-direction initial momenta of secondaries. [MeV/c]
                                             [Size: #nSavedSec] */
                            vSecPZ,     /*!< Vector of Z-direction initial momenta of secondaries. [MeV/c]
                                             [Size: #nSavedSec] */
                            vSecDWall,  /*!< Vector of distances from secondary vertices to the SK barrel. [cm]
                                             [Size: #nSavedSec] */
                            vSecMom,    ///< Vector of initial momenta of secondaries. [MeV/c] [Size: #nSavedSec]
                            vSecT;      ///< Vector of secondary creation times. [ns] [Size: #nSavedSec]

        // Variables for neutrino interaction
        int                 nNInNeutVec,  ///< Number of neutrons in NEUT vectors.
                            neutIntMode,  ///< NEUT interaction mode. @see: \c nemodsel.F of NEUT.
                            nVecInNeut;   ///< Number of NEUT vectors.
        float               neutIntMom;   ///< NEUT interaction momentum.
        std::vector<int>    vNeutVecPID;  ///< Vector of NEUT vector PIDs. [Size: #nVecInNeut]

        // Variables from primary stack
        int                 nVec;    ///< Number of primary vectors in MC.
        float               vecx,    ///< X coordinate of the primary vertex.
                            vecy,    ///< Y coordinate of the primary vertex.
                            vecz;    ///< Z coordinate of the primary vertex.
        std::vector<int>    vVecPID; ///< Vector of primary vector PIDs. [Size: #nVec]
        std::vector<float>  vVecPX,  ///< Vector of X-direction initial momenta of primary vectors. [MeV/c] [Size: #nVec]
                            vVecPY,  ///< Vector of Y-direction initial momenta of primary vectors. [MeV/c] [Size: #nVec]
                            vVecPZ,  ///< Vector of Z-direction initial momenta of primary vectors. [MeV/c] [Size: #nVec]
                            vVecMom; ///< Vector of initial momenta of primary vectors. [MeV/c] [Size: #nVec]


        //////////////////////////////////////
        // Candidates and feature variables //
        //////////////////////////////////////

        std::vector<NTagCandidate> vCandidates; ///< A container for saved capture candidates.

        IVecMap iCandidateVarMap; /*!< A map from feature variable name to vectors of
                                       integer feature variables of all saved candidates. */
        FVecMap fCandidateVarMap; /*!< A map from feature variable name to vectors of
                                       float feature variables of all saved candidates. */

        /************************************************************************************************/

    friend class NTagCandidate;
};

#endif
