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

void Store::RemoveKey(std::string key)
{
    // map
    fMap.erase(key);

    // vector
    fKeyOrder.erase(std::remove(fKeyOrder.begin(), fKeyOrder.end(), key), fKeyOrder.end());
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
    for (auto const& key: fKeyOrder) {
        if (key.length() > maxWidth)
            maxWidth = key.length();
    }

    for (auto const& key: fKeyOrder)
        std::cout << std::left << std::setw(maxWidth+1) << key << ": " << fMap.at(key) << "\n";
    std::cout << std::endl;
}

void Store::MakeBranches()
{
    if (fIsOutputTreeSet) {
         for (auto const& key: fKeyOrder) {
            auto value = fMap[key];
            if (Get(key, tmpFloat)) {
                std::size_t readSize; tmpInt = std::stoi(value, &readSize);
                if (readSize==value.size())
                    fOutputTree->Branch(key.c_str(), &tmpInt);
                else
                    fOutputTree->Branch(key.c_str(), &tmpFloat);
            }
            else {
                tmpStr = value;
                fOutputTree->Branch(key.c_str(), &tmpStr);
            }
        }
    }

    fillCounter = 0;
}

void Store::FillTree()
{
    if (fIsOutputTreeSet) {
        for (auto const& key: fKeyOrder) {
            if (!Get(key, tmpFloat)) Get(key, tmpStr);
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