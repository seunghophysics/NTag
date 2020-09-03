#ifndef NTAGARGPARSER_HH
#define NTAGARGPARSER_HH 1

#include <vector>
#include <string>
#include <algorithm>

// original code by @iain from stackoverflow

class NTagArgParser{

    public:
        NTagArgParser(int &argc, char **argv) {
            for (int iArg = 1; iArg < argc; ++iArg)
                this->tokens.push_back(std::string(argv[iArg]));
        }

        const std::string& GetOption(const std::string &option) const {
            std::vector<std::string>::const_iterator itr;
            itr =  std::find(this->tokens.begin(), this->tokens.end(), option);
            if (itr != this->tokens.end() && ++itr != this->tokens.end()) {
                return *itr;
            }
            static const std::string empty_string("");
            return empty_string;
        }

        bool OptionExists(const std::string &option) const {
            return std::find(this->tokens.begin(), this->tokens.end(), option)
                   != this->tokens.end();
        }

    private:
        std::vector<std::string> tokens;
};

#endif