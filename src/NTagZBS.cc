#include <math.h>
#include <stdlib.h>

#include <TFile.h>

#include <skheadC.h>
#include <skvectC.h>

#include <SKLibs.hh>
#include "NTagZBS.hh"

NTagZBS::NTagZBS(const char* fileName, bool useData, unsigned int verbose)
: NTagIO(fileName, useData, verbose), lun(10) {}

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
    
    OpenFile(fInFileName);
    ReadFile();
}

void NTagZBS::OpenFile(const char* fileName)
{
    kzinit_();

    // Set rflist and open file
    int ipt = 1;
    int openError;

    set_rflist_(&lun, fileName, "LOCAL", "", "RED", "", "", "recl=5670 status=old", "", "",
                strlen(fileName),5,0,3,0,0,20,0,0);
    skopenf_(&lun, &ipt, "Z", &openError);

    if (openError) {
        std::cerr << "[NTagAnalysis]: File open error." << std::endl;
        exit(1);
    }

    // Set SK options and SK geometry
    const char* skoptn = "31,30,26,25"; skoptn_(skoptn, strlen(skoptn));
    skheadg_.sk_geometry = 4; geoset_();

    // Initialize BONSAI
    bonsai_ini_();
}

void NTagZBS::OpenFile(const char* inFileName, const char* oFileName)
{
    kzinit_();

    // Set rflist and open file
    int ipt = 1;
    int openError;

    set_rflist_(&lun, inFileName, "LOCAL", "", "RED", "", "", "recl=5670 status=old", "", "",
                strlen(inFileName),5,0,3,0,0,20,0,0);
    skopenf_(&lun, &ipt, "Z", &openError);

    if (openError) {
        std::cerr << "[NTagAnalysis]: File open error." << std::endl;
        exit(1);
    }

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
