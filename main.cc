#include <string.h>
#include <iostream>
//#include <sstream>

#include "NTagAnalysis.hh"

int main(int argc, char** argv)
{
    const char* fileName = argv[1];

    /* Harada test*/
    /*
    unsigned int verbose;
    if(argc == 1) verbose = vDefault;
    else{
        std::stringstream strValue(argv[2]);
        strValue >> verbose;
    }
    */
    
    NTagAnalysis nt(fileName, false, vDebug);
    nt.PrintMessage("Done!");

    return 0;
}
