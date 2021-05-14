#include <iostream>

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

int main()
{
    ToolChain toolChain;
    toolChain.sharedData.ReadConfig("NTagConfig");

    std::string logFilePath;
    if (toolChain.sharedData.ntagInfo.Get("log_file_path", logFilePath))
        toolChain.SetLogFilePath(logFilePath);

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
    toolChain.AddTool(&readHits);
    toolChain.AddTool(&readMCInfo);
    toolChain.AddTool(&setPromptVertex);
    toolChain.AddTool(&subtractToF);
    toolChain.AddTool(&searchCandidates);
    toolChain.AddTool(&extractFeatures);
    toolChain.AddTool(&applyTMVA);
    toolChain.AddTool(&outputWriter);

    toolChain.Initialize();
    while (skRead.GetReadStatus() == readOK)
        toolChain.Execute();
    toolChain.Finalize();

    exit(0);
}
