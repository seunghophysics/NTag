#include "PathGetter.hh"

#include "Logger.hh"


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

Logger::~Logger() { Flush(); ofstr.close(); }

Logger& operator<<(Logger& logger, std::ostream& (*func)(std::ostream&))
{
    func(logger.GetOutputFileStream());
    func(std::cout);
    return logger;
}