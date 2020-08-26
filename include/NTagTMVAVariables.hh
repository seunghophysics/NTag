#ifndef NTAGTMVAVARIABLES_HH
#define NTAGTMVAVARIABLES_HH 1

#include <map>
#include <string>
#include <type_traits>

#include <NTagMessage.hh>

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
        NTagTMVAVariables();
        ~NTagTMVAVariables();
        
        void  Clear();
        template <typename T>
        void  PushBack(const char* key, T value){ if (std::is_integral<T>::value) iEventVectorMap[key]->push_back(value);
                                                  else fEventVectorMap[key]->push_back(value); }
        
        template <typename T>
        T Get(const char* key) { if (std::is_integral<T>::value) return iVariableMap[key];
                                 else return fVariableMap[key]; };
        std::vector<float>* GetVector(const char* key) { return fEventVectorMap[key]; }
        float Get(const char* key, int iCandidate) { return fEventVectorMap[key]->at(iCandidate); }
        std::vector<const char*> Keys();
        
        void AddVariablesToReader(TMVA::Reader* reader);
        void SetBranchAddressToTree(TTree* tree);
        void SetVariablesForCaptureCandidate(int iCandidate);
        
        int  GetNumberOfCandidates();
        
    private:
        IVarMap iVariableMap;
        IVecMap iEventVectorMap;
        FVarMap fVariableMap;
        FVecMap fEventVectorMap;
        
        NTagMessage msg;
        /*
        float               mva_N10, mva_N50, mva_N200;
        float               mva_dt, mva_trmsold, mva_trms50;
        float               mva_nwall, mva_evis, mva_sumQ, mva_spread;
        float               mva_beta1_50, mva_beta2_50, mva_beta3_50, mva_beta4_50, mva_beta5_50;
        float               mva_tbsenergy, mva_tbswall, mva_tbsgood;
        float               mva_tbsdirks, mva_tbspatlik, mva_tbsovaq;
        float               mva_Prompt_Nfit, mva_Prompt_BONSAI, mva_BONSAI_NFit;
        */
        
        friend class NTagTMVA;
        friend class NTagEventInfo;
};

#endif