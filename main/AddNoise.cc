#include "SuperManager.h"
#undef MAXPM
#undef MAXPMA

#include "ArgParser.hh"
#include "Calculator.hh"
#include "PMTHitCluster.hh"
#include "NoiseManager.hh"
#include "Printer.hh"
#include "SKIO.hh"

int main(int argc, char **argv)
{
    ArgParser parser(argc, argv);
    Printer msg("AddNoise");

    // argv: (input) (output) (sk6/083920)
    if (argc < 4) {
        msg.Print("Usage: (input file) (output file) (noise_path)");
        msg.Print("Option: -start (noise start time)");
        msg.Print("Option: -end   (noise end time)");
        msg.Print("Option: -seed  (random seed for noise file selection)");
        return -1;
    }

    auto strTStart = parser.GetOption("-start"); // us
    auto strTEnd   = parser.GetOption("-end");   // us
    auto strSeed   = parser.GetOption("-seed");

    float tStart = strTStart.empty() ?  1000 /* ns */ : std::stof(strTStart);
    float tEnd   = strTEnd.empty()   ? 536e3 /* ns */ : std::stof(strTEnd);
    int   seed   = strSeed.empty()   ?     0          : std::stoi(strSeed);

    const char* inputFilePath  = argv[1];
    const char* outputFilePath = argv[2];
    const char* noiseType      = argv[3];

    // Read input MC
    SKIO inputMC = SKIO(inputFilePath, mInput);
    inputMC.OpenFile();
    int nInputEvents = inputMC.GetNumberOfEvents();
    msg.Print(Form("Input file: %s", inputFilePath));
    msg.Print(Form("Number of events in input file: %d", nInputEvents));

    // Open output MC
    SKIO outputMC = SKIO(outputFilePath, mOutput);
    outputMC.OpenFile();
    msg.Print(Form("Output file: %s", outputFilePath));

    NoiseManager noiseManager(noiseType, nInputEvents, tStart, tEnd, seed);

    // Event loop
    for (int eventID=1; eventID<=nInputEvents; eventID++) {

        msg.Print(Form("Event ID: %d", eventID));

        // Get input MC hits
        inputMC.ReadEvent(eventID);
        PMTHitCluster inputMCHits(sktqz_);

        // Append dummy hits
        noiseManager.AddNoise(&inputMCHits);
        outputMC.FillTQREAL(inputMCHits);
        outputMC.Write();
    }

    inputMC.CloseFile();
    outputMC.CloseFile();

    return 0;
}
