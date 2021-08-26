#ifndef NCAPTURE_HH
#define NCAPTURE_HH

#include "Taggable.hh"
#include "Particle.hh"

class NCapture : public Taggable
{
    public:
        NCapture();
        void AddGamma(const Particle& particle);

    private:
        int nGamma;
        
    //ClassDef(NCapture, 1)
};

#endif