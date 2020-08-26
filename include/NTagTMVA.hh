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

class NTagEventInfo;

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
        void SetReader(TString methodName, TString weightFileName);
        
        void SetMethods();
        void UseMethod(const char* methodName) { fUse[methodName] = 1; }
        
        template <typename T>
        void  PushBack(const char* key, T value){ if (std::is_integral<T>::value) fVariables.iEventVectorMap[key]->push_back(value);
                                                  else fVariables.fEventVectorMap[key]->push_back(value); }
        
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
    friend class NTagEventInfo;
};

#endif