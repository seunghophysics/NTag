#include "TFile.h"
#include "TTree.h"

#include "skroot.h"
#undef MAXPM
#undef MAXPMA
#undef MAXHWSK
#undef SECMAXRNG

#include "skparmC.h"
#include "sktqC.h"
#include "skheadC.h"

#include "ArgParser.hh"
#include "Printer.hh"
#include "SKIO.hh"
#include "SKLibs.hh"
#include "NoiseManager.hh"
#include "EventNTagManager.hh"
#include "git.h"

void PrintNTag();
void PrintVersion();

int main(int argc, char** argv)
{
    PrintNTag();

    ArgParser parser(argc, argv);

    if (parser.OptionExists("-debug"))
        SKIO::SetVerbose(true);

    EventNTagManager ntagManager;
    Printer msg("NTag", pDEFAULT);

    const std::string inputFilePath = parser.GetOption("-in");
    const std::string outputFilePath = parser.GetOption("-out");
    const std::string outDataFilePath = parser.GetOption("-outdata");
    const std::string macroPath = parser.GetOption("-macro");

    if (parser.OptionExists("-mode")) {
        auto mode = parser.GetOption("-mode");
        auto cardPath = GetENV("NTAGLIBPATH") + std::string("card/") + mode + std::string(".card");
        if (DoesExist(cardPath)) parser.ReadFile(cardPath);
        else msg.Print("Wrong input mode: " + parser.GetOption("-mode"), pERROR);
    }

    // read macro and override by arguments
    if (parser.OptionExists("-macro"))
        parser.ReadFile(parser.GetOption("-macro"));

    ntagManager.ReadArguments(parser);
    Store& settings = ntagManager.GetSettings();
    settings.Set("commit", gitcommit);
    settings.Set("tag", gittag);

    // delayed mode check
    if (!settings.HasKey("delayed_vertex")) {
        msg.Print(Form("No delayed vertex option specified, "
                       "use BONSAI for delayed signal vertex reconstruction by default..."), pWARNING);
        settings.Set("delayed_vertex", "bonsai");
    }

    // prompt vertex check
    if (!settings.HasKey("prompt_vertex")) {

        if (settings.GetString("delayed_vertex")=="prompt") {
            msg.Print("No prompt vertex option specified for delayed vertex mode \"prompt\"!", pERROR);
        }
        else {
            msg.Print(Form("No prompt vertex option specified. NOT correcting photon ToF in signal search by default..."), pWARNING);
            settings.Set("prompt_vertex", "none");
            settings.Set("correct_tof", "false");
        }
    }

    if (!outDataFilePath.empty()) {
        SuperManager* superManager = SuperManager::GetManager();
        superManager->CreateTreeManager(mInput, inputFilePath.data(), outDataFilePath.data(), 0);
    }
    SKIO output(outDataFilePath, mOutput);
    SKIO input(inputFilePath, mInput);

    // SKIO settings
    //SKIO::SetSKGeometry(settings.GetInt("SKGEOMETRY"));
    // don't do bad channel mask at this tage
    //auto skOptionNoBad = TString(settings.GetString("SKOPTN")).ReplaceAll(",25", "");
    //SKIO::SetSKOption(skOptionNoBad.Data());
    SKIO::SetSKOption(settings.GetString("SKOPTN"));
    SKIO::SetSKBadChOption(settings.GetInt("SKBADOPT"));
    SKIO::SetRefRunNo(settings.GetInt("REFRUNNO"));

    if (parser.GetOption("-prompt_vertex")=="stmu") {
        input.AddSKOption(23);
        settings.Set("SKOPTN", input.GetSKOption());
    }

    // output MC
    if (!outDataFilePath.empty()) {
        settings.Set("write_bank", true);
        output.OpenFile();
        ntagManager.SetOutDataFile(&output);
    }

    input.OpenFile();
    int nInputEvents = input.GetNumberOfEvents();
    input.DumpSettings();

    msg.Print("Input file: " + inputFilePath);
    msg.Print(Form("Number of events in input file: %d", nInputEvents));

    // NTagManager reads settings from the arguments
    // Settings specified in the arguments will override the default
    //ntagManager.ReadArguments(parser);
    ntagManager.ApplySettings();
    ntagManager.DumpSettings();

    // read noise settings
    NoiseManager* noiseManager = nullptr;
    if (settings.GetBool("add_noise", false)) {
        noiseManager = new NoiseManager;
        noiseManager->ApplySettings(settings, nInputEvents);

        // PMT deadtime will be covered in EventNTagManager,
        // so override PMT deadtime in noiseManager with zero for now
        noiseManager->SetPMTDeadtime(0);
        ntagManager.SetNoiseManager(noiseManager);
    }

    // set output file and trees
    TFile* ntagOutFile = nullptr;
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
        ntagManager.MakeTrees(ntagOutFile);
    }

    // event loop
    int eventStart = parser.OptionExists("-event_start")? std::stoi(parser.GetOption("-event_start")) : 1;

    for (int eventID=eventStart; eventID<=nInputEvents; eventID++) {
        std::cout << "\n"; msg.Print(Form("Processing Event #%d / %d...", eventID, nInputEvents));
        input.ReadEvent(eventID);
        ntagManager.ProcessEvent();
    }

    // just in case the final data event was SHE without AFT
    if (!ntagManager.GetHits().IsEmpty())
        ntagManager.SearchAndFill();

    // save output and exit
    ntagManager.WriteTrees();
    if (ntagOutFile)  ntagOutFile->Close();
    if (noiseManager) delete noiseManager;

    return 0;
}

void PrintNTag()
{
    std::cout << "\n" <<std::endl;
    std::cout << "              _____________________________________"       << std::endl;
    std::cout << "                _   _     _____      _       ____  "       << std::endl;
    std::cout << "               | \\ |\"|   |_ \" _| U  /\"\\  uU /\"___|u" << std::endl;
    std::cout << "              <|  \\| |>    | |    \\/ _ \\/ \\| |  _ /"   << std::endl;
    std::cout << "              U| |\\  |u   /| |\\   / ___ \\  | |_| | "    << std::endl;
    std::cout << "               |_| \\_|   u |_|U  /_/   \\_\\  \\____| "   << std::endl;
    std::cout << "               ||   \\\\,-._// \\\\_  \\\\    >>  _)(|_  " << std::endl;
    std::cout << "               (_\")  (_/(__) (__)(__)  (__)(__)__) "      << std::endl;
    std::cout << "              _____________________________________"       << std::endl;

    PrintVersion();
}

void PrintVersion()
{
    std::string commit(gitcommit);
    std::cout << "              NTag version " << gittag << " (" << commit.substr(0,6) << ")\n"
              << "              Last updated " << gitdate << "\n\n" << std::endl;
}