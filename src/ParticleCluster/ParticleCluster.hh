#ifndef PARTICLECLUSTER_HH
#define PARTICLECLUSTER_HH

#include <skvectC.h>
#define SECMAXRNG 4000
#include <apscndryC.h>

#include "Particle.hh"
#include "Cluster.hh"

class ParticleCluster : public Cluster<Particle> 
{
    public:
        ParticleCluster() {}
        ParticleCluster(vcwork_common primary, secndprt_common secondary);
        
        void SetT0(float t0);
        
        void Sort();
        void DumpAllElements() const;
    
    //ClassDef(ParticleCluster, 1)
};

#endif