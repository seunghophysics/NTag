#include <math.h>
#include <stdlib.h>

#include <TFile.h>

#include <skheadC.h>
#include <skvectC.h>

#include <SKLibs.hh>
#include "NTagZBS.hh"

NTagZBS::NTagZBS(const char* inFileName, const char* outFileName, bool useData, unsigned int verbose)
: NTagIO(inFileName, outFileName, useData, verbose), lun(10) {}

NTagZBS::~NTagZBS() { bonsai_end_(); }

void NTagZBS::Initialize()
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

void NTagZBS::OpenFile()
{
    kzinit_();

    // Set rflist and open file
    int ipt = 1;
    int openError;

    set_rflist_(&lun, fInFileName, "LOCAL", "", "RED", "", "", "recl=5670 status=old", "", "",
                strlen(fInFileName),5,0,3,0,0,20,0,0);
    skopenf_(&lun, &ipt, "Z", &openError);

    if (openError)
        PrintMessage("File open error.", pERROR);

    // Set SK options and SK geometry
    const char* skoptn = "31,30,26,25"; skoptn_(skoptn, strlen(skoptn));
    skheadg_.sk_geometry = 4; geoset_();

    // Initialize BONSAI
    bonsai_ini_();
}

void NTagZBS::ReadFile()
{
    // Read data event-by-event
    int readStatus;
    int eventID = 0;
    bool bEOF = false;
    
    while (!bEOF) {
        Clear();
        readStatus = skread_(&lun);
        CheckMC();
        
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
                skclosef_(&lun);
                bEOF = true;
                break;
        }
    }
}

void NTagZBS::ReadEvent()
{
    if (!bData) {
        SetMCInfo();
    }
    
    SetEventHeader();
    SetAPFitInfo();
    SetToFSubtractedTQ();

    SearchCaptureCandidates();
    GetTMVAoutput();


    ntvarTree->Fill();
    if (!bData) truthTree->Fill();
}