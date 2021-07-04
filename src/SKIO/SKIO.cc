#include <skheadC.h>
#undef MAXHWSK
#include <fortran_interface.h>
#undef MAXPM
#undef MAXPMA

#include "SKIO.hh"

bool SKIO::fIsZEBRAInitialized = false;

SKIO::SKIO()
: fIOMode(mInput), fFileFormat(mZBS), fFilePath(""), fSKOption("31,30,26,25"), fSKGeometry(5), fSKBadChOption(0), fRefRunNo(62428),
fNEvents(0), fCurrentEventID(0), fIsFileOpen(false)
{}

SKIO::SKIO(const char* fileName, IOMode mode)
: SKIO()
{
    SetFilePath(fileName);
    fIOMode = mode;
    
    if (fIOMode == mInput) {
        GetNumberOfEvents();
    }
}

SKIO::~SKIO()
{
    if (fIsFileOpen)
        CloseFile();
}

void SKIO::OpenFile()
{
    if (fFilePath != "")
        OpenFile(fFilePath, fIOMode);
    else
        std::cerr << "[SKIO] File path not specified!\n";
}

void SKIO::OpenFile(const char* fileName, IOMode mode)
{
    if (strlen(fileName)) SetFilePath(fileName);
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
            kzinit_();
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
            std::cerr << "[SKIO] Error occurred while opening the ZBS file: " + fFilePath << "\n";
        }
    }

    // SKROOT
    else if (fFileFormat == mSKROOT) {
        if (fIOMode == mInput) {
            skroot_open_read_(&logicalUnit);
            skroot_set_input_file_(&logicalUnit, fFilePath.Data(), fFilePath.Length());
            skroot_init_(&logicalUnit);
        }
        else if (fIOMode == mOutput)
            skroot_open_write_(&logicalUnit, fFilePath.Data(), fFilePath.Length());
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

    fNEvents = 0;
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
        std::cerr << Form("[SKIO] The input eventID (given: %d) to SKIO::ReadEvent should be within (1 <= eventID <= nEvents == %d).\n", eventID, fNEvents);
        
    // valid eventID
    else {
        if (eventID >= fCurrentEventID) {
            for (int i=fCurrentEventID; i<eventID; i++)
                ReadNextEvent();
        }
        else {
            CloseFile();
            OpenFile(fFilePath, fIOMode);
            ReadEvent(eventID);
        }
    }
}

void SKIO::WriteTQREAL(PMTHitCluster& hitCluster)
{
    if (fFileFormat == mZBS)
        FillTQREALBank(hitCluster);
    else if (fFileFormat == mSKROOT)
        FillTQREALBranch(hitCluster);
}

int SKIO::GetNumberOfEvents()
{
    if (!fNEvents) {
        if (!fIsFileOpen)
            OpenFile();
    
        // do skread until eof
        int logicalUnit = fIOMode;
        int readStatus = mReadOK;
        int nEvents = 0;
        while (readStatus == mReadOK) {
            readStatus = skread_(&logicalUnit);
            if (readStatus == mReadOK) nEvents++;
        }
        CloseFile();
        fNEvents = nEvents;
        
        if (!fNEvents) {
            std::cerr << "[SKIO] The input file " + fFilePath + " is empty!\n"; 
        }
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
    std::cout << Form("[SKIO] %s file path: ", (fIOMode==mInput? "Read": "Write")) + fFilePath << "\n";
    std::cout << "[SKIO] SK option: " + fSKOption << "\n";
    std::cout << "[SKIO] SK geometry: " << fSKGeometry << "\n";
    std::cout << "[SKIO] SK bad channel option: " << fSKBadChOption << "\n";
    std::cout << "[SKIO] SK reference run number: " << fRefRunNo << "\n";
    std::cout << "\n";
}

void FillCommon(PMTHitCluster& hitCluster)
{
    int nHits = hitCluster.GetSize();
    sktqz_.nqiskz = nHits;
    rawtqinfo_.nqisk_raw = nHits;
    rawtqinfo_.pc2pe_raw = 2.46; // SK5
    
    for (int iHit=0; iHit<nHits; iHit++) {
        auto hit = hitCluster[iHit];
        sktqz_.tiskz[iHit] = hit.t();
        sktqz_.qiskz[iHit] = hit.q();
        sktqz_.icabiz[iHit] = hit.i();
        sktqz_.ihtiflz[iHit] = hit.f()<<16;
        
        rawtqinfo_.icabbf_raw[iHit] = hit.i() + (hit.f()<<16);
        rawtqinfo_.tbuf_raw[iHit] = hit.t() + (skheadqb_.it0xsk - skheadqb_.it0sk) / COUNT_PER_NSEC;
        rawtqinfo_.qbuf_raw[iHit] = hit.q();
    }
}

void FillTQREALBank(PMTHitCluster& hitCluster)
{
    FillCommon(hitCluster);
    write_tq_();   
}

void FillTQREALBranch(PMTHitCluster& hitCluster)
{
    FillCommon(hitCluster);
    
    int logicalUnit = 20;
    skroot_set_tree_(&logicalUnit);
    skroot_fill_tree_(&logicalUnit);
    skroot_clear_(&logicalUnit);
}