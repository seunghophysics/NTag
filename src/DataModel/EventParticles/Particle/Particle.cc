#include "Particle.hh"

Particle::Particle(int id, float time, TVector3 vtx, TVector3 mom, unsigned int interactionID)
: pid(id), t(time), v(vtx), p(mom), intID(interactionID) {}