#include <string.h>
#include <iostream>

#include <TROOT.h>

#include "NTagROOT.hh"
#include "NTagZBS.hh"
#include "NTagTMVA.hh"
#include "NTagArgParser.hh"
#include "NTagMessage.hh"

int main(int argc, char** argv)
{
    /********************/
    /* Argument parsing */
    /********************/

    NTagArgParser parser(argc, argv);

    // Names
    const std::string &inputName     = parser.GetOption("-in");
    const std::string &tmpOutName    = parser.GetOption("-out");
    const std::string &tmpWeightName = parser.GetOption("-weight");
    const std::string &tmpMethodName = parser.GetOption("-method");

    // Custom vertex coordinates
    const std::string &vx = parser.GetOption("-vx");
    const std::string &vy = parser.GetOption("-vy");
    const std::string &vz = parser.GetOption("-vz");

    // Verbosity
    unsigned int pVERBOSE = pDEFAULT;
    if (parser.OptionExists("-debug")) {
        pVERBOSE = pDEBUG;
    }

    NTagMessage msg("", pVERBOSE);

    // Choose between default name and optional name
    std::string outputName, weightName, methodName;
    if (inputName.empty())  msg.Print("Please specify input file name: ./bin/main -in [input file] ...", pERROR);
    if (tmpOutName.empty())    outputName = "out/NTagOut.root";
    else                       outputName = tmpOutName;
    if (tmpWeightName.empty()) weightName = "weights/MLP_Gd0.02p.xml";
    else                       weightName = tmpWeightName;
    if (tmpMethodName.empty()) methodName = "MLP";
    else                       methodName = tmpMethodName;

    /********************/
    /* Main application */
    /********************/

    // Train with MC-based NTag output and generate new weights
    if (parser.OptionExists("-train")) {

        if (tmpOutName.empty())
            outputName = "weights/new/NTagTMVA_Test_Results.root";

        msg.Print(Form("Start testing MVA methods with input: ") + inputName);
        msg.Print("Generating new weight files in weights/new...");

        NTagTMVA nt(inputName.c_str(), outputName.c_str(), pVERBOSE);
        nt.MakeWeights();

        msg.Print("Training complete. New weights are in directory weights/new.");
        msg.Print(Form("Test results saved in output: ") + outputName);

    }

    // Apply weights to an NTag output root file
    else if (parser.OptionExists("-apply")) {

        if (tmpOutName.empty())
            outputName = TString(inputName).ReplaceAll(".root", Form("_%s.root", methodName.c_str()));

        msg.Print(Form("Applying %s method with weight %s to an NTag output named %s...",
                        methodName.c_str(), weightName.c_str(), inputName.c_str()));

        NTagTMVA nt(inputName.c_str(), outputName.c_str(), pVERBOSE);
        nt.ApplyWeight(methodName, weightName);

        msg.Print(Form("NTag output with new TMVA output saved in: ") + outputName);

    }

    // Process SK data / MC files
    else {

        // Process SKROOT with TQREAL filled
        if (TString(inputName).EndsWith(".root")) {

            msg.Print(Form("Processing SKROOT file: ") + inputName);

            NTagROOT nt(inputName.c_str(), outputName.c_str(), pVERBOSE);
            nt.TMVATools.SetReader(methodName, weightName);

            if (!vx.empty() && !vy.empty() && !vz.empty()) {
                msg.Print(Form("Setting custom prompt vertex as (%s, %s, %s)...",
                                vx.c_str(), vy.c_str(), vz.c_str()));
                nt.SetCustomVertex(std::stof(vx), std::stof(vy), std::stof(vz));
            }

            nt.ReadFile();

            msg.Print(Form("NTag output with new TMVA output saved in: ") + outputName);

        }

        // Process ZBS
        else {

            msg.Print(Form("Processing ZBS file: ") + inputName);

            NTagZBS nt(inputName.c_str(), outputName.c_str(), pVERBOSE);
            nt.TMVATools.SetReader(methodName, weightName);

            if (!vx.empty() && !vy.empty() && !vz.empty()) {
                msg.Print(Form("Setting custom prompt vertex as (%s, %s, %s)...",
                                vx.c_str(), vy.c_str(), vz.c_str()));
                nt.SetCustomVertex(std::stof(vx), std::stof(vy), std::stof(vz));
            }

            nt.ReadFile();

            msg.Print(Form("NTag output with new TMVA output saved in: ") + outputName);

        }

    }

    return 0;
}
