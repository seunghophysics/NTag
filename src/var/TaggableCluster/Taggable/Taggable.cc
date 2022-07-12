#include <iostream>
#include <iomanip>

#include "Taggable.hh"

Taggable::Taggable()
: fVertex(), fT(0), fE(0), fEarlyIndex(-1), fDelayedIndex(-1), fTaggedType(typeMissed),
  fParentVertex(), fParentT(0), fParentE(0), fParentIntID(0), fParentIndex(-1) {}

Taggable::Taggable(TaggableType tType, float time, float energy, TVector3 vertex)
: fVertex(vertex), fT(time), fE(energy), fEarlyIndex(-1), fDelayedIndex(-1), fType(tType), fTaggedType(typeMissed),
  fParentVertex(), fParentT(0), fParentE(0), fParentIntID(0), fParentIndex(-1) {}

void Taggable::Dump()
{
    std::cout << "Vertex: " << std::setprecision(2) << fVertex.x() << " ," << fVertex.y() << " ," << fVertex.z()
              << " Time: " << fT << " ns" << " Energy: "  << fE << " MeV\n";
}

void Taggable::SetParent(const Particle& parent, int id)
{
    fParentT = parent.Time();
    fParentE = parent.Energy();
    fParentIntID = parent.IntID();
    fParentIndex = id;
}

void Taggable::SetCandidateIndex(const std::string& key, int id)
{
    if (key=="Early") fEarlyIndex = id;
    else fDelayedIndex = id;
}
int Taggable::GetCandidateIndex(const std::string& key) const
{
    if (key=="Early") return fEarlyIndex;
    else return fDelayedIndex;
}