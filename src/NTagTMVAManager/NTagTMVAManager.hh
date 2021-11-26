#ifndef NTAGTMVAMANAGER_HH
#define NTAGTMVAMANAGER_HH

#include <map>
#include <string>

namespace TMVA
{
    class Reader;
    class Factory;
}

#include "Printer.hh"

class NTagTMVAManager
{
    public:
        NTagTMVAManager();
        ~NTagTMVAManager();
        
        void SetMethods(bool turnOn);
    
        void TrainWeights(const char* inFileName, const char* outFileName);
        void ApplyWeights();

    private:
        TMVA::Factory* fFactory;
        TMVA::Reader*  fReader;
    
        std::map<std::string, bool> fUse;
        
        Printer fMsg;
};

#endif