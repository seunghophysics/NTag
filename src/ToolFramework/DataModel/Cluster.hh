#ifndef CLUSTER_HH
#define CLUSTER_HH

#include <cassert>
#include <vector>
#include <iostream>
#include <type_traits>

template <class T>
class Cluster
{
    public:
        Cluster() { nElements = 0; }

        virtual inline void Append(const T& element) { vElements.push_back(element); nElements++; }
        virtual void Clear() { vElements.clear(); nElements = 0; }
        virtual bool IsEmpty() { return vElements.empty(); }

        virtual void DumpAllElements() {}

        unsigned int GetSize()
        {
            assert(nElements == static_cast<unsigned int>(vElements.size()));
            return nElements;
        }

        T& operator[] (int index) const { return vElements[index]; }
        T& At(int index) { return vElements[index]; }

    protected:
        std::vector<T> vElements;
        unsigned int nElements;
};

#endif