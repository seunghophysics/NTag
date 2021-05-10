#ifndef SUBTRACTTOF_HH
#define SUBTRACTTOF_HH

#include <TVector3.h>

#include "Tool.hh"

class SubtractToF : public Tool
{
    public:
        SubtractToF() { name = "SubtractToF"; }

        bool Initialize();
        bool Execute();
        bool Finalize();
};

#endif