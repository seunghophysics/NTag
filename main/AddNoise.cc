#include <skheadC.h>
#undef MAXHWSK
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
    inputMC.SetSKOption(settings.GetString("SKOPTN").c_str()); // M.Harada
    inputMC.SetSKBadChOption(settings.GetInt("SKBADOPT"));// M.Harada
    inputMC.SetRefRunNo(settings.GetInt("REFRUNNO"));// M.Harada
    inputMC.OpenFile();
    auto nInputEvents = inputMC.GetNumberOfEvents();

    // Open output MC
    SKIO outputMC = SKIO(outputFilePath, mOutput);
    outputMC.SetSKOption(settings.GetString("SKOPTN").c_str()); // M.Harada
    outputMC.SetSKBadChOption(settings.GetInt("SKBADOPT"));// M.Harada
    outputMC.SetRefRunNo(settings.GetInt("REFRUNNO"));// M.Harada
    outputMC.OpenFile();

    msg.Print(Form("Input file: %s", inputFilePath.c_str()));
    msg.Print(Form("Number of events in input file: %d", nInputEvents));
    msg.Print(Form("Output file: %s", outputFilePath.c_str()));

    NoiseManager noiseManager(settings.GetString("noise_type").c_str(), nInputEvents,
                              settings.GetFloat("TNOISESTART"), settings.GetFloat("TNOISEEND"), settings.GetInt("NOISESEED"));
    noiseManager.DumpSettings();
    if (settings.GetBool("debug", false))
        noiseManager.SetVerbosity(pDEBUG);

    // Event loop
    for (int eventID=1; eventID<=nInputEvents; eventID++) {
        msg.Print(Form("Processing event #%d...\x1b[A\r", eventID));

        // Get input MC hits
        inputMC.ReadEvent(eventID);
        if (!(skhead_.idtgsk & (1<<28))) continue;// Added by M.Harada
        std::cout << sktqz_.nqiskz<<std::endl;
        PMTHitCluster inputMCHits(sktqz_);

        // Append dummy hits
        noiseManager.AddNoise(&inputMCHits);
        outputMC.FillTQREAL(inputMCHits);
        outputMC.Write();
    }

    inputMC.CloseFile();
    outputMC.CloseFile();

    std::cout << "\n";
    msg.Print(Form("Noise addition done!"));
    msg.Print(Form("Output: %s", outputFilePath.c_str()));

    return 0;
}
