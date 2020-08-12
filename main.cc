#include <string.h>
#include <iostream>

#include "NTagROOT.hh"
#include "NTagZBS.hh"

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
        NTagROOT nt(inputName.c_str(), outputName.c_str(), false, pDEBUG);
        nt.SetCustomVertex(-400, 0, -1200);
        nt.Initialize();
        nt.PrintMessage("Done!");
    }
    else {
        NTagZBS nt(inputName.c_str(), outputName.c_str(), false, pDEBUG);
        nt.Initialize();
        nt.PrintMessage("Done!");
    }
    
    return 0;
}
