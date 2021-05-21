#ifndef APPLYTMVA_HH
#define APPLYTMVA_HH

#include <memory>

#include "Tool.hh"

namespace TMVA
{
    class Reader;
}

class ApplyTMVA : public Tool
{
    public:
        ApplyTMVA():likelihoodThreshold(0.7) { name = "ApplyTMVA"; }

        bool Initialize();
        bool Execute();
        bool Finalize();

        bool CheckSafety();

        float GetClassifierOutput(Candidate* candidate);

    private:
        std::map<std::string, float> featureContainer;
        int captureType;
        float likelihoodThreshold;

        std::string mvaMethodName, weightFilePath;
        TMVA::Reader* tmvaReader;
};

#endif