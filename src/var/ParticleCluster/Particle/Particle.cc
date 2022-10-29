#include <iostream>

#include "Particle.hh"

Particle::Particle(int id, float time, TVector3 vtx, TVector3 mom,
                   int parPID, unsigned int interactionID, TVector3 parVtx, TVector3 parMom)
: fT(time), fPID(id), fParentPID(parPID), fIntID(interactionID), fParentIndex(-1), fVertex(vtx), fMomentum(mom),
  fParentVertex(parVtx), fParentMomentum(parMom) {}

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
    else if (1e9<code && code<1e10) {
        // nucleus code; return approximate mass in MeV
        return (code/10) % 1000 * 931;
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