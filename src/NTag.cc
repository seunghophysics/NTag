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
#include "OutputWriter.hh"

int main(int argc, char** argv)
{
    ToolChain toolChain;
    toolChain.sharedData.ReadConfig("NTagConfig");
    
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
    OutputWriter outputWriter;

    // FIFO
    toolChain.AddTool(&skRead);
    //toolChain.AddTool(&readHits);
    //toolChain.AddTool(&readMCInfo);
    //toolChain.AddTool(&setPromptVertex);
    //toolChain.AddTool(&subtractToF);
    //toolChain.AddTool(&searchCandidates);
    //toolChain.AddTool(&extractFeatures);
    //toolChain.AddTool(&applyTMVA);
    //toolChain.AddTool(&outputWriter);

    toolChain.Initialize();
    while (skRead.GetReadStatus() == readOK)
        toolChain.Execute();
    toolChain.Finalize();

    std::cout << "skread execounter: " << skRead.GetCounter() << std::endl;
    exit(0);
}
