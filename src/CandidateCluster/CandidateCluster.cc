#include <algorithm>
#include <iomanip>
#include <cassert>
#include <cmath>

#include "TTree.h"

#include "NTagGlobal.hh"
#include "Taggable.hh"
#include "Printer.hh"
#include "CandidateCluster.hh"

CandidateCluster::CandidateCluster() {}
CandidateCluster::CandidateCluster(const char* className) { fName = className; }
CandidateCluster::CandidateCluster(const CandidateCluster& cluster) { fElement = cluster.GetVector(); }
CandidateCluster& CandidateCluster::operator=(CandidateCluster const& rhs)
{
    Cluster<Candidate>::operator=(rhs); return *this;
}

CandidateCluster::~CandidateCluster()
{
    for (auto& pair: fFeatureVectorMap) {
        delete pair.second;
        fFeatureVectorMap[pair.first] = 0;
    }
}

void CandidateCluster::DumpAllElements(std::vector<std::string> keys) const
{
    Printer msg;
    msg.PrintBlock(fName + " Candidates", pSUBEVENT);

    if (!GetSize()) {
        msg.Print("No candidate in cluster!");
    }
    else {
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

        for (unsigned int iCandidate = 0; iCandidate < GetSize(); iCandidate++) {
            std::cout << std::right << std::setw(4) << iCandidate+1 << " ";
            auto candidateFeatureMap = fElement[iCandidate].GetFeatureMap();
            for (auto const& key: keys) {
                int textWidth = key.size()>6 ? key.size() : 6;
                float value = fElement[iCandidate][key];

                if (TString(key).Contains("Index")) {
                    std::cout << std::right << std::setw(textWidth) << (value>=0 ? std::to_string(int(value+1)) : "-") << " ";
                }
                else if (TString(key).Contains("FitT") && fabs(value) < 10) {
                    std::cout << std::fixed << std::setprecision(2) << std::setw(textWidth) << value << " ";
                }
                else if (key == "Label") {
                    if (value <= lNoise) std::cout << std::right << std::setw(textWidth) << "-";
                    else if (value <= lDecayE) std::cout << std::right << std::setw(textWidth) << "e";
                    else if (value <= lnH) std::cout << std::right << std::setw(textWidth) << "nH";
                    else if (value <= lnGd) std::cout << std::right << std::setw(textWidth) << "nGd";
                    else if (value <= lRemnant) std::cout << std::right << std::setw(textWidth) << "=";
                    else if (value <= lUndefined) std::cout << std::right << std::setw(textWidth) << "?";
                    std::cout << " ";
                }
                else if (key == "TagClass") {
                    if (value == typeE) std::cout << std::right << std::setw(textWidth) << "e";
                    else if (value == typeN) std::cout << std::right << std::setw(textWidth) << "n";
                    else std::cout << std::right << std::setw(textWidth) << "-";
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
    if (!GetSize()) {
        //std::cerr << "No elements in CandidateCluster... skipping CandidateCluster::FillVectorMap." << std::endl;
    }

    else {
        //auto baseFeatureMap = element[0].GetFeatureMap();
        bool areFeaturesIdentical = true;
        for (unsigned int iCandidate = 0; iCandidate < GetSize(); iCandidate++) {
            auto comparedFeatureMap = fElement[iCandidate].GetFeatureMap();

            for (auto const& basePair: fFeatureVectorMap) {

                if (!comparedFeatureMap.count(basePair.first)) {
                    std::cerr << "Registered key " << basePair.first << " not found in candidate!" << std::endl;
                    areFeaturesIdentical = false;
                }
                else {
                    fFeatureVectorMap[basePair.first]->resize(iCandidate);
                    fFeatureVectorMap[basePair.first]->push_back(comparedFeatureMap[basePair.first]);
                }
            }

            for (auto const& comparedPair: comparedFeatureMap) {
                if (!fFeatureVectorMap.count(comparedPair.first)) {
                    std::cerr << "Candidate key " << comparedPair.first << " not found in registered keys!" << std::endl;
                    areFeaturesIdentical = false;
                }
            }
        }

        if (!areFeaturesIdentical) {
            std::cerr << "Make sure all candidates share the same set of features specified by CandidateCluster::RegisterFeatureNames!" << std::endl;
        }

        assert(fFeatureVectorMap.begin()->second->size() == GetSize());
    }
}

void CandidateCluster::MakeBranches()
{
    if (fIsOutputTreeSet) {
        for (auto& pair: fFeatureVectorMap) {
            fOutputTree->Branch(pair.first.c_str(), &(pair.second));
        }
    }
}