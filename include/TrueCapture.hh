#ifndef TRUECAPTURE_HH
#define TRUECAPTURE_HH

#include "Cluster.hh"
#include "Particle.hh"

class TrueCapture : public Cluster<Particle>
{
    public:
        TrueCapture();
        void Append(const Particle& particle);

        inline float Time() const { return captureTime; }
        inline float Energy() const { return gammaEnergy; }
        inline TVector3 Vertex() const { return vertex; }

        void Dump();

    private:
        TVector3 vertex;
        float captureTime;
        float gammaEnergy;
};

#endif