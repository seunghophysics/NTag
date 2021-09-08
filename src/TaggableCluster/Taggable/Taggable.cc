#include <iostream>
#include <iomanip>

#include "Taggable.hh"

Taggable::Taggable()
: t(0), E(0), v(), earlyIndex(-1), delayedIndex(-1), taggedType(typeMissed) {}

Taggable::Taggable(TaggableType tType, float time, float energy, TVector3 vertex)
: type(tType), t(time), E(energy), v(vertex), earlyIndex(-1), delayedIndex(-1), taggedType(typeMissed) {}

void Taggable::Dump()
{
    std::cout << "Vertex: " << std::setprecision(2) << v.x() << " ," << v.y() << " ," << v.z()
              << " Time: " << t << " ns" << " Energy: "  << E << " MeV\n";
}

void Taggable::SetCandidateIndex(const std::string& key, int id)
{ 
    if (key=="Early") earlyIndex = id; 
    else delayedIndex = id; 
}
int Taggable::GetCandidateIndex(const std::string& key) const 
{ 
    if (key=="Early") return earlyIndex;
    else return delayedIndex;
}