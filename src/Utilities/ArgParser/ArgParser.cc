#include "Store.hh"
#include "ArgParser.hh"

ArgParser::ArgParser()
:commandMap({{"in", "input_file_path"}, 
             {"out", "output_file_path"},
             {"log", "log_file_path"},
             {"tools", "tools_list_path"},
             {"weight", "weight_file_path"},
             {"method", "mva_method_name"},
             {"vertex", "prompt_vertex"},
             {"PVXRES", "smearing_resolution"},
             {"SKOPTN", "sk_options"},
             {"SKBADOPT", "sk_bad_channel_option"},
             {"REFRUNNO", "reference_run"}})
{}

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

void ArgParser::OverrideStore(Store* store)
{
    for (unsigned int i=0; i<tokens.size(); i++) {
    
        // skip tokens without a hyphen at head 
        if (tokens[i].at(0) != '-') continue;

        auto token = tokens[i].substr(1, tokens[i].size()-1);
        auto option = GetOption(tokens[i]);
        
        // if token is non-numeric
        if (!CheckType<float>(token)) {
            if (commandMap.count(token)) {
                if (!option.empty())
                    store->Set(commandMap[token], option);
                else
                    store->Set(commandMap[token], 1);
            }
            else {
                if (!option.empty())
                    store->Set(token, option);
                else
                    store->Set(token, 1);
            }
        }    
    }
}