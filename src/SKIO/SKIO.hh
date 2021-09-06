#ifndef SKIO_HH
#define SKIO_HH

#include <TString.h>

#include <Printer.hh>
#include <PMTHitCluster.hh>

#include "stdlib.h"

extern "C"
{
    void kzbloc_(const char*,int&,size_t);
    void kzbdel_(const char*,size_t);
    void kzbcr0_(const char*,int&,size_t);
    void kzrep0_(const char*,int&,const char*,int&,int *,size_t,size_t);
    void kznsg0_(const char*,int&,size_t);
    void kzget0_(const char*,int&,int&,int*,size_t);
    void kzwrit_(int*);
    void kzeclr_();
}

extern "C" 
{
    void kzinit_();
    void kzlimit_(int*);
    void kzenum_(int*, int*);
    //void kzbloc_(const char*, int*);
    void kzget1_(const char*, int*, int*, void*, int);
    //void kzbdel_(const char*);
    void kzbcr1_(const char*, int*, const char*, int*, int*, const char*, int*, int, int, int);
    void kzrep1_(const char*, int*, int*, void*, int);
    void kzldat_(const char*, int*);
    void lbfcopy_(void*, void*, int*);
    void write_tq_();
    void geoset_();
    void set_rflist_(int*, const char*, const char*, const char*, const char*,
                     const char*, const char*, const char*, const char*, const char*,
                     int, int, int, int, int, int, int, int, int);
    void reset_rflist_();
    void skopenf_(int*, int*, const char*, int*);
    void skoptn_(const char*, int);
    void skbadopt_(int*);
    void skbadch_(int*, int*, int*);
    int  skread_(int*);
    void skclosef_(int*);
    void skroot_init_(int*);
}

enum IOMode
{
    mInput = 10,
    mOutput = 20
};

enum FileFormat
{
    mZBS,
    mSKROOT
};

enum ReadStatus
{
    mReadOK,
    mReadError,
    mReadEOF
};

class SKIO
{
    public:
        SKIO();
        SKIO(std::string fileName, IOMode mode=mInput);
        ~SKIO();

        void OpenFile();
        void OpenFile(std::string fileName, IOMode mode=mInput);
        void CloseFile();
        
        int ReadNextEvent();
        int ReadEvent(int eventID);
        
        void WriteTQREAL(PMTHitCluster& hitCluster);
        
        int GetNumberOfEvents();
        int GetCurrentEventID();
        
        void DumpSettings();
        
        const char* GetFilePath() { return fFilePath.Data(); }
        void SetFilePath(std::string filePath) { fFilePath = filePath; fFileFormat = fFilePath.EndsWith(".root") ? mSKROOT : mZBS; }
        
        const char* GetSKOption() { return fSKOption.Data(); }
        void SetSKOption(const char* skOption) { fSKOption = skOption; }
        
        int GetSKGeometry() { return fSKGeometry; }
        void SetSKGeometry(int skGeometry) { fSKGeometry = skGeometry; }
        
        int GetSKBadChOption() { return fSKBadChOption; }
        void SetSKBadChOption(int skBadChOption) { fSKBadChOption = skBadChOption; }
        
        int GetRefRunNo() { return fRefRunNo; }
        void SetRefRunNo(int refRunNo) { fRefRunNo = refRunNo; }

    private:
        IOMode fIOMode;
        FileFormat fFileFormat;
        TString fFilePath;
        TString fSKOption;
        int fSKGeometry;
        int fSKBadChOption;
        int fRefRunNo;
        
        int fNEvents, fCurrentEventID;
        
        bool fIsFileOpen;
        
        static bool fIsZEBRAInitialized;
        
        Printer fMsg;
};

void FillCommon(PMTHitCluster& hitCluster);
void FillTQREALBank(PMTHitCluster& hitCluster);
void FillTQREALBranch(PMTHitCluster& hitCluster);
void ReadNTagBank();
void WriteNTagBank();

#endif
