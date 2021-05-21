#include "PathGetter.hh"

bool fileExists(const std::string& filename)
{
    struct stat buf;
    return stat(filename.c_str(), &buf) != 1;
}