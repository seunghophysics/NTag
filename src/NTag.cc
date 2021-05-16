#include <iostream>

#include "ArgParser.hh"
#include "Printer.hh"
#include "ToolChain.hh"

#include "SKRead.hh"
#include "NTupleMatcher.hh"
#include "ReadHits.hh"
#include "ReadMCInfo.hh"
#include "SetPromptVertex.hh"
#include "SubtractToF.hh"
#include "SearchCandidates.hh"
#include "ExtractFeatures.hh"
#include "ApplyTMVA.hh"
#include "WriteOutput.hh"

int main(int argc, char** argv)
{
    PrintNTag();

    ToolChain toolChain;
    toolChain.sharedData.ReadConfig("/disk02/usr6/han/NTag/NTagConfig");
    toolChain.sharedData.ntagInfo.Set("ntag_commit_id", NTagVersion);
    
    ArgParser parser(argc, argv);
    parser.OverrideStore(&(toolChain.sharedData.ntagInfo));

    std::string logFilePath;
    if (toolChain.sharedData.ntagInfo.Get("log_file_path", logFilePath))
        toolChain.SetLogFilePath(logFilePath);
        
    int verbose;
    if (toolChain.sharedData.ntagInfo.Get("verbose", verbose))
        toolChain.SetVerbosity(verbose);

    SKRead skRead;
    NTupleMatcher nTupleMatcher;
    ReadHits readHits;
    ReadMCInfo readMCInfo;
    SetPromptVertex setPromptVertex;
    SubtractToF subtractToF;
    SearchCandidates searchCandidates;
    ExtractFeatures extractFeatures;
    ApplyTMVA applyTMVA;
    WriteOutput writeOutput;

    // FIFO
    toolChain.AddTool(&skRead);
    toolChain.AddTool(&nTupleMatcher);
    toolChain.AddTool(&readHits);
    toolChain.AddTool(&readMCInfo);
    toolChain.AddTool(&setPromptVertex);
    toolChain.AddTool(&subtractToF);
    toolChain.AddTool(&searchCandidates);
    toolChain.AddTool(&extractFeatures);
    toolChain.AddTool(&applyTMVA);
    toolChain.AddTool(&writeOutput);
    
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

    std::cerr << skRead.GetCounter() << std::endl;
    
    exit(0);
}