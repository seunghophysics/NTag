#include "ToolChain.hh"

#include "Tool.hh"

void Tool::ConnectToToolChain(ToolChain* toolChain)
{
    eventData = &(toolChain->eventData);
    logger = &(toolChain->logger);
}