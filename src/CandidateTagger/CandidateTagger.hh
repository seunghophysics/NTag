#ifndef CANDIDATETAGGER_HH
#define CANDIDATETAGGER_HH

#include "Candidate.hh"

class CandidateTagger
{
    public:
        CandidateTagger() {}
        ~CandidateTagger() {}

        virtual float GetLikelihood(const Candidate& candidate) { return 0; }
        virtual int Classify(const Candidate& candidate) { return 0; }
};

#endif