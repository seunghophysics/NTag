#include "ArgParser.hh"
#include "Store.hh"
#include "Calculator.hh"
#include "NTagTMVATagger.hh"
#include "NTagCutTagger.hh"

int main(int argc, char** argv)
{
    ArgParser parser(argc, argv);
    Store settings;
    settings.Initialize(GetENV("NTAGLIBPATH")+"/NTagConfig");
    settings.ReadArguments(parser);
    settings.Print();

    auto inFilePath = settings.GetString("in");
    auto outFilePath = settings.GetString("out");
    auto weightPath = settings.GetString("weight");

    auto taggerType = settings.GetString("tagger");
    if (taggerType=="tmva") {
        NTagTMVATagger tagger;
        if (weightPath=="default")
            weightPath = settings.GetString("delayed_vertex");
        if (settings.GetBool("tag_e"))
            tagger.SetECut(settings.GetFloat("E_NHITSCUT"), settings.GetFloat("E_TIMECUT"));
        tagger.SetNCut(settings.GetFloat("TAGOUTCUT"));
        tagger.Initialize(weightPath);
        tagger.Apply(inFilePath.c_str(), outFilePath.c_str(), settings.GetFloat("TMATCHWINDOW"));
    }
    else if (taggerType=="cuts") {
        NTagCutTagger tagger;
        tagger.SetCuts(settings.GetFloat("SCINTCUT"),
                       settings.GetFloat("GOODNESSCUT"),
                       settings.GetFloat("DIRKSCUT"),
                       settings.GetFloat("DISTCUT"));
        tagger.Apply(inFilePath.c_str(), outFilePath.c_str(), settings.GetFloat("TMATCHWINDOW"));
    }

    return 0;
}