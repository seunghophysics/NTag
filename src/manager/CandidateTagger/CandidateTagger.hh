#ifndef CANDIDATETAGGER_HH
#define CANDIDATETAGGER_HH

#include "TCut.h"

#include "TreeOut.hh"
#include "Candidate.hh"
#include "Printer.hh"

class NTagTMVAManager;

class CandidateTagger : public TreeOut
{
    public:
        CandidateTagger(std::string fitterName="", Verbosity verbose=pDEFAULT);
        ~CandidateTagger();
        
        void SetVerbosity(Verbosity verbose) { fMsg.SetVerbosity(verbose); }
        void SetTMATCHWINDOW(float t) { TMATCHWINDOW = t; }
        void SetECuts(std::string cuts="0");
        void SetNCuts(std::string cuts="0");

        virtual void Apply(std::string inFilePath, std::string outFilePath, NTagTMVAManager* tmvaManager=0);
        virtual void OverrideSettings(std::string outFilePath);

        virtual int Classify(const Candidate& candidate);
        
    protected:
        float TMATCHWINDOW;

    private:
        std::string fECuts;
        std::string fNCuts;
        TTreeFormula* fECutFormula;
        TTreeFormula* fNCutFormula; 
        std::map<std::string, float> fFeatureMap;

        std::string fName;
        Printer fMsg;
};

#endif