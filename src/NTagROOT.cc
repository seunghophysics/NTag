#include <math.h>
#include <string.h>
#include <stdlib.h>

#include <TFile.h>

#include <skroot.h>
#undef MAXHWSK
#include <skheadC.h>
#include <sktqC.h>
#include <skvectC.h>

#include "NTagROOT.hh"
#include "SKLibs.hh"

NTagROOT::NTagROOT(const char* inFileName, const char* outFileName, Verbosity verbose)
: NTagIO(inFileName, outFileName, verbose) { Initialize(); SetVertexMode(mBONSAI); }

NTagROOT::~NTagROOT() { bonsai_end_(); }

void NTagROOT::OpenFile()
{
    skroot_open_read_(&lun);
    skroot_set_input_file_(&lun, fInFileName, strlen(fInFileName));
    skroot_init_(&lun);
}

void NTagROOT::CloseFile()
{
    skroot_close_(&lun);
    skroot_end_();
}

void NTagROOT::ReadSecondaries()
{
    // test
    msg.Print("Reading secondaries from SKG4...");
    apflscndprt_();
}

void NTagROOT::SetFitInfo()
{
    SetLowFitInfo();
}