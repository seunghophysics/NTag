#include "TFile.h"
#include "TTree.h"

#include "skroot.h"
#undef MAXPM
#undef MAXPMA
#undef MAXHWSK

#include "skparmC.h"
#include "sktqC.h"

#include "ArgParser.hh"
#include "Printer.hh"
#include "SKIO.hh"
#include "SKLibs.hh"
#include "NoiseManager.hh"
#include "EventNTagManager.hh"

#include "skheadC.h"

int main(int argc, char** argv)
{
    ArgParser parser(argc, argv);
    const std::string inputFilePath = parser.GetOption("-in");
    const std::string outputFilePath = parser.GetOption("-out");
    const std::string outDataFilePath = parser.GetOption("-outdata");

    Printer msg("NTag", pDEFAULT);

    msg.Print("input file: " + inputFilePath);
    
    if (!outDataFilePath.empty()) {
        SuperManager* superManager = SuperManager::GetManager();
        superManager->CreateTreeManager(mInput, inputFilePath.data(), outDataFilePath.data(), 0);
    }

    SKIO outputMC(outDataFilePath, mOutput);

    // input MC
    SKIO inputMC = SKIO(inputFilePath, mInput);

    if (parser.GetOption("-prompt_vertex")=="stmu")
        inputMC.AddSKOption(23);

    auto skOption = parser.GetOption("-SKOPTN");
    auto skBadOpt = parser.GetOption("-SKBADOPT");
    auto skRefRun = parser.GetOption("-REFRUNNO");
    if (!skOption.empty())
        inputMC.SetSKOption(skOption.data());

    if (!skBadOpt.empty())
        inputMC.SetSKBadChOption(std::stoi(skBadOpt));

    if (!skRefRun.empty())
        inputMC.SetRefRunNo(std::stoi(skRefRun));

    EventNTagManager ntagManager;

    // output MC
    if (!outDataFilePath.empty()) {
        ntagManager.GetSettings().Set("write_bank", true);
        outputMC.OpenFile();
        ntagManager.SetOutDataFile(&outputMC);
    }

    inputMC.DumpSettings();
    inputMC.OpenFile();
    int nInputEvents = inputMC.GetNumberOfEvents();

    msg.Print("Input file: " + inputFilePath);
    msg.Print(Form("Number of events in input file: %d", nInputEvents));

    // read settings from the arguments
    // settings specified in the arguments will override the default
    ntagManager.ReadArguments(parser);
    ntagManager.ApplySettings();
    ntagManager.DumpSettings();

    NoiseManager* noiseManager = nullptr;
    if (ntagManager.GetSettings().GetBool("add_noise")) {
        std::string noiseType; ntagManager.GetSettings().Get("noise_type", noiseType);
        float tNoiseStart = ntagManager.GetSettings().GetFloat("TNOISESTART") * 1e3 + 1000;
        float tNoiseEnd   = ntagManager.GetSettings().GetFloat("TNOISEEND") * 1e3 + 1000;
        int   noiseSeed   = ntagManager.GetSettings().GetInt("NOISESEED");
        noiseManager = new NoiseManager(noiseType, nInputEvents, tNoiseStart, tNoiseEnd, noiseSeed);
        ntagManager.SetNoiseManager(noiseManager);
    }

    TFile* ntagOutFile = nullptr;
    // set output file and trees
    if (outputMC.GetFileFormat()==mSKROOT && outputMC.GetFilePath()!="") {
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
        inputMC.ReadEvent(eventID);
        ntagManager.ProcessEvent();
    }
    ntagManager.WriteTrees();
    if (ntagOutFile)  ntagOutFile->Close();
    if (noiseManager) delete noiseManager;

    return 0;
}