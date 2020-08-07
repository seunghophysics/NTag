#include <math.h>
#include <string.h>
#include <stdlib.h>

#include <TFile.h>

#include <skroot.h>
#undef MAXHWSK
#include <skheadC.h>
#include <skvectC.h>

#include <SKLibs.hh>
#include <NTagROOT.hh>
//#include "SKLowe_root.h"
//#include "loweroot.h"

NTagROOT::NTagROOT(const char* inFileName, const char* outFileName, bool useData, unsigned int verbose)
: NTagIO(inFileName, outFileName, useData, verbose), lun(10) {}

NTagROOT::~NTagROOT() { bonsai_end_(); }

void NTagROOT::Initialize()
{
    // default setting
    NTagIO::Initialize();

    // custom settings
    SetN10Limits(5, 50);
    SetN200Max(140);
    SetT0Threshold(2.);		 // [us]
    SetDistanceCut(4000.);	 // [cm]
    SetTMatchWindow(40.);	 // [ns]
    SetTPeakSeparation(50.); // [us]

    OpenFile();
    ReadFile();
}

void NTagROOT::OpenFile()
{
    skroot_open_(&lun, fOutFileName, strlen(fOutFileName));
    skroot_set_input_file_(&lun, fInFileName, strlen(fInFileName));
    skroot_initialize_(&lun);

    // Set SK options and SK geometry
    const char* skoptn = "31,30,26,25"; skoptn_(skoptn, strlen(skoptn));
    skheadg_.sk_geometry = 4; geoset_();

    // Initialize BONSAI
    bonsai_ini_();
}

void NTagROOT::ReadFile()
{
    // Read data event-by-event
    int readStatus;
    int eventID = 0;
    bool bEOF = false;

    while (!bEOF) {
        Clear();
        readStatus = skread_(&lun);
        switch (readStatus) {
            case 0: // event read
                eventID++;
                std::cout << "\n" << std::endl;
                PrintMessage("###########################", pDEBUG);
                PrintMessage(Form("Event ID: %d", eventID), pDEBUG);
                PrintMessage("###########################", pDEBUG);
                int inPMT;
                if (!bData) {
                    skgetv_();
                    inpmt_(skvect_.pos, inPMT);
                    if (inPMT) {
                        PrintMessage(
                            Form("True vertex is in PMT. Skipping event %d...",
                                 eventID), pDEBUG);
                    break;
                    }
                }
                ReadEvent();
                break;

            case 1: // read-error
                break;

            case 2: // end of input
                PrintMessage(Form("Reached the end of input. Closing file..."), pDEFAULT);
                skroot_close_(&lun);
                skroot_end_();
                bEOF = true;
                break;
        }
    }
}

void NTagROOT::ReadEvent()
{
    SetEventHeader();
    SetLowFitInfo();
    SetToFSubtractedTQ();

    SearchCaptureCandidates();
    GetTMVAoutput();

    if (!bData) {
        SetMCInfo();
    }

    ntvarTree->Fill();
    if (!bData) truthTree->Fill();
}

