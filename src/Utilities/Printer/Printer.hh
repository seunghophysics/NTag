#ifndef PRINTER_HH
#define PRINTER_HH

#include <string>
#include <TString.h>

extern const char* gitcommit;
extern const char* gitcommitdate;
static std::string NTagVersion = std::string("\"") + std::string(gitcommit) + std::string("\"");
static std::string NTagDate    = gitcommitdate;

void PrintNTag();
void PrintVersion();
void PrintBlock(TString line);

#endif