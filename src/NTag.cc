#include <iostream>

#include "ArgParser.hh"
#include "PathGetter.hh"
#include "Printer.hh"
#include "ToolChain.hh"

#include "SKRead.hh"
#include "NTupleMatcher.hh"
#include "ReadHits.hh"
#include "AddNoise.hh"
#include "ReadMCInfo.hh"
#include "SetPromptVertex.hh"
#include "SubtractToF.hh"
#include "SearchCandidates.hh"
#include "ExtractFeatures.hh"
#include "ApplyTMVA.hh"
#include "WriteOutput.hh"

static std::string installPath = GetENV("NTAGPATH");

int main(int argc, char** argv)
{
    PrintNTag();
    
    if (GetCWD() != installPath) {
        PrintBlock("NTAGPATH");
        std::cout << installPath << std::endl;
    }

    ToolChain toolChain;
    toolChain.sharedData.ReadConfig(installPath + "NTagConfig");
    toolChain.sharedData.ntagInfo.Set("ntag_commit_id", NTagVersion);
    
    ArgParser parser(argc, argv);
    parser.OverrideStore(&(toolChain.sharedData.ntagInfo));

    std::string logFilePath;
    if (toolChain.sharedData.ntagInfo.Get("log_file_path", logFilePath))
        toolChain.SetLogFilePath(logFilePath);
        
    int verbose = pDEFAULT;
    if (toolChain.sharedData.ntagInfo.Get("verbose", verbose))
        toolChain.SetVerbosity(verbose);

    std::map<std::string, Tool*> toolMap;
    
    SKRead skRead;                     toolMap[skRead.GetName()] = &skRead;
    NTupleMatcher nTupleMatcher;       toolMap[nTupleMatcher.GetName()] = &nTupleMatcher;
    ReadHits readHits;                 toolMap[readHits.GetName()] = &readHits;
    AddNoise addNoise;                 toolMap[addNoise.GetName()] = &addNoise;
    ReadMCInfo readMCInfo;             toolMap[readMCInfo.GetName()] = &readMCInfo;
    SetPromptVertex setPromptVertex;   toolMap[setPromptVertex.GetName()] = &setPromptVertex;
    SubtractToF subtractToF;           toolMap[subtractToF.GetName()] = &subtractToF;
    SearchCandidates searchCandidates; toolMap[searchCandidates.GetName()] = &searchCandidates;
    ExtractFeatures extractFeatures;   toolMap[extractFeatures.GetName()] = &extractFeatures;
    ApplyTMVA applyTMVA;               toolMap[applyTMVA.GetName()] = &applyTMVA;
    WriteOutput writeOutput;           toolMap[writeOutput.GetName()] = &writeOutput;
    
    Store toolList;
    std::string toolsListFilePath = installPath + "NTagToolsList";
    toolChain.sharedData.ntagInfo.Get("tools_list_path", toolsListFilePath);
    toolList.Initialize(toolsListFilePath);
    
    for (auto& pair: toolList.GetMap()) {
        if (toolMap.count(pair.second)) {
            toolChain.AddTool(toolMap[pair.second]);
        }
        else {
            PrintBlock("Error: " + toolsListFilePath);
            std::cerr << "Tool " << pair.second << " not found! Skipping this tool..." << std::endl;
            std::cerr << "Please specify the correct name for the tool!" << std::endl;   
        }
    }
    
    if (verbose >= pDEFAULT) {
        toolChain.sharedData.ntagInfo.Print();
        toolChain.PrintAllTools();
    }

    toolChain.Initialize();
    while (skRead.GetReadStatus() == readOK) {
        try { toolChain.Execute(); } 
        catch (...) { break; }
    }
    toolChain.Finalize();
    
    exit(0);
}