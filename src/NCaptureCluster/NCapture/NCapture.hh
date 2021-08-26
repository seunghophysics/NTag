#ifndef NCAPTURE_HH
#define NCAPTURE_HH

#include "Particle.hh"

class NCapture
{
    public:
        NCapture();
        void AddGamma(const Particle& particle);

        inline float Time() const { return t; }
        inline float Energy() const { return E; }
        inline TVector3 Vertex() const { return v; }

        void Dump();

    private:
        TVector3 v;
        float t;
        float E;
        int nGamma;
        
    //ClassDef(NCapture, 1)
};

#endif