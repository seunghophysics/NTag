#include <stdlib.h>

#include <TFile.h>

#include <skheadC.h>
#include <skvectC.h>

#include "SKLibs.hh"
#include "NTagZBS.hh"

NTagZBS::NTagZBS(const char* inFileName, const char* outFileName, Verbosity verbose)
: NTagIO(inFileName, outFileName, verbose) { Initialize(); SetVertexMode(mAPFIT); }

NTagZBS::~NTagZBS() { bonsai_end_(); }

void NTagZBS::OpenFile()
{
    // Set rflist and open file
    int ipt = 1;
    int openError;

    set_rflist_(&lun, fInFileName, "LOCAL", "", "RED", "", "", "recl=5670 status=old", "", "",
                strlen(fInFileName),5,0,3,0,0,20,0,0);
    skopenf_(&lun, &ipt, "Z", &openError);

    if (openError)
        msg.Print("File open error.", pERROR);
}

void NTagZBS::CloseFile()
{
    skclosef_(&lun);
}

void NTagZBS::SetFitInfo()
{
    SetAPFitInfo();
}