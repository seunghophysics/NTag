#ifndef EXTRACTFEATURES_HH
#define EXTRACTFEATURES_HH

#include "Tool.hh"

class ExtractFeatures : public Tool
{
    public:
        ExtractFeatures() { name = "ExtractFeatures"; }

        bool Initialize();
        bool Execute();
        bool Finalize();

        bool CheckSafety();

    private:
        bool inputIsMC;
        float tWidth;
        float tMatchWindow;
        float initGridWidth, minGridWidth, gridShrinkRate, vertexSearchRange;
};

#endif