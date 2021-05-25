#ifndef CLUSTER_HH
#define CLUSTER_HH

#include <cassert>
#include <vector>
#include <iostream>

#include "Rtypes.h"

template <class T>
class Cluster
{
    public:
        Cluster():element(), nElements(0) {}

        virtual inline void Append(const T& elm) { element.push_back(elm); nElements++; }
        //virtual inline void Append(T elm) { element.push_back(elm); nElements++; }
        virtual inline void MoveAppend(T& elm) { element.push_back(std::move(elm)); nElements++; }
        virtual void Clear() { element.clear(); nElements = 0; }
        virtual bool IsEmpty() { return element.empty(); }
        virtual void Copy(Cluster<T>* copiedCluster)
        {
            Clear();
            int n = copiedCluster->GetSize();
            for (int i=0; i<n; i++)
                Append(copiedCluster->At(i));
        }

        virtual void DumpAllElements() {}

        unsigned int GetSize()
        {
            assert(nElements == static_cast<unsigned int>(element.size()));
            return nElements;
        }

        T& operator[] (int index) { return element[index]; }
        T& At(int index) { return element[index]; }

    protected:
        std::vector<T> element;
        unsigned int nElements;

    ClassDef(Cluster, 1)
};

#endif