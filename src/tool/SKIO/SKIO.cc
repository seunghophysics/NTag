#include <skheadC.h>
#undef MAXHWSK
#include <fortran_interface.h>
#include "skbadcC.h"
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

int SKIO::fTmpOut = 0;
int SKIO::fBackupOut = 0;
bool SKIO::fVerbose = false;

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
    //auto woBadOpt = fSKOption.ReplaceAll(",25", "");
    //skoptn_(woBadOpt.Data(), woBadOpt.Length());
    skoptn_(fSKOption.Data(), fSKOption.Length());

    // bad channel options
    skbadopt_(&fSKBadChOption);
    // SK custom bad channel masking (M. Harada)
    // (SK option 25: mask bad channel)
    // (SK option 26: read bad channel from input file)
    //if (fSKOption.Contains("25"))
    //    SetBadChannels(fRefRunNo);

    //int logicalUnit = fFileFormat==mZBS ? fIOMode : mInput;
    int logicalUnit = fIOMode;

    // ZBS
    if (fFileFormat == mZBS) {

        SKIO::DisableConsoleOut();

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

        SKIO::EnableConsoleOut();

        if (openError) {
            fMsg.Print("SKOPENF returned error status while opening the input ZBS: " + fFilePath, pERROR);
        }
    }

    // SKROOT
    else if (fFileFormat == mSKROOT) {
        SKIO::DisableConsoleOut();
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
        SKIO::EnableConsoleOut();
    }

    fIsFileOpen = true;

    if (fIOMode == mInput)
        fNEvents = GetNumberOfEvents();
}

void SKIO::CloseFile()
{
    int logicalUnit = fIOMode;

    SKIO::DisableConsoleOut();
    if (fFileFormat == mZBS) {
        skclosef_(&logicalUnit);
    }
    else if (fFileFormat == mSKROOT) {
        skroot_close_(&logicalUnit);
        skroot_end_();
    }
    SKIO::EnableConsoleOut();

    //fNEvents = 0;
    fCurrentEventID = 0;
    fIsFileOpen = false;
}

int SKIO::ReadNextEvent()
{
    SKIO::DisableConsoleOut();
    int logicalUnit = fIOMode;
    int readStatus = skread_(&logicalUnit);
    if (readStatus == mReadOK) fCurrentEventID++;
    SKIO::EnableConsoleOut();
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
            fMsg.Print("Counting the number of events in the input file...");
            
            SKIO::DisableConsoleOut();
            while (readStatus == mReadOK) {
                readStatus = skread_(&logicalUnit);
                if (readStatus == mReadOK) nEvents++;
                //std::cout << "[SKIO] Number of events: " << nEvents << "\r";
            }
            SKIO::EnableConsoleOut();

            CloseFile();
            fNEvents = nEvents;
            if (wasFileOpen) OpenFile();
        }

        else if (fFileFormat == mSKROOT) {
            skroot_get_entries_(&logicalUnit, &nEvents);
            fNEvents = nEvents;

            TreeManager* mgr  = skroot_get_mgr(&logicalUnit);
            TTree* tree = mgr->GetTree();
            Header* header = mgr->GetHEAD();
            tree->GetEntry(0);
            skhead_.nrunsk = header->nrunsk;
            skhead_.mdrnsk = header->mdrnsk;

            if (!wasFileOpen) CloseFile();
        }

        if (!fNEvents) {
            fMsg.Print("The given input file at " + fFilePath + " is empty!", pERROR);
        }
    }

    //SetSKGeometry(FindSKGeometry());

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
    fMsg.Print(Form("SK geometry: %d", skheadg_.sk_geometry));
    fMsg.Print("SK option: " + fSKOption);
    fMsg.Print("SK bad channel option: " + std::to_string(fSKBadChOption));
    fMsg.Print(Form("SK reference run number: %d", fRefRunNo));
    std::cout << "\n";
}

void SKIO::SetRefRunNo(int refRunNo)
{
    fRefRunNo = refRunNo;
}

int SKIO::SetBadChannels(int runNo, int subrunNo, bool fetchTQ)
{
    int badchError = 0; int darkError = 0;
    combad_.log_level_skbadch = 4; // silent
    comdark_.log_level_skdark = 4; // silent
    SKIO::DisableConsoleOut();
    skbadch_(&runNo, &subrunNo, &badchError);
    skdark_(&runNo, &darkError);
    if (fetchTQ) {
        skbadch_mask_tqz_();
        tqrealsk_();
    }
    SKIO::EnableConsoleOut();
    return (badchError>=0) && (darkError==0);
}

void SKIO::ResetBadChannels()
{
    combad_.nbad = 0;
    combada_.nbada = 0;
    combad00_.nbad0 = 0;

    for (int iPMT=0; iPMT<MAXPM; iPMT++) {
        combad_.ibad[iPMT] = 0;
        combad_.isqbad[iPMT] = 0;
        combad00_.ibad0[iPMT] = 0;
        combad00_.isqbad0[iPMT] = 0;
    }

    for (int iPMT=0; iPMT<MAXPMA; iPMT++) {
        combada_.ibada[iPMT] = 0;
        combada_.isqbada[iPMT] = 0;
    }
}

//int SKIO::FindSKGeometry()
//{
    /*
    // data
    if (skhead_.mdrnsk) {
        if (skhead_.nrunsk < 80000) return 4;
        else if (skhead_.nrunsk < 83000) return 5;
        else if (skhead_.nrunsk < 88000) return 6;
        else return 7;
    }
    // mc (no change)
    else {
        return SKIO::GetSKGeometry();
    }
    */

//   return skheadg_.sk_geometry;
//}

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
        mgr->GetEntry();

        trgOffset = -MCINFO->prim_pret0[0];
    }
    else if (format==mZBS) {
        SKIO::DisableConsoleOut();
        trginfo_(&trgOffset);
        trgOffset -= 1000;
        SKIO::EnableConsoleOut();
    }

    return trgOffset;
}

void SKIO::DisableConsoleOut()
{
    if (!fVerbose) {
        fflush(stdout);
        fBackupOut = dup(1);
        FILE* f = fopen("/dev/null", "w");
        fTmpOut = fileno(f);
        dup2(fTmpOut, 1);
        fclose(f);
    }
}

void SKIO::EnableConsoleOut()
{
    if (!fVerbose)
    {
        fflush(stdout);
        dup2(fBackupOut, 1);
        close(fBackupOut);
    }
}