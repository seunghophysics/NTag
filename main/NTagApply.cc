#include "ArgParser.hh"
#include "Store.hh"
#include "Calculator.hh"
#include "NTagTMVATagger.hh"

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

    NTagTMVATagger tagger;

    if (weightPath=="default")
        weightPath = settings.GetString("delayed_vertex");
    if (settings.GetBool("tag_e"))
        tagger.SetECut(settings.GetFloat("E_NHITSCUT"), settings.GetFloat("E_TIMECUT"));

    tagger.SetNCut(settings.GetFloat("N_OUTCUT"));
    tagger.Initialize(weightPath);
    tagger.Apply(inFilePath.c_str(), outFilePath.c_str(), settings.GetFloat("TMATCHWINDOW"));

    return 0;
}