/*******************************************
*
* @file ArgParser.hh
*
* @brief Defines ArgParser.
*
********************************************/

#ifndef ARGPARSER_HH
#define ARGPARSER_HH 1

#include <algorithm>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

class Store;

// original code by @iain from stackoverflow

/********************************************************
 * @brief Argument parser used in the main function.
 *******************************************************/
class ArgParser{

    public:
        ArgParser();

        /**
         * @brief Constructor of ArgParser.
         * @param argc \c argc of the main function.
         * @param argv \c argv of the main function.
         */
        ArgParser(int &argc, char **argv);

        /**
         * @brief Constructor of ArgParser for input file stream.
         * @param f Input file stream.
         */
        ArgParser(std::ifstream& f);

        /**
         * @brief Concatenate tokens to form a single ArgParser.
         * @param addedParser ArgParser to add.
         */
        ArgParser& operator+=(ArgParser& addedParser) {
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
        const std::string& GetOption(const std::string& option) const {
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
        bool OptionExists(const std::string& option) const {
            return std::find(this->tokens.begin(), this->tokens.end(), option)
                   != this->tokens.end();
        }
        
        void OverrideStore(Store* store);

    private:
        std::vector<std::string> tokens;
        std::map<std::string, std::string> commandMap;
};

#endif