#include <csignal>
#include <iostream>

#include "Tool.hh"
#include "ToolChain.hh"

ToolChain::ToolChain()
: initialized(false), logger() {}
ToolChain::~ToolChain() {}

void ToolChain::AddTool(Tool* tool)
{
    tool->ConnectToToolChain(this);
    tools.push_back(tool);
}

void ToolChain::SetLogFilePath(std::string logOutPath)
{
    logger.SetOutputPath(logOutPath);
}

bool ToolChain::Initialize()
{
    if (!initialized) {
        initialized = true;

        for (auto& tool: tools) {
            bool initSuccess = tool->Initialize();
            if (!initSuccess) {
                std::cerr << "Tool " << tool->GetName()
                          << " could not initialize." << std::endl;
                initialized = false;
                break;
            }
        }

    }
    if (initialized)
        std::cout << "ToolChain successfully initialized." << std::endl;
    else
        std::cerr << "ToolChain initialization failed." << std::endl;
    return initialized;
}

bool ToolChain::Execute(unsigned long nEvents)
{
    if (initialized) {
        for (unsigned long iEvent=0; iEvent<nEvents; iEvent++) {
            for (auto& tool: tools) {
                try {
                    tool->CheckSafetyAndExecute();
                } catch (...) {
                    logger.Log("Stopping execution and finalizing...\n");
                    goto endExecution;
                }
                std::cout << "\n";
            }
        }
    endExecution:
        std::cout << "\n" << std::endl;
        return true;
    }
    else {
        std::cerr << "ToolChain did not successfully initialize. Skipping execution..." << std::endl;
        return false;
    }
}

bool ToolChain::Finalize()
{
    for (auto& tool: tools) {
        tool->Finalize();
    }
    std::cout << "ToolChain finalized." << std::endl;
    return true;
}