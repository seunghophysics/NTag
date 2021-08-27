#include "Candidate.hh"

Candidate::Candidate(const unsigned int iHit)
: hitID(iHit) {}

//std::map<std::string, float> Candidate::GetFeatureMap()
//{
//    std::map<std::string ,float> map;
//
//    for (auto const& pair: storeMap) {
//        map[pair.first] = std::stof(pair.second);
//    }
//
//    return map;
//}