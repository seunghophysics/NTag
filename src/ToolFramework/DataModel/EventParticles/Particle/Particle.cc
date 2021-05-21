#include "Particle.hh"

Particle::Particle(int pid, float time, TVector3 vtx, TVector3 mom, unsigned int intID)
: pdgCode(pid), creationTime(time), vertex(vtx), momentum(mom), creationIntID(intID) {}