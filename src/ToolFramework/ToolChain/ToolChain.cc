#include <csignal>
#include <iostream>
#include <iomanip>

#include "Printer.hh"

#include "Tool.hh"
#include "ToolChain.hh"

ToolChain::ToolChain()
: initialized(false), logger() {}
ToolChain::~ToolChain()
{
    for (auto& tool: tools) {
        delete tool;
    }
}

void ToolChain::AddTool(Tool* tool)
{
    tool->ConnectToToolChain(this);
    tools.push_back(tool);
}

void ToolChain::PrintAllTools()
{
    PrintBlock("Connected tools");

    int i = 0;
    for (auto& tool: tools) {
        i++;
        std::cout << std::left << std::setw(6) << Form("[%d] ", i) << ": " << tool->GetName() << "\n";
    }
    std::cout << std::endl;
}

void ToolChain::SetLogFilePath(std::string logOutPath)
{
    logger.SetOutputPath(logOutPath);
}

void ToolChain::SetVerbosity(int verbose)
{
    if (pNONE <= verbose && verbose <= pDEBUG)
        logger.SetVerbosity(static_cast<Verbosity>(verbose));
    else {
        std::cerr << "Input verbosity " << verbose << "is out of verbosity range." << std::endl;
        std::cerr << "Supported verbosity range: " << pNONE <<  " - " << pDEBUG << std::endl; 
        std::cerr << "Setting verbosity to default." << std::endl;
    }
}

bool ToolChain::Initialize()
{
    if (!initialized) {
        initialized = true;
        PrintBlock("Initializing toolchain...");

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
        PrintBlock("ToolChain initialized.");
    else
        PrintBlock("ToolChain initialization failed.");
    return initialized;
}

bool ToolChain::Execute(unsigned long nEvents)
{
    if (initialized) {
        PrintBlock("Executing toolchain...");
        for (unsigned long iEvent=0; iEvent<nEvents; iEvent++) {
            for (auto& tool: tools) {
                try {
                    tool->CheckSafetyAndExecute();
                } catch (ExceptionBehavior& e) {
                    if (e == eSKIPEVENT) {
                        logger.Log("Skipping this event...\n");
                        goto skipEvent;
                    }
                    else if (e == eENDRUN) {
                        logger.Log("Ending the run and finalizing the toolchain...\n");
                        throw e;
                    }
                }
                std::cout << "\n";
            }
        }
    skipEvent:
        std::cout << std::endl;
        return true;
    }
    else {
        std::cerr << "ToolChain did not successfully initialize. Skipping execution..." << std::endl;
        return false;
    }
}

bool ToolChain::Finalize()
{
    PrintBlock("Finalizing toolchain...");
    for (auto& tool: tools) {
        tool->Finalize();
    }
    std::cout << "ToolChain finalized." << std::endl;
    return true;
}