#include <iostream>
#include <iomanip>

#include "Printer.hh"

Printer::Printer(const char* className, Verbosity verbose):
fClassName(className), fVerbosity(verbose) {}
Printer::~Printer() {}

void Printer::PrintTag(Verbosity vType)
{
    switch (vType) {
        case pNONE:
            break;
        case pERROR:
            std::cerr << "\033[4;31m" << "[Error in " << fClassName << "]"; break;
        case pWARNING:
            std::cout << "\033[4;33m" << "[" << fClassName << " WARNING] "; break;
        case pDEFAULT:
            std::cout << "[" << fClassName << "] "; break;
        case pDEBUG:
            std::cout << "\033[0;34m" << "[" << fClassName << " DEBUG] "; break;
    }
}

void Printer::Print(TString msg, Verbosity vType, bool newLine)
{
    if (vType <= fVerbosity) {
        PrintTag(vType);
        if (vType == pERROR) {
            std::cerr << "\033[m " << msg;
            exit(1);
        }
        else {
            std::cout << "\033[m" << msg;
            if (newLine) std::cout << std::endl;
        }
    }
}

void Printer::PrintBlock(TString line, BlockSize size, Verbosity vType, bool newLine)
{
    std::string blockWall(size, '=');
    TString coloredLine = "\033[1;36m" + line + "\033[m";

    std::cout << "\n";
    Print(blockWall, vType);
    if (size == pMAIN) {
        Print("", vType, false);
        std::cout << std::right << std::setw((size + coloredLine.Length())/2) << coloredLine << std::endl;
    }
    else
        Print(coloredLine, vType);
    Print(blockWall, vType);

    if (newLine) std::cout << std::endl;
}

void Printer::PrintTitle(TString line)
{
    std::cout << "\n\n********** " << line << " **********\n";
}

float Printer::Timer(TString msg, std::clock_t tStart, Verbosity vType)
{
    float tDuration = (std::clock() - tStart) / (float) CLOCKS_PER_SEC;

    if (tDuration > 60.) {
        Print(msg + Form(" took %f min", tDuration / 60.), vType);
    }
    else Print(msg + Form(" took %f sec", tDuration), vType);

    return tDuration;
}