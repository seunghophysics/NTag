#ifndef NTAGTMVATAGGER_HH
#define NTAGTMVATAGGER_HH

#include "CandidateTagger.hh"
#include "NTagTMVAManager.hh"

class NTagTMVATagger : public CandidateTagger
{
    public:
        NTagTMVATagger(Verbosity verbose=pDEFAULT);
        ~NTagTMVATagger();

        void Initialize(std::string weightPath="");
        
        void SetECut(float tmin, float n50cut, float tcut) 
        { 
            TMIN = tmin; E_N50CUT = n50cut; E_TIMECUT = tcut; fDoTagE = true; 
        }
        void SetNCut(float outcut) { N_OUTCUT = outcut; }
        
        float GetLikelihood(const Candidate& candidate) { return fTMVAManager.GetTMVAOutput(candidate); }
        int Classify(const Candidate& candidate);
        
    private:
        NTagTMVAManager fTMVAManager;
        std::string fWeightFilePath;
        
        bool fDoTagE;
        float TMIN, E_N50CUT, E_TIMECUT, N_OUTCUT;
};

#endif