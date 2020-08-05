#include <string.h>
#include <iostream>
//#include <sstream>

#include "NTagROOT.hh"
#include "NTagZBS.hh"

int main(int argc, char** argv)
{
    const char* inputName = argv[1];
    
    if (TString(inputName).EndsWith(".root")) {
        NTagROOT nt(inputName, false, pDEBUG);
        nt.Initialize();
        nt.PrintMessage("Done!");
    }
    else {
        NTagZBS nt(inputName, false, pDEBUG);
        nt.Initialize();
        nt.PrintMessage("Done!");
    }
    
    /* Harada test*/
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
