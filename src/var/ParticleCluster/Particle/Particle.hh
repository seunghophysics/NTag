#ifndef PARTICLE_HH
#define PARTICLE_HH

#include <map>
#include <cmath>

#include "TVector3.h"

#include "ParticleTable.hh"

float GetMass(int code);
TString GetParticleName(int code);
TString GetInteractionName(int code);
TString GetNEUTModeName(int code);

class Particle
{
    public:
        Particle(): fT(0), fPID(0), fParentPID(0), fIntID(0), fVertex(), fMomentum() {}
        Particle(int id, float time, TVector3 vtx, TVector3 mom, int parPID=0, unsigned int interactionID=0);

        inline int PID() const { return fPID; }
        inline float Time() const { return fT; }
        inline TVector3 Vertex() const { return fVertex; }
        inline TVector3 Momentum() const { return fMomentum; }
        inline float Energy() const { float m = GetMass(fPID); return std::sqrt(fMomentum.Mag2() + m*m) - m; }
        inline unsigned int IntID() const { return fIntID; }
        inline int ParentPID() const { return fParentPID; }

        inline bool operator==(const Particle& particle) const {
            return (fPID == particle.PID()) & (fVertex == particle.Vertex()) & (fMomentum == particle.Momentum());
        };

        inline bool operator!=(const Particle& particle) const { return !(*this == particle); }

        inline void AddT0(float t0) { fT += t0; }

        TString GetName() const;
        TString GetIntName() const;

    private:
        float fT;
        int fPID, fParentPID;
        unsigned int fIntID;
        TVector3 fVertex, fMomentum;

    //ClassDef(Particle, 1)
};

#endif