#ifndef PARTICLE_HH
#define PARTICLE_HH

#include "TVector3.h"

class Particle
{
    public:
        Particle(): pid(0), parentPID(0), t(0), intID(0), v(), p() {}
        Particle(int id, float time, TVector3 vtx, TVector3 mom, int parPID=0, unsigned int interactionID=0);

        inline int PID() const { return pid; }
        inline float Time() const { return t; }
        inline TVector3 Vertex() const { return v; }
        inline TVector3 Momentum() const { return p; }
        inline unsigned int IntID() const { return intID; }
        inline int ParentPID() const { return parentPID; }

    private:
        float t;
        int pid, parentPID;
        unsigned int intID;
        TVector3 v, p;
        
    ClassDef(Particle, 1)
};

#endif