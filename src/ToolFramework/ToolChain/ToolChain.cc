#include <iostream>

#include "Tool.hh"
#include "ToolChain.hh"

ToolChain::ToolChain() 
: initialized(false) {}
ToolChain::~ToolChain() {}

void ToolChain::AddTool(Tool* tool) 
{
    tool->ConnectEventData(eventData);
    tools.push_back(tool);
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

bool ToolChain::Execute(int nEvents)
{
    if (initialized) {
        for (int iEvent=0; iEvent<nEvents; iEvent++) {
            for (auto& tool: tools) {
                tool->Execute();
            }
        }
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