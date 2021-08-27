#include <iomanip>
#include <cassert>
#include <cmath>

#include "CandidateCluster.hh"

CandidateCluster::CandidateCluster() {}

CandidateCluster::~CandidateCluster()
{
    for (auto& pair: featureVectorMap)
        delete pair.second;
}

void CandidateCluster::DumpAllElements()
{
    if (!GetSize()) {
        std::cerr << "No candidate to print!" << std::endl;
    }
    else {
        int xWidth = 10;
        std::cout << "\033[4m\n No. ";
        auto baseFeatureMap = fElement[0].GetFeatureMap();
        for (auto const& pair: baseFeatureMap) {
            int textWidth = pair.first.size()>6 ? pair.first.size() : 6;
            std::cout << std::right << std::setw(textWidth) << pair.first << " ";
        }
        std::cout << "\033[0m\n";

        for (int iCandidate = 0; iCandidate < GetSize(); iCandidate++) {
            std::cout << std::right << std::setw(4) << iCandidate+1 << " ";
            auto candidateFeatureMap = fElement[iCandidate].GetFeatureMap();
            for (auto const& pair: candidateFeatureMap) {
                int textWidth = pair.first.size()>6 ? pair.first.size() : 6;
                float value = pair.second;

                if (fabs(value) < 1 && value != 0) {
                    value = roundf(value*100)/100;
                    std::cout << std::fixed << std::setprecision(2) << std::setw(textWidth) << value << " ";
                }
                else {
                    int roundedValue = (int)(value+0.5f);
                    std::cout << std::right << std::setprecision(2) << std::setw(textWidth) << roundedValue << " ";
                }

            }
            std::cout << std::endl;
        }
    }
}


void CandidateCluster::FillVectorMap()
{
    if (!GetSize())
        std::cerr << "No elements in CandidateCluster... skipping CandidateCluster::FillVectorMap." << std::endl;

    else {
        //auto baseFeatureMap = element[0].GetFeatureMap();
        bool areFeaturesIdentical = true;
        for (int iCandidate = 0; iCandidate < GetSize(); iCandidate++) {
            auto comparedFeatureMap = fElement[iCandidate].GetFeatureMap();

            for (auto const& basePair: featureVectorMap) {

                if (!comparedFeatureMap.count(basePair.first)) {
                    std::cerr << "Registered key " << basePair.first << " not found in candidate!" << std::endl;
                    areFeaturesIdentical = false;
                }
                else {
                    featureVectorMap[basePair.first]->resize(iCandidate);
                    featureVectorMap[basePair.first]->push_back(comparedFeatureMap[basePair.first]);
                }
            }

            for (auto const& comparedPair: comparedFeatureMap) {
                if (!featureVectorMap.count(comparedPair.first)) {
                    std::cerr << "Candidate key " << comparedPair.first << " not found in registered keys!" << std::endl;
                    areFeaturesIdentical = false;
                }
            }
        }

        if (!areFeaturesIdentical) {
            std::cerr << "Make sure all candidates share the same set of features specified by CandidateCluster::RegisterFeatureNames!" << std::endl;
        }
        
        assert(featureVectorMap.begin()->second->size() == GetSize());
    }
}