#ifndef SKREAD_HH
#define SKREAD_HH

#include "Tool.hh"

class SKRead : public Tool
{
    public:
        SKRead() { name = "SKRead"; }
        
        bool Initialize();
        bool Execute();
        bool Finalize();
};

#endif