#ifndef NTAGCUTTAGGER_HH
#define NTAGCUTTAGGER_HH

#include "CandidateTagger.hh"

class NTagCutTagger : public CandidateTagger
{
    public:
        NTagCutTagger(Verbosity verbose=pDEFAULT);
        ~NTagCutTagger();

        void OverrideSettings(const char* outFilePath);

        void SetCuts(float scintCut, float goodCut, float dirksCut, float distCut, float eCut)
        {
            SCINTCUT = scintCut; GOODNESSCUT = goodCut;
            DIRKSCUT = dirksCut; DISTCUT = distCut; ECUT = eCut;
        }

        int Classify(const Candidate& candidate);
        
    private:
        float SCINTCUT, GOODNESSCUT, DIRKSCUT, DISTCUT, ECUT;
};

#endif