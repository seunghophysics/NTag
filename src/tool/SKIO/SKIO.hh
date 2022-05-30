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
    void zbsinit_(); // best working solution for opening a large zbs
    void filltqreal_();
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

        void FillTQREAL(PMTHitCluster& hitCluster);
        void Write();

        int GetNumberOfEvents();
        int GetCurrentEventID();

        void DumpSettings();

        const char* GetFilePath() { return fFilePath.Data(); }
        void SetFile(std::string filePath, IOMode mode=mInput);
        FileFormat GetFileFormat() { return fFileFormat; }

        const char* GetSKOption() { return fSKOption.Data(); }
        static void SetSKOption(std::string skOption) { fSKOption = skOption; }
        static void AddSKOption(int opt) { fSKOption += ("," + std::to_string(opt)); }

        static int GetSKGeometry() { return fSKGeometry; }
        static void SetSKGeometry(int skGeometry) { fSKGeometry = skGeometry; }

        static int GetSKBadChOption() { return fSKBadChOption; }
        static void SetSKBadChOption(int skBadChOption) { fSKBadChOption = skBadChOption; }

        static int GetRefRunNo() { return fRefRunNo; }
        static void SetRefRunNo(int refRunNo) { fRefRunNo = refRunNo; }

        static void ClearTQCommon();
        static void SetSecondaryCommon(FileFormat format=mZBS);
        static float GetMCTriggerOffset(FileFormat format=mZBS);

        static bool IsZEBRAInitialized() { return fIsZEBRAInitialized; }

    private:
        IOMode fIOMode;
        FileFormat fFileFormat;
        TString fFilePath;

        int fNEvents, fCurrentEventID;

        bool fIsFileOpen;

        static TString fInFilePath;
        static TString fOutFilePath;
        static bool fIsZEBRAInitialized;

        static TString fSKOption;
        static int fSKGeometry;
        static int fSKBadChOption;
        static int fRefRunNo;

        Printer fMsg;
};


#endif