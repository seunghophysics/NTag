#ifndef NTAGPATH_HH
#define NTAGPATH_HH 1

#include <stdlib.h>
#include <unistd.h>
#include <string>

static std::string GetCWD()
{
    char tempPath[255];
    return ( getcwd(tempPath, sizeof(tempPath)) ? std::string( tempPath ) + "/" : std::string("") );
}

static std::string GetENV(const char* pathName)
{
    char tempPath[255];
    return ( getenv(pathName) ? std::string( getenv(pathName) ) + "/" : std::string("") );
}

#endif