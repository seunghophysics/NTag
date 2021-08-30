#ifndef PARTICLETABLE_HH
#define PARTICLETABLE_HH

#include <map>
#include <TString.h>

extern std::map<int, TString> gPIDMap;
extern std::map<int, float> gPIDMassMap;
extern std::map<int, TString> gIntIDMap;
extern std::map<int, int> gG3toG4PIDMap;
extern std::map<int, TString> gNEUTModeMap;
                                    
enum G3IntCode
{
    iDECAY = 5,
    iNCAPTURE = 18
};

enum PDGCode
{
    GAMMA = 22,
    PROTON = 2212,
    NEUTRON = 2112,
    ELECTRON = 11,
    MUON = 13
};

#endif