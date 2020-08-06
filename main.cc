#include <string.h>
#include <iostream>
//#include <sstream>

#include "NTagROOT.hh"
#include "NTagZBS.hh"

int main(int argc, char** argv)
{
	// For more convenience in future, I suggest
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
        NTagZBS nt(inputName, false, pDEBUG);
        nt.Initialize();
        nt.PrintMessage("Done!");
    }
    
    /*
    unsigned int verbose;
    if(argc == 1) verbose = pDEFAULT;
    else{
        std::stringstream strValue(argv[2]);
        strValue >> verbose;
    }
    */

    return 0;
}
