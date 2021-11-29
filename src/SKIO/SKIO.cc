#include <skheadC.h>
#undef MAXHWSK
#include <fortran_interface.h>
#undef MAXPM
#undef MAXPMA

#include "nbnkC.h"

#include "SKLibs.hh"
#include "SKIO.hh"

bool SKIO::fIsZEBRAInitialized = false;

SKIO::SKIO()
: fIOMode(mInput), fFileFormat(mZBS), fFilePath(""), fSKOption("31,30,26,25"), fSKGeometry(5), fSKBadChOption(0), fRefRunNo(62428),
fNEvents(0), fCurrentEventID(0), fIsFileOpen(false), fMsg("SKIO")
{}

SKIO::SKIO(std::string fileName, IOMode mode)
: SKIO()
{
    SetFilePath(fileName); // sets file format also
    fIOMode = mode;
    
    if (fFileFormat == mSKROOT) {
        SuperManager* superManager = SuperManager::GetManager();
        superManager->CreateTreeManager(mInput, fFilePath.Data(), "0", 0);
    }

    if (fIOMode == mInput) {
        GetNumberOfEvents();
    }
    //else if (fIOMode == mOutput) {
    //    OpenFile();
    //}
}

SKIO::~SKIO()
{
    if (fIsFileOpen)
        CloseFile();
}

void SKIO::OpenFile()
{
    if (fFilePath != "")
        OpenFile(fFilePath.Data(), fIOMode);
    else
        fMsg.Print("File path not specified!", pERROR);
}

void SKIO::OpenFile(std::string fileName, IOMode mode)
{
    if (!fileName.empty()) SetFilePath(fileName);
    else fMsg.Print("The given input file at " + fFilePath + " is empty!", pERROR);
    fIOMode = mode;

    // SK option
    skoptn_(fSKOption.Data(), fSKOption.Length());

    // SK geometry
    skheadg_.sk_geometry = fSKGeometry; geoset_();

    // SK custom bad channel masking (M. Harada)
    // (SK option 25: mask bad channel)
    // (SK option 26: read bad channel from input file)
    if ( !fSKOption.Contains("25") && !fSKOption.Contains("26") ) {
        int refSubRunNo = 0; int outputErrorStatus = 0;
        skbadch_(&fRefRunNo, &refSubRunNo, &outputErrorStatus);
        skbadopt_(&fSKBadChOption);
    }

    //int logicalUnit = fFileFormat==mZBS ? fIOMode : mInput;
    int logicalUnit = fIOMode;

    // ZBS
    if (fFileFormat == mZBS) {

        // Initialize ZEBRA
        if (!fIsZEBRAInitialized) {
            zbsinit_();
            //int iLimit = 4000000;
            //kzlimit_(&iLimit);
            fIsZEBRAInitialized = true;
        }

        // Set rflist and open file
        int fileIndex = 1;
        int openError;
        const char* ioMode = fIOMode==mInput ? "RED" : "WRT";
        const char* comment = fIOMode==mInput ? "recl=5670 status=old" : "recl=5670 status=new";

        set_rflist_(&logicalUnit, fFilePath.Data(),
                    "LOCAL", "", ioMode, "", "", comment, "", "",
                    fFilePath.Length(), 5, 0, 3, 0, 0, 20, 0, 0);
        skopenf_(&logicalUnit, &fileIndex, "Z", &openError);

        if (openError) {
            fMsg.Print("SKOPENF returned error status while opening the input ZBS: " + fFilePath, pERROR);
        }
    }

    // SKROOT
    else if (fFileFormat == mSKROOT) {
        if (fIOMode == mInput) {
            kzinit_();

            skroot_open_read_(&logicalUnit);
            skroot_set_input_file_(&logicalUnit, fFilePath.Data(), fFilePath.Length());
            skroot_init_(&logicalUnit);
        }
        else if (fIOMode == mOutput) {
            logicalUnit = mInput;
            skroot_open_(&logicalUnit, fFilePath.Data(), fFilePath.Length());
            logicalUnit = mOutput;
        }

    }

    fIsFileOpen = true;
}

void SKIO::CloseFile()
{
    int logicalUnit = fIOMode;

    if (fFileFormat == mZBS) {
        skclosef_(&logicalUnit);
    }
    else if (fFileFormat == mSKROOT) {
        skroot_close_(&logicalUnit);
        skroot_end_();
    }

    //fNEvents = 0;
    fCurrentEventID = 0;
    fIsFileOpen = false;
}

int SKIO::ReadNextEvent()
{
    int logicalUnit = fIOMode;
    int readStatus = skread_(&logicalUnit);
    if (readStatus == mReadOK) fCurrentEventID++;
    return readStatus;
}

int SKIO::ReadEvent(int eventID)
{
    int readStatus = 0;

    // invalid eventID
    if ((eventID < 1) || (fNEvents < eventID))
        fMsg.Print(Form("The input eventID (given: %d) to SKIO::ReadEvent should be within (1 <= eventID <= nEvents == %d).\n",
                        eventID, fNEvents), pERROR);

    // valid eventID
    else {
        if (eventID >= fCurrentEventID) {
            for (int i=fCurrentEventID; i<eventID; i++)
                readStatus = ReadNextEvent();
        }
        else {
            CloseFile();
            OpenFile(fFilePath.Data(), fIOMode);
            readStatus = ReadEvent(eventID);
        }
    }

    return readStatus;
}

void SKIO::FillTQREAL(PMTHitCluster& hitCluster)
{
    if (fFileFormat == mZBS) {
        hitCluster.FillCommon();
        filltqreal_();
        //WriteZBS();
    }
    else if (fFileFormat == mSKROOT) {
        int logicalUnit = mInput;
        skroot_get_entry_(&logicalUnit); // get tree entry from input ROOT
        hitCluster.FillCommon();
        skroot_set_tree_(&logicalUnit);  // common header, tqreal, tqareal to ROOT
        skroot_fill_tree_(&logicalUnit);
        skroot_clear_(&logicalUnit);
    }
}

void SKIO::Write()
{
    if (fFileFormat==mZBS) {
        int lun = 20;
        kzwrit_(&lun); kzeclr_();
    }
}

int SKIO::GetNumberOfEvents()
{
    if (!fNEvents && !fIsFileOpen) {

        int logicalUnit = fIOMode;
        int nEvents = 0;

        OpenFile();

        if (fFileFormat == mZBS) {
            // do skread until eof
            int readStatus = mReadOK;

            std::cout << "\n";
            fMsg.Print("Counting the number of events in the input file...\n");
            while (readStatus == mReadOK) {
                readStatus = skread_(&logicalUnit);
                if (readStatus == mReadOK) nEvents++;
                std::cout << "[SKIO] Number of events: " << nEvents << "\r";
            }

            //CloseFile();
            //OpenFile();
        }

        else if (fFileFormat == mSKROOT) {
            skroot_get_entries_(&logicalUnit, &nEvents);
        }

        fNEvents = nEvents;

        if (!fNEvents) {
            fMsg.Print("The given input file at " + fFilePath + " is empty!", pERROR);
        }

        if (fIsFileOpen) CloseFile();
    }

    return fNEvents;
}

int SKIO::GetCurrentEventID()
{
    return fCurrentEventID;
}

void SKIO::DumpSettings()
{
    std::cout << "\n";
    fMsg.Print(Form("%s file path: ", (fIOMode==mInput? "Read": "Write")) + fFilePath);
    fMsg.Print(Form("SK geometry: %d", fSKGeometry));
    fMsg.Print("SK option: " + fSKOption);
    fMsg.Print("SK bad channel option: " + fSKBadChOption);
    fMsg.Print(Form("SK reference run number: %d", fRefRunNo));
    std::cout << "\n";
}

void ClearTQCommon()
{
    for (int iHit=0; iHit<30*MAXPM; iHit++) {
        sktqz_.nqiskz = 0;
        sktqz_.tiskz[iHit] = 0;
        sktqz_.qiskz[iHit] = 0;
        sktqz_.icabiz[iHit] = 0;
        sktqz_.ihtiflz[iHit] = 0;

        rawtqinfo_.nqisk_raw = 0;
        rawtqinfo_.tbuf_raw[iHit] = 0;
        rawtqinfo_.qbuf_raw[iHit] = 0;
        rawtqinfo_.icabbf_raw[iHit] = 0;
    }
}