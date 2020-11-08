#include <algorithm>
#include <cstring>

#include <TTree.h>
#include <TMVA/Reader.h>

#include "NTagTMVAVariables.hh"

NTagTMVAVariables::NTagTMVAVariables(Verbosity verbose)
{
    msg = NTagMessage("TMVAVariables", verbose);
    Clear();
}

NTagTMVAVariables::~NTagTMVAVariables() {}

void NTagTMVAVariables::Clear()
{
    iVariableMap["NHits"] = 0;
    iVariableMap["N200"] = 0;

    for (const auto& pair: iVariableMap) {
        if (iEventVectorMap.find(pair.first) == iEventVectorMap.end())
            iEventVectorMap[pair.first] = new std::vector<int>();
        else
            iEventVectorMap[pair.first]->clear();
        fVariableMap[pair.first] = 0.;
    }

    fVariableMap["AngleMean"] = 0.;
    fVariableMap["AngleStdev"] = 0.;
    fVariableMap["AngleSkew"] = 0.;
    fVariableMap["TRMS"] = 0.;
    fVariableMap["Beta1"] = 0.;
    fVariableMap["Beta2"] = 0.;
    fVariableMap["Beta3"] = 0.;
    fVariableMap["Beta4"] = 0.;
    fVariableMap["Beta5"] = 0.;
    fVariableMap["DWall"] = 0.;
    fVariableMap["DWallMeanDir"] = 0.;
    fVariableMap["prompt_nfit"] = 0.;

    for (const auto& pair: fVariableMap) {
        if (fEventVectorMap.find(pair.first) == fEventVectorMap.end())
            fEventVectorMap[pair.first] = new std::vector<float>();
        else
            fEventVectorMap[pair.first]->clear();
    }
}

std::vector<std::string> NTagTMVAVariables::Keys()
{
    std::vector<std::string> keys;

    for (const auto& pair: fVariableMap) {
            keys.push_back(pair.first);
    }

    return keys;
}

void NTagTMVAVariables::AddVariablesToReader(TMVA::Reader* reader)
{
    for (auto& pair: fVariableMap) {
        msg.Print(Form("Adding variable %s...", pair.first.c_str()), pDEBUG);
        reader->AddVariable(pair.first, &pair.second);
    }
    std::cout << std::endl;

    reader->AddSpectator("CaptureType", &captureType);
}

void NTagTMVAVariables::SetBranchAddressToTree(TTree* tree)
{
    for (auto& pair: iEventVectorMap) {
        tree->SetBranchAddress(pair.first.c_str(), &(pair.second));
    }

    for (auto& pair: fEventVectorMap) {
        if (!iVariableMap.count(pair.first))
            tree->SetBranchAddress(pair.first.c_str(), &(pair.second));
    }
}

void NTagTMVAVariables::MakeBranchesToTree(TTree* tree)
{
    for (auto& pair: iEventVectorMap) {
        tree->Branch(pair.first.c_str(), &(pair.second));
    }

    for (auto& pair: fEventVectorMap) {
        if (!iVariableMap.count(pair.first))
            tree->Branch(pair.first.c_str(), &(pair.second));
    }
}

void NTagTMVAVariables::SetVariablesForCaptureCandidate(int iCandidate)
{
    for (const auto& pair: iEventVectorMap) {
        iVariableMap[pair.first] = pair.second->at(iCandidate);
    }

    for (const auto& pair: fEventVectorMap) {
        if (iVariableMap.count(pair.first))
            fVariableMap[pair.first] = iVariableMap[pair.first];
        else
            fVariableMap[pair.first] = pair.second->at(iCandidate);
    }
}

void NTagTMVAVariables::FillVectorMap()
{
    for (auto const& pair: fVariableMap) {
        fEventVectorMap[pair.first]->push_back(pair.second);
    }
}

void NTagTMVAVariables::DumpCurrentVariables()
{
    for (const auto& pair: fVariableMap) {
        msg.Print(Form("%s: %f", pair.first.c_str(), pair.second));
    }
}

int NTagTMVAVariables::GetNumberOfCandidates()
{
    return static_cast<int>(fEventVectorMap.begin()->second->size());
}