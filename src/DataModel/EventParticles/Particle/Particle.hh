#ifndef PARTICLE_HH
#define PARTICLE_HH

#include "TVector3.h"

class Particle
{
    public:
        Particle(int pid, float time, TVector3 vtx, TVector3 mom, unsigned int intID=0);

        inline int PID() const { return pdgCode; }
        inline float Time() const { return creationTime; }
        inline float DecayTime() const { return decayTime; }
        inline TVector3 Vertex() const { return vertex; }
        inline TVector3 Momentum() const { return momentum; }
        inline unsigned int IntID() const { return creationIntID; }

    private:
        float creationTime, decayTime;
        int pdgCode;
        unsigned int creationIntID, decayIntID;
        TVector3 vertex, momentum;
};

#endif