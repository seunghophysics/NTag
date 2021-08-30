#ifndef CLUSTER_HH
#define CLUSTER_HH

#include <cassert>
#include <vector>
#include <iostream>

//#include "Rtypes.h"

template <class T>
class Cluster
{
    public:
        Cluster():fElement() {}

        virtual inline void Append(const T& elm) { fElement.push_back(elm); }
        virtual inline void Append(Cluster<T>& cluster)
        {
            auto addedVector = cluster.GetVector();
            fElement.insert(fElement.end(), addedVector.begin(), addedVector.end());
        }

        virtual inline void MoveAppend(T& elm) { fElement.push_back(std::move(elm)); }
        virtual void Erase(int id) { fElement.erase(fElement.begin() + id); }
        virtual void Clear() { fElement.clear(); }
        virtual bool IsEmpty() { return fElement.empty(); }
        virtual void Copy(Cluster<T>* copiedCluster)
        {
            Clear();
            int n = copiedCluster->GetSize();
            for (int i=0; i<n; i++)
                Append(copiedCluster->At(i));
        }

        virtual void DumpAllElements() const {}

        const unsigned int GetSize() const { return fElement.size(); }
        std::string GetName() const { return fName; }
        
        typename std::vector<T>::iterator begin() { return fElement.begin(); }
        typename std::vector<T>::iterator end() { return fElement.end(); }
        typename std::vector<T>::const_iterator begin() const { return fElement.begin(); }
        typename std::vector<T>::const_iterator end() const { return fElement.end(); }
        typename std::vector<T>::const_iterator cbegin() const { return fElement.cbegin(); }
        typename std::vector<T>::const_iterator cend() const { return fElement.cend(); }

        const std::vector<T>& GetVector() { return fElement; }

        T& operator[] (int index) { return fElement[index]; }
        T& At(int index) { return fElement[index]; }

    protected:
        std::string fName;
        std::vector<T> fElement;

    //ClassDef(Cluster, 1)
};

#endif