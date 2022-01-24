#include <iostream>

#include "NTagTMVAManager.hh"
#include "ArgParser.hh"
#include "Printer.hh"

int main(int argc, char** argv)
{
    ArgParser parser(argc, argv);
    const std::string inputFilePath = parser.GetOption("-in");
    const std::string outputFilePath = parser.GetOption("-out");
    const std::string outDirPath = parser.GetOption("-dir");

    Printer msg("NTagTrain", pDEFAULT);

    msg.Print("Input file: " + inputFilePath);

    NTagTMVAManager manager;
    manager.SetMethods(true);
    manager.TrainWeights(inputFilePath.data(), outputFilePath.data(), outDirPath.data());

    return 0;
}