/*******************************************
*
* @file Candidate.hh
*
* @brief Defines Candidate.
*
********************************************/

#ifndef CANDIDATE_HH
#define CANDIDATE_HH

#include <map>
#include <string>
#include <algorithm>
#include <iostream>
#include <vector>

//#include "Rtypes.h"

/*******************************************
*
* @brief The container of signal candidate's
* features.
*
* @details This class is basically a map whose
* keys are the names of the candidate's features
* and values are the features themselves in
* floating point numbers.
*
* Values can be set and retrieved using
* Candidate::Set and Candidate::Get functions.
*
* One can pass an unsigned integer index that
* may be useful in positioning the candidate
* among hits.
*
* @see EventNTagManager::FindFeatures for the
* use of Candidate::Set functions and how the
* passed hit index in the constructor could be
* used to position the candidate among PMT hits
* and fetch relevant features.
*
********************************************/

class Candidate
{
    public:
        /**
         * @brief Constructor of class Candidate.
         * @param iHit A hit ID that can help identify the location of the candidate among hits.
         */
        Candidate(unsigned int iHit=0): fHitID(iHit) {}

        /**
         * @brief Returns hit ID of the candidate.
         * @return Hit ID.
         */
        inline unsigned int HitID() const { return fHitID; }

        /**
         * @brief Sets hit ID of the candidate.
         * @param id Hit ID.
         */
        inline void SetHitID(unsigned int id) { fHitID = id; }

        /**
         * @brief Returns feature value given the feature name.
         * @param key Name of the feature.
         * @note \c key must be the name of an existent feature.
         * @return The value of the feature.
         */
        const float operator[](const std::string& key) const { return fFeatureMap.at(key); }

        /**
         * @brief Sets the feature name and value.
         * @param key Name of the feature.
         * @param value Value of the feature.
         * @note \c key already exists, \c value will update its corresponding value.
         */
        void Set(const std::string& key, float value) { fFeatureMap[key] = value; }

        /**
         * @brief Returns feature value given the feature name.
         * @param key Name of the feature.
         * @param value Value to return in case \c key is non-existent.
         * @note If \c key does not exist in the candidate's feature map, the passed parameter \c value will be returned.
         * Use this function instead of Candidate::operator[] to avoid errors due to non-existent keys.
         * @return The value of the feature in case \c key exists, otherwise \c value.
         */
        const float Get(const std::string& key, float value=0) const { if (fFeatureMap.count(key)) return fFeatureMap.at(key); else return value; }

        /**
         * @brief Clears all registered features.
         */
        void Clear() { fFeatureMap.clear(); }

        /**
         * @brief Prints out all feature names and values to the screen.
         */
        void Dump() const { for (auto const& pair: fFeatureMap) { std::cout << pair.first << ": " << pair.second; } }

        /**
         * @brief Returns the registered feature map.
         * @return The map of feature names (string) and values (float).
         */
        const std::map<std::string, float>& GetFeatureMap() const { return fFeatureMap; }

    protected:
        std::map<std::string, float> fFeatureMap;
        unsigned int fHitID;

    //ClassDef(Candidate, 1);
};

#endif