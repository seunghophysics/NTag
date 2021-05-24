#ifndef CANDIDATE_HH
#define CANDIDATE_HH

#include <map>
#include <string>

#include "Rtypes.h"

class Candidate
{
    public:
        Candidate(const unsigned int iHit=0);

        inline unsigned int HitID() const { return hitID; }
        
        float& operator[](const std::string& key) { return featureMap[key];}
        void Set(const std::string& key, float value) { featureMap[key] = value; }
        float& Get(const std::string& key) { return featureMap[key];}
        void Clear() { featureMap.clear(); }

        std::map<std::string, float>& GetFeatureMap() { return featureMap; }

    private:
        std::map<std::string, float> featureMap;
        unsigned int hitID;
        
    ClassDef(Candidate, 1);
};

#endif