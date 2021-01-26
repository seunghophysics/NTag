#include <string.h>
#include <iostream>

#include <TROOT.h>
#include <TString.h>

#include "NTagPath.hh"
#include "NTagROOT.hh"
#include "NTagZBS.hh"
#include "NTagTMVA.hh"
#include "NTagArgParser.hh"
#include "NTagMessage.hh"
#include "NTagZBSTQReader.hh"
#include "apmringC.h"

static std::string NTagVersion = "0.0.1";
static std::string NTagDate    = "Dec 7, 2020";
void PrintNTag();
void PrintVersion();

void ProcessSKFile(NTagIO* nt, NTagArgParser& parser);

static std::string inputName, outputName, weightName, methodName;
static std::string installPath = GetENV("NTAGPATH");
static std::string vx, vy, vz;

int main(int argc, char** argv)
{
    PrintNTag();

    /********************/
    /* Argument parsing */
    /********************/

    NTagArgParser parser(argc, argv);

    // Names
    inputName     = parser.GetOption("-in");
    outputName    = parser.GetOption("-out");
    weightName    = parser.GetOption("-weight");
    methodName    = parser.GetOption("-method");

    // Verbosity
    Verbosity pVERBOSE = pDEFAULT;
    if (parser.OptionExists("-debug")) {
        pVERBOSE = pDEBUG;
    }

    NTagMessage msg("", pVERBOSE);

    // Choose between default name and optional name

    if (GetCWD() != installPath)
        msg.Print(Form("Using NTag in $NTAGPATH: ") + installPath);

    if (inputName.empty())  msg.Print("Please specify input file name: NTag -in [input file] ...", pERROR);
    if (weightName.empty()) weightName = installPath + "weights/MLP_Gd0.02p.xml";
    if (methodName.empty()) methodName = "MLP";

    /********************/
    /* Main application */
    /********************/

    // Train with MC-based NTag output and generate new weights
    if (parser.OptionExists("-train")) {

        if (outputName.empty())
            outputName = installPath + "weights/new/NTagTMVA_TestResults.root";

        msg.PrintBlock("Train mode", pMAIN, pDEFAULT, false);
        msg.Print("Input file         : " + inputName);
        msg.Print("Output weight path : " + installPath + "weights/new\n\n");

        NTagTMVA nt(inputName.c_str(), outputName.c_str(), pVERBOSE);

        // Set training methods if custom options are given
        TString methods = TString(parser.GetOption("-method"));

        if (!methods.IsNull()) {
            nt.SetMethods(false);
            TObjArray* methodArray = methods.Tokenize(",");
            for (int i = 0; i < methodArray->GetEntries(); i++)
                nt.UseMethod(((TObjString *)(methodArray->At(i)))->String().Data());
        }

        bool isMultiClass = parser.OptionExists("-multiclass") ? true : false;
        nt.MakeWeights(isMultiClass);

        msg.Print("Training complete. New weights are in directory weights/new.");
        msg.Print(Form("Test results saved in output: ") + outputName);

    }

    // Apply weights to an NTag output root file
    else if (parser.OptionExists("-apply")) {

        if (outputName.empty())
            outputName = TString(inputName).ReplaceAll(".root", Form("_%s.root", methodName.c_str()));

        msg.PrintBlock("Apply mode", pMAIN, pDEFAULT, false);
        msg.Print("Input file  : " + inputName);
        msg.Print("Output file : " + outputName + "\n\n");

        NTagTMVA nt(inputName.c_str(), outputName.c_str(), pVERBOSE);
        nt.ApplyWeight(methodName, weightName);

        msg.Print(Form("NTag output with new TMVA output saved in: ") + outputName);

    }

    // ZBS TQ Reader: Read TQ information only from ZBS input and dump to output
    else if (parser.OptionExists("-readTQ") && !TString(inputName).EndsWith(".root")) {

        if (outputName.empty())
            outputName = TString(inputName).ReplaceAll(".", "_TQ.");

        msg.PrintBlock("TQReader mode", pMAIN, pDEFAULT, false);
        msg.Print("Input file  : " + inputName);
        msg.Print("Output file : " + outputName + "\n\n");

        NTagZBSTQReader* nt = new NTagZBSTQReader(inputName.c_str(), outputName.c_str(), pVERBOSE);
        nt->SetVertexMode(mTRUE);
        nt->ReadFile();
        nt->WriteOutput();

        delete nt;
    }


    // Process SK data / MC files
    else {

        if (outputName.empty()) outputName = installPath + "out/NTagOut.root";

        // Process SKROOT with TQREAL filled
        if (TString(inputName).EndsWith(".root")) {

            msg.PrintBlock("Tag mode", pMAIN, pDEFAULT, false);
            msg.Print("Input file  : " + inputName);
            msg.Print("Output file : " + outputName);

            NTagROOT* nt = new NTagROOT(inputName.c_str(), outputName.c_str(), pVERBOSE);
            ProcessSKFile(nt, parser);

            msg.Print(Form("NTag output with new TMVA output saved in: ") + outputName);
            delete nt;
        }

        // Process ZBS
        else {

            msg.PrintBlock("Tag mode", pMAIN, pDEFAULT, false);
            msg.Print("Input file  : " + inputName);
            msg.Print("Output file : " + outputName);

            NTagZBS* nt = new NTagZBS(inputName.c_str(), outputName.c_str(), pVERBOSE);
            ProcessSKFile(nt, parser);

            msg.Print(Form("NTag output with new TMVA output saved in: ") + outputName);
            delete nt;
        }

    }

    return 0;
}

void ProcessSKFile(NTagIO* nt, NTagArgParser& parser)
{
    NTagMessage msg("");

    nt->TMVATools.SetReader(methodName, weightName);

    // Set NHits limits
    const std::string &NHITSTH = parser.GetOption("-NHITSTH");
    const std::string &NHITSMX = parser.GetOption("-NHITSMX");
    if (!NHITSTH.empty()) {
        if (!NHITSMX.empty())
            nt->SetNHitsLimits(std::stoi(NHITSTH), std::stoi(NHITSMX));
        else
            nt->SetNHitsLimits(std::stoi(NHITSTH));
    }
    else if (!NHITSMX.empty()) {
        nt->SetNHitsLimits(NTagDefault::NHITSTH, std::stoi(NHITSMX));
    }

    // Set T0 limits
    const std::string &T0TH = parser.GetOption("-T0TH");
    const std::string &T0MX = parser.GetOption("-T0MX");
    if (!T0TH.empty()) {
        if (!T0MX.empty())
            nt->SetT0Limits(std::stoi(T0TH), std::stoi(T0MX));
        else
            nt->SetT0Limits(std::stoi(T0TH));
    }
    else if (!T0MX.empty()) {
        nt->SetT0Limits(NTagDefault::T0TH, std::stoi(T0MX));
    }

    // Set PMT dead time width for RBN reduction
    const std::string &TRBNWIDTH = parser.GetOption("-TRBNWIDTH");
    if (!TRBNWIDTH.empty()) {
        nt->SetTRBNWidth(std::stof(TRBNWIDTH));
    }

    // Set TWIDTH
    const std::string &TWIDTH = parser.GetOption("-TWIDTH");
    if (!TWIDTH.empty()) {
        nt->SetNHitsWidth(std::stof(TWIDTH));
    }
    
    // Set VTXSRCRANGE
    const std::string &VTXSRCRANGE = parser.GetOption("-VTXSRCRANGE");
    if (!VTXSRCRANGE.empty()) {
        nt->SetDistanceCut(std::stof(VTXSRCRANGE));
    }
    
    // Set MINGRIDWIDTH
    const std::string &MINGRIDWIDTH = parser.GetOption("-MINGRIDWIDTH");
    if (!MINGRIDWIDTH.empty()) {
        nt->SetMinGridWidth(std::stof(MINGRIDWIDTH));
    }

    // Set prompt vertex resolution
    const std::string &PVXRES = parser.GetOption("-PVXRES");
    if (!PVXRES.empty()) {
        nt->SetVertexResolution(std::stof(PVXRES));
    }

    // Turn TMVA on/off (default: on)
    if (parser.OptionExists("-noMVA")) {
        nt->UseTMVA(false);
    }

    // Turn Neut-fit on/off (default: on)
    if (parser.OptionExists("-noFit")) {
        nt->UseTMVA(false);
        nt->UseNeutFit(false);
    }

    // Save residual TQ (default: off)
    if (parser.OptionExists("-saveTQ")) {
        nt->SetSaveTQFlagAs(true);
    }

    // Force MC mode (default: off)
    if (parser.OptionExists("-forceMC")) {
        nt->ForceMCMode(true);
    }

    // Use ToF-subtracted (residual) time (default: on)
    if (parser.OptionExists("-noTOF")) {
        nt->UseResidual(false);
        nt->SetTMatchWindow(100.);
        nt->SetTPeakSeparation(150.);
    }

    // Save signal flags from source file (MC-only)
    const std::string &sigTQFileName = parser.GetOption("-sigTQpath");
    if (!sigTQFileName.empty()) {
        nt->SetSignalTQ(sigTQFileName.c_str());
        //nt->SetSaveTQFlagAs(true);
    }

    // Vertex options

    // Custom vertex coordinates
    vx = parser.GetOption("-vx");
    vy = parser.GetOption("-vy");
    vz = parser.GetOption("-vz");

    if (!vx.empty() || !vy.empty() || !vz.empty()) {
        msg.Print(Form("Setting custom prompt vertex as (%s, %s, %s)...",
                        vx.c_str(), vy.c_str(), vz.c_str()));
        nt->SetCustomVertex(std::stof(vx), std::stof(vy), std::stof(vz));
    }
    else if (parser.OptionExists("-usetruevertex")) {
        nt->SetVertexMode(mTRUE);
    }
    else if (parser.OptionExists("-usestmuvertex")) {
        nt->SetVertexMode(mSTMU);
    }

    nt->ReadFile();
    nt->WriteOutput();
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
    std::cout << "                  NTag Version " << NTagVersion
              << " (" << NTagDate << ")\n\n" << std::endl;
}
