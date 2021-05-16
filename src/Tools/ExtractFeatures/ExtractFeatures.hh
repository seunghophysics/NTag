#ifndef EXTRACTFEATURES_HH
#define EXTRACTFEATURES_HH

#include "Tool.hh"

class ExtractFeatures : public Tool
{
    public:
        ExtractFeatures():
        inputIsMC(false), tWidth(14), tMatchWindow(50),
        initGridWidth(800), minGridWidth(50), gridShrinkRate(0.5), vertexSearchRange(5000)
        { name = "ExtractFeatures"; }

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