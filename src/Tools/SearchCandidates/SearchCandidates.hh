#ifndef SEARCHCANDIDATES_HH
#define SEARCHCANDIDATES_HH

#include "Tool.hh"

class SearchCandidates : public Tool
{
    public:
        SearchCandidates() { name = "SearchCandidates"; }
    
        bool Initialize();
        bool Execute();
        bool Finalize();
        
        bool CheckSafety();
        
    private:
        TVector3 promptVertex;
        float T0TH, T0MX, TWIDTH, TMINPEAKSEP;
        int   NHITSTH, NHITSMX, N200TH, N200MX;
};

#endif