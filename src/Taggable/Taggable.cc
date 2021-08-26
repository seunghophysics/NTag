#include <iostream>
#include <iomanip>

#include "Taggable.hh"

Taggable::Taggable()
: t(0), E(0), v() {}

Taggable::Taggable(float time, float energy, TVector3 vertex)
: t(time), E(energy), v(vertex) {}

void Taggable::Dump()
{
    std::cout << "Vertex: " << std::setprecision(2) << v.x() << " ," << v.y() << " ," << v.z()
              << " Time: " << t << " ns" << " Energy: "  << E << " MeV\n";
}