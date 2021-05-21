#ifndef APPLYTMVA_HH
#define APPLYTMVA_HH

#include <memory>

#include "TMVA/Reader.h"

#include "Tool.hh"

class ApplyTMVA : public Tool
{
    public:
        ApplyTMVA():likelihoodThreshold(0.7), tmvaReader("") { name = "ApplyTMVA"; }

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
        TMVA::Reader tmvaReader;
};

#endif