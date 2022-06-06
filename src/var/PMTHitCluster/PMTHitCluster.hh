#ifndef PMTHITCLUSTER_HH
#define PMTHITCLUSTER_HH

#include <functional>
#include <algorithm>

#include <skparmC.h>
#include <sktqC.h>

#include "PMTHit.hh"
#include "Cluster.hh"

class TTree;
class TQReal;

typedef struct OpeningAngleStats {
    float mean, median, stdev, skewness;
} OpeningAngleStats;

class PMTHitCluster : public Cluster<PMTHit>, public TreeOut
{
    public:
        PMTHitCluster();
        PMTHitCluster(sktqz_common sktqz);
        PMTHitCluster(sktqaz_common sktqaz);
        PMTHitCluster(TQReal* tqreal, int flag=2/* default: in-gate */);

        void Append(const PMTHit& hit);
        void Append(const PMTHitCluster& hitCluster, bool inGateOnly=false);
        void Clear();
        void AddTQReal(TQReal* tqreal, int flag=2/* default: in-gate */);

        void SetVertex(const TVector3& inVertex);
        inline const TVector3& GetVertex() const { return fVertex; }
        bool HasVertex() { return fHasVertex; }
        void RemoveVertex();
        void FindMeanDirection();

        void Sort();

        void DumpAllElements() const { for (auto& hit: fElement) hit.Dump(); }

        void FillTQReal(TQReal* tqreal);
        void FillCommon();

        const PMTHit& operator[] (int iHit) const { return fElement[iHit]; }

        PMTHitCluster Slice(int startIndex, Float tWidth);
        PMTHitCluster Slice(int startIndex, Float minusT, Float plusT);
        PMTHitCluster SliceRange(Float startT, Float minusT, Float plusT);
        PMTHitCluster SliceRange(Float minusT, Float plusT);

        unsigned int GetIndex(PMTHit hit);
        unsigned int GetLowerBoundIndex(Float t) 
        { 
            return std::lower_bound(fElement.begin(), fElement.end(), PMTHit(t, 0, 1, 1)) - fElement.begin();
        }
        unsigned int GetUpperBoundIndex(Float t) 
        {
            auto index = std::upper_bound(fElement.begin(), fElement.end(), PMTHit(t, 0, 1, 1)) - fElement.begin();
            return index? --index : index;
        }

        inline const TVector3& GetMeanDirection() const { return fMeanDirection; }

        void AddTimeOffset(Float tOffset);
        void ApplyDeadtime(Float deadtime);

        template<typename T>
        float Find(std::function<T(const PMTHit&)> projFunc,
                   std::function<T(const std::vector<T>&)> calcFunc)
        {
            return calcFunc(GetProjection(projFunc));
        }

        template<typename T>
        std::vector<T> GetProjection(std::function<T(const PMTHit&)> lambda) const
        {
            std::vector<T> output;
            for_each(fElement.begin(), fElement.end(), [&](PMTHit hit){ output.push_back(lambda(hit)); });
            return output;
        }

        template<typename T>
        std::vector<T> operator[](std::function<T(const PMTHit&)> lambda) const { return GetProjection(lambda); }

        PMTHit GetLastHit() { return fElement.back(); }

        PMTHitCluster& operator+=(const Float& time);
        PMTHitCluster& operator-=(const Float& time);

        std::array<float, 6> GetBetaArray();
        OpeningAngleStats GetOpeningAngleStats();

        void SetAsSignal(bool b);
        float GetSignalRatio();
        
        //void FindHitProperties();
        PMTHitCluster Slice(std::function<float(const PMTHit&)> lambda, float min, float max) const;
        void ApplyCut(std::function<float(const PMTHit&)> lambda, float min, float max);

        void MakeBranches();
        void ClearBranches();
        void FillTree();
        
    private:
        bool fIsSorted, fHasVertex;
        TVector3 fVertex, fMeanDirection;

        std::vector<float> fT, fQ;
        std::vector<int> fI, fS;

        void SetToF(bool unset=false);
};

PMTHitCluster operator+(const PMTHitCluster& hitCluster, const Float& time);
PMTHitCluster operator-(const PMTHitCluster& hitCluster, const Float& time);

#endif