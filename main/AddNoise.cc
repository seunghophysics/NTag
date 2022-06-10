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
    Printer msg("AddNoise");
    Store settings;

    if (!GetENV("NTAGLIBPATH").empty())
        settings.Initialize(GetENV("NTAGLIBPATH")+"/NTagConfig");
    settings.ReadArguments(parser);
    settings.Print();

    auto inputFilePath = settings.GetString("in");
    auto outputFilePath = settings.GetString("out");

    // Read input MC
    SKIO inputMC = SKIO(inputFilePath, mInput);

    SKIO::SetSKGeometry(settings.GetInt("SKGEOMETRY"));
    SKIO::SetSKOption("31,30");
    SKIO::SetSKBadChOption(0);
    SKIO::SetRefRunNo(settings.GetInt("REFRUNNO"));

    inputMC.OpenFile();
    auto nInputEvents = inputMC.GetNumberOfEvents();

    // Open output MC
    SKIO outputMC = SKIO(outputFilePath, mOutput);
    outputMC.OpenFile();
    outputMC.DumpSettings();

    msg.Print(Form("Input file: %s", inputFilePath.c_str()));
    msg.Print(Form("Number of events in input file: %d", nInputEvents));
    msg.Print(Form("Output file: %s", outputFilePath.c_str()));

    NoiseManager noiseManager;
    noiseManager.ApplySettings(settings, nInputEvents);

    // Event loop
    for (int eventID=1; eventID<=nInputEvents; eventID++) {
        msg.Print(Form("Processing event #%d...\x1b[A\r", eventID));

        // Get input MC hits
        inputMC.ReadEvent(eventID);
        PMTHitCluster inputMCIDHits(sktqz_);
        PMTHitCluster inputMCODHits(sktqaz_);

        // Append dummy hits
        noiseManager.AddIDNoise(&inputMCIDHits);
        if (settings.GetBool("add_noise_OD", false))
            noiseManager.AddODNoise(&inputMCODHits);

        inputMCIDHits.Append(inputMCODHits);
        auto& outputHits = inputMCIDHits;

        outputMC.FillTQREAL(outputHits);
        outputMC.Write();
    }

    inputMC.CloseFile();
    outputMC.CloseFile();

    std::cout << "\n";
    msg.Print(Form("Noise addition done!"));
    msg.Print(Form("Output: %s", outputFilePath.c_str()));

    return 0;
}