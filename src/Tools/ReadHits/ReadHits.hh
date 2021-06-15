#ifndef READHITS_HH
#define READHITS_HH

#include "Tool.hh"

enum TriggerType
{
    tELSE,
    tSHE,
    tAFT
};

class ReadHits : public Tool
{
    public:
        ReadHits(): prevEvTrigType(tELSE), currentEvTrigType(tELSE), isPrevEvProcessed(true)
        { name = "ReadHits"; }

        bool Initialize();
        bool Execute();
        bool Finalize();

        bool CheckSafety();
        
    private:
        void SetTriggerType();
        TriggerType prevEvTrigType;
        TriggerType currentEvTrigType;
        bool isPrevEvProcessed;

};

#endif