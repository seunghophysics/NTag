#ifndef PARTICLECLUSTER_HH
#define PARTICLECLUSTER_HH

#include <skvectC.h>
#include <apscndryC.h>
#undef SECMAXRNG

#include "Particle.hh"
#include "Cluster.hh"

class ParticleCluster : public Cluster<Particle>, public TreeOut
{
    public:
        ParticleCluster() {}
        ParticleCluster(vcwork_common primary, secndprt_common secondary);

        void ReadCommonBlock(vcwork_common primary, secndprt_common secondary);

        void SetT0(float t0);

        void Sort();
        void DumpAllElements() const;

        void MakeBranches();
        void FillTree();

    private:
        void FindParents();

        // pid
        std::vector<int> fPIDVector;
        std::vector<int> fParentPIDVector;
        std::vector<int> fParentIndexVector;
        std::vector<int> fInteractionIDVector;
        std::vector<float> fTimeVector;
        std::vector<float> fXVector;
        std::vector<float> fYVector;
        std::vector<float> fZVector;
        std::vector<float> fPXVector;
        std::vector<float> fPYVector;
        std::vector<float> fPZVector;
        std::vector<float> fKEVector;

        std::vector<float> fParentPXVector, fParentVXVector;

    //ClassDef(ParticleCluster, 1)
};

#endif