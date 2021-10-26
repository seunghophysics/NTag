#include <iomanip>
#include <ios>
#include <algorithm>

#include "TTree.h"

#include "Printer.hh"
#include "Store.hh"

static const char vecDelimiter = ',';

void Store::Initialize(std::string configFilePath)
{
    std::ifstream file(configFilePath.c_str());
    std::string line;

    if (file.is_open()) {
        while (getline(file,line)) {
            if (line.size() > 0) {
                if (line.at(0) == '#') continue;
                std::string key, value;
                std::stringstream stream(line);
                if (stream >> key >> value)
                    Set(key, value);
            }
        }
    }
    else
        std::cout << "\033[38;5;196m WARNING: Config file "
                  << configFilePath << " does not exist. No config loaded \033[0m" << std::endl;
    file.close();
}

void Store::ReadArguments(const ArgParser& argParser)
{
    auto& optionPairs = argParser.GetOptionPairs();
    
    for (auto const& pair: optionPairs) {
        Set(pair.first, pair.second);
    }
}

void Store::Print() const
{
    Printer msg;
    msg.PrintBlock(name + ": Keys and values");

    unsigned int maxWidth = 0;
    for (auto const& key: keyOrder) {
        if (key.length() > maxWidth)
            maxWidth = key.length();
    }

    for (auto const& key: keyOrder)
        std::cout << std::left << std::setw(maxWidth+1) << key << ": " << storeMap.at(key) << "\n";
    std::cout << std::endl;
}

void Store::MakeBranches()
{
    if (fIsOutputTreeSet) {
         for (auto const& pair: storeMap) {
            auto key = pair.first.c_str();
            auto value = pair.second;

            if (Get(key, tmpNum)) {
                fOutputTree->Branch(key, &tmpNum);
            }
            else {
                tmpStr = value;
                fOutputTree->Branch(key, &tmpStr);
            }
        }
    }

    fillCounter = 0;
}

void Store::FillTree()
{
    if (fIsOutputTreeSet) {
        for (auto const& pair: storeMap) {
            auto key = pair.first;
                if (!Get(key, tmpNum)) {
                    Get(key, tmpStr);
                }

            fOutputTree->GetBranch(key.c_str())->Fill();
        }

        fillCounter++;
    }
}

void Store::WriteTree()
{
    fOutputTree->SetEntries(fillCounter);
    TreeOut::WriteTree();
}

std::istream& operator>>(std::istream& istr, TVector3& vec)
{
    std::string coordinate;
    unsigned int iDim = 0;

    while (iDim < 3 && std::getline(istr, coordinate, vecDelimiter)) {
        vec[iDim] = ::atof(coordinate.c_str());
        iDim++;
    }

    return istr;
}

std::ostream& operator<<(std::ostream& ostr, TVector3 vec)
{
    ostr << vec.x() << vecDelimiter << vec.y() << vecDelimiter << vec.z();
    return ostr;
}