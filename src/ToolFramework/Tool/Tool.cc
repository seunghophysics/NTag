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
    if (safeToExecute || CheckSafety()) {
        Execute(); IncrementCounter();
        return true;
    }
    else return false;
}