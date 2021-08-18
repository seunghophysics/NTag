#include "Particle.hh"

Particle::Particle(int id, float time, TVector3 vtx, TVector3 mom, int parPID, unsigned int interactionID)
: pid(id), t(time), v(vtx), p(mom), parentPID(parPID), intID(interactionID) {}

TString Particle::GetName()
{
    if (!gPIDMap.count(2112)) {
        gPIDMap[2112]   = "n";
        gPIDMap[2212]   = "p";
        gPIDMap[22]     = "gamma";
        gPIDMap[11]     = "e-";
        gPIDMap[-11]    = "e+";
        gPIDMap[100045] = "d";
    }

    if (gPIDMap.count(pid))
        return gPIDMap[pid];
    else
        return TString(std::to_string(pid));
}

TString Particle::GetInteractionName()
{
    if (!gIntIDMap.count(18)) {
        gIntIDMap[18] = "Capture";
        gIntIDMap[7]  = "Compt.";
        gIntIDMap[9]  = "Brems.";
        gIntIDMap[10] = "Delta";
        gIntIDMap[11] = "Annihi.";
        gIntIDMap[12] = "Hadr.";
    }

    if (gIntIDMap.count(intID))
        return gIntIDMap[intID];
    else
        return TString(std::to_string(intID));
}