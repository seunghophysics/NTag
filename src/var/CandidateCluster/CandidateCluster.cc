#include <algorithm>
#include <iomanip>
#include <cassert>
#include <cmath>

#include "TTree.h"

#include "NTagGlobal.hh"
#include "Taggable.hh"
#include "Printer.hh"
#include "CandidateCluster.hh"

CandidateCluster::CandidateCluster(): fNCandidates(0) {}
CandidateCluster::CandidateCluster(const char* className): CandidateCluster() { fName = className; }
CandidateCluster::CandidateCluster(const CandidateCluster& cluster): CandidateCluster() { fElement = cluster.GetVector(); }
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

void CandidateCluster::DumpAllElements(std::vector<std::string> keys, bool showTaggedOnly) const
{
    Printer msg;
    msg.PrintBlock(fName + (showTaggedOnly?" Tagged":"") + " Candidates", pSUBEVENT);

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

            if (showTaggedOnly && fElement[iCandidate].Get("TagClass")==0) continue;

            std::cout << std::right << std::setw(4) << iCandidate+1 << " ";
            auto candidateFeatureMap = fElement[iCandidate].GetFeatureMap();
            for (auto const& key: keys) {
                int textWidth = key.size()>6 ? key.size() : 6;
                float value = fElement[iCandidate][key];
                auto keyString = TString(key);
                if (keyString.Contains("Index")) {
                    std::cout << std::right << std::setw(textWidth) << (value>=0 ? std::to_string(int(value+1)) : "-") << " ";
                }
                else if (keyString.Contains("FitT") && fabs(value) < 10) {
                    std::cout << std::fixed << std::setprecision(2) << std::setw(textWidth) << value << " ";
                }
                else if (keyString.Contains("BSenergy")) {
                    std::cout << std::setw(textWidth) << (value>=0 ? Form("%3.2f",value) : "-");
                }
                else if (key == "Label") {
                    if (value <= lNoise) std::cout << std::right << std::setw(textWidth) << "-";
                    else if (value <= lDecayE) std::cout << std::right << std::setw(textWidth) << "e";
                    else if (value <= lnH) std::cout << std::right << std::setw(textWidth) << "nH";
                    else if (value <= lnGd) std::cout << std::right << std::setw(textWidth) << "nGd";
                    else if (value <= lGamma) std::cout << std::right << std::setw(textWidth) << "g";
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
                    auto value = comparedFeatureMap[basePair.first];
                    if (std::isnan(value) || std::isinf(value)) {
                        if (std::isnan(value)) {
                            std::cerr << Form("Candidate #%d: key %s value is NaN!", iCandidate, basePair.first.c_str()) << std::endl;
                        }
                        if (std::isinf(value)) {
                            std::cerr << Form("Candidate #%d: key %s value is inf!", iCandidate, basePair.first.c_str()) << std::endl;
                        }
                        std::cerr << Form("Dumping all features in map...", basePair.first) << std::endl;
                        DumpAllElements(gNTagFeatures);
                        abort();
                    }
                    else
                        fFeatureVectorMap[basePair.first]->push_back(value);
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
            std::cerr << "Make sure all candidates share the same set of features"
                         " specified by CandidateCluster::RegisterFeatureNames!" << std::endl;
        }

        assert(fFeatureVectorMap.begin()->second->size() == GetSize());
    }

    fNCandidates = GetSize();
}

void CandidateCluster::MakeBranches()
{
    if (fIsOutputTreeSet) {
        fOutputTree->Branch("NCandidates", &fNCandidates);
        for (auto& pair: fFeatureVectorMap) {
            fOutputTree->Branch(pair.first.c_str(), &(pair.second));
        }
    }
}
