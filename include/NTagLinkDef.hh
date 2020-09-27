#ifndef NTAGLINKDEF_HH
#define NTAGLINKDEF_HH 1

#include <vector>

#ifdef __CINT__

#pragma link C++ class vector<vector<int> >+;
#pragma link C++ class vector<vector<float> >+;

#endif

#endif