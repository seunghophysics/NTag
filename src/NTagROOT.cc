#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <algorithm>
#include <iterator>

#include <TFile.h>

#include <skroot.h>
#undef MAXHWSK
#include <skheadC.h>
#include <sktqC.h>
#include <skvectC.h>
#include <apscndryC.h>

#include "SKLibs.hh"
#include "NTagROOT.hh"

NTagROOT::NTagROOT(const char* inFileName, const char* outFileName, Verbosity verbose)
: NTagIO(inFileName, outFileName, verbose) { Initialize(); SetVertexMode(mBONSAI); }

NTagROOT::~NTagROOT() {}

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
    int lun = 10;

    TreeManager* mgr  = skroot_get_mgr(&lun);
    SecondaryInfo* SECONDARY = mgr->GetSECONDARY();
    mgr->GetEntry();

    secndprt_.nscndprt = SECONDARY->nscndprt;

    std::copy(std::begin(SECONDARY->iprtscnd), std::end(SECONDARY->iprtscnd), std::begin(secndprt_.iprtscnd));
    std::copy(std::begin(SECONDARY->iprntprt), std::end(SECONDARY->iprntprt), std::begin(secndprt_.iprntprt));
    std::copy(std::begin(SECONDARY->lmecscnd), std::end(SECONDARY->lmecscnd), std::begin(secndprt_.lmecscnd));
    std::copy(std::begin(SECONDARY->tscnd), std::end(SECONDARY->tscnd), std::begin(secndprt_.tscnd));

    std::copy(&SECONDARY->vtxscnd[0][0], &SECONDARY->vtxscnd[0][0] + 3*SECMAXRNG, &secndprt_.vtxscnd[0][0]);
    std::copy(&SECONDARY->pscnd[0][0], &SECONDARY->pscnd[0][0] + 3*SECMAXRNG, &secndprt_.pscnd[0][0]);
}

void NTagROOT::SetFitInfo()
{
    SetLowFitInfo();
}