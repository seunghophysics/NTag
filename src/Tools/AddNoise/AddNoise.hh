#ifndef ADDNOISE_HH
#define ADDNOISE_HH

#include "Tool.hh"

class AddNoise : public Tool
{
    public:
        AddNoise() { name = "AddNoise"; }
        
        bool Initialize();
        bool Execute();
        bool Finalize();
        
        bool CheckSafety();
};

#endif