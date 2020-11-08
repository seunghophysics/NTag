/*******************************************
*
* @file NTagTMVAVariables.hh
*
* @brief Defines NTagTMVAVariables.
*
********************************************/

#ifndef NTAGTMVAVARIABLES_HH
#define NTAGTMVAVARIABLES_HH 1

#include <map>
#include <string>
#include <type_traits>

#include "NTagMessage.hh"

namespace TMVA
{
    class Reader;
}
class TTree;

/** A type definition for a map from a character key to an integer variable. */
typedef std::map<std::string, int>   IVarMap;
/** A type definition for a map from a character key to an float variable. */
typedef std::map<std::string, float> FVarMap;
/** A type definition for a map from a character key to a vector of integer variables. */
typedef std::map<std::string, std::vector<int>*>   IVecMap;
/** A type definition for a map from a character key to a vector of float variables. */
typedef std::map<std::string, std::vector<float>*> FVecMap;

/********************************************************
 * @brief The class for handling capture candidate
 * variables input to TMVA.
 *
 * This class holds maps to all integer and float
 * variables of neutron capture candidates that are to
 * be classified by TMVA. The values of variables that
 * are put in directly to TMVA are saved in #iVariableMap
 * and #fVariableMap, which are containers of integer and
 * float variables, respectively. Meanwhile,
 * #iEventVectorMap and #fEventVectorMap are pushed back
 * from NTagEventInfo and thet hold vectors of all
 * candidate variables obtainable from one event. The
 * values for one specific candidate is extracted using
 * the member function
 * NTagTMVAVariables::SetVariablesForCaptureCandidate.
 *
 * The main purpose of this class is to automate
 * clearing vectors and setting branches to an output
 * tree, so that users can easily add/remove
 * variables to/from TMVA without a hassle. Add/remove
 * variables from NTagTMVAVariables::Clear and you're
 * good to go. All other chores are taken care of by
 * the range-based loops inside the member functions.
 *******************************************************/
class NTagTMVAVariables
{
    public:
        /**
         * @brief Constructor of NTagTMVAVariables.
         * @details Calls NTagTMVAVariables::Clear to make rooms for the input variables to use in TMVA classification.
         * @param verbose #Verbosity.
         */
        NTagTMVAVariables(Verbosity verbose=pDEFAULT);
        ~NTagTMVAVariables();

        /**
         * @brief Sets all map values to default.
         * @details This function also serves as a custom list of variables to put into the TMVA.
         * If you are adding another variable, make sure to put #NTagTMVAVariables::PushBack
         * somewhere in NTagEventInfo to save it during the event processing.
         * #iVariableMap, #fVariableMap, #iEventVectorMap, and #fEventVectorMap are set to default.
         * (0 for non-vector members, a new vector for vector members.)
         */
        void Clear();

        /**
         * @brief Returns a vector of all variable names (keys).
         */
        std::vector<std::string> Keys();

        /**
         * @brief Adds all variables declared in #NTagTMVAVariables::Clear to a TMVA reader.
         * @param reader A TMVA reader to add variables.
         */
        void AddVariablesToReader(TMVA::Reader* reader);

        /**
         * @brief Sets branch address of variables to a tree.
         * @param tree A tree to set branch address.
         */
        void SetBranchAddressToTree(TTree* tree);

        /**
         * @brief Make branches of the variables to a tree.
         * @param tree A tree to make branches.
         */
        void MakeBranchesToTree(TTree* tree);

        /**
         * @brief Sets VariableMap[variable] = EventVectorMap[variable][iCandidate].
         * @details
         * @param iCandidate The index of a capture candidate. (0 < \c iCandidate < #NTagEventInfo::nCandidates)
         */
        void SetVariablesForCaptureCandidate(int iCandidate);

        /**
         * @brief Dump the keys and values of #iVariableMap and and #fVariableMap to the output stream.
         */
        void DumpCurrentVariables();

        /**
         * @brief Returns the number of candidates saved in this class.
         * @return The size of a vector (key: "NHits") held by #iEventVectorMap.
         */
        int  GetNumberOfCandidates();

        // Functions to store and fetch variables
        /**
         * @brief Type-independent `std::vector.push_back` replacement.
         * @details Pushes \c value back to the event vector map. Typename must be specified to
         * determine whether \c value should be pushed back to #iEventVectorMap or #fEventVectorMap.
         * @param key Name of a variable.
         * @param value A value to push back.
         */
        template <typename T>
        void                PushBack(std::string key, T value)
                            { if (std::is_integral<T>::value) iEventVectorMap[key]->push_back(value);
                              else fEventVectorMap[key]->push_back(value); }

        /**
         * @brief Gets the value of a given key.
         * @param key Name of a variable.
         * @return The value of \c key in a variable map. Typename must be specified to
         * determine whether \c key should be looked up in #iVariableMap or #fVariableMap.
         */
        template <typename T>
        T                   Get(std::string key)
                            { if (std::is_integral<T>::value) return iVariableMap[key];
                              else return fVariableMap[key]; }

        /**
         * @brief Gets the variable value of a specific capture candidate.
         * @param key Name of a variable.
         * @param iCandidate The index of the capture candidate.
         * @return The `iCandidate`-th element of the vector of \c key. Typename must be specified to
         * determine whether \c key should be taken from #iEventVectorMap or #fEventVectorMap.
         */
        template <typename T>
        T                   Get(std::string key, int iCandidate)
                            { if (std::is_integral<T>::value) return iEventVectorMap[key]->at(iCandidate);
                              else return fEventVectorMap[key]->at(iCandidate); }

        /**
         * @brief Gets float vector of \c key.
         * @param key Name of a variable.
         * @return The float vector of \c key from #fEventVectorMap.
         */
        std::vector<float>* GetVector(std::string key)
                            { return fEventVectorMap[key]; }

        /**
         * @brief Sets the \c key value of #iVariableMap or #fVariableMap as \c var.
         * @param key Feature variable name.
         * @param var Feature variable value.
         */
        template <typename T>
        void Set(std::string key, T var) { if (std::is_integral<T>::value) iVariableMap[key] = var;
                                           else fVariableMap[key] = var; }

        /**
         * @brief Pushes back variables from variable maps to the corresponding vectors in event vector maps.
         */
        void FillVectorMap();
        /**
         * @brief Checks if a \c key is a input feature variable to TMVA NN.
         * @param key Feature variable name.
         * @return \c true if \p key is in #fVariableMap, otherwise \c false.
         */
        bool IsTMVAVariable(std::string key) { if (fVariableMap.find(key) == fVariableMap.end()) return false; else return true; }
        
        void SetCaptureType(int id) { captureType = id; }
        
    private:
        IVarMap iVariableMap;    ///< A map from a character key to an integer variable.
        IVecMap iEventVectorMap; ///< A map from a character key to a vector of integer variables.
        FVarMap fVariableMap;    ///< A map from a character key to a float variable.
        FVecMap fEventVectorMap; ///< A map from a character key to a vector of float variables.

        int captureType;
        
        NTagMessage msg;

        friend class NTagTMVA;
        friend class NTagEventInfo;
};

#endif