/*******************************************
*
* @file NTagPath.hh
*
* @brief Defines static functions
* #GetCWD and #GetENV.
*
********************************************/

#ifndef NTAGPATH_HH
#define NTAGPATH_HH 1

#include <stdlib.h>
#include <unistd.h>
#include <string>

/** Gets current working directory. */
static std::string GetCWD()
{
    char tempPath[255];
    return ( getcwd(tempPath, sizeof(tempPath)) ? std::string( tempPath ) + "/" : std::string("") );
}

/** 
 * @brief Gets path to the given environment variable.
 * @param pathName Name of the environment variable. 
 */
static std::string GetENV(const char* pathName)
{
    char tempPath[255];
    return ( getenv(pathName) ? std::string( getenv(pathName) ) + "/" : std::string("") );
}

#endif