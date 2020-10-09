#ifndef NTAGCANDIDATE_HH
#define NTAGCANDIDATE_HH 1

#include <vector>

class NTagEventInfo;

class NTagCandidate
{
    public:
        NTagCandidate();
        ~NTagCandidate();
        
        void SetHitInfo(const std::vector<float>& rawT,
                        const std::vector<float>& resT,
                        const std::vector<float>& Q,
                        const std::vector<int>& I,
                        const std::vector<int>& sigF);
                        
        void DumpHitInfo();
        
    private:
        int   firstHitID, ///< Vector of all indices of the earliest hit in each candidate.
                           ///< The indices are based off #vSortedT_ToF.
              n10n,       ///< Vector of Neut-fit N10.  [Size: #nCandidates]
              n1300;      ///< Vector of N1300. [Size: #nCandidates]
        float tRMS10n,    ///< Vector of Neut-fit TRMS in 10 ns window. [Size: #nCandidates]
              tRMS50,     ///< Vector of TRMS in 10 ns window. [Size: #nCandidates]
              reconCTn,   ///< Vector of Neut-fit reconstructed capture time. [Size: #nCandidates]
              nvx,        /*!< Vector of X coordinates of Neut-fit capture vertex.
                                [Size: #nCandidates] */
              nvy,        /*!< Vector of Y coordinates of Neut-fit capture vertex.
                                [Size: #nCandidates] */
              nvz,        /*!< Vector of Z coordinates of Neut-fit capture vertex.
                                [Size: #nCandidates] */
              bsvx,       /*!< Vector of X coordinates of BONSAI-fit capture vertex.
                                [Size: #nCandidates] */
              bsvy,       /*!< Vector of X coordinates of BONSAI-fit capture vertex.
                                [Size: #nCandidates] */
              bsvz,       /*!< Vector of X coordinates of BONSAI-fit capture vertex.
                                [Size: #nCandidates] */
              bsReconCT,  ///< Vector of BONSAI-fit capture time. [ns] [Size: #nCandidates]
              beta14_10,  ///< Vector of &beta;_14's in 10 ns window. [Size: #nCandidates]
              beta14_50,  ///< Vector of &beta;_14's in 50 ns window. [Size: #nCandidates]
              tMVAOutput; ///< Vector of TMVA classifier outputs. [Size: #nCandidates]
                            
        std::vector<float> vHitRawTimes, ///< Vector of residual hit times. [Size: #nCandidates]
                           vHitResTimes, ///< Vector of residual hit times. [Size: #nCandidates]
                           vHitChargePE; ///< Vector of deposited charge in photoelectrons.
        std::vector<int>   vHitCableIDs, ///< Vector of hit cable IDs. [Size: #nCandidates]
                           vHitSigFlags; ///< Vector of signal flags. (0: bkg, 1: sig) [Size: #nCandidates]
};

#endif