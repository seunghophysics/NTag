#include "ToolChain.hh"

#include "Tool.hh"

bool Tool::inputIsMC = false;

void Tool::ConnectToToolChain(ToolChain* toolChain)
{
    sharedData = &(toolChain->sharedData);
    logger = &(toolChain->logger);
}

bool Tool::CheckSafetyAndExecute()
{
    if (safeToExecute) {
        Execute(); IncrementCounter();
        return true;
    }
    else {
        try {
            safeToExecute = CheckSafety();
        } catch (...) {
            
        }
        if (safeToExecute)
            CheckSafetyAndExecute();
        else
            throw eSKIPEVENT;
    }
}