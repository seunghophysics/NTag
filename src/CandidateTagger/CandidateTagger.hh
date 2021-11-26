#ifndef CANDIDATETAGGER_HH
#define CANDIDATETAGGER_HH

#include "Candidate.hh"
#include "Printer.hh"

class CandidateTagger
{
    public:
        CandidateTagger(const char* fitterName, Verbosity verbose=pDEFAULT):
        fMsg(fitterName, verbose) {}
        ~CandidateTagger() {}
        
        void SetVerbosity(Verbosity verbose) { fMsg.SetVerbosity(verbose); }

        virtual void Apply(const char* inFilePath, const char* outFilePath);

        virtual float GetLikelihood(const Candidate& candidate) { return 0; }
        virtual int Classify(const Candidate& candidate) { return 0; }
        
    private:
        std::string fName;
        Printer fMsg;
};

#endif