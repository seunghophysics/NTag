#include <string.h>
#include <iostream>

#include "NTagROOT.hh"
#include "NTagZBS.hh"

int main(int argc, char** argv)
{
	// argv 1 : output file
	// argv 2 ~  : Input file ...
    const char* outputName = argv[1];
    const char* inputName = argv[2];
    
    if (TString(inputName).EndsWith(".root")) {
        NTagROOT nt(inputName, outputName, false, pDEBUG);
        nt.Initialize();
        nt.PrintMessage("Done!");
    }
    else {
        NTagZBS nt(inputName, outputName, false, pDEBUG);
        nt.Initialize();
        nt.PrintMessage("Done!");
    }
    
    return 0;
}
