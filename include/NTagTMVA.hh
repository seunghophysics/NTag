#ifndef NTAGTMVA_HH
#define NTAGTMVA_HH 1

#include <map>
#include <vector>
#include <string>

#include <TCut.h>
#include <TFile.h>

#include "NTagMessage.hh"
#include "NTagTMVAVariables.hh"

namespace TMVA
{
    class Reader;
    class Factory;
}

class NTagEventInfo;

typedef std::pair<float, float>      Range;
typedef std::map<const char*, Range> RangeMap;

class NTagTMVA
{
    public:
        NTagTMVA(unsigned int verbose=pDEFAULT);
        NTagTMVA(const char* inFileName, const char* outFileName, unsigned int verbose=pDEFAULT);
        ~NTagTMVA();

        void Constructor();

        inline void SetInOutFileNames(const char* inFileName, const char* outFileName)
                                     { fInFileName = inFileName; fOutFileName = outFileName; }

        /********************/
        /* Making weight    */
        /********************/

        // Settings
        void SetMethods();
        void UseMethod(const char* methodName) { fUse[methodName] = 1; }

        // Cuts used in training
        void SetSigCut(TString sc) { fSigCut = sc; }
        void SetBkgCut(TString bc) { fBkgCut = bc; }
        void AddSigCut(TString sc) { fSigCut += " && " + sc; }
        void AddBkgCut(TString bc) { fBkgCut += " && " + bc; }

        void MakeWeights();

        /********************/
        /* Applying weight  */
        /********************/

        // Settings
        void SetReader(TString methodName, TString weightFileName);

        // Cuts used in TMVA output generation
        void  SetReaderCutRange(const char* key, float low, float high) { fRangeMap[key] = Range(low, high); }
        void  DumpReaderCutRange();
        bool  IsInRange(const char* key);
        bool  CandidateCut();

        // TMVA output from TMVAVariables in event
        float GetOutputFromCandidate(int iCandidate);

        void ApplyWeight(TString methodName, TString weightFileName);

    private:
        NTagTMVAVariables fVariables;
        NTagMessage       msg;

        unsigned int fVerbosity;

        const char* fInFileName;
        const char* fOutFileName;

        // Making weights
        std::map<std::string, int> fUse;
        TMVA::Factory*             fFactory;
        TCut                       fSigCut, fBkgCut;

        // Applying weight
        TMVA::Reader*  fReader;
        TString        fReaderMethodName;
        TString        fReaderWeightFileName;

        RangeMap fRangeMap;

    friend class NTagTMVAVariables;
    friend class NTagEventInfo;
    friend class NTagIO;
};

#endif