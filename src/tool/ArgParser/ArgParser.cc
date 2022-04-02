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
    std::string arg;
    while (std::getline(f, arg, '\n')) {
        // skip blank line
        if (arg.empty()) continue;
        // skip comments starting with #, else read line
        if (arg.at(0) != '#') {
            std::stringstream ss(arg);
            // space is the delimiter
            while (std::getline(ss, arg, ' ')) {
                this->fTokens.push_back(arg);
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

        auto token = fTokens[i].substr(1, fTokens[i].size()-1);
        auto option = GetOption(fTokens[i]);

        // if token is non-numeric
        if (!CheckType<float>(token)) {
            // option is numeric or string without hypen
            if (!option.empty()) {
                if (CheckType<float>(option) || option.at(0) != '-')
                    fOptionPairs.push_back({token, option});
                else
                    fOptionPairs.push_back({token, option});
            }
            // no option: token is boolean true
            else
                fOptionPairs.push_back({token, "true"});
        }
    }
}