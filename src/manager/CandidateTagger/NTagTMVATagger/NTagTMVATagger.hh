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
        
        void OverrideSettings(const char* outFilePath);
        
        void SetECut(float n50cut, float tcut) 
        { 
            E_NHITSCUT = n50cut; E_TIMECUT = tcut; fDoTagE = true; 
        }
        void SetNCut(float outcut) { TAGOUTCUT = outcut; }
        
        float GetLikelihood(const Candidate& candidate) { return fTMVAManager.GetTMVAOutput(candidate); }
        int Classify(const Candidate& candidate);
        
    private:
        NTagTMVAManager fTMVAManager;
        std::string fWeightFilePath;
        
        bool fDoTagE;
        float E_NHITSCUT, E_TIMECUT, TAGOUTCUT;
};

#endif