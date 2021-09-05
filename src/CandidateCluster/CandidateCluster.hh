#ifndef CANDIDATECLUSTER_HH
#define CANDIDATECLUSTER_HH

#include <memory>

#include <skparmC.h>
#include <sktqC.h>

#include "Candidate.hh"
#include "Cluster.hh"
#include "PMTHitCluster.hh"

class TTree;

class CandidateCluster : public Cluster<Candidate>
{
    public:
        CandidateCluster();
        CandidateCluster(const char* className);
        ~CandidateCluster();
        
        void Sort();
        void DumpAllElements(std::vector<std::string> keys={}) const;
        void Clear() { Cluster::Clear(); for (auto& pair: fFeatureVectorMap) pair.second->clear(); }
        
        void FillVectorMap();
        const std::map<std::string, std::vector<float>*>& GetFeatureVectorMap() const { return fFeatureVectorMap; }
        void RegisterFeatureNames(const std::vector<std::string>& keyList)
        { 
            for (auto const& key: keyList)
                RegisterFeatureName(key);
        };
        void RegisterFeatureName(const std::string& key) 
        { 
            if (!fFeatureVectorMap.count(key))
                fFeatureVectorMap[key] = new std::vector<float>; 
        }

        void MakeBranches();
        
    private:
        std::map<std::string, std::vector<float>*> fFeatureVectorMap;
};

#endif