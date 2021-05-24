#include <iomanip>
#include <cmath>

#include "EventCandidates.hh"

EventCandidates::~EventCandidates()
{
    for (auto& pair: featureVectorMap)
        delete pair.second;
}

void EventCandidates::Print()
{
    if (!nElements) {
        std::cerr << "No candidate to print!" << std::endl;
    }
    else {
        int xWidth = 10;
        std::cout << "\033[4m\n No. ";
        auto baseFeatureMap = vElements[0].GetFeatureMap();
        for (auto const& pair: baseFeatureMap) {
            int textWidth = pair.first.size()>6 ? pair.first.size() : 6;
            std::cout << std::right << std::setw(textWidth) << pair.first << " ";
        }
        std::cout << "\033[0m\n";

        for (int iCandidate = 0; iCandidate < nElements; iCandidate++) {
            std::cout << std::right << std::setw(4) << iCandidate+1 << " ";
            auto candidateFeatureMap = vElements[iCandidate].GetFeatureMap();
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


void EventCandidates::FillVectorMap()
{
    if (!nElements)
        std::cerr << "No elements in EventCandidates... skipping EventCandidates::FillVectorMap." << std::endl;

    else {
        auto baseFeatureMap = vElements[0].GetFeatureMap();
        bool areFeaturesIdentical = true;
        for (int iCandidate = 0; iCandidate < nElements; iCandidate++) {
            auto comparedFeatureMap = vElements[iCandidate].GetFeatureMap();

            for (auto const& basePair: baseFeatureMap) {

                if (!comparedFeatureMap.count(basePair.first)) {
                    areFeaturesIdentical = false;
                }
                else {
                    if (!featureVectorMap.count(basePair.first)) {
                        featureVectorMap[basePair.first] = new std::vector<float>;
                    }

                    featureVectorMap[basePair.first]->resize(iCandidate);
                    featureVectorMap[basePair.first]->push_back(comparedFeatureMap[basePair.first]);
                }
            }

            for (auto const& comparedPair: comparedFeatureMap) {
                if (!baseFeatureMap.count(comparedPair.first)) {
                    areFeaturesIdentical = false;
                }
            }
        }

        if (!areFeaturesIdentical) {
            std::cerr << "Please make sure that all candidates same the same set of features!" << std::endl;
        }
        
        assert(featureVectorMap.begin()->second->size() == nElements);
    }
}