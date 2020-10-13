#include <string.h>
#include <iostream>

#include <TROOT.h>

#include "NTagPath.hh"
#include "NTagROOT.hh"
#include "NTagZBS.hh"
#include "NTagTMVA.hh"
#include "NTagArgParser.hh"
#include "NTagMessage.hh"
#include "NTagZBSTQReader.hh"

void ProcessSKFile(NTagIO* nt, NTagArgParser& parser);

static std::string inputName, outputName, weightName, methodName;
static std::string installPath = GetENV("NTAGPATH");
static std::string vx, vy, vz;

int main(int argc, char** argv)
{
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
            outputName = "weights/new/NTagTMVA_TestResults.root";

        msg.Print(Form("Start testing MVA methods with input: ") + inputName);
        msg.Print("Generating new weight files in $NTAGPATH/weights/new...");

        NTagTMVA nt(inputName.c_str(), outputName.c_str(), pVERBOSE);
        nt.MakeWeights();

        msg.Print("Training complete. New weights are in directory weights/new.");
        msg.Print(Form("Test results saved in output: ") + outputName);

    }

    // Apply weights to an NTag output root file
    else if (parser.OptionExists("-apply")) {

        if (outputName.empty())
            outputName = TString(inputName).ReplaceAll(".root", Form("_%s.root", methodName.c_str()));

        msg.Print(Form("Applying %s method with weight %s to an NTag output named %s...",
                        methodName.c_str(), weightName.c_str(), inputName.c_str()));

        NTagTMVA nt(inputName.c_str(), outputName.c_str(), pVERBOSE);
        nt.ApplyWeight(methodName, weightName);

        msg.Print(Form("NTag output with new TMVA output saved in: ") + outputName);

    }

    // ZBS TQ Reader: Read TQ information only from ZBS input and dump to output
    else if (parser.OptionExists("-readTQ") && !TString(inputName).EndsWith(".root")) {

        if (outputName.empty())
            outputName = TString(inputName).ReplaceAll(".", "_TQ.");

        msg.Print(Form("Exporting TQ to %s...", outputName.c_str()));

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

            msg.Print(Form("Processing SKROOT file: ") + inputName);

            NTagROOT* nt = new NTagROOT(inputName.c_str(), outputName.c_str(), pVERBOSE);
            ProcessSKFile(nt, parser);

            msg.Print(Form("NTag output with new TMVA output saved in: ") + outputName);
            delete nt;
        }

        // Process ZBS
        else {
            msg.Print(Form("Processing ZBS file: ") + inputName);

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

    // Set N10 limits
    const std::string &N10TH = parser.GetOption("-N10TH");
    const std::string &N10MX = parser.GetOption("-N10MX");
    if (!N10TH.empty()) {
        if (!N10MX.empty())
            nt->SetN10Limits(std::stoi(N10TH), std::stoi(N10MX));
        else
            nt->SetN10Limits(std::stoi(N10TH));
    }
    else if (!N10MX.empty()) {
        nt->SetN10Limits(NTagDefault::N10TH, std::stoi(N10MX));
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
        nt->SetTRBNWidth(std::stof(TRBNWIDTH)*1.e3);
    }

    // Turn TMVA on/off (default: on)
    if (parser.OptionExists("-noMVA")) {
        nt->UseTMVA(false);
    }

    // Save residual TQ (default: off)
    if (parser.OptionExists("-saveTQ")) {
        nt->SetSaveTQFlagAs(true);
    }

    // Save signal flags from source file (MC-only)
    const std::string &sigTQFileName = parser.GetOption("-sigTQpath");
    if (!sigTQFileName.empty()) {
        nt->SetSignalTQ(sigTQFileName.c_str());
        nt->SetSaveTQFlagAs(true);
    }

    // Vertex options

    // Custom vertex coordinates
    vx = parser.GetOption("-vx");
    vy = parser.GetOption("-vy");
    vz = parser.GetOption("-vz");

    if (!vx.empty() && !vy.empty() && !vz.empty()) {
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