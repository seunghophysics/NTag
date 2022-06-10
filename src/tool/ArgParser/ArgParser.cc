#include "ArgParser.hh"

ArgParser::ArgParser() {}

ArgParser::ArgParser(int &argc, char **argv)
: ArgParser()
{
    for (int iArg = 1; iArg < argc; ++iArg)
        this->fTokens.push_back(std::string(argv[iArg]));

    SetOptionPairs();
}

ArgParser::ArgParser(std::ifstream& f)
: ArgParser()
{
    std::string line;
    while (std::getline(f, line, '\n')) {
        // skip blank line
        if (line.empty()) continue;
        // skip comments starting with #, else read line
        if (line.at(0) != '#') {
            std::stringstream ss(line);
            std::string arg;
            bool first = true;
            // space is the delimiter
            while (std::getline(ss, arg, ' ')) {
                if (!arg.empty()) {
                    // first argument is option
                    if (first) {
                        this->fTokens.push_back('-' + arg);
                        first = false;
                    }
                    // what follows is value
                    else
                        this->fTokens.push_back(arg);
                }
            }
        }
    }

    SetOptionPairs();
}

void ArgParser::SetOptionPairs()
{
    fOptionPairs.clear();

    for (unsigned int i=0; i<fTokens.size(); i++) {

        // skip tokens without a hyphen at head
        if (fTokens[i].at(0) != '-') continue;

        auto option = fTokens[i].substr(1, fTokens[i].size()-1);
        auto value = GetOption(fTokens[i]);

        // if option is non-numeric
        if (!CheckType<float>(option)) {
            // value is numeric or string without hyphen
            if (!value.empty()) {
                if (CheckType<float>(value) || value.at(0) != '-')
                    fOptionPairs.push_back({option, value});
                else
                    fOptionPairs.push_back({option, value});
            }
            // no value: token is boolean true
            else
                fOptionPairs.push_back({option, "true"});
        }
    }
}

void ArgParser::DumpOptionPairs()
{ 
    for (auto const& pair: fOptionPairs) std::cout << pair.first << " " << pair.second << "\n"; 
}

void ArgParser::DumpTokens()
{ 
    for (auto const& token: fTokens) std::cout << token << "\n"; 
}