#ifndef NTAGCANDIDATE_HH
#define NTAGCANDIDATE_HH 1

#include <array>
#include <vector>

#include "NTagMessage.hh"

class NTagEventInfo;

typedef std::map<const char*, int> IVarMap;
typedef std::map<const char*, float> FVarMap;

class NTagCandidate
{
    public:
        NTagCandidate(int id, NTagEventInfo* eventInfo);
        ~NTagCandidate();
        
        void SetHitInfo(const std::vector<float>& rawT,
                        const std::vector<float>& resT,
                        const std::vector<float>& Q,
                        const std::vector<int>& I,
                        const std::vector<int>& sigF);
        void SetVariables();
        void SetVariablesWithinTWindow(int tWindow);
        void SetTrueInfo();
        void SetNNVariables();
        void SetTMVAOutput();
        
        void DumpHitInfo();
        void DumpVariables();
        
        // Calculator
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
        NTagEventInfo* currentEvent;
        
        FVarMap fVarMap;
        IVarMap iVarMap;
    
        int candidateID;
        //
        //int   candidateID,
        //      firstHitID, ///< Vector of all indices of the earliest hit in each candidate.
        //                  ///< The indices are based off #vSortedT_ToF.
        //      captureType,
        //      trueCaptureID,
        //      n10,        /// Number of hits in 10 ns window.
        //      n50,        /// Number of hits in 50 ns window.
        //      n200,       /// Number of hits in 200 ns window.
        //      n10n,       ///< Vector of Neut-fit N10.  [Size: #nCandidates]
        //      n1300;      ///< Vector of N1300. [Size: #nCandidates]
        //float tRMS10,     ///< RMS of PMT hit times in 10 ns
        //      tRMS10n,    ///< Vector of Neut-fit TRMS in 10 ns window. [Size: #nCandidates]
        //      tRMS50,     ///< Vector of TRMS in 10 ns window. [Size: #nCandidates]
        //      tSpread10,  ///< Spread of PMT hit times (max-min) in 10 ns
        //      qSum10,     /// Sum of Q in 10 ns (p.e.)
        //      reconCT,    ///< Reconstructed capture time (ns)
        //      reconCTn,   ///< Vector of Neut-fit reconstructed capture time. [Size: #nCandidates]
        //      nvx,        /*!< Vector of X coordinates of Neut-fit capture vertex. */
        //      nvy,        /*!< Vector of Y coordinates of Neut-fit capture vertex. */
        //      nvz,        /*!< Vector of Z coordinates of Neut-fit capture vertex. */
        //      dWalln,
        //      bsvx,       /*!< Vector of X coordinates of BONSAI-fit capture vertex. */
        //      bsvy,       /*!< Vector of X coordinates of BONSAI-fit capture vertex. */
        //      bsvz,       /*!< Vector of X coordinates of BONSAI-fit capture vertex. */
        //      bsE,
        //      bsGood,
        //      bsDirks,
        //      bsPatlik,
        //      bsOvaq,
        //      bsReconCT,  ///< Vector of BONSAI-fit capture time. [ns] [Size: #nCandidates]
        //      beta14_10,  ///< Vector of &beta;_14's in 10 ns window. [Size: #nCandidates]
        //      beta14_50,  ///< Vector of &beta;_14's in 50 ns window. [Size: #nCandidates]
        //      prompt_bonsai,
        //      prompt_nfit,
        //      bonsai_nfit,
        //      tMVAOutput; ///< Vector of TMVA classifier outputs. [Size: #nCandidates]
              
        std::array<float, 6> beta_10;
        std::array<float, 6> beta_50;
                            
        std::vector<float> vHitRawTimes, ///< Vector of residual hit times. [Size: #nCandidates]
                           vHitResTimes, ///< Vector of residual hit times. [Size: #nCandidates]
                           vHitChargePE; ///< Vector of deposited charge in photoelectrons.
        std::vector<int>   vHitCableIDs, ///< Vector of hit cable IDs. [Size: #nCandidates]
                           vHitSigFlags; ///< Vector of signal flags. (0: bkg, 1: sig) [Size: #nCandidates]
                           
    friend class NTagEventInfo;
};

#endif