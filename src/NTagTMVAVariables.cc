#include <algorithm>

#include <TTree.h>
#include <TMVA/Reader.h>

#include <NTagTMVAVariables.hh>

NTagTMVAVariables::NTagTMVAVariables()
{
    msg = NTagMessage("TMVAVariables", pDEBUG);
    Clear();
}

NTagTMVAVariables::~NTagTMVAVariables() {}

void NTagTMVAVariables::Clear()
{
    iVariableMap["N10"] = 0;
    iVariableMap["N200"] = 0;
    iVariableMap["N50"] = 0;
    
    fVariableMap["dt"] = 0.;
    fVariableMap["sumQ"] = 0.;
    fVariableMap["spread"] = 0.;
    fVariableMap["trmsold"] = 0.;
    fVariableMap["beta1"] = 0.;
    fVariableMap["beta2"] = 0.;
    fVariableMap["beta3"] = 0.;
    fVariableMap["beta4"] = 0.;
    fVariableMap["beta5"] = 0.;
    fVariableMap["prompt_nfit"] = 0.;
    fVariableMap["tbsenergy"] = 0.;
    fVariableMap["tbswall"] = 0.;
    fVariableMap["tbsgood"] = 0.;
    fVariableMap["tbsdirks"] = 0.;
    fVariableMap["tbspatlik"] = 0.;
    fVariableMap["tbsovaq"] = 0.;
    fVariableMap["prompt_bonsai"] = 0.;
    fVariableMap["nwall"] = 0.;
    fVariableMap["trms40"] = 0.;
    fVariableMap["bonsai_nfit"] = 0.;
    
    for (const auto& pair: iVariableMap) {
        iEventVectorMap[pair.first] = new std::vector<int>();
        fVariableMap[pair.first] = 0.;
    }
    
    for (const auto& pair: fVariableMap) {
        fEventVectorMap[pair.first] = new std::vector<float>();
    }
}

std::vector<const char*> NTagTMVAVariables::Keys()
{
    std::vector<const char*> keys;
    
    for (const auto& pair: iVariableMap) {
        keys.push_back(pair.first);
    }
    
    for (const auto& pair: fVariableMap) {
        if (std::find(keys.begin(), keys.end(), pair.first) != keys.end())
            keys.push_back(pair.first);
    }
    
    return keys;
}

void NTagTMVAVariables::AddVariablesToReader(TMVA::Reader* reader)
{
    for (auto& pair: fVariableMap) {
        //msg.Print(Form("Adding variable %s...", pair.first), pDEBUG);
        reader->AddVariable(pair.first, &pair.second);
    }

    /*
    reader->AddVariable( "evis", &mva_evis );
    reader->AddVariable( "N10", &mva_N10 );
    reader->AddVariable( "N200", &mva_N200 );
    reader->AddVariable( "N50", &mva_N50 );
    reader->AddVariable( "dt" , &mva_dt );
    reader->AddVariable( "sumQ" , &mva_sumQ );
    reader->AddVariable( "spread" , &mva_spread );
    reader->AddVariable( "trmsold", &mva_trmsold );
    reader->AddVariable( "beta1", &mva_beta1_50 );
    reader->AddVariable( "beta2", &mva_beta2_50 );
    reader->AddVariable( "beta3", &mva_beta3_50 );
    reader->AddVariable( "beta4", &mva_beta4_50 );
    reader->AddVariable( "beta5", &mva_beta5_50 );
    reader->AddVariable( "AP_Nfit:=sqrt((vx-nvx)*(vx-nvx)+(vy-nvy)*(vy-nvy)+(vz-nvz)*(vz-nvz))",&mva_Prompt_Nfit );
    reader->AddVariable( "tbsenergy", &mva_tbsenergy );
    reader->AddVariable( "tbswall", &mva_tbswall );
    reader->AddVariable( "tbsgood", &mva_tbsgood );
    reader->AddVariable( "tbsdirks", &mva_tbsdirks);
    reader->AddVariable( "tbspatlik", &mva_tbspatlik );
    reader->AddVariable( "tbsovaq", &mva_tbsovaq );
    reader->AddVariable( "AP_BONSAI:=sqrt((vx-tbsvx)*(vx-tbsvx)+(vy-tbsvy)*(vy-tbsvy)+(vz-tbsvz)*(vz-tbsvz))",&mva_Prompt_BONSAI );
    reader->AddVariable( "nwall", &mva_nwall );
    reader->AddVariable( "trms40", &mva_trms50 );
    reader->AddVariable( "Nfit_BONSAI:=sqrt((nvx-tbsvx)*(nvx-tbsvx)+(nvy-tbsvy)*(nvy-tbsvy)+(nvz-tbsvz)*(nvz-tbsvz))",&mva_BONSAI_NFit );
    */
}

void NTagTMVAVariables::SetBranchAddressToTree(TTree* tree)
{
    for (auto& pair: iEventVectorMap) {
        tree->SetBranchAddress(pair.first, &(pair.second));
    }

    for (auto& pair: fEventVectorMap) {
        if (!iVariableMap.count(pair.first))
            tree->SetBranchAddress(pair.first, &(pair.second));
    }
}

void NTagTMVAVariables::SetVariablesForCaptureCandidate(int iCandidate)
{
    for (const auto& pair: iEventVectorMap) {
        iVariableMap[pair.first] = pair.second->at(iCandidate);
    }
    
    for (const auto& pair: fEventVectorMap) {
        if (iVariableMap.count(pair.first))
            fVariableMap[pair.first] = iVariableMap[pair.first];
        else
            fVariableMap[pair.first] = pair.second->at(iCandidate);
    }
}

int NTagTMVAVariables::GetNumberOfCandidates()
{
    return static_cast<int>(iEventVectorMap["N10"]->size());
}