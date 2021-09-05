#ifndef STORE_HH
#define STORE_HH

#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>

#include <TVector3.h>

class TTree;

std::istream& operator>>(std::istream& istr, TVector3& vec);
std::ostream& operator<<(std::ostream& ostr, TVector3 vec);

template<typename T>
bool CheckType(const std::string& str)
{
    T t{};
    std::stringstream ss;
    ss << str;
    ss >> t;
    return !ss.fail();
}

class Store{

    public:
        Store():fOutputTree(NULL) {}
        Store(const char* className): name(className) {}
        void Initialize(std::string configFilePath);

        virtual void Print() const;
        void Clear() { storeMap.clear(); keyOrder.clear(); }
        bool HasKey(std::string key) { return (storeMap.count(key) > 0); }

        template <typename T>
        bool Get(std::string key, T& out)
        {
            if (storeMap.count(key) > 0) {

                std::stringstream stream(storeMap[key]);
                stream >> out;
                return !stream.fail();
            }

            else return false;
        }

        template<typename T>
        void Set(std::string key, T in)
        {
            std::stringstream stream;
            stream << in;
            if (!storeMap.count(key)) keyOrder.push_back(key);
            storeMap[key] = stream.str();
        }
        
        const std::map<std::string, std::string>& GetMap() { return storeMap; }
        
        // TTree access
        void SetTree(TTree& tree) { fOutputTree = &tree; }
        TTree* GetTree() { return fOutputTree; }
        void FillTree();
        void WriteTree() { if (fOutputTree == NULL) fOutputTree->Write(); }
        virtual void MakeBranches();

    protected:
        std::string name;
        std::map<std::string, std::string> storeMap;
        TTree* fOutputTree;
        bool fIsOutputTreeSet;
        
    private:    
        std::vector<std::string> keyOrder;
        
        // temporary values for branching
        TVector3 tmpVec;
        float tmpNum;
        std::string tmpStr;
        int fillCounter;
};

template bool CheckType<float>(const std::string& str);
template bool CheckType<TVector3>(const std::string& str);

#endif