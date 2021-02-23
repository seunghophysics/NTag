/*******************************************
*
* @file NTagArgParser.hh
*
* @brief Defines NTagArgParser.
*
********************************************/

#ifndef NTAGARGPARSER_HH
#define NTAGARGPARSER_HH 1

#include <vector>
#include <string>
#include <algorithm>

// original code by @iain from stackoverflow

/********************************************************
 * @brief Argument parser used in the main function.
 *******************************************************/
class NTagArgParser{

    public:
        NTagArgParser();

        /**
         * @brief Constructor of NTagArgParser.
         * @param argc \c argc of the main function.
         * @param argv \c argv of the main function.
         */
        NTagArgParser(int &argc, char **argv) {
            for (int iArg = 1; iArg < argc; ++iArg)
                this->tokens.push_back(std::string(argv[iArg]));
        }

        /**
         * @brief Constructor of NTagArgParser for input file stream.
         * @param f Input file stream.
         */
        NTagArgParser(std::ifstream& f) {
            std::string arg;
            while (std::getline(f, arg)) {
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

        /**
         * @brief Concatenate tokens to form a single NTagArgParser.
         * @param addedParser NTagArgParser to add.
         */
        NTagArgParser& operator+=(NTagArgParser& addedParser) {
            auto addedToken = addedParser.GetTokens();
            this->tokens.insert(std::end(this->tokens), std::begin(addedToken), std::end(addedToken));
            return *this;
        }

        void SetTokens(std::vector<std::string>& token) { tokens = token; }
        std::vector<std::string>& GetTokens() { return tokens; }

        /**
         * @brief Get the argument following a flag.
         * @param option Flag string to look up.
         * @return The subsequent argument if a flag is specified, otherwise an empty string.
         */
        const std::string& GetOption(const std::string &option) const {
            std::vector<std::string>::const_iterator itr;
            itr = std::find(this->tokens.begin(), this->tokens.end(), option);
            if (itr != this->tokens.end() && ++itr != this->tokens.end()) {
                return *itr;
            }
            static const std::string empty_string("");
            return empty_string;
        }

        /**
         * @brief Check if an option is specified.
         * @param option Option string to look up.
         * @return \c true if the option is specified, otherwise \c false
         */
        bool OptionExists(const std::string &option) const {
            return std::find(this->tokens.begin(), this->tokens.end(), option)
                   != this->tokens.end();
        }

    private:
        std::vector<std::string> tokens;
};

#endif