#include <iostream>

#include "ArgParser.hh"
#include "ToolChain.hh"

#include "SKRead.hh"
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
    ToolChain toolChain;
    toolChain.sharedData.ReadConfig("/disk02/usr6/han/NTag/NTagConfig");
    
    ArgParser parser(argc, argv);
    parser.OverrideStore(&(toolChain.sharedData.ntagInfo));

    std::string logFilePath;
    if (toolChain.sharedData.ntagInfo.Get("log_file_path", logFilePath))
        toolChain.SetLogFilePath(logFilePath);
        
    int verbose;
    if (toolChain.sharedData.ntagInfo.Get("verbose", verbose))
        toolChain.SetVerbosity(verbose);

    toolChain.sharedData.ntagInfo.Print();

    SKRead skRead;
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
    //toolChain.AddTool(&readHits);
    //toolChain.AddTool(&readMCInfo);
    //toolChain.AddTool(&setPromptVertex);
    //toolChain.AddTool(&subtractToF);
    //toolChain.AddTool(&searchCandidates);
    //toolChain.AddTool(&extractFeatures);
    //toolChain.AddTool(&applyTMVA);
    //toolChain.AddTool(&WriteOutput);

    toolChain.Initialize();
    while (skRead.GetReadStatus() == readOK)
        toolChain.Execute();
    toolChain.Finalize();

    std::cerr << skRead.GetCounter() << std::endl;
    
    exit(0);
}
