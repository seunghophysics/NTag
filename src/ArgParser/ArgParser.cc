#include "ArgParser.hh"

ArgParser::ArgParser() {}

ArgParser::ArgParser(int &argc, char **argv)
: ArgParser()
{
    for (int iArg = 1; iArg < argc; ++iArg)
        this->tokens.push_back(std::string(argv[iArg]));
}

ArgParser::ArgParser(std::ifstream& f) 
: ArgParser()
{
    std::string arg;
    while (std::getline(f, arg, '\n')) {
        // skip blank line
        if (arg.empty()) continue;
        // skip comments starting with #, else read line
        if (arg.at(0) != '#') {
            std::stringstream ss(arg);
            // space is the delimiter
            while (std::getline(ss, arg, ' ')) {
                this->tokens.push_back(arg);
            }
        }
    }
}