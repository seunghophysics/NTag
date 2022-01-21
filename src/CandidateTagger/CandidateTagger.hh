#ifndef CANDIDATETAGGER_HH
#define CANDIDATETAGGER_HH

#include "Candidate.hh"
#include "Printer.hh"

class CandidateTagger
{
    public:
        CandidateTagger(const char* fitterName="", Verbosity verbose=pDEFAULT):
        TMATCHWINDOW(200), fMsg(fitterName, verbose) { fName = fitterName; }
        ~CandidateTagger() {}
        
        void SetVerbosity(Verbosity verbose) { fMsg.SetVerbosity(verbose); }

        virtual void Apply(const char* inFilePath, const char* outFilePath, float tMatchWindow=200);
        virtual void OverrideSettings(const char* outFilePath);

        virtual float GetLikelihood(const Candidate& candidate) { return 0; }
        virtual int Classify(const Candidate& candidate) { return 0; }
        
    protected:
        float TMATCHWINDOW;
    private:
        std::string fName;
        Printer fMsg;
};

#endif