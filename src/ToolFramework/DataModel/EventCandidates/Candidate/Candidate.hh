#ifndef CANDIDATE_HH
#define CANDIDATE_HH

#include <map>
#include <string>

class Candidate
{
    public:
        Candidate(const unsigned int iHit);

        inline unsigned int HitID() const { return hitID; }
        
        float& operator[](const std::string& key) { return featureMap[key];}
        void Set(const std::string& key, float value) { featureMap[key] = value; }
        float& Get(const std::string& key) { return featureMap[key];}

        std::map<std::string, float>& GetFeatureMap() { return featureMap; }

    private:
        std::map<std::string, float> featureMap;
        unsigned int hitID;
};

#endif