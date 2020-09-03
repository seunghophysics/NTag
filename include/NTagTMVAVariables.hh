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

typedef std::map<const char*, int>   IVarMap;
typedef std::map<const char*, float> FVarMap;
typedef std::map<const char*, std::vector<int>*>   IVecMap;
typedef std::map<const char*, std::vector<float>*> FVecMap;

class NTagTMVAVariables
{
    public:
        NTagTMVAVariables(unsigned int verbose=pDEFAULT);
        ~NTagTMVAVariables();

        void Clear();
        std::vector<const char*> Keys();

        void AddVariablesToReader(TMVA::Reader* reader);
        void SetBranchAddressToTree(TTree* tree);
        void MakeBranchesToTree(TTree* tree);
        void SetVariablesForCaptureCandidate(int iCandidate);
        void DumpCurrentVariables();

        int  GetNumberOfCandidates();

        // Functions to store and fetch variables
        template <typename T>
        void                PushBack(const char* key, T value)
                            { if (std::is_integral<T>::value) iEventVectorMap[key]->push_back(value);
                              else fEventVectorMap[key]->push_back(value); }
        template <typename T>
        T                   Get(const char* key)
                            { if (std::is_integral<T>::value) return iVariableMap[key];
                              else return fVariableMap[key]; }
        template <typename T>
        T                   Get(const char* key, int iCandidate)
                            { if (std::is_integral<T>::value) return iEventVectorMap[key]->at(iCandidate);
                              else return fEventVectorMap[key]->at(iCandidate); }
        std::vector<float>* GetVector(const char* key)
                            { return fEventVectorMap[key]; }

    private:
        IVarMap iVariableMap;
        IVecMap iEventVectorMap;
        FVarMap fVariableMap;
        FVecMap fEventVectorMap;

        NTagMessage msg;

        friend class NTagTMVA;
        friend class NTagEventInfo;
};

#endif