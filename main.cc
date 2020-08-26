#include <string.h>
#include <iostream>

#include <TROOT.h>

#include "NTagROOT.hh"
#include "NTagZBS.hh"
#include "NTagTMVA.hh"

int main(int argc, char** argv)
{
    std::string inputName, outputName;

    if (argc == 1) {
        std::cerr << "[NTag] Usage: ./bin/main [output name] [input file]" << std::endl;
    }
    else if (argc == 2) {
        // argv 1    : input file
        outputName = "out/NTagOut.root";
        inputName  = argv[1];
    }
    else if (argc > 2) {
        // argv 1    : output file
        // argv 2 ~  : input file ...
        outputName = argv[1];
        inputName  = argv[2];
    }
    
    if (TString(inputName).EndsWith(".root")) {
        NTagROOT nt(inputName.c_str(), outputName.c_str(), true, pDEBUG);
        nt.SetCustomVertex(-400, 0, 0);
        nt.SetN10Limits(7, 50);
        nt.ReadFile();
    }
    else {
        NTagZBS nt(inputName.c_str(), outputName.c_str(), false, pDEBUG);
        nt.SetN10Limits(7, 50);
        nt.ReadFile();
    }
    
    /*
    NTagTMVA nt(inputName.c_str(), outputName.c_str(), pDEBUG);
    nt.SetReaderCutRange("N10", 7, 50);
    nt.SetReaderCutRange("dt", 0, 500.e3);
    nt.DumpReaderCutRange();
    nt.ApplyWeight("MLP", "weights/new/NTagTMVA_MLP.weights.xml");
    //nt.Train();
    */
    return 0;
}