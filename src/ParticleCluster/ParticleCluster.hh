#ifndef PARTICLECLUSTER_HH
#define PARTICLECLUSTER_HH

#include "Particle.hh"
#include "Cluster.hh"

class ParticleCluster : public Cluster<Particle> 
{
    public:
        ParticleCluster() {}
        
        void Sort();
        void DumpAllElements();
    
    //ClassDef(ParticleCluster, 1)
};

#endif