#ifndef PMTHITCLUSTER_HH
#define PMTHITCLUSTER_HH

#include <functional>

#include "PMTHit.hh"
#include "Cluster.hh"

typedef struct OpeningAngleStats
{
    float mean, median, stdev, skewness;
} OpeningAngleStats;

class PMTHitCluster : public Cluster<PMTHit>
{
    public:
        PMTHitCluster();

        void SetVertex(const TVector3& inVertex);
        inline const TVector3& GetVertex() const { return vertex; }
        bool HasVertex() { return bHasVertex; }
        void RemoveVertex();

        void Sort();

        void DumpAllElements() { for (auto& hit: vElements) hit.Dump(); }

        PMTHit operator[] (int iHit) const { return vElements[iHit]; }

        PMTHitCluster Slice(int startIndex, float tWidth);
        PMTHitCluster Slice(int startIndex, float minusT, float plusT);

        template<typename T>
        float Find(std::function<T(const PMTHit&)> projFunc,
                   std::function<T(const std::vector<T>&)> calcFunc)
        {
            return calcFunc(GetProjection(projFunc));
        }

        template<typename T>
        std::vector<T> GetProjection(std::function<T(const PMTHit&)> lambda)
        {
            std::vector<T> output;
            for_each(vElements.begin(), vElements.end(), [&](PMTHit hit){ output.push_back(lambda(hit)); });

            return output;
        }

        std::vector<float> T();

        template<typename T>
        std::vector<T> operator[](std::function<T(const PMTHit&)> lambda) { return GetProjection(lambda); }

        PMTHit GetLastHit() { return vElements.back(); }

        std::array<float, 6> GetBetaArray();
        OpeningAngleStats GetOpeningAngleStats();

    private:
        bool bSorted;
        bool bHasVertex;
        TVector3 vertex;

        void SetToF(bool unset=false);
};

typedef PMTHitCluster EventPMTHits;

#endif