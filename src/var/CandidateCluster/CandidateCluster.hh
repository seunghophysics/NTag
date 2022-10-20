
/*******************************************
*
* @file CandidateCluster.hh
*
* @brief Defines CandidateCluster.
*
********************************************/

#ifndef CANDIDATECLUSTER_HH
#define CANDIDATECLUSTER_HH

#include <memory>

#include <skparmC.h>
#include <sktqC.h>

#include "Candidate.hh"
#include "Cluster.hh"
#include "PMTHitCluster.hh"

class TTree;

/*******************************************
*
* @brief The Cluster of Candidate objects.
*
* @details This class inherits from 
*
* Values can be set and retrieved using
* Candidate::Set and Candidate::Get functions.
*
* One can pass an unsigned integer index that
* may be useful in positioning the candidate
* among hits.
*
* @see EventNTagManager::FindFeatures for the
* use of Candidate::Set functions and how the
* passed hit index in the constructor could be
* used to position the candidate among PMT hits
* and fetch relevant features.
*
********************************************/

class CandidateCluster : public Cluster<Candidate>, public TreeOut
{
    public:
        CandidateCluster();
        CandidateCluster(const char* className);
        CandidateCluster(const CandidateCluster& cluster);
        ~CandidateCluster();

        CandidateCluster& operator=(CandidateCluster const& rhs);

        void Sort();
        void DumpAllElements(std::vector<std::string> keys={}, bool showTaggedOnly=false) const;
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
        int fNCandidates;
        std::map<std::string, std::vector<float>*> fFeatureVectorMap;
};

#endif