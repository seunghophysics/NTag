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

        std::map<std::string, std::vector<float>*> featureVectorMap;
};

#endif