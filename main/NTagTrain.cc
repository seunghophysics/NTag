#include <iostream>

#include "NTagTMVAManager.hh"
#include "NTagGlobal.hh"
#include "ArgParser.hh"
#include "Printer.hh"

int main(int argc, char** argv)
{
    ArgParser parser(argc, argv);
    const std::string inputFilePath = parser.GetOption("-in");
    const std::string outputFilePath = parser.GetOption("-out");
    const std::string outDirPath = parser.GetOption("-dir");
    const std::string featurelist = parser.GetOption("-custom_mva_features");
    auto addWords = [](std::vector<std::string> &base, std::string str){
      while ( str != "" ){
        auto pos = str.find(',');
        if( pos == std::string::npos ){
          base.push_back( str);
          str = "";
        }
        else {
          base.push_back( str.substr(0, pos) );
          str = str.substr(pos+1);
        }
      }
      return;
    };
    if( featurelist != "" ){
      gTMVAFeatures.clear();
      addWords( gTMVAFeatures, featurelist );
    }

    std::cout << std::endl;

    Printer msg("NTagTrain", pDEFAULT);

    msg.Print("Input file: " + inputFilePath);

    NTagTMVAManager manager;
    manager.SetMethods(true);
    manager.TrainWeights(inputFilePath.data(), outputFilePath.data(), outDirPath.data());

    return 0;
}
