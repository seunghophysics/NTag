#ifndef CANDIDATE_HH
#define CANDIDATE_HH

#include "Store.hh"
#include "PMTHitCluster.hh"

class Candidate : public Store
{
    public:
        Candidate(const unsigned int iHit);

        inline unsigned int HitID() const { return hitID; }

        template<typename T>
        void CopyFeatures(std::map<std::string, T> &map)
        {
            for (auto& pair: storeMap) {
                map[pair.first] = T();
            }
        }

        std::map<std::string, float> GetFeatureMap();

    private:
        unsigned int hitID;
};

#endif