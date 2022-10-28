/*******************************************
*
* @file Cluster.hh
*
* @brief Defines Cluster.
*
********************************************/

#ifndef CLUSTER_HH
#define CLUSTER_HH

#include <cassert>
#include <vector>
#include <iostream>

#include "TTree.h"
//#include "Rtypes.h"

#include "TreeOut.hh"

/*******************************************
*
* @brief Vector-like dynamic container class.
*
* @details This class is basically a vector-like
* dynamic container with some useful functions.
*
********************************************/

template <class T>
class Cluster
{
    public:
        Cluster(): fElement() {}

        /**
         * @brief Appends a new element.
         * @param elm An element to append.
         * @note Same as \c std::vector::push_back
         */
        virtual inline void Append(const T& elm) { fElement.push_back(elm); }

        /**
         * @brief Concatenates another cluster to itself.
         * @param cluster A Cluster object to append.
         */
        virtual inline void Append(Cluster<T>& cluster)
        {
            auto addedVector = cluster.GetVector();
            fElement.insert(fElement.end(), addedVector.begin(), addedVector.end());
        }

        virtual inline void MoveAppend(T& elm) { fElement.push_back(std::move(elm)); }

        /**
         * @brief Erase an element of a given index.
         * @param id The index of an element to delete.
         * @note Same as \c std::vector::erase
         */
        virtual void Erase(int id) { fElement.erase(fElement.begin() + id); }

        /**
         * @brief Erase an element of a given index.
         * @param id The index of an element to delete.
         * @note Same as \c std::vector::clear
         */
        virtual void Clear() { fElement.clear(); }

        /**
         * @brief Returns true if the Cluster object has no element.
         * @return \c true if the Cluster::fElement is empty, else \c false
         * @note Same as \c std::vector::empty
         */
        virtual bool IsEmpty() const { return fElement.empty(); }

        /**
         * @brief Deep copies a Cluster object to itself.
         * @param copiedCluster The pointer to a Cluster object to copy.
         */
        virtual void Copy(Cluster<T>* copiedCluster)
        {
            Clear();
            int n = copiedCluster->GetSize();
            for (int i=0; i<n; i++)
                Append(copiedCluster->At(i));
        }

        /**
         * @brief Prints out information of the saved elements. (virtual)
         * @details This function should be implemented in derived classes.
         */
        virtual void DumpAllElements() const {}

        /**
         * @brief Returns the size of the object.
         * @return The size of the object.
         * @note Same as \c std::vector::size
         */
        const unsigned int GetSize() const { return fElement.size(); }

        /** Returns the name of the object */
        std::string GetName() const { return fName; }

        /**
         * @brief Returns a vector of lambda(element).
         * @param lambda A function to act (project) upon the object's elements.
         * @return A vector whose elements are lambda function values of the elements.
         * @note Useful for projecting a specific variable of an element.
         */
        template<typename Functor>
        std::vector<T> GetProjection(Functor const& lambda)
        {
            std::vector<T> output;
            for_each(fElement.begin(), fElement.end(), [&](T elem){ output.push_back(lambda(elem)); });

            return output;
        }

        typename std::vector<T>::iterator begin() { return fElement.begin(); }
        typename std::vector<T>::iterator end() { return fElement.end(); }
        typename std::vector<T>::const_iterator begin() const { return fElement.begin(); }
        typename std::vector<T>::const_iterator end() const { return fElement.end(); }
        typename std::vector<T>::const_iterator cbegin() const { return fElement.cbegin(); }
        typename std::vector<T>::const_iterator cend() const { return fElement.cend(); }

        /**
         * @brief Returns the STL vector of the elements.
         */
        const std::vector<T>& GetVector() const { return fElement; }

        T& operator[] (int index) { return fElement.at(index); }         //< Returns the element of a given index.
        T& At(int index) { return fElement.at(index); }                  //< Returns the element of a given index.
        const T& ConstAt(int index) const { return fElement.at(index); } //< Returns the element of a given index.

        T& First() { return fElement.at(0); }    //< Returns the first element.
        T& Last() { return fElement.back(); } //< Returns the last element.

    protected:
        std::string fName;
        std::vector<T> fElement;

    //ClassDef(Cluster, 1)
};

#endif