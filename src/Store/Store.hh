#ifndef STORE_HH
#define STORE_HH

#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>

#include <TVector3.h>

#include "ArgParser.hh"
#include "TreeOut.hh"

std::istream& operator>>(std::istream& istr, TVector3& vec);
std::ostream& operator<<(std::ostream& ostr, TVector3 vec);

//template<typename T>
//bool CheckType(const std::string& str)
//{
//    T t{};
//    std::stringstream ss;
//    ss << str;
//    ss >> t;
//    return !ss.fail();
//}

class Store : public TreeOut
{

    public:
        Store():TreeOut() {}
        Store(const char* className): name(className) {}
        void Initialize(std::string configFilePath);
        void ReadArguments(const ArgParser& argParser);

        virtual void Print() const;
        void Clear() { fMap.clear(); fKeyOrder.clear(); }
        bool HasKey(std::string key) { return (fMap.count(key) > 0); }
        void RemoveKey(std::string key);
        
        template <typename T>
        bool Get(std::string key, T& out)
        {
            if (fMap.count(key) > 0) {

                std::stringstream stream(fMap[key]);
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
            if (!fMap.count(key)) fKeyOrder.push_back(key);
            fMap[key] = stream.str();
        }
        
        bool GetBool(std::string key, bool emptyVal=false)
        {
            if (fMap.count(key) > 0) {
                if (fMap[key]=="true" || fMap[key]=="1") return true;
                else if (fMap[key]=="false" || fMap[key]=="0") return false;
                else {
                    std::cerr << "Key " << key << " in the Store " << name
                              << " has a non-boolean value " << fMap[key] << "\n";
                    return emptyVal;
                }
            }
            else return emptyVal;
        }
        
        int GetInt(std::string key, int emptyVal=0)
        {
            if (fMap.count(key) > 0)
                return std::stoi(fMap[key]);
            else return emptyVal;
        }
        
        float GetFloat(std::string key, float emptyVal=0)
        {
            if (fMap.count(key) > 0)
                return std::stof(fMap[key]);
            else return emptyVal;
        }

        const std::map<std::string, std::string>& GetMap() { return fMap; }

        // TTree access
        void MakeBranches();
        void FillTree();
        void WriteTree();

    protected:
        std::string name;
        std::map<std::string, std::string> fMap;

    private:
        std::vector<std::string> fKeyOrder;

        // temporary values for branching
        TVector3 tmpVec;
        float tmpNum;
        std::string tmpStr;
        int fillCounter;
};

//template bool CheckType<float>(const std::string& str);
//template bool CheckType<TVector3>(const std::string& str);

#endif