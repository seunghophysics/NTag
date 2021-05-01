#ifndef READHITS_HH
#define READHITS_HH

#include "Tool.hh"

class ReadHits : public Tool
{
    public:
        ReadHits() { name = "ReadHits"; }
        
        bool Initialize();
        bool Execute();
        bool Finalize();
};

#endif