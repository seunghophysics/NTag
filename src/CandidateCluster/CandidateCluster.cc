#include <algorithm>
#include <iomanip>
#include <cassert>
#include <cmath>

#include "Printer.hh"
#include "CandidateCluster.hh"

CandidateCluster::CandidateCluster() {}
CandidateCluster::CandidateCluster(const char* className) { fName = className; }

CandidateCluster::~CandidateCluster()
{
    for (auto& pair: featureVectorMap)
        delete pair.second;
}

void CandidateCluster::Sort()
{
    //std::sort(fElement.begin(), fElement.end(), 
    //[](const Candidate& candidate1, const Candidate& candidate2){ 
    //    return candidate1.Get("ReconCT") < candidate2.Get("ReconCT"); });
}

void CandidateCluster::DumpAllElements(std::vector<std::string> keys) const
{
    Printer msg;
    msg.PrintTitle(fName + " Candidates");

    if (!GetSize()) {
        msg.Print("No candidate in cluster!");
    }
    else {
        int xWidth = 10;
        std::cout << "\033[4m No. ";
        auto baseFeatureMap = fElement[0].GetFeatureMap();
        
        if (keys.empty()) {
            for (auto const& pair: baseFeatureMap)
                keys.push_back(pair.first);
        }
        
        for (auto const& key: keys) {
            int textWidth = key.size()>6 ? key.size() : 6;
            std::cout << std::right << std::setw(textWidth) << key << " ";
        }
        std::cout << "\033[0m\n";

        for (int iCandidate = 0; iCandidate < GetSize(); iCandidate++) {
            std::cout << std::right << std::setw(4) << iCandidate+1 << " ";
            auto candidateFeatureMap = fElement[iCandidate].GetFeatureMap();
            for (auto const& key: keys) {
                int textWidth = key.size()>6 ? key.size() : 6;
                float value = fElement[iCandidate][key];
                
                if (TString(key).Contains("Index")) {
                    std::cout << std::right << std::setw(textWidth) << (value>=0 ? std::to_string(int(value+1)) : "-") << " "; 
                }
                else if (TString(key).Contains("ReconCT") && fabs(value) < 10) {
                    std::cout << std::fixed << std::setprecision(2) << std::setw(textWidth) << value << " ";
                }
                else if (key == "CaptureType") {
                    if (value <= lNoise) std::cout << std::right << std::setw(textWidth) << "-";
                    else if (value <= lDecayE) std::cout << std::right << std::setw(textWidth) << "e";
                    else if (value <= lnH) std::cout << std::right << std::setw(textWidth) << "nH";
                    else if (value <= lnGd) std::cout << std::right << std::setw(textWidth) << "nGd";
                    else if (value <= lRemnant) std::cout << std::right << std::setw(textWidth) << "remnant";
                    else if (value <= lUndefined) std::cout << std::right << std::setw(textWidth) << "?";
                    std::cout << " ";
                }
                else if (fabs(value) < 1 && value != 0) {
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