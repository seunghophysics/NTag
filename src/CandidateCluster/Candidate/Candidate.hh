#ifndef CANDIDATE_HH
#define CANDIDATE_HH

#include <map>
#include <string>
#include <iostream>

//#include "Rtypes.h"

enum TrueLabel
{
    lNoise,
    lDecayE,
    lnH,
    lnGd,
    lRemnant,
    lUndefined
};

class Candidate
{
    public:
        Candidate(const unsigned int iHit=0);

        inline unsigned int HitID() const { return hitID; }
        inline void SetHitID(unsigned int id) { hitID = id; }

        const float operator[](const std::string& key) const { return featureMap.at(key); }
        void Set(const std::string& key, float value) { featureMap[key] = value; }
        const float Get(const std::string& key, float value=0) const { if (featureMap.count(key)) return featureMap.at(key); else return value; }
        void Clear() { featureMap.clear(); }

        void Dump() const { for (auto const& pair: featureMap) { std::cout << pair.first << ": " << pair.second; } }

        const std::map<std::string, float>& GetFeatureMap() const { return featureMap; }

    private:
        std::map<std::string, float> featureMap;
        unsigned int hitID;

    //ClassDef(Candidate, 1);
};

#endif