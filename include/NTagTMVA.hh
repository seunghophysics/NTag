#ifndef NTAGTMVA_HH
#define NTAGTMVA_HH 1

#include <map>
#include <vector>
#include <string>

#include <TCut.h>
#include <TFile.h>

#include <NTagMessage.hh>
#include <NTagTMVAVariables.hh>

namespace TMVA
{
    class Reader;
    class Factory;
}

typedef std::pair<float, float> Range;
typedef std::map<const char*, Range> RangeMap;

class NTagTMVA
{
    public:
        NTagTMVA(unsigned int verbose=pDEFAULT);
        NTagTMVA(const char* inFileName, const char* outFileName, unsigned int verbose=pDEFAULT);
        ~NTagTMVA();
        
        inline void SetInOutFileNames(const char* inFileName, const char* outFileName)
                                     { fInFileName = inFileName; fOutFileName = outFileName; }
        inline void SetReader(TString methodName, TString weightFileName)
                             { fReaderMethodName = methodName + " method"; fReaderWeightFileName = weightFileName; }
        
        void SetMethods();
        void UseMethod(const char* methodName) { fUse[methodName] = 1; }
        
        void SetSigCut(TString sc) { fSigCut = sc; }
        void SetBkgCut(TString bc) { fBkgCut = bc; }
        void AddSigCut(TString sc) { fSigCut += " && " + sc; }
        void AddBkgCut(TString bc) { fBkgCut += " && " + bc; }
        
        void Train();
        
        void  DumpReaderCutRange();
        void  SetReaderCutRange(const char* key, float low, float high) { fRangeMap[key] = Range(low, high); }
        bool  IsInRange(const char* key);
        bool  CandidateCut();
        float GetOutputFromCandidate(int iCandidate);
        
        void GetReaderOutputFromEntry(long iEntry);
        void ApplyWeight(TString methodName, TString weightFileName);
        
    private:
        NTagTMVAVariables fVariables;
        
        NTagMessage  msg;
        unsigned int fVerbosity;
    
        const char* fInFileName;
        const char* fOutFileName;
    
        std::map<std::string, int> fUse;
        TMVA::Factory* fFactory;
        
        TMVA::Reader*  fReader;
        TString        fReaderMethodName;
        TString        fReaderWeightFileName;
        
        RangeMap fRangeMap;
        TCut fSigCut, fBkgCut;
        
    friend class NTagTMVAVariables;
};

#endif