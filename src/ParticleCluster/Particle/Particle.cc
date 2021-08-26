#include "Particle.hh"

Particle::Particle(int id, float time, TVector3 vtx, TVector3 mom, int parPID, unsigned int interactionID)
: pid(id), t(time), v(vtx), p(mom), parentPID(parPID), intID(interactionID) {}

TString Particle::GetName() const
{
    return GetParticleName(pid);
}

TString Particle::GetIntName() const
{
    return GetInteractionName(intID);
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