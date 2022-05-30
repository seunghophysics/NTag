#include <skheadC.h>
#undef MAXHWSK
#include <fortran_interface.h>
#undef MAXPM
#undef MAXPMA
#undef SECMAXRNG

#include "apscndryC.h"
#include "nbnkC.h"

#include "SKLibs.hh"
#include "SKIO.hh"

bool SKIO::fIsZEBRAInitialized = false;
TString SKIO::fInFilePath = "";
TString SKIO::fOutFilePath = "";

TString SKIO::fSKOption = "31,30";
int SKIO::fSKGeometry = 5;
int SKIO::fSKBadChOption = 0;
int SKIO::fRefRunNo = 85619;

SKIO::SKIO()
: fIOMode(mInput), fFileFormat(mZBS), fFilePath(""),
  fNEvents(0), fCurrentEventID(0), fIsFileOpen(false), fMsg("SKIO")
{}

SKIO::SKIO(std::string fileName, IOMode mode)
: SKIO()
{
    SetFile(fileName, mode);
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

void SKIO::SetFile(std::string filePath, IOMode mode) 
{ 
    fFilePath = filePath; 
    fFileFormat = fFilePath.EndsWith(".root") ? mSKROOT : mZBS; 
    fIOMode = mode;
    if (fIOMode==mInput)
        fInFilePath = fFilePath;
    else if (fIOMode==mOutput)
        fOutFilePath = fFilePath;
}

void SKIO::OpenFile(std::string fileName, IOMode mode)
{
    if (!fileName.empty()) SetFile(fileName, mode);
    else fMsg.Print("The given file path is an empty string!", pERROR);

    if (fFileFormat==mSKROOT && fIOMode==mOutput) {
        SuperManager* superManager = SuperManager::GetManager();
        if (fInFilePath!="") {
            superManager->DeleteTreeManager(mInput);
            //superManager->CreateTreeManager(mInput, fInFilePath.Data(), fOutFilePath.Data(), 0);
        }
    }

    // SK option
    skoptn_(fSKOption.Data(), fSKOption.Length());

    // SK geometry
    skheadg_.sk_geometry = fSKGeometry; geoset_();

    // SK custom bad channel masking (M. Harada)
    // (SK option 25: mask bad channel)
    // (SK option 26: read bad channel from input file)
    if ( fSKOption.Contains("25") ) {
        int refSubRunNo = 0; int outputErrorStatus = 0;
        skbadopt_(&fSKBadChOption);
        skbadch_(&fRefRunNo, &refSubRunNo, &outputErrorStatus);
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
            skroot_set_input_file_(&logicalUnit, fInFilePath.Data(), fInFilePath.Length());
            skroot_init_(&logicalUnit);
        }

    }

    fIsFileOpen = true;
    
    if (fIOMode == mInput)
        fNEvents = GetNumberOfEvents();
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
    if (fNEvents==0) {
        int logicalUnit = fIOMode;
        int nEvents = 0;

        bool wasFileOpen = fIsFileOpen;
        if (!fIsFileOpen) OpenFile();

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

            CloseFile(); 
            fNEvents = nEvents;
            if (wasFileOpen) OpenFile();
        }

        else if (fFileFormat == mSKROOT) {
            skroot_get_entries_(&logicalUnit, &nEvents);
            fNEvents = nEvents;
            if (!wasFileOpen) CloseFile();
        }

        if (!fNEvents) {
            fMsg.Print("The given input file at " + fFilePath + " is empty!", pERROR);
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
    fMsg.Print(Form("%s file path: ", (fIOMode==mInput? "Read": "Write")) + fFilePath);
    fMsg.Print(Form("SK geometry: %d", fSKGeometry));
    fMsg.Print("SK option: " + fSKOption);
    fMsg.Print("SK bad channel option: " + std::to_string(fSKBadChOption));
    fMsg.Print(Form("SK reference run number: %d", fRefRunNo));
    std::cout << "\n";
}

void SKIO::ClearTQCommon()
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

void SKIO::SetSecondaryCommon(FileFormat format)
{
    if (format==mSKROOT) {
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
    else if (format==mZBS) {
        apflscndprt_();
    }
}

float SKIO::GetMCTriggerOffset(FileFormat format)
{
    float trgOffset = 0;

    if (format==mSKROOT) {
        int lun = 10;

        TreeManager* mgr  = skroot_get_mgr(&lun);
        MCInfo* MCINFO = mgr->GetMC();
        TQReal* TQREAL = mgr->GetTQREALINFO();
        mgr->GetEntry();

        trgOffset = -MCINFO->prim_pret0[0];
    }
    else if (format==mZBS) {
        trginfo_(&trgOffset);
        trgOffset -= 1000;
    }
    
    return trgOffset;
}
