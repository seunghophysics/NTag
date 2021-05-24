#ifndef PRINTER_HH
#define PRINTER_HH

#include <string>
#include <TString.h>

void PrintNTag(const char* version, const char* date);
void PrintVersion(const char* version, const char* date);
void PrintBlock(TString line);

#endif