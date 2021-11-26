#ifndef NTAGTMVATAGGER_HH
#define NTAGTMVATAGGER_HH

#include "CandidateTagger.hh"
#include "NTagTMVAManager.hh"

class NTagTMVATagger : public CandidateTagger
{
    public:
        NTagTMVATagger();
        ~NTagTMVATagger();
        
        void Initialize() { fTMVAManager.InitializeReader(); }
        
        void SetECut(float t0th, float n50cut, float tcut) 
        { 
            T0TH = t0th; E_N50CUT = n50cut; E_TIMECUT = tcut; fDoTagE = true; 
        }
        void SetNCut(float outcut) { N_OUTCUT = outcut; }
        
        float GetLikelihood(const Candidate& candidate) { return fTMVAManager.GetTMVAOutput(candidate); }
        int Classify(const Candidate& candidate);
        
    private:
        NTagTMVAManager fTMVAManager;
        
        bool fDoTagE;
        float T0TH, E_N50CUT, E_TIMECUT, N_OUTCUT;
};

#endif