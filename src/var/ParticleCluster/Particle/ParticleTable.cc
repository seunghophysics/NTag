/**
 * @file ParticleTable.cc
 */

#include "ParticleTable.hh"

std::map<int, TString> gPIDMap = {{11,         "e-"},
                                  {-11,        "e+"},
                                  {12,         "nu_e"},
                                  {-12,        "anti-nu_e"},
                                  {13,         "mu-"},
                                  {-13,        "mu+"},
                                  {14,         "nu_mu"},
                                  {-14,        "anti-nu_mu"},
                                  {15,         "tau-"},
                                  {-15,        "tau+"},
                                  {16,         "nu_tau"},
                                  {-16,        "anti-nu_tau"},
                                  {22,         "gamma"},
                                  {111,        "pi0"},
                                  {211,        "pi-"},
                                  {-211,       "pi+"},
                                  {130,        "K0L"},
                                  {321,        "K+"},
                                  {-321,       "K-"},
                                  {221,        "eta"},
                                  {2112,       "n"},
                                  {-2112,      "anti-n"},
                                  {2212,       "p"},
                                  {-2212,      "anti-p"},
                                  {3112,       "sigma-"},
                                  {3122,       "lambda"},
                                  {3212,       "sigma0"},
                                  {3222,       "sigma+"},
                                  {1000010020, "d"},
                                  {1000010030, "t"},
                                  {1000020030, "He3"},
                                  {1000020040, "alpha"},
                                  {1000080160, "16O"},
                                  {1000080170, "17O"},
                                  {1000160330, "33S"},
                                  {1000240540, "54Cr"},
                                  {1000260570, "57Fe"},
                                  {1000280630, "63Ni"},
                                  {1000320710, "71Ge"},
                                  {1000320720, "72Ge"},
                                  {1000320730, "73Ge"},
                                  {1000320740, "74Ge"},
                                  {1000320750, "75Ge"},
                                  {1000641560, "156Gd"},
                                  {1000641570, "157Gd"},
                                  {1000641580, "158Gd"},
                                  {1000832100, "210Bi"}};
                                  
std::map<int, float> gPIDMassMap = {{1000010020, 1875.613},
                                    {1000010030, 2808.921},
                                    {1000020030, 2809.412},
                                    {1000020040, 3727.379},
                                    {1000080160, 14899.16},
                                    {1000641560, 145240.5},
                                    {1000641580, 147105.4},
                                    {0         , 0}};

//! [Geant3 interaction code]
std::map<int, TString> gIntIDMap = {{0,  "-"},
                                    {5,  "Decay"},
                                    {6,  "PairProd"},
                                    {7,  "Compt."},
                                    {8,  "Photo-e."},
                                    {9,  "Brems."},
                                    {10, "Delta"},
                                    {11, "Annihil."},
                                    {12, "Hadronic"},
                                    {13, "HadElas."},
                                    {20, "HadInel."},
                                    {18, "nCapture"}};
//! [Geant3 interaction code]

std::map<int, int> gG3toG4PIDMap = {{1,  22},         // gamma
                                    {2,  -11},        // positron
                                    {3,  11},         // electron
                                    {4,  0},          // neutrino (undefined)
                                    {5,  -13},        // mu+
                                    {6,  13},         // mu-
                                    {7,  111},        // pi0
                                    {8,  211},        // pi+
                                    {9,  -211},       // pi-
                                    {10, 130},        // K0L
                                    {11, 321},        // K+
                                    {12, -321},       // K-
                                    {13, 2112},       // n
                                    {14, 2212},       // p
                                    {15, -2212},      // anti-p
                                    {16, 310},        // K0S
                                    {17, 221},        // eta
                                    {18, 3122},       // lambda
                                    {19, 3222},       // sigma+
                                    {20, 3212},       // sigma0
                                    {21, 3112},       // sigma-
                                    {22, 3322},       // xi0
                                    {23, 3312},       // xi-
                                    {24, 3334},       // omega-
                                    {25, -2112},      // anti-n
                                    {26, -2112},      // anti-labmda
                                    {27, -3222},      // anti-sigma-
                                    {28, -3212},      // anti-sigma0
                                    {29, -3212},      // anti-sigma+
                                    {30, -3322},      // anti-xi0
                                    {31, -3312},      // anti-xi+
                                    {32, -3334},      // anti-omega+
                                    {45, 1000010020}, // deuteron
                                    {46, 1000010030}, // triton
                                    {47, 1000020040}, // alpha
                                    {48, 0},          // geantino
                                    {50, 0},          // cherenkov
                                    // for skdetsim
                                    // refer to gt2pd.h in skdetsim
                                    {100045, 1000010020},  // deuteron
                                    {100046, 1000010030},  // triton
                                    {100047, 1000020040},  // alpha
                                    {100048, 0},           // geantino
                                    {100049, 1000020030},  // 3He
                                    {100069, 1000080160}}; // 16O


std::map<int, TString> gNEUTModeMap = {{1, "CCQE"},
                                       {-1, "CCQE"},
                                       {2, "CCQE (+N)"},
                                       {-2, "CCQE (+N)"},
                                       {11, "CC1pi (delta)"},
                                       {12, "CC1pi (delta)"},
                                       {13, "CC1pi (delta)"},
                                       {-11, "CC1pi (delta)"},
                                       {-12, "CC1pi (delta)"},
                                       {-13, "CC1pi (delta)"},
                                       {15, "CC1pi (diff)"},
                                       {-15, "CC1pi (diff)"},
                                       {16, "CC1pi (coh)"},
                                       {-16, "CC1pi (coh)"},
                                       {17, "CC1gamma (delta)"},
                                       {-17, "CC1gamma (delta)"},
                                       {18, "CC1K"},
                                       {-18, "CC1K"},
                                       {19, "CC1K"},
                                       {-19, "CC1K"},
                                       {20, "CC1K"},
                                       {-20, "CC1K"},
                                       {21, "CC multi-pi"},
                                       {-21, "CC multi-Pi"},
                                       {22, "CC1eta (delta)"},
                                       {-22, "CC1eta (delta)"},
                                       {23, "CC1K (delta)"},
                                       {-23, "CC1K (delta)"},
                                       {26, "CCDIS"},
                                       {-26, "CCDIS"},
                                       {31, "NC1pi (delta)"},
                                       {32, "NC1pi (delta)"},
                                       {33, "NC1pi (delta)"},
                                       {34, "NC1pi (delta)"},
                                       {-31, "NC1pi (delta)"},
                                       {-32, "NC1pi (delta)"},
                                       {-33, "NC1pi (delta)"},
                                       {-34, "NC1pi (delta)"},
                                       {36, "NC1pi (coh)"},
                                       {-36, "NC1pi (coh)"},
                                       {38, "NC1gamma"},
                                       {-38, "NC1gamma"},
                                       {39, "NCpi (coh)"},
                                       {-39, "NCpi (coh)"},
                                       {41, "NC multi-pi"},
                                       {-41, "NC multi-pi"},
                                       {42, "NC1eta (delta)"},
                                       {-42, "NC1eta (delta)"},
                                       {43, "NC1eta (delta)"},
                                       {-43, "NC1eta (delta)"},
                                       {44, "NC1K (delta)"},
                                       {-44, "NC1K (delta)"},
                                       {45, "NC1K (delta)"},
                                       {-45, "NC1K (delta)"},
                                       {46, "NCDIS"},
                                       {-46, "NCDIS"},
                                       {51, "NC elastic"},
                                       {52, "NC elastic"},
                                       {-51, "NC elastic"},
                                       {-52, "NC elastic"}};