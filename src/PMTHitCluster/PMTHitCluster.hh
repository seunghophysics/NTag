#ifndef PMTHITCLUSTER_HH
#define PMTHITCLUSTER_HH

#include <functional>

#include <skparmC.h>
#include <sktqC.h>

#include "PMTHit.hh"
#include "Cluster.hh"

class TTree;

typedef struct OpeningAngleStats {
    float mean, median, stdev, skewness;
} OpeningAngleStats;

class PMTHitCluster : public Cluster<PMTHit>
{
    public:
        PMTHitCluster();
        PMTHitCluster(sktqz_common sktqz);
        
        void Append(const PMTHit& hit);

        void SetVertex(const TVector3& inVertex);
        inline const TVector3& GetVertex() const { return vertex; }
        bool HasVertex() { return bHasVertex; }
        void RemoveVertex();

        void Sort();

        void DumpAllElements() { for (auto& hit: fElement) hit.Dump(); }

        PMTHit operator[] (int iHit) const { return fElement[iHit]; }

        PMTHitCluster Slice(int startIndex, float tWidth);
        PMTHitCluster Slice(int startIndex, float minusT, float plusT);
        
        void ApplyDeadtime(float deadtime);

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
            for_each(fElement.begin(), fElement.end(), [&](PMTHit hit){ output.push_back(lambda(hit)); });

            return output;
        }

        template<typename T>
        std::vector<T> operator[](std::function<T(const PMTHit&)> lambda) { return GetProjection(lambda); }

        PMTHit GetLastHit() { return fElement.back(); }

        std::array<float, 6> GetBetaArray();
        OpeningAngleStats GetOpeningAngleStats();
        TVector3 FindTRMSMinimizingVertex(float INITGRIDWIDTH=800, float MINGRIDWIDTH=50, float GRIDSHRINKRATE=0.5, float VTXSRCRANGE=5000);

    private:
        bool bSorted;
        bool bHasVertex;
        TVector3 vertex;

        void SetToF(bool unset=false);
};

#endif