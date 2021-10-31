#include <skheadC.h>
#undef MAXHWSK
#include <fortran_interface.h>
#undef MAXPM
#undef MAXPMA

#include "nbnkC.h"

#include "SKIO.hh"
bool SKIO::fIsZEBRAInitialized = false;

SKIO::SKIO()
: fIOMode(mInput), fFileFormat(mZBS), fFilePath(""), fSKOption("31,30,26,25"), fSKGeometry(5), fSKBadChOption(0), fRefRunNo(62428),
fNEvents(0), fCurrentEventID(0), fIsFileOpen(false), fMsg("SKIO")
{}

SKIO::SKIO(std::string fileName, IOMode mode)
: SKIO()
{
    SetFilePath(fileName);
    fIOMode = mode;

    if (fIOMode == mInput) {
        GetNumberOfEvents();
    }
    else if (fIOMode == mOutput) {
        OpenFile();
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
            int iLimit = 4000000;
            kzlimit_(&iLimit);
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

        int logicalUnit = fIOMode;
        int nEvents = 0;

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
            OpenFile();
        }

        else if (fFileFormat == mSKROOT) {
            skroot_get_entries_(&logicalUnit, &nEvents);
        }

        fNEvents = nEvents;

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
    fMsg.Print("SK bad channel option: " + fSKBadChOption);
    fMsg.Print(Form("SK reference run number: %d", fRefRunNo));
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
    int logicalUnit = mInput;
    skroot_get_entry_(&logicalUnit); // get tree entry from input ROOT

    FillCommon(hitCluster);

    skroot_set_tree_(&logicalUnit); // common header, tqreal, tqareal to ROOT
    skroot_fill_tree_(&logicalUnit);
    skroot_clear_(&logicalUnit);
}

// copied from $ATMPD_ROOT/src/analysis/neutron/ntag/WriteNtag.cc

void ReadNTagBank()
{
    int version;
  const char * bnkname="NTAG";
  int len=strlen(bnkname);

  int buffsize=8+MAXNP*28;

  char buff[4*buffsize];
  float * fbuff=(float*)buff;
  int * ibuff=(int*)buff;
  int nsg;
  kznsg0_(bnkname,nsg,len);
  if(nsg<0)
    {
      std::cerr<<"No neutron bank exists";
    }
  else {

  int zero=0;
  int ndata;
  kzget0_(bnkname,zero,ndata,ibuff,len);
  int index=0;
  version=ibuff[index++];
  ntag_.nn=ibuff[index++];
  ntag_.trgtype=ibuff[index++];
  ntag_.n200m=ibuff[index++];
  ntag_.lasthit=fbuff[index++];
  ntag_.t200m=fbuff[index++];
  if(version>=1)
    {
      ntag_.np=ibuff[index++];
    }
  if(version>=2)
    {
      ntag_.mctruth_nn=ibuff[index++];
    }
  int num=ntag_.nn;
  if(version>=4) num=ntag_.np;
  for(int i=0;i<num;i++)
    {
      ntag_.ntime[i]=fbuff[index++];
      ntag_.goodness[i]=fbuff[index++];
      for(int j=0;j<3;j++)
    {
      ntag_.nvx[i][j]=fbuff[index++];
    }
      for(int j=0;j<3;j++)
    {
      ntag_.bvx[i][j]=fbuff[index++];
    }
      ntag_.nlow[i]=ibuff[index++];
      ntag_.n300[i]=ibuff[index++];
      ntag_.phi[i]=fbuff[index++];
      ntag_.theta[i]=fbuff[index++];
      ntag_.trmsold[i]=fbuff[index++];
      ntag_.trmsdiff[i]=fbuff[index++];
      ntag_.mintrms6[i]=fbuff[index++];
      ntag_.mintrms3[i]=fbuff[index++];
      ntag_.bswall[i]=fbuff[index++];
      ntag_.bse[i]=fbuff[index++];
      ntag_.fpdis[i]=fbuff[index++];
      ntag_.bfdis[i]=fbuff[index++];
      ntag_.nc[i]=ibuff[index++];
      ntag_.fwall[i]=fbuff[index++];
      ntag_.n10[i]=ibuff[index++];
      ntag_.n10d[i]=ibuff[index++];
      ntag_.t0[i]=fbuff[index++];
      if(version>=2)
    {
      ntag_.mctruth_neutron[i]=ibuff[index++];
    }
      if(version>=3)
    {
      ntag_.bse2[i]=fbuff[index++];
    }
      if(version>=4)
    {
      ntag_.tag[i]=ibuff[index++];
    }

    }

    std::cout << "np: " << ntag_.np << "\n";
    std::cout << "nn: " << ntag_.nn << "\n";
    std::cout << "mctruth_nn: " << ntag_.mctruth_nn << "\n";
    for (int i=0; i<ntag_.np; i++)
    {
        std::cout << "goodness[" << i << "]: " << ntag_.goodness[i];
        std::cout << " mctruth_neutron[" << i << "]: " << ntag_.mctruth_neutron[i];
        std::cout << " tag[" << i << "]: " << ntag_.tag[i] << "\n";
    }
  }
}

void WriteNTagBank()
{
    int version=4;
    const char* bnkname="NTAG";
    int len=strlen(bnkname);

    int buffsize=8+MAXNP*28;
    char buff[4*buffsize];//char = 8 bits, float/int=32 bits
    float * fbuff=(float*) buff;
    int * ibuff=(int*)buff;

    int exi;
    kzbloc_(bnkname,exi,len);
    if(exi!=0) kzbdel_(bnkname,len);

    int ierr;
    kzbcr0_(bnkname,ierr,len);
    int index=0;
    ibuff[index++]=version;
    ibuff[index++]=ntag_.nn;
    ibuff[index++]=ntag_.trgtype;
    ibuff[index++]=ntag_.n200m;
    fbuff[index++]=ntag_.lasthit;
    fbuff[index++]=ntag_.t200m;
    ibuff[index++]=ntag_.np;
    ibuff[index++]=ntag_.mctruth_nn;
    for(int i=0;i<ntag_.np;i++)
    {
        fbuff[index++]=ntag_.ntime[i];
        fbuff[index++]=ntag_.goodness[i];
        for(int j=0;j<3;j++)
        {
          fbuff[index++]=ntag_.nvx[i][j];
        }
        for(int j=0;j<3;j++)
        {
          fbuff[index++]=ntag_.bvx[i][j];
        }
        ibuff[index++]=ntag_.nlow[i];
        ibuff[index++]=ntag_.n300[i];
        fbuff[index++]=ntag_.phi[i];
        fbuff[index++]=ntag_.theta[i];
        fbuff[index++]=ntag_.trmsold[i];
        fbuff[index++]=ntag_.trmsdiff[i];
        fbuff[index++]=ntag_.mintrms6[i];
        fbuff[index++]=ntag_.mintrms3[i];
        fbuff[index++]=ntag_.bswall[i];
        fbuff[index++]=ntag_.bse[i];
        fbuff[index++]=ntag_.fpdis[i];
        fbuff[index++]=ntag_.bfdis[i];
        ibuff[index++]=ntag_.nc[i];
        fbuff[index++]=ntag_.fwall[i];
        ibuff[index++]=ntag_.n10[i];
        ibuff[index++]=ntag_.n10d[i];
        fbuff[index++]=ntag_.t0[i];
        ibuff[index++]=ntag_.mctruth_neutron[i];
        fbuff[index++]=ntag_.bse2[i];
        ibuff[index++]=ntag_.tag[i];
    }


    int zero=0;
    int one=1;
    kzrep0_(bnkname,zero,"I",index,ibuff,len,one);

    int luno = 20;
    kzwrit_(&luno);
    kzeclr_();
}