#include "ArgParser.hh"
#include "NTagTMVATagger.hh"

int main(int argc, char** argv)
{
    ArgParser parser(argc, argv);
    const std::string inFilePath = parser.GetOption("-in");
    const std::string outFilePath = parser.GetOption("-out");
    const std::string weightPath = parser.GetOption("-weight");
    std::string tMatchWindow = parser.GetOption("-TMATCHWINDOW");
    if (tMatchWindow.empty()) tMatchWindow = "200";
    
    NTagTMVATagger tagger;
    tagger.Initialize(weightPath);
    tagger.Apply(inFilePath.c_str(), outFilePath.c_str(), std::stof(tMatchWindow));
    
    return 0;
}