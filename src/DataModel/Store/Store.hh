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
        void Initialize(std::string configFilePath);

        virtual void Print();
        void Clear() { storeMap.clear(); }
        bool HasKey(std::string key) { return (storeMap.count(key) > 0); }

        template <typename T>
        bool Get(std::string name, T& out)
        {
            if (storeMap.count(name) > 0) {

                std::stringstream stream(storeMap[name]);
                stream >> out;
                return !stream.fail();
            }

            else return false;
        }

        template<typename T>
        void Set(std::string name, T in)
        {
            std::stringstream stream;
            stream << in;
            if (!storeMap.count(name)) keyOrder.push_back(name);
            storeMap[name] = stream.str();
        }
        
        const std::map<std::string, std::string>& GetMap() { return storeMap; }

    protected:
        std::string name;
        std::map<std::string, std::string> storeMap;
        
    private:    
        std::vector<std::string> keyOrder;
};

template bool CheckType<float>(const std::string& str);
template bool CheckType<TVector3>(const std::string& str);

#endif