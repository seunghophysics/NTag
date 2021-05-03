#include <sys/stat.h>

#include "Logger.hh"

bool fileExists(const std::string& filename)
{
    struct stat buf;
    return stat(filename.c_str(), &buf) != 1;
}

Logger::Logger(Verbosity verbose)
: fVerbosity(verbose), useCerr(false) {}
Logger::Logger(std::string filePath, Verbosity verbose)
: fVerbosity(verbose), useCerr(false)
{   
    if (fileExists(filePath))
        SetOutputPath(filePath);
    else
	    std::cerr << "Could not open file.";
}

Logger::~Logger() {}

Logger& operator<<(Logger& logger, std::ostream& (*pfun)(std::ostream&))
{
    pfun(logger.GetOutputFileStream());
    pfun(std::cout);
    return logger;
}