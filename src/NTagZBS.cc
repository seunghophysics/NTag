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
    skheadg_.sk_geometry = 5; geoset_();

    // Initialize BONSAI
    bonsai_ini_();
}

void NTagZBS::ReadFile()
{
    // Read data event-by-event
    int readStatus;
    bool bEOF = false;
    auto startTime = std::clock();

    while (!bEOF) {

        readStatus = skread_(&lun);
        CheckMC();

        switch (readStatus) {
            case 0: // event read
                std::cout << "\n" << std::endl;
                PrintMessage("###########################", pDEBUG);
                PrintMessage(Form("RUN %d EVENT %d", skhead_.nrunsk, skhead_.nevsk), pDEBUG);
                PrintMessage(Form("Process No. %d", nProcessedEvents+1), pDEBUG);
                PrintMessage("###########################", pDEBUG);

                // If MC
                if (!bData) {
                    int inPMT;
                    skgetv_();
                    inpmt_(skvect_.pos, inPMT);

                    // Skip event with vertex in PMT
                    if (inPMT) {
                        PrintMessage(
                            Form("True vertex is in PMT. Skipping event %d...",
                                 nProcessedEvents+1), pDEBUG);
                        break;
                    }
                }

                ReadEvent();
                nProcessedEvents++;

                break;

            case 1: // read-error
                PrintMessage("FILE READ ERROR OCCURED!", pERROR);
                break;

            case 2: // end of input
                PrintMessage(Form("Reached the end of input. Closing file..."), pDEFAULT);
                skclosef_(&lun);
                bEOF = true;

                PrintMessage(Form("Number of processed events: %d", nProcessedEvents), pDEFAULT);
                Timer("Reading this file", startTime, pDEFAULT);
                break;
        }
    }
}

void NTagZBS::ReadEvent()
{
    if (bData) ReadDataEvent(); // Data
    else       ReadMCEvent();   // MC
}

void NTagZBS::ReadMCEvent()
{
    // DONT'T FORGET TO CLEAR!
    Clear();

    // MC-only truth info
    SetMCInfo();

    // Prompt-peak info
    SetEventHeader();
    SetAPFitInfo();

    // Hit info (all hits)
    AppendRawHitInfo();
    SetToFSubtractedTQ();

    // Tagging starts here!
    SearchCaptureCandidates();
    GetTMVAoutput();

    // DONT'T FORGET TO FILL!
    ntvarTree->Fill();
    truthTree->Fill();
}

void NTagZBS::ReadDataEvent()
{
    // If current event is AFT, append TQ and fill output.
    if (skhead_.idtgsk & 1<<29) {
        PrintMessage("Saving SHE+AFT...", pDEBUG);
        ReadAFTEvent();
    }

    // If previous event was SHE without following AFT,
    // just fill output because there's nothing to append.
    else if (!vTISKZ.empty()) {
        PrintMessage("Saving SHE without AFT...", pDEBUG);
        SetToFSubtractedTQ();

        // Tagging starts here!
        SearchCaptureCandidates();
        GetTMVAoutput();

        ntvarTree->Fill();
    }

    // If current event is SHE,
    // save raw hit info and don't fill output.
    if (skhead_.idtgsk & 1<<28) {
        PrintMessage("Reading SHE...", pDEBUG);
        ReadSHEEvent();
    }
}

void NTagZBS::ReadSHEEvent()
{
    // DONT'T FORGET TO CLEAR!
    Clear();

    // Prompt-peak info
    SetEventHeader();
    SetAPFitInfo();

    // Hit info (SHE: close-to-prompt hits only)
    AppendRawHitInfo();
}

void NTagZBS::ReadAFTEvent()
{
    // Append hit info (AFT: delayed hits after prompt)
    AppendRawHitInfo();
    SetToFSubtractedTQ();

    // Tagging starts here!
    SearchCaptureCandidates();
    GetTMVAoutput();

    // DONT'T FORGET TO FILL!
    // (ntvar only for data)
    ntvarTree->Fill();
}