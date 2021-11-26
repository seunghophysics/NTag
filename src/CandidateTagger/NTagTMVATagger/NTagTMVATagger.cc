#include "NTagGlobal.hh"

#include "NTagTMVATagger.hh"

NTagTMVATagger::NTagTMVATagger(Verbosity verbose)
: CandidateTagger("NTagTMVATagger", verbose), 
fDoTagE(false), E_N50CUT(50), E_TIMECUT(20), N_OUTCUT(0.7) {}
NTagTMVATagger::~NTagTMVATagger() {}

int NTagTMVATagger::Classify(const Candidate& candidate)
{
    int tagClass = 0;
    int n50 = candidate.Get("N50", -1);
    float reconCT = candidate.Get("ReconCT");
    float tmvaOut = GetLikelihood(candidate);

    // simple cuts mode for e/n separation
    if (fDoTagE) {
        if (reconCT < T0TH*1e-3)                        tagClass = typeE;      // e: muechk && before ntag
        else if (n50 > E_N50CUT && reconCT < E_TIMECUT) tagClass = typeE;      // e: ntag && elike
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