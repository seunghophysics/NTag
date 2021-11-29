#ifndef NTAGTMVAMANAGER_HH
#define NTAGTMVAMANAGER_HH

#include <map>
#include <string>

namespace TMVA
{
    class Reader;
    class Factory;
}

#include "CandidateTagger.hh"
#include "Printer.hh"

class NTagTMVAManager
{
    public:
        NTagTMVAManager();
        ~NTagTMVAManager();
        
        void InitializeReader();
        void SetMethods(bool turnOn);
    
        float GetTMVAOutput(const Candidate& candidate);

        void TrainWeights(const char* inFileName, const char* outFileName);
        //void ApplyWeights(const char* inFileName, const char* outFileName);

    private:
        TMVA::Factory* fFactory;
        TMVA::Reader*  fReader;
        
        std::map<std::string, float> fFeatureContainer;
        int fCandidateLabel;
    
        std::map<std::string, bool> fUse;
        
        Printer fMsg;
};

#endif