#include <fstream>

#include <TROOT.h>
#include <TF1.h>
#include <TH1F.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>
#include <TChain.h>
#include <TChainElement.h>
#include <TRandom3.h>

#include <tqrealroot.h>
#undef MAXPM
#undef MAXPMA

#include <Calculator.hh>
#include <SKIO.hh>
#include "NoiseManager.hh"

NoiseManager::NoiseManager()
: fNoiseTree(0), fNoiseTreeName("data"),
  fNoisePath("/disk02/calib3/usr/han/dummy"), fNoiseType("sk6"), 
  fNoiseCut(Form("HEADER.idtgsk & %d || HEADER.idtgsk == %d", mRandomWide, mT2KDummy)),
  fNoiseSeed(0),
  fNoiseEventLength(1000e3),
  fNoiseStartTime(1e3), fNoiseEndTime(536e3), fNoiseWindowWidth(536e3),
  fNoiseT0(0),
  fIDMaxN200(60), fODMaxN200(20
  ),
  //fMinHitsLimit(50e3), fMaxHitsLimit(100e3),
  //fMinHitDensity(10e-3), fMaxHitDensity(50e-3), // hits per nanosecond
  fPMTDeadtime(900), fIDDarkRatekHz(7.5), fODDarkRatekHz(4.0),
  fCurrentIDHitIndex(0), fCurrentODHitIndex(0),
  fCurrentEntry(-1), fNEntries(0),
  fPartID(0), fNParts(2),
  fDoRepeat(true), fDoN200Cut(false),
  fMsg("NoiseManager")
{}

NoiseManager::NoiseManager(TChain* tree)
: NoiseManager()
{
    SetNoiseTree(tree);
}

NoiseManager::~NoiseManager()
{
    if (fNoiseTree) delete fNoiseTree;
}

void NoiseManager::SetNoiseTimeRange(float startTime, float endTime)
{
    fNoiseStartTime = startTime*1e3 + 1000;
    fNoiseEndTime = endTime*1e3 + 1000;
    fNoiseWindowWidth = fNoiseEndTime - fNoiseStartTime;
    fNParts = (int)( fNoiseEventLength / fNoiseWindowWidth);
}

void NoiseManager::DumpSettings()
{
    fMsg.PrintBlock("NoiseManager settings");

    if (fNoiseTree) {
        fMsg.Print(Form("Noise type: " + fNoiseType));
        fMsg.Print(Form("Total dummy trigger entries: %d", fNoiseTree->GetEntries(fNoiseCut)));
        fMsg.Print(Form("Repetition allowed? %s", (fDoRepeat ? "yes" : "no")));
        if (fDoN200Cut) fMsg.Print(Form("Noise MaxN200: %d (ID), %d (OD)", fIDMaxN200, fODMaxN200));
    }
    else {
        fMsg.Print(Form("ID dark rate: %3.2f kHz", fIDDarkRatekHz));
        fMsg.Print(Form("OD dark rate: %3.2f kHz", fODDarkRatekHz));
    }
    fMsg.Print(Form("Noise range: [%3.2f, %3.2f] usec (T_trigger=0)", fNoiseStartTime*1e-3-1, fNoiseEndTime*1e-3-1));
    fMsg.Print(Form("Seed: %d", fNoiseSeed));
    fMsg.Print(Form("PMT deadtime: %3.2f ns", fPMTDeadtime));
    std::cout << "\n";
}

void NoiseManager::AddNoiseFileToChain(TChain* chain, TString noiseFilePath)
{
    static std::vector<TString> usedFilesList;

    TFile* dummyFile = 0;
    TTree* tree = 0;
    int nAddedEntries = 0;

    dummyFile = TFile::Open(noiseFilePath);
    tree = (TTree*)(dummyFile->Get(fNoiseTreeName));

    if (tree) {
        nAddedEntries = tree->GetEntries(fNoiseCut);
        dummyFile->Close();
    }

    if (nAddedEntries && FindIndex(usedFilesList, noiseFilePath)<0) {
        fMsg.Print(Form("Adding dummy file at ") + noiseFilePath + Form(": %d entries", nAddedEntries));
        chain->Add(noiseFilePath);
        fNEntries += nAddedEntries;
        usedFilesList.push_back(noiseFilePath);
    }
}

void NoiseManager::SetNoiseTree(TChain* tree)
{
    fNoiseTree = tree;
    fIDTQReal = 0; fNoiseTree->SetBranchAddress("TQREAL", &fIDTQReal);
    fODTQReal = 0; fNoiseTree->SetBranchAddress("TQAREAL", &fODTQReal);
    fHeader = 0; fNoiseTree->SetBranchAddress("HEADER", &fHeader);
    fNEntries = fNoiseTree->GetEntries();

    fNoiseTree->GetEntry();
}

void NoiseManager::DumpNoiseFileList(TString pathToList)
{
    std::ofstream outFile;
    outFile.open(pathToList.Data(), std::ios::out);

    // dump settings
    outFile << "TNOISESTART " << (fNoiseStartTime-1000)*1e-3 << "\n";
    outFile << "TNOISEEND "   << (fNoiseEndTime-1000)*1e-3 << "\n";
    outFile << "PMTDEADTIME " << fPMTDeadtime << "\n";
    outFile << "noise_cut "   << fDoN200Cut << "\n";
    outFile << "IDMAXN200 "   << fIDMaxN200 << "\n";
    outFile << "ODMAXN200 "   << fODMaxN200 << "\n";
    outFile << "\n";

    // dump file paths
    TObjArray* fileList = fNoiseTree->GetListOfFiles();
    auto nFiles = fileList->GetEntries();
    for (int iFile=0; iFile<nFiles; iFile++) {
        auto filePath = ((TChainElement*)fileList->At(iFile))->GetTitle();
        outFile << filePath << "\n";
    }
    //outFile << "\n";

    outFile.close();
}

void NoiseManager::SetNoiseTreeFromOptions(TString option, int nInputEvents, float tStart, float tEnd, int seed)
{
    fNoiseType = option;
    SetNoiseTimeRange(tStart, tEnd);

    // Read dummy (TChain)
    TChain* dummyChain = new TChain(fNoiseTreeName);

    TObjArray* opt = option.Tokenize(':');
    TString base = ((TObjString*)(opt->At(0)))->GetString();
    TString run = "";
    int minRun = -1;
    int maxRun = 1000000;
    if (option.Contains(":")) {
        run = "0" + ((TObjString*)(opt->At(1)))->GetString();
        if (run.Contains("-")) {
            TObjArray* runRange = run.Tokenize('-');
            minRun = ((TObjString*)(runRange->At(0)))->GetString().Atoi();
            maxRun = ((TObjString*)(runRange->At(1)))->GetString().Atoi();
            run = "";
        }
    }

    SetSeed(seed);
    TString dummyRunPath = fNoisePath + "/" + option;
    int nRequiredEvents = nInputEvents / fNParts;

    std::vector<TString> runDirs = GetListOfSubdirectories(fNoisePath + "/" + base);
    std::vector<TString> fileList;
    if (run != "") fileList = GetListOfFiles(fNoisePath + "/" + base + "/" + run);

    TString dummyFilePath;

    while (fNEntries <= 2*nRequiredEvents) {
        if (run == "") {
            dummyRunPath = PickRandom(runDirs);
            int dummyRunNo = ((TObjString*)((dummyRunPath.Tokenize('/'))->Last()))->GetString().Atoi();
            if (!dummyRunNo) continue; // skip any non-numeric run directory name
            if (dummyRunNo < minRun || maxRun < dummyRunNo ) continue;
            fileList = GetListOfFiles(dummyRunPath, ".root");
        }
        if (!fileList.empty())
            dummyFilePath = PickRandom(fileList);
        AddNoiseFileToChain(dummyChain, dummyFilePath);
    }

    SetNoiseTree(dummyChain);
}

void NoiseManager::SetNoiseTreeFromList(TString pathToList)
{
    std::ifstream inFile;
    inFile.open(pathToList.Data());
    
    // read in settings
    std::string option; float value;
    float startTime = fNoiseStartTime*1e-3 - 1;
    float endTime = fNoiseEndTime*1e-3 - 1;
    while (inFile >> option >> value) {
        if (option=="TNOISESTART")   startTime    = value;
        if (option=="TNOISEEND")     endTime      = value;
        if (option=="PMTDEADTIME")   fPMTDeadtime = value;
        if (option=="noise_cut")      fDoN200Cut   = value;
        if (option=="IDMAXN200")     fIDMaxN200   = value;
        if (option=="ODMAXN200")     fODMaxN200   = value;
    }
    SetNoiseTimeRange(startTime, endTime);

    inFile.clear();
    inFile.seekg(0);

    // read in files
    TChain* dummyChain = new TChain(fNoiseTreeName);
    std::string dummyFilePath;
    while (std::getline(inFile, dummyFilePath)) {
        if (dummyFilePath[0]=='/') 
            AddNoiseFileToChain(dummyChain, dummyFilePath);
    }

    inFile.close();
    SetNoiseTree(dummyChain);
}

void NoiseManager::SetNoiseTreeFromWildcard(TString wildcard, float tStart, float tEnd)
{
    SetNoiseTimeRange(tStart, tEnd);

    TChain* dummyChain = new TChain(fNoiseTreeName);
    dummyChain->Add(wildcard);
    SetNoiseTree(dummyChain);
}

void NoiseManager::ApplySettings(Store& settings, int nInputEvents)
{
    auto noiseType   = settings.GetString("noise_type");
    auto idDarkRate  = settings.GetFloat("IDDARKRATE");
    auto odDarkRate  = settings.GetFloat("ODDARKRATE");
    auto doN200Cut   = settings.GetBool("noise_cut", false);
    auto idMaxN200   = settings.GetInt("IDMAXN200", 60);
    auto odMaxN200   = settings.GetInt("ODMAXN200", 20);
    auto inputNoise  = settings.GetString("in_noise");
    auto noiseList   = settings.GetString("dump_noise");
    auto tNoiseStart = settings.GetFloat("TNOISESTART", 0);
    auto tNoiseEnd   = settings.GetFloat("TNOISEEND", 535);
    auto noiseSeed   = settings.GetInt("NOISESEED");
    auto debug       = settings.GetBool("debug", false);

    SetSeed(noiseSeed);
    SetNoiseMaxN200(idMaxN200, odMaxN200, doN200Cut);
    if (debug) SetVerbosity(pDEBUG);
    if (noiseType == "simulate") {
        SetDarkRate(idDarkRate, odDarkRate);
    }
    else {
        if (!inputNoise.empty()) {
            if (TString(inputNoise).EndsWith(".root"))
                SetNoiseTreeFromWildcard(inputNoise, tNoiseStart, tNoiseEnd);
            else
                SetNoiseTreeFromList(inputNoise);
        }
        else {
            SetNoisePath(settings.GetString("noise_path"));
            SetNoiseTreeFromOptions(noiseType, nInputEvents, tNoiseStart, tNoiseEnd, noiseSeed);
        }
        if (!noiseList.empty())
            DumpNoiseFileList(noiseList);

        SetRepeat(settings.GetBool("repeat_noise", true));
    }
    
    DumpSettings();
}

void NoiseManager::GetNextNoiseEvent()
{
    fPartID = 0; fCurrentIDHitIndex = 0; fCurrentODHitIndex = 0;

    fCurrentEntry++; fIDNoiseEventHits.Clear(); fODNoiseEventHits.Clear();
    if (fCurrentEntry < fNEntries) {
        fNoiseTree->GetEntry(fCurrentEntry);
        if (fHeader->idtgsk & mRandomWide || fHeader->idtgsk == mT2KDummy)
            SetNoiseEventHits();
        else
            GetNextNoiseEvent();
    }
    else {
        fMsg.Print("Noise tree reached its end!", pWARNING);
        if (fCurrentEntry == fNEntries && fDoRepeat) {
            // start from beginning
            fMsg.Print("Repetition allowed: going back to the first entry in noise tree...", pWARNING);
            fCurrentEntry = 0;
            fNoiseTree->GetEntry(fCurrentEntry);
        }
        else {
            // error
            fMsg.Print("Repetition disallowed. To allow, use NoiseManager::SetRepeat(true). Aborting program...", pERROR);
        }
    }
}

void NoiseManager::SetNoiseEventHits()
{
    bool useThisNoiseEvent = true;

    if (fDoN200Cut) {
        // dark selection: OD max N200 <= 20 && ID max N200 <= 50
        auto idHist = Histogram(fIDTQReal->T, 5000, -500e3, 500e3);
        auto odHist = Histogram(fODTQReal->T, 5000, -500e3, 500e3);
        for (int iBin=0; iBin<5000; iBin++) {
            if (odHist[iBin].second > fODMaxN200 | idHist[iBin].second > fIDMaxN200) {
                fMsg.Print(Form("Rejecting noise event with OD N200 %d and ID N200 %d...", 
                                 odHist[iBin].second, idHist[iBin].second), pWARNING);
                useThisNoiseEvent = false; break;
            }
        }
    }

    // populate hit clusters
    if (useThisNoiseEvent) {
        PopulateHitCluster(&fIDNoiseEventHits);
        PopulateHitCluster(&fODNoiseEventHits, true);

        int nHits = fIDNoiseEventHits.GetSize();
        fNoiseEventLength = fIDNoiseEventHits[nHits-1].t() - fIDNoiseEventHits[0].t();
        fNParts = (int)(fNoiseEventLength / fNoiseWindowWidth);
        fNoiseT0 = fIDNoiseEventHits[0].t() + (fNoiseEventLength - fNParts*fNoiseWindowWidth)/2.;
    }

    else GetNextNoiseEvent();
}

void NoiseManager::AddNoise(PMTHitCluster* signalHits, PMTHitCluster* noiseHits, int& currentHitIndex, float darkRate, bool OD)
{
    // noise from noise files
    if (fNoiseTree) {
        if (fCurrentEntry == -1 || fPartID == fNParts) {
            GetNextNoiseEvent();
            fMsg.Print(Form("Current noise entry: %d", fCurrentEntry), pDEBUG);
        }

        float partStartTime = fNoiseT0 + fPartID * fNoiseWindowWidth;
        float partEndTime = partStartTime + fNoiseWindowWidth;

        while (noiseHits->At(currentHitIndex).t() < partStartTime) currentHitIndex++;

        while (noiseHits->At(currentHitIndex).t() < partEndTime) {
            PMTHit hit = noiseHits->At(currentHitIndex);
            hit += (fNoiseStartTime - partStartTime);
            signalHits->Append(hit);
            currentHitIndex++;
        }
    }

    // in case fNoiseTree is empty, simulate noise
    else {
        int nSegments = int(fNoiseWindowWidth / fPMTDeadtime);
        float expected = darkRate * 1e3 * fPMTDeadtime * 1e-9;

        unsigned int iMinPMT = !OD? 1 : 20001;
        unsigned int iMaxPMT = !OD? MAXPM : 20000+MAXPMA;

        for (unsigned int iPMT=iMinPMT; iPMT<=iMaxPMT; iPMT++) {
            for (unsigned int iSeg=0; iSeg<=nSegments; iSeg++) {
                int sampled = ranGen.Poisson(expected);
                if (sampled) {
                    float hitT = fNoiseStartTime + (iSeg + ranGen.Uniform()) * fPMTDeadtime;
                    if (hitT < fNoiseEndTime) {
                        float hitQ = abs(ranGen.Gaus(1, 0.7));
                        PMTHit hit(hitT, hitQ, iPMT, 2/* in-gate */);
                        signalHits->Append(hit);
                    }
                }
            }
        }
    }
    signalHits->ApplyDeadtime(fPMTDeadtime);
    signalHits->Sort();
    fPartID++;
}

void NoiseManager::PopulateHitCluster(PMTHitCluster* hitCluster, bool OD)
{
    std::vector<float> t = !OD? fIDTQReal->T : fODTQReal->T;
    std::vector<float> q = !OD? fIDTQReal->Q : fODTQReal->Q;
    std::vector<int>   i = !OD? fIDTQReal->cables : fODTQReal->cables;

    unsigned int nRawHits = t.size();

    for (unsigned int j=0; j<=nRawHits; j++) {
        if (-1000e3 < t[j] && t[j] < 1000e3)
            hitCluster->Append({t[j], q[j], i[j]&0x0000FFFF, 2/*in-gate flag*/});
        else
            fMsg.Print(Form("Skipping hit with time T=%3.2f msec which is outside of range [-1, 1] msec...", t[j]*1e-6), pDEBUG);
    }

    hitCluster->Sort();
}

void NoiseManager::AddIDNoise(PMTHitCluster* signalHits)
{
    AddNoise(signalHits, &fIDNoiseEventHits, fCurrentIDHitIndex, fIDDarkRatekHz);
}

void NoiseManager::AddODNoise(PMTHitCluster* signalHits)
{
    AddNoise(signalHits, &fODNoiseEventHits, fCurrentODHitIndex, fODDarkRatekHz, true);
}