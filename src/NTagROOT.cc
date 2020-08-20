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
    int eventID = 0;
    bool bEOF = false;
    
    while (!bEOF) {

        Clear();
        readStatus = skread_(&lun);
        CheckMC();
				cout<<" Initial read  "<<sktqz_.nqiskz<<"  "<<sktqz_.tiskz[0]<<endl;
        
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

    if (!bData) {
    	ClearOutputVariable();

    	SetEventHeader();
      SetMCInfo();
    	SetLowFitInfo();

			//Set Time setting	
			SetT0Threshold(2.);// [us]
			SetTEndLimit(5.35e5);// [ns]
			SetTOffset(0.);// [ns]
    }
		else {
    	SetEventHeader();

			if ((skhead_.idtgsk & 1<<28)) {//SHE event
        ClearOutputVariable();
    		SetLowFitInfo();
				SetT0Threshold(2.);// [us]
				SetTEndLimit(4.e4);// [ns]
				SetTOffset(0.);// [ns]
				
				SetSHEFlag(true);
			}
			else if ((skhead_.idtgsk & 1<<29)) {//AFT event
				if (PreEvent - nev != -1) {
					SetPreEvent(nev);
					return;
				}
				SetT0Threshold(0.);// [us]
				SetTEndLimit(5.e5);// [ns]
				SetTOffset(3.5e4);// [ns]
				
				SetSHEFlag(false);
			}
		}
    
    SetToFSubtractedTQ();

    SearchCaptureCandidates();
    GetTMVAoutput();


    if (bData) {
			if (bSHEFlag) {
				SetSaveWait(true);
				SetPreEvent(nev);
			} 
			else {
				ntvarTree->Fill();
				SetSaveWait(false);
			}
		}

    if (!bData) {
			ntvarTree->Fill();
			truthTree->Fill();
		}

		SetPreEvent(nev);
}
