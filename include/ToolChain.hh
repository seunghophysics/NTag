#ifndef TOOLCHAIN_HH
#define TOOLCHAIN_HH

#include <vector>

#include "DataModel.hh"
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
        bool Execute(unsigned long nEvents=1);
        bool Finalize();

        DataModel sharedData;
        Logger logger;

    private:
        bool initialized;
        std::vector<Tool*> tools;
};

#endif