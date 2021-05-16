#include <iostream>

#include "ArgParser.hh"
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

extern const char* gitcommit;
extern const char* gitcommitdate;
static std::string NTagVersion = std::string("\"") + std::string(gitcommit) + std::string("\"");
static std::string NTagDate    = gitcommitdate;

void PrintNTag();
void PrintVersion();

int main(int argc, char** argv)
{
    PrintNTag();

    ToolChain toolChain;
    toolChain.sharedData.ReadConfig("/disk02/usr6/han/NTag/NTagConfig");
    toolChain.sharedData.ntagInfo.Set("CommitID", NTagVersion);
    
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
    //toolChain.AddTool(&applyTMVA);
    toolChain.AddTool(&writeOutput);

    toolChain.Initialize();
    while (skRead.GetReadStatus() == readOK) {
        try { toolChain.Execute(); } 
        catch (...) { break; }
    }
    toolChain.Finalize();

    std::cerr << skRead.GetCounter() << std::endl;
    
    exit(0);
}

void PrintNTag()
{
    std::cout << "\n\n";
    std::cout << "              _____________________________________\n"       ;
    std::cout << "                _   _     _____      _       ____  \n"       ;
    std::cout << "               | \\ |\"|   |_ \" _| U  /\"\\  uU /\"___|u\n" ;
    std::cout << "              <|  \\| |>    | |    \\/ _ \\/ \\| |  _ /\n"   ;
    std::cout << "              U| |\\  |u   /| |\\   / ___ \\  | |_| | \n"    ;
    std::cout << "               |_| \\_|   u |_|U  /_/   \\_\\  \\____| \n"   ;
    std::cout << "               ||   \\\\,-._// \\\\_  \\\\    >>  _)(|_  \n" ;
    std::cout << "               (_\")  (_/(__) (__)(__)  (__)(__)__) \n"      ;
    std::cout << "              _____________________________________\n"       ;

    PrintVersion();
}

void PrintVersion()
{
    std::cout << "                Commit ID: " << NTagVersion.substr(1, 11)
              // "              _____________________________________\n"
              << "\n                (" << NTagDate << ")\n\n" << std::endl;
}