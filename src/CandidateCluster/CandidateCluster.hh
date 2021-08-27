#ifndef CANDIDATECLUSTER_HH
#define CANDIDATECLUSTER_HH

#include <memory>

#include <skparmC.h>
#include <sktqC.h>

#include "Candidate.hh"
#include "Cluster.hh"
#include "PMTHitCluster.hh"

class CandidateCluster : public Cluster<Candidate>
{
    public:
        CandidateCluster();
        ~CandidateCluster();
        
        void DumpAllElements();
        void Clear() { Cluster::Clear(); for (auto& pair: featureVectorMap) pair.second->clear(); }
        void FillVectorMap();
        void RegisterFeatureNames(const std::vector<std::string>& keyList)
        { 
            for (auto const& key: keyList)
                RegisterFeatureName(key);
        };
        void RegisterFeatureName(const std::string& key) 
        { 
            if (!featureVectorMap.count(key))
                featureVectorMap[key] = new std::vector<float>; 
        }

        std::map<std::string, std::vector<float>*> featureVectorMap;
};

#endif