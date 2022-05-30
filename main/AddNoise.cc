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

    inputMC.SetSKGeometry(settings.GetInt("SKGEOMETRY"));
    inputMC.SetSKOption(settings.GetString("SKOPTN"));
    inputMC.SetSKBadChOption(settings.GetInt("SKBADOPT"));
    inputMC.SetRefRunNo(settings.GetInt("REFRUNNO"));

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
    noiseManager.SetNoisePath(settings.GetString("noise_path"));
    noiseManager.GenerateNoiseTree(settings.GetString("noise_type").c_str(), nInputEvents,
                                   settings.GetFloat("TNOISESTART"), settings.GetFloat("TNOISEEND"), settings.GetInt("NOISESEED"));
    if (settings.GetBool("debug", false))
        noiseManager.SetVerbosity(pDEBUG);
    noiseManager.SetRepeat(settings.GetBool("repeat_noise", false));
    noiseManager.DumpSettings();

    SoftwareTrigManager softwareTrg(settings.GetInt("REFRUNNO"));
  

    // Event loop
    for (int eventID=1; eventID<=nInputEvents; eventID++) {
        msg.Print(Form("Processing event #%d...\x1b[A\r", eventID));

        // Get input MC hits
        inputMC.ReadEvent(eventID);
        PMTHitCluster inputMCHits(sktqz_);

        // Append dummy hits
        noiseManager.AddNoise(&inputMCHits);

        // Apply software trigger, if needed
        if (settings.GetBool("apply_softwaretrig", false)) {
          softwareTrg.ApplyTrigger(&inputMCHits);
          outputMC.FillHEADER(softwareTrg);
        }

        outputMC.FillTQREAL(inputMCHits);
        outputMC.FillTree(inputMCHits);
        outputMC.Write();
    }

    inputMC.CloseFile();
    outputMC.CloseFile();

    std::cout << "\n";
    msg.Print(Form("Noise addition done!"));
    msg.Print(Form("Output: %s", outputFilePath.c_str()));

    return 0;
}
