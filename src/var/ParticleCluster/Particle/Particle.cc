#include <iostream>

#include "Particle.hh"

Particle::Particle(int id, float time, TVector3 vtx, TVector3 mom, int parPID, unsigned int interactionID)
: fT(time), fPID(id), fParentPID(parPID), fIntID(interactionID), fVertex(vtx), fMomentum(mom) {}

TString Particle::GetName() const
{
    return GetParticleName(fPID);
}

TString Particle::GetIntName() const
{
    return GetInteractionName(fIntID);
}

float GetMass(int code)
{
    TParticlePDG* pdgInfo = gPDG->GetParticle(code);
    if (pdgInfo)
        return pdgInfo->Mass()*1e3; // GeV->MeV
    else if (gPIDMassMap.count(abs(code))) {
        return gPIDMassMap[abs(code)];
    }
    else {
        std::cerr << "Code " << code << " does not exist in PDG nor ParticleTable, returning 0...\n";
        return 0;
    }
}

TString GetParticleName(int code)
{
    if (gPIDMap.count(code))
        return gPIDMap[code];
    else
        return TString(std::to_string(code));
}

TString GetInteractionName(int code)
{
    if (gIntIDMap.count(code))
        return gIntIDMap[code];
    else
        return TString(std::to_string(code));
}

TString GetNEUTModeName(int code)
{
    if (gNEUTModeMap.count(code))
        return gNEUTModeMap[code];
    else
        return TString(std::to_string(code));
}