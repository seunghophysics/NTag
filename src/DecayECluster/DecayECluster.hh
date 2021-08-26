#ifndef DECAYECLUSTER_HH
#define DECAYECLUSTER_HH

#include "Cluster.hh"
#include "Taggable.hh"
#include "ParticleCluster.hh"

class DecayECluster : public Cluster<Taggable>
{
    public:
        DecayECluster();
        DecayECluster(const ParticleCluster& particleCluster);
        
        void Sort();
        void DumpAllElements();
};

#endif