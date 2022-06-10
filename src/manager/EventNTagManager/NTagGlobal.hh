#ifndef NTAGGLOBAL_HH
#define NTAGGLOBAL_HH

#include <string>
#include <vector>

enum VertexMode
{
    mNONE, mAPFIT, mBONSAI, mFITQUN, mCUSTOM, mTRUE, mSTMU, mTRMS, mPROMPT, mLOWFIT
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

static std::vector<std::string> gMuechkFeatures =  {"FitT", "fvx", "fvy", "fvz", "DWall", "dirx", "diry", "dirz",
                                                    "NHits", "GateType", "Goodness", "Label", "TagIndex", "DTaggable", "TagClass"};

static std::vector<std::string> gNTagFeatures = {"NHits", "N30", "N50", "N200", "N1300", "FitT", "TRMS", "QSum",
                                                 "Beta1", "Beta2", "Beta3", "Beta4", "Beta5",
                                                 "BSenergy", "BSdirks", "BSovaq",
                                                 "OpeningAngleMean", "OpeningAngleSkew", "OpeningAngleStdev",
                                                 "MeanDirAngleMean", "MeanDirAngleRMS",
                                                 "fvx", "fvy", "fvz", "DTaggable", "FitGoodness", "DPrompt", "DWall", "DWallMeanDir",
                                                 "SignalRatio", "TagOut", "TagIndex", "TagClass", "Label"};

static std::vector<std::string> gTMVAFeatures = {"NHits", "N200", "TRMS",
                                                 "Beta1", "Beta5",
                                                 "OpeningAngleMean", "OpeningAngleSkew", "OpeningAngleStdev",
                                                 "MeanDirAngleMean", "MeanDirAngleRMS",
                                                 "DWall", "DWallMeanDir"};

static std::vector<std::string> gCmdOptions = {"force_flat", "outdata", "write_bank", "noise_path", "noise_type", "save_hits",
                                               "add_noise", "repeat_noise", "in_noise", "dump_noise", "IDDARKRATE", "ODDARKRATE",
                                               "noise_cut", "IDMAXN200", "ODMAXN200",
                                               "weight", "debug", "in", "out", "tmva", "correct_tof", "macro",
                                               "prompt_vertex", "delayed_vertex", "vx", "vy", "vz", "tag_e",
                                               "SKGEOMETRY", "SKOPTN", "SKBADOPT", "REFRUNNO",
                                               "TMIN", "TMAX", "TRBNWIDTH", "PVXRES", "PVXBIAS", "NODHITMX",
                                               "TNOISESTART", "TNOISEEND", "NOISESEED",
                                               "TWIDTH", "NHITSTH", "NHITSMX", "N200MX", "TCANWIDTH", "MINNHITS", "MAXNHITS",
                                               "TMINPEAKSEP", "TMATCHWINDOW",
                                               "TRMSTWIDTH", "INITGRIDWIDTH", "MINGRIDWIDTH", "GRIDSHRINKRATE", "VTXMAXRADIUS",
                                               "E_CUTS", "N_CUTS",
                                               "print"};

#endif