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
        Candidate(const unsigned int iHit=0): fHitID(iHit) {}

        inline unsigned int HitID() const { return fHitID; }
        inline void SetHitID(unsigned int id) { fHitID = id; }

        const float operator[](const std::string& key) const { return fFeatureMap.at(key); }
        void Set(const std::string& key, float value) { fFeatureMap[key] = value; }
        const float Get(const std::string& key, float value=0) const { if (fFeatureMap.count(key)) return fFeatureMap.at(key); else return value; }
        void Clear() { fFeatureMap.clear(); }

        void Dump() const { for (auto const& pair: fFeatureMap) { std::cout << pair.first << ": " << pair.second; } }

        const std::map<std::string, float>& GetFeatureMap() const { return fFeatureMap; }

    private:
        std::map<std::string, float> fFeatureMap;
        unsigned int fHitID;

    //ClassDef(Candidate, 1);
};

#endif