#include "SuperManager.h"
#undef MAXPM
#undef MAXPMA

#include "ArgParser.hh"
#include "Calculator.hh"
#include "PMTHitCluster.hh"
#include "NoiseManager.hh"
#include "Printer.hh"
#include "Store.hh"
#include "SKIO.hh"

int main(int argc, char **argv)
{
    ArgParser parser(argc, argv);
    Store settings;
    Printer msg("AddNoise");
    settings.Initialize(GetENV("NTAGLIBPATH")+"/NTagConfig");
    settings.ReadArguments(parser);
    settings.Print();

    auto inputFilePath = settings.GetString("in");
    auto outputFilePath = settings.GetString("out");

    // Read input MC
    SKIO inputMC = SKIO(inputFilePath, mInput);
    inputMC.OpenFile();
    int nInputEvents = inputMC.GetNumberOfEvents();

    // Open output MC
    SKIO outputMC = SKIO(outputFilePath, mOutput);
    outputMC.OpenFile();

    msg.Print(Form("Input file: %s", inputFilePath.c_str()));
    msg.Print(Form("Number of events in input file: %d", nInputEvents));
    msg.Print(Form("Output file: %s", outputFilePath.c_str()));

    NoiseManager noiseManager(settings.GetString("noise_type").c_str(), nInputEvents,
                              settings.GetFloat("TNOISESTART"), settings.GetFloat("TNOISEEND"), settings.GetInt("NOISESEED"));
    noiseManager.DumpSettings();

    // Event loop
    for (int eventID=1; eventID<=nInputEvents; eventID++) {

        msg.Print(Form("Event ID: %d\r", eventID));

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