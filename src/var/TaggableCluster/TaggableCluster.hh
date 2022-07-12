#ifndef TAGGABLECLUSTER_HH
#define TAGGABLECLUSTER_HH

#include "Cluster.hh"

#include "Taggable.hh"
#include "ParticleCluster.hh"

class TaggableCluster : public Cluster<Taggable>, public TreeOut
{
    public:
        TaggableCluster();
        TaggableCluster(const ParticleCluster& particleCluster);

        void ReadParticleCluster(const ParticleCluster& particleCluster);
        void AddParticle(TaggableType taggableType, const Particle& particle, const ParticleCluster& particleCluster);

        void Sort();
        void DumpAllElements() const;

        void SetPromptVertex(TVector3 v) { fPromptVertex = v; }

        void MakeBranches();
        void FillTree();

    private:
        TVector3 fPromptVertex;

        std::vector<int> fTypeVector;
        std::vector<int> fTaggedTypeVector;
        std::vector<float> fTimeVector;
        std::vector<float> fEnergyVector;
        std::vector<float> fXVector;
        std::vector<float> fYVector;
        std::vector<float> fZVector;
        std::vector<float> fDistVector;
        std::vector<float> fDWallVector;
        std::vector<int> fEarlyIndexVector;
        std::vector<int> fDelayedIndexVector;
        std::vector<int> fParentIndexVector;
        std::vector<int> fParentIntIDVector;
        std::vector<float> fParentXVector;
        std::vector<float> fParentYVector;
        std::vector<float> fParentZVector;
        std::vector<float> fParentEnergyVector;
        std::vector<float> fParentTimeVector;
};

#endif