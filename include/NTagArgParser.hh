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
 * @brief Argument parser used in main function.
 *******************************************************/
class NTagArgParser{

    public:
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