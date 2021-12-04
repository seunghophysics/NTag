#ifndef NTAGGLOBAL_HH
#define NTAGGLOBAL_HH

#include <string>
#include <vector>

enum VertexMode
{
    mNONE, mAPFIT, mBONSAI, mFITQUN, mCUSTOM, mTRUE, mSTMU, mTRMS, mPROMPT
};

enum TriggerType
{
    tELSE, tSHE, tAFT
};

enum TrueLabel
{
    lNoise, lDecayE, lnH, lnGd, lRemnant, lUndefined
};

enum TaggableType
{
    typeMissed,
    typeE,
    typeN,
    typeEN
};

static std::vector<std::string> gMuechkFeatures =  {"ReconCT", "x", "y", "z", "DWall", "dirx", "diry", "dirz",
                                                    "NHits", "GateType", "Goodness", "Label", "TagIndex", "TagClass"};

static std::vector<std::string> gNTagFeatures = {"NHits", "N50", "N200", "N1300", "ReconCT", "TRMS", "QSum",
                                                 "Beta1", "Beta2", "Beta3", "Beta4", "Beta5",
                                                 "OpeningAngleMean", "OpeningAngleSkew", "OpeningAngleStdev", 
                                                 "MeanDirAngleMean", "MeanDirAngleRMS",
                                                 "dvx", "dvy", "dvz", "DPrompt", "DWall", "DWallMeanDir",
                                                 "SignalRatio", "TagOut", "TagIndex", "TagClass", "Label"};
                                                 
static std::vector<std::string> gTMVAFeatures = {"NHits", "N200", "TRMS",
                                                 "Beta1", "Beta5",
                                                 "OpeningAngleMean", "OpeningAngleSkew", "OpeningAngleStdev", 
                                                 "MeanDirAngleMean", "MeanDirAngleRMS",
                                                 "DPrompt", "DWall", "DWallMeanDir"};

#endif