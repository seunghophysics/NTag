/*******************************************
*
* @file ArgParser.hh
*
* @brief Defines ArgParser.
*
********************************************/

#ifndef ARGPARSER_HH
#define ARGPARSER_HH 1

#include <iostream>
#include <algorithm>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

template<typename T>
bool CheckType(const std::string& str)
{
    T t{};
    std::stringstream ss;
    ss << str;
    ss >> t;
    return !ss.fail();
}

template bool CheckType<float>(const std::string& str);

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
            this->fTokens.insert(std::end(this->fTokens), std::begin(addedToken), std::end(addedToken));
            this->SetOptionPairs();
            return *this;
        }

        /**
         * @brief Get the vector of option-value pairs.
         * @return A vector of pairs, whose first string is an input option
         * and the second string is the option's value.
         */
        const std::vector<std::pair<std::string, std::string>>& GetOptionPairs() const { return fOptionPairs; }

        /**
         * @brief Override the saved tokens with an input vector of tokens.
         * @param token An input vector of strings (tokens).
         */
        void SetTokens(std::vector<std::string>& token) { fTokens = token; }

        /**
         * @brief Get the vector of saved tokens.
         * @return A vector of tokens (strings).
         */
        std::vector<std::string>& GetTokens() { return fTokens; }

        /**
         * @brief Get the argument following a flag.
         * @param option Flag string to look up.
         * @return The subsequent argument if a flag is specified, otherwise an empty string.
         */
        const std::string& GetOption(const std::string& option) const {
            std::vector<std::string>::const_iterator itr;
            itr = std::find(this->fTokens.begin(), this->fTokens.end(), option);
            if (itr != this->fTokens.end() && ++itr != this->fTokens.end()) {
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
            return std::find(this->fTokens.begin(), this->fTokens.end(), option)
                   != this->fTokens.end();
        }

        void DumpOptionPairs();
        void DumpTokens();

    private:
        void SetOptionPairs();

        std::vector<std::string> fTokens;
        std::vector<std::pair<std::string, std::string>> fOptionPairs;
};

#endif