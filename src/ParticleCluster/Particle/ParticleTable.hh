#ifndef PARTICLETABLE_HH
#define PARTICLETABLE_HH

std::map<int, TString> gPIDMap = {{11,        "e-"},
                                  {-11,       "e+"},
                                  {12,        "nu_e"},
                                  {-12,        "anti-nu_e"},
                                  {13,        "mu-"},
                                  {-13,       "mu+"},
                                  {14,        "nu_mu"},
                                  {-14,       "anti-nu_mu"},
                                  {15,        "tau-"},
                                  {-15,       "tau+"},
                                  {16,        "nu_tau"},
                                  {-16,       "anti-nu_tau"},
                                  {22,        "gamma"},
                                  {111,       "pi0"},
                                  {211,       "pi-"},
                                  {-211,      "pi+"},
                                  {130,       "K0L"},
                                  {321,       "K+"},
                                  {-321,      "K-"},
                                  {221,       "eta"},
                                  {2112,      "n"},
                                  {-2112,     "anti-n"},
                                  {2212,      "p"},
                                  {-2212,     "anti-p"},
                                  {100010020, "d"}};
                                  //{10004,     "d"},

std::map<int, TString> gIntIDMap = {{18, "Capture"},
                                    {7 , "Compt."},
                                    {9 , "Brems."},
                                    {10, "Delta"},
                                    {11, "Annihi."},
                                    {12, "Hadr."}};

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
                                    {50, 0}};         // cherenkov

#endif