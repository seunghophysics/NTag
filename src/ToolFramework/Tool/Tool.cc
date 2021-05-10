#include "ToolChain.hh"

#include "Tool.hh"

void Tool::ConnectToToolChain(ToolChain* toolChain)
{
    sharedData = &(toolChain->sharedData);
    logger = &(toolChain->logger);
}

bool Tool::CheckSafetyAndExecute()
{
    if (safeToExecute)
        Execute();
    else if (CheckSafety())
        Execute();
}