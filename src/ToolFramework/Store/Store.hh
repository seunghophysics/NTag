#ifndef STORE_HH
#define STORE_HH

#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>

class Store{

    public:
        void Print();
        void Clear() { m_variables.clear(); }
        bool HasKey(std::string key) { return (m_variables.count(key) != 0); }
    
        template <typename T> 
        bool Get(std::string name, T& out)
        {
            if (m_variables.count(name) > 0) {
    
                std::stringstream stream(m_variables[name]);
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
            m_variables[name] = stream.str();
        }
 
    private:
        std::map<std::string, std::string> m_variables;
};

#endif