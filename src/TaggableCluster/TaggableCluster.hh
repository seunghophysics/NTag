#ifndef TAGGABLECLUSTER_HH
#define TAGGABLECLUSTER_HH

#include "Cluster.hh"
#include "Taggable.hh"
#include "ParticleCluster.hh"

class TaggableCluster : public Cluster<Taggable>
{
    public:
        TaggableCluster();
        TaggableCluster(const ParticleCluster& particleCluster);
        
        void Sort();
        void DumpAllElements() const;
        
        void SetPromptVertex(TVector3 v) { fPromptVertex = v; }
        
    private:
        TVector3 fPromptVertex;
};

#endif