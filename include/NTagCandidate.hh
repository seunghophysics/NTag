/*******************************************
*
* @file NTagCandidate.hh
*
* @brief Defines NTagCandidate.
*
********************************************/

#ifndef NTAGCANDIDATE_HH
#define NTAGCANDIDATE_HH 1

#include <array>
#include <vector>

#include "NTagMessage.hh"
#include "NTagTMVAVariables.hh"

class NTagEventInfo;

/********************************************************
 * @brief The class representing a neutron capture
 * candidate.
 *
 * The vectors of TQ information that are
 * extracted in NTagEventInfo::SearchCaptureCandidates
 * are input to this class via NTagCandidate::SetHitInfo
 * and all relevant feature variables are calculated
 * within this class using its member functions.
 *
 * The hit information are saved in #vHitRawTimes,
 * #vHitResTimes, #vHitChargePE, #vHitCableIDs, and
 * #vHitSigFlags. Feature variables that are extracted
 * by NTagCandidate::SetVariables are saved in #iVarMap
 * if the variable is integer and #fVarMap if the
 * variable is float. These variable containers are of
 * type IVarMap and FVarMap, which are basically maps
 * from C++ STL.
 *
 * All variables initialized in these maps are
 * automatically pushed to
 * NTagEventInfo::iCandidateVarMap and
 * NTagEventInfo::fCandidateVarMap via
 * NTagEventInfo::SetCandidateVariables,
 * and eventually filled in the \c ntvar tree via
 * NTagIO::AddCandidateVariablesToNtvarTree. Since these
 * branches are not known until one event is fully
 * processed, they are created when the \c ntvar tree is
 * filled for the first time at NTagIO::FillTrees.
 *
 * Some of the feature variables with matching names in
 * NTagTMVAVariables::Clear will be pushed back to
 * NTagTMVAVariables and generate classifier output.
 *******************************************************/
class NTagCandidate
{
    public:

        /**
         * @brief Constructor of NTagCandidate.
         * @param id Candidate ID of the constructed candidate.
         * @param eventInfo A pointer to the concurrent NTagEventInfo.
         */
        NTagCandidate(int id, NTagEventInfo* eventInfo);

        /**
         * @brief Destructor of NTagCandidate.
         */
        ~NTagCandidate();


        //////////////////////////////////////////////
        // Setter functions for candidate variables //
        //////////////////////////////////////////////

        /**
         * @brief Copies input vectors to private member vectors:
         * #vHitResTimes, #vHitChargePE, #vHitCableIDs, and #vHitSigFlags.
         * @param rawT A vector of raw PMT hit times. [ns]
         * @param resT A vector of residual (ToF-subtracted) PMT hit times. [ns]
         * @param Q A vector of PMT deposit charge in p.e.
         * @param I A vector of hit PMT cable IDs.
         * @param sigF A vector of hit signal flags. (0: bkg, 1: sig)
         */
        void SetHitInfo(const std::vector<float>& rawT,
                        const std::vector<float>& resT,
                        const std::vector<float>& Q,
                        const std::vector<int>& I,
                        const std::vector<int>& sigF);

        /**
         * @brief Set feature variables in #iVarMap and #fVarMap.
         * @details Called inside NTagEventInfo::SavePeakFromHit which is called in
         * NTagEventInfo::SearchCaptureCandidates. Calls other setter functions,
         * i.e., NTagCandidate::SetVariablesWithinTWindow, NTagCandidate::SetTrueInfo,
         * NTagCandidate::SetNNVariables, NTagCandidate::SetTMVAOutput.
         */
        void SetVariables();

        /**
         * @brief Set feature variables within given time window \c tWindow.
         * @details The input parameter \c tWindow serves as a mode for setting variables.
         * Currently two modes (50 ns and 1300 ns) are supported, so that for 50 ns mode,
         * this function searches for TRMS-minimizing vertex (Neut-fit vertex) within &plusmn
         * 25 ns, and for 1300 ns mode, it feeds all hits within [-520.8 ns, +779.2 ns] with
         * the reconstructed capture time \a "ReconCT" being 0 into BONSAI to extracxt
         * BONSAI variables, i.e., \a "BSgood", \a "BSpatlik", \a "BSovaq", etc.
         * See the source code for details.
         * @param tWindow Time window modes in ns. Only 50 and 1300 are supported at the moment.
         * @see NTagCandidate::MinimizeTRMS for the Neut-fit algorithm.
         */
        void SetVariablesWithinTWindow(int tWindow);

        /**
         * @brief Searches for the relevant true capture in case the input file is MC.
         * @details This function looks for the relevant true capture by looking for a true capture
         * time (from #NTagEventInfo::vTruecT) that is close to the candidate's reconstructed capture time.
         * The width of the time window for a match, which is NTagEventInfo::TMATCHWINDOW,
         * is by default NTagDefault::TMATCHWINDOW and can be manually set by
         * NTagEventInfo::SetTMatchWindow.
         */
        void SetTrueInfo();

        /**
         * @brief Pushes back feature variables that are specified in NTagTMVAVariables to a neural network.
         */
        void SetNNVariables();

        /**
         * @brief Extracts TMVA classifier output by calling NTagTMVA::GetOutputFromCandidate.
         */
        void SetTMVAOutput();


        ///////////////////////
        // Printer functions //
        ///////////////////////

        /**
         * @brief Dump raw hit info, i.e., #vHitResTimes, #vHitChargePE, #vHitCableIDs, and #vHitSigFlags.
         */
        void DumpHitInfo();

        /**
         * @brief Dump all feature variables stored in #iVarMap and #fVarMap.
         */
        void DumpVariables();


        //////////////////////////
        // Calculator functions //
        //////////////////////////

        /**
         * @brief Evaluate &beta;_i values of a hit cluster for i = 1...5 and return those in an array.
         * @param PMTID A vector of PMT cable IDs. The locations of the PMTs are fetched from #PMTXYZ.
         * @param startIndex The starting index of a hit cluster.
         * @param nHits The number of total hits in a cluster.
         * @return An size-6 array of &beta; values. The i-th element of the returned array
         * is the i-th &beta; value. The 0-th element is a dummy filled with 0.
         * @see For the details of the &beta; values, see Eq. (5) of the SNO review article at
         * <a href="https://arxiv.org/pdf/1602.02469.pdf">arXiv:1602.02469</a>.
         */
        std::array<float, 6> GetBetaArray(const std::vector<int>& PMTID);
        /**
         * @brief Gets the minimum RMS value of hit-times by searching for the minizing vertex.
         * @param T A vector of PMT hit times. [ns]
         * @param PMTID A vector of PMT cable IDs corresponding to each hit in \p T.
         * @param fitVertex The array to have minimizing vertex coordinates filled.
         * @return The RMS value of the extracted hit cluster from the input hit-tme vector \p T.
         * \p fitVertex is also returned as the coordinates of the TRMS minimizing vertex of \p T.
         * @note The input hit-time vector must not have ToF subtracted as ToF will be subtracted inside this function.
         */
        float MinimizeTRMS(const std::vector<float>& T, const std::vector<int>& PMTID, float fitVertex[3]);

    private:
        Verbosity fVerbosity;
        NTagMessage msg;
        NTagEventInfo* currentEvent; ///< A pointer to the concurrent NTagEventInfo.

        FVarMap fVarMap; ///< A map of integer feature variables.
        IVarMap iVarMap; ///< A map of float feature variables.

        int candidateID; ///< Candidate ID of the candidate.

        std::array<float, 6> beta_10; ///< An array of beta variables calculated within 10 ns.
        std::array<float, 6> beta_50; ///< An array of beta variables calculated within 50 ns.

        std::vector<float> vHitRawTimes, ///< Vector of residual hit times. [Size: N10]
                           vHitResTimes, ///< Vector of residual hit times. [Size: N10]
                           vHitChargePE; ///< Vector of deposited charge in photoelectrons. [Size: N10]
        std::vector<int>   vHitCableIDs, ///< Vector of hit cable IDs. [Size: N10]
                           vHitSigFlags; ///< Vector of signal flags. (0: bkg, 1: sig) [Size: N10]

    friend class NTagEventInfo;
};

#endif