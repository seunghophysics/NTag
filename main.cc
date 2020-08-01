#include <string.h>
#include <iostream>

#include <skparmC.h>
#include <skheadC.h>
#include <sktqC.h>

#include "NTagAnalysis.hh"
#include "SKLibs.hh"

int main(int argc, char** argv)
{
    const char* fileName = argv[1];
    NTagAnalysis nt(fileName, false);

    return 0;
}