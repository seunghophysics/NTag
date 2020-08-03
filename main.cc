#include <string.h>
#include <iostream>
#include <sstream>

#include <skparmC.h>
#include <skheadC.h>
#include <sktqC.h>

#include "NTagAnalysis.hh"
#include "SKLibs.hh"

int main(int argc, char** argv)
{
    const char* fileName = argv[1];
    
    unsigned int verbose;
    if(argc == 1) verbose = vDefault;
    else{
        std::stringstream strValue(argv[2]);
        strValue >> verbose;
    }
    
    NTagAnalysis nt(fileName, false, verbose);
    nt.PrintMessage("Done!");

    return 0;
}