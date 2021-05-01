#ifndef TOOLCHAIN_HH
#define TOOLCHAIN_HH

#include <vector>

#include "EventData.hh"

class Tool;

class ToolChain
{
    public:
        ToolChain();
        ~ToolChain();
        
        void AddTool(Tool* tool);
        bool Initialize();
        bool Execute(int nEvents=1);
        bool Finalize();
        
        EventData eventData;
        
    private:
        std::vector<Tool*> tools;
        
        bool initialized;
};

#endif