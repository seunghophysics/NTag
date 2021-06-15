#ifndef EVENTCANDIDATES_HH
#define EVENTCANDIDATES_HH

#include <memory>

#include "Candidate.hh"
#include "Cluster.hh"

class EventCandidates : public Cluster<Candidate>
{
    public:
        ~EventCandidates();
        
        void Print();
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