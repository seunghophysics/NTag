#ifndef PARTICLE_HH
#define PARTICLE_HH

#include <map>

#include "TVector3.h"

#include "ParticleTable.hh"

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
        
        inline void AddT0(float t0) { t += t0; }

        TString GetName() const;
        TString GetIntName() const;
        
    private:
        float t;
        int pid, parentPID;
        unsigned int intID;
        TVector3 v, p;
        
    //ClassDef(Particle, 1)
};

TString GetParticleName(int code);
TString GetInteractionName(int code);

#endif