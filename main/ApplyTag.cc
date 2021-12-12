#include "ArgParser.hh"
#include "NTagTMVATagger.hh"

int main(int argc, char** argv)
{
    ArgParser parser(argc, argv);
    const std::string inFilePath = parser.GetOption("-in");
    const std::string outFilePath = parser.GetOption("-out");
    const std::string weightPath = parser.GetOption("-weight");
    
    NTagTMVATagger tagger;
    tagger.Initialize(weightPath);
    tagger.Apply(inFilePath.c_str(), outFilePath.c_str());
    
    return 0;
}