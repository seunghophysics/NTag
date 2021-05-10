#ifndef STORE_HH
#define STORE_HH

#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>

#include <TVector3.h>

std::istream& operator>>(std::istream& istr, TVector3& vec);
std::ostream& operator<<(std::ostream& ostr, TVector3 vec);

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
                return true;
            }

            else return false;
        }

        template<typename T>
        void Set(std::string name, T in)
        {
            std::stringstream stream;
            stream << in;
            storeMap[name] = stream.str();
        }

    protected:
        std::map<std::string, std::string> storeMap;
};

#endif