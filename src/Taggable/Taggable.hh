#ifndef TAGGABLE_HH
#define TAGGABLE_HH

#include "TVector3.h"

class Taggable
{
    public:
        Taggable();
        Taggable(float, float, TVector3);

        inline float Time() const { return t; }
        inline float Energy() const { return E; }
        inline TVector3 Vertex() const { return v; }

        void Dump();

    protected:
        TVector3 v;
        float t;
        float E;
        bool isTagged;
};

#endif