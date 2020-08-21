#include <math.h>
#include <string.h>
#include <stdlib.h>

#include <TFile.h>

#include <skroot.h>
#undef MAXHWSK
#include <skheadC.h>
#include <sktqC.h>
#include <skvectC.h>

#include <SKLibs.hh>
#include <NTagROOT.hh>

NTagROOT::NTagROOT(const char* inFileName, const char* outFileName, bool useData, unsigned int verbose)
: NTagIO(inFileName, outFileName, useData, verbose), lun(10) {}

NTagROOT::~NTagROOT() { bonsai_end_(); }

void NTagROOT::Initialize()
{
    // default setting
    NTagIO::Initialize();

    // custom settings
    SetN10Limits(7, 60);
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
    skroot_open_read_(&lun);
    skroot_set_input_file_(&lun, fInFileName, strlen(fInFileName));
    skroot_init_(&lun);

    // Set SK options and SK geometry
    const char* skoptn = "31,30,26,25"; skoptn_(skoptn, strlen(skoptn));
    skheadg_.sk_geometry = 4; geoset_();
    
    // Initialize BONSAI
    kzinit_();
    bonsai_ini_();
}

void NTagROOT::ReadFile()
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
                                 nProcessedEvents), pDEBUG);
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
                skroot_close_(&lun);
                skroot_end_();
                bEOF = true;
                
                PrintMessage(Form("Number of processed events: %d", nProcessedEvents), pDEFAULT);
                Timer("Reading this file", startTime, pDEFAULT);
                break;
        }
    }
}

void NTagROOT::ReadEvent()
{
    if (bData) ReadDataEvent(); // Data
    else       ReadMCEvent();   // MC
}

void NTagROOT::ReadMCEvent()
{
    // DONT'T FORGET TO CLEAR!
    Clear();
    
    // MC-only truth info
    SetMCInfo();
    
    // Prompt-peak info
    SetEventHeader();
    SetLowFitInfo();
    
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

void NTagROOT::ReadDataEvent()
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

void NTagROOT::ReadSHEEvent()
{
    // DONT'T FORGET TO CLEAR!
    Clear();
    
    // Prompt-peak info
    SetEventHeader();
    SetLowFitInfo();
    
    // Hit info (SHE: close-to-prompt hits only)
    AppendRawHitInfo();
}

void NTagROOT::ReadAFTEvent()
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