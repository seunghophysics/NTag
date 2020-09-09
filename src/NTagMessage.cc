#include <iostream>

#include "NTagMessage.hh"

NTagMessage::NTagMessage(const char* className, Verbosity verbose):
fClassName(className), fVerbosity(verbose) {}
NTagMessage::~NTagMessage() {}

void NTagMessage::PrintTag(Verbosity vType)
{
    switch (vType) {
        case pDEFAULT:
            std::cout << "[NTag" << fClassName << "] "; break;
        case pWARNING:
            std::cout << "\033[4;33m" << "[NTag" << fClassName << " WARNING] "; break;
        case pERROR:
            std::cerr << "\033[4;31m" << "[Error in NTag" << fClassName << "]"; break;
        case pDEBUG:
            std::cout << "\033[0;34m" << "[NTag" << fClassName << " DEBUG] "; break;
    }
}

void NTagMessage::Print(TString msg, Verbosity vType)
{
    if (vType <= fVerbosity) {
        PrintTag(vType);
        if (vType == pERROR) {
            std::cerr << "\033[m" << msg << std::endl;
            exit(1);
        }
        else std::cout << "\033[m" << msg << std::endl;
    }
}

void NTagMessage::Print(const char* msg, Verbosity vType)
{
    if (vType <= fVerbosity) {
        PrintTag(vType);
        if (vType == pERROR) std::cerr << "\033[m" << msg << std::endl;
        else std::cout << "\033[m" << msg << std::endl;
    }
}

float NTagMessage::Timer(TString msg, std::clock_t tStart, Verbosity vType)
{
    float tDuration = (std::clock() - tStart) / (float) CLOCKS_PER_SEC;

    if (tDuration > 60.) {
        Print(msg + Form(" took %f min", tDuration / 60.), vType);
    }
    else Print(msg + Form(" took %f sec", tDuration), vType);

    return tDuration;
}