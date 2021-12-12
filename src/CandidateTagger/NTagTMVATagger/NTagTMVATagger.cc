#include "NTagGlobal.hh"

#include "NTagTMVATagger.hh"

NTagTMVATagger::NTagTMVATagger(Verbosity verbose)
: CandidateTagger("NTagTMVATagger", verbose), 
fDoTagE(false), E_N50CUT(50), E_TIMECUT(20), N_OUTCUT(0.7) {}
NTagTMVATagger::~NTagTMVATagger() {}

void NTagTMVATagger::Initialize(std::string weightPath)
{
    if (!weightPath.empty() || weightPath != "default")
        fTMVAManager.SetWeightPath(weightPath);
    fTMVAManager.InitializeReader();
}

int NTagTMVATagger::Classify(const Candidate& candidate)
{
    int tagClass = 0;
    int n50 = candidate.Get("N50", -1);
    float fitT = candidate.Get("FitT");
    float tmvaOut = GetLikelihood(candidate);

    // simple cuts mode for e/n separation
    if (fDoTagE) {
        if (fitT < TMIN)                        tagClass = typeE;      // e: muechk && before ntag
        else if (n50 > E_N50CUT && fitT < E_TIMECUT) tagClass = typeE;      // e: ntag && elike
        else if (tmvaOut > N_OUTCUT)                    tagClass = typeN;      // n: ntag && !e-like && n-like
        else                                            tagClass = typeMissed; // otherwise noise
    }
    // naive tagging mode without e/n separation
    else {
        if (n50 < 0)                 tagClass = typeE;      // e: muechk
        else if (tmvaOut > N_OUTCUT) tagClass = typeN;      // n: ntag && out cut
        else                         tagClass = typeMissed; // otherwise noise
    }

    return tagClass;
}