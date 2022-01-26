#include "TFile.h"
#include "TTree.h"

#include "skroot.h"
#undef MAXPM
#undef MAXPMA
#undef MAXHWSK

#include "skparmC.h"
#include "sktqC.h"
#include "skheadC.h"

#include "ArgParser.hh"
#include "Printer.hh"
#include "SKIO.hh"
#include "SKLibs.hh"
#include "NoiseManager.hh"
#include "EventNTagManager.hh"

int main(int argc, char** argv)
{
    ArgParser parser(argc, argv);
    EventNTagManager ntagManager;
    ntagManager.ReadArguments(parser);
    Store& settings = ntagManager.GetSettings();

    const std::string inputFilePath = parser.GetOption("-in");
    const std::string outputFilePath = parser.GetOption("-out");
    const std::string outDataFilePath = parser.GetOption("-outdata");

    Printer msg("NTag", pDEFAULT);

    if (!outDataFilePath.empty()) {
        SuperManager* superManager = SuperManager::GetManager();
        superManager->CreateTreeManager(mInput, inputFilePath.data(), outDataFilePath.data(), 0);
    }
    SKIO output(outDataFilePath, mOutput);
    SKIO input(inputFilePath, mInput);

    // SKIO settings
    if (parser.GetOption("-prompt_vertex")=="stmu")
        input.AddSKOption(23);

    input.SetSKOption(settings.GetString("SKOPTN").c_str());
    input.SetSKBadChOption(settings.GetInt("SKBADOPT"));
    input.SetRefRunNo(settings.GetInt("REFRUNNO"));

    // output MC
    if (!outDataFilePath.empty()) {
        settings.Set("write_bank", true);
        output.OpenFile();
        ntagManager.SetOutDataFile(&output);
    }

    input.DumpSettings();
    input.OpenFile();
    int nInputEvents = input.GetNumberOfEvents();

    msg.Print("Input file: " + inputFilePath);
    msg.Print(Form("Number of events in input file: %d", nInputEvents));

    // NTagManager reads settings from the arguments
    // Settings specified in the arguments will override the default
    ntagManager.ReadArguments(parser);
    ntagManager.ApplySettings();
    ntagManager.DumpSettings();

    NoiseManager* noiseManager = nullptr;
    if (settings.GetBool("add_noise", false)) {
        std::string noiseType; settings.Get("noise_type", noiseType);
        float tNoiseStart = settings.GetFloat("TNOISESTART");
        float tNoiseEnd   = settings.GetFloat("TNOISEEND");
        int   noiseSeed   = settings.GetInt("NOISESEED");
        noiseManager = new NoiseManager(noiseType, nInputEvents, tNoiseStart, tNoiseEnd, noiseSeed);
        noiseManager->DumpSettings();
        ntagManager.SetNoiseManager(noiseManager);
    }

    TFile* ntagOutFile = nullptr;
    // set output file and trees
    if (output.GetFileFormat()==mSKROOT && output.GetFilePath()!="") {
        int lun = 10;
        TreeManager* mgr = skroot_get_mgr(&lun);
        TFile* outFile = mgr->GetOTree()->GetCurrentFile();
        ntagManager.MakeTrees(outFile);
    }
    else {
        if (outputFilePath.empty())
            msg.Print("Output file path is empty! Please specify it with -out option.", pERROR);
        ntagOutFile = new TFile(outputFilePath.c_str(), "recreate");
        ntagManager.MakeTrees();
    }

    // event loop
    for (int eventID=1; eventID<=nInputEvents; eventID++) {
        std::cout << "\n"; msg.Print(Form("Processing Event #%d / %d...", eventID, nInputEvents));
        input.ReadEvent(eventID);
        ntagManager.ProcessEvent();
    }
    ntagManager.WriteTrees();
    if (ntagOutFile)  ntagOutFile->Close();
    if (noiseManager) delete noiseManager;

    return 0;
}