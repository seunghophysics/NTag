#ifndef TOOLCHAIN_HH
#define TOOLCHAIN_HH

#include <vector>

#include "EventData.hh"
#include "Logger.hh"

class Tool;

class ToolChain
{
    public:
        ToolChain();
        ~ToolChain();
        
        void AddTool(Tool* tool);
        void SetLogOutputPath(std::string logOutPath);
        
        bool Initialize();
        bool Execute(int nEvents=1);
        bool Finalize();
        
        EventData eventData;
        Logger logger;
        
    private:
        bool initialized;

        std::vector<Tool*> tools;     
};

#endif