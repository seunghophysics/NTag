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
  fNoiseEventLength(1000e3),
  fNoiseStartTime(1e3), fNoiseEndTime(536e3), fNoiseWindowWidth(536e3),
  fNoiseT0(0),
  fMinHitsLimit(50e3), fMaxHitsLimit(100e3),
  fMinHitDensity(10e-3), fMaxHitDensity(50e-3), // hits per nanosecond
  fPMTDeadtime(900),
  fCurrentHitID(0),
  fCurrentEntry(-1), fNEntries(0),
  fPartID(0), fNParts(2),
  fDoRepeat(true),
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

    fMsg.Print(Form("Noise type: " + fNoiseType));
    fMsg.Print(Form("Noise range: [%3.2f, %3.2f] usec (T_trigger=0)", fNoiseStartTime*1e-3-1, fNoiseEndTime*1e-3-1));
    //fMsg.Print(Form("Seed: %d", ranGen.GetSeed()));
    fMsg.Print(Form("3-sigma hit density range: (%3.1f, %3.1f) hits/us", fMinHitDensity, fMaxHitDensity));
    fMsg.Print(Form("Total dummy trigger entries: %d", fNoiseTree->GetEntries(fNoiseCut)));
    fMsg.Print(Form("PMT deadtime: %3.2f ns", fPMTDeadtime));
    fMsg.Print(Form("Repetition allowed? %s", (fDoRepeat ? "yes" : "no")));
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
    fTQReal = 0; fNoiseTree->SetBranchAddress("TQREAL", &fTQReal);
    fHeader = 0; fNoiseTree->SetBranchAddress("HEADER", &fHeader);
    fNEntries = fNoiseTree->GetEntries();

    fNoiseTree->GetEntry();

    tree->Draw(Form("TQREAL.nhits>>hNHits(500, %f, %f)", fMinHitsLimit, fMaxHitsLimit), fNoiseCut, "", 1000);
    TH1F* hNHits = (TH1F*)gROOT->FindObject("hNHits");
    std::cout << hNHits->GetEntries() << std::endl;
    TF1* gausFunc = new TF1("gaus", "gaus", fMinHitsLimit, fMaxHitsLimit);
    gausFunc->SetParLimits(0, 0, 1000);
    gausFunc->SetParLimits(1, fMinHitsLimit, fMaxHitsLimit);
    gausFunc->SetParLimits(2, 0, (fMaxHitsLimit-fMinHitsLimit)/2.);
    TFitResultPtr gausFit = hNHits->Fit(gausFunc, "S", "goff");
    const double* fitParams = gausFit->GetParams();
    double fitMean = fitParams[1]; double fitSigma = fitParams[2];

    float leftEdge = fitMean - 3 * fitSigma;
    float rightEdge = fitMean + 3 * fitSigma;

    fMinHitDensity = leftEdge / 1000.;
    fMaxHitDensity = rightEdge / 1000.;

    //fMsg.Print(Form("Noise hits per entry: %3.2f +- %3.2f hits", fitMean, fitSigma));
    //fMsg.Print(Form("Total entries in noise tree: %d", fNEntries));
}

void NoiseManager::DumpNoiseFileList(TString pathToList)
{
    std::ofstream outFile;
    outFile.open(pathToList.Data(), std::ios::out);

    // dump settings
    outFile << "TNOISESTART " << (fNoiseStartTime-1000)*1e-3 << "\n";
    outFile << "TNOISEEND " << (fNoiseEndTime-1000)*1e-3 << "\n";
    //outFile << "PMTDEADTIME " << fPMTDeadtime << "\n";
    //outFile << "MINHITDENSITY " << fMinHitDensity << "\n";
    //outFile << "MAXHITDENSITY " << fMaxHitDensity << "\n";
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
        //if (option=="PMTDEADTIME")   fPMTDeadtime  = value;
        //if (option=="MINHITDENSITY") fMinHitDensity  = value;
        //if (option=="MAXHITDENSITY") fMaxHitDensity  = value;
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
    auto inputNoise  = settings.GetString("in_noise");
    auto noiseList   = settings.GetString("dump_noise");
    auto tNoiseStart = settings.GetFloat("TNOISESTART", 0);
    auto tNoiseEnd   = settings.GetFloat("TNOISEEND", 535);
    auto noiseSeed   = settings.GetInt("NOISESEED");
    auto debug       = settings.GetBool("debug", false);

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
    if (debug) SetVerbosity(pDEBUG);

    DumpSettings();
}

void NoiseManager::GetNextNoiseEvent()
{
    fPartID = 0; fCurrentHitID = 0;

    fCurrentEntry++; fNoiseEventHits.Clear();
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
    fT = fTQReal->T;
    fQ = fTQReal->Q;
    fI = fTQReal->cables;
    unsigned int nRawHits = fT.size();

    for (unsigned int j=0; j<=nRawHits; j++) {
        if (-1000e3 < fT[j] && fT[j] < 1000e3)
            fNoiseEventHits.Append({fT[j], fQ[j], fI[j]&0x0000FFFF, 2/*in-gate flag*/});
        else
            fMsg.Print(Form("Skipping hit with time T=%3.2f msec which is outside of range [-1, 1] msec...", fT[j]*1e-6), pWARNING);
    }

    fNoiseEventHits.Sort();
    int nHits = fNoiseEventHits.GetSize();
    fNoiseEventLength = fNoiseEventHits[nHits-1].t() - fNoiseEventHits[0].t();
    float rawHitDensity = nRawHits / 1000.;
    fNParts = (int)(fNoiseEventLength / fNoiseWindowWidth);

    if (fNParts <= 0 || rawHitDensity < fMinHitDensity || rawHitDensity > fMaxHitDensity) {
        fMsg.Print(Form("Skipping inappropriate entry with fNParts = %d, rawHitDensity = %3.2f hits/us\n", fNParts, rawHitDensity), pWARNING, false);
        GetNextNoiseEvent();
    }

    fNoiseT0 = fT[0] + (fNoiseEventLength - fNParts*fNoiseWindowWidth)/2.;
}

void NoiseManager::AddNoise(PMTHitCluster* signalHits)
{
    if (fCurrentEntry == -1 || fPartID == fNParts) {
        GetNextNoiseEvent();
        fMsg.Print(Form("Current noise entry: %d", fCurrentEntry), pDEBUG);
    }

    float partStartTime = fNoiseT0 + fPartID * fNoiseWindowWidth;
    float partEndTime = partStartTime + fNoiseWindowWidth;

    while (fNoiseEventHits[fCurrentHitID].t() < partStartTime) fCurrentHitID++;

    while (fNoiseEventHits[fCurrentHitID].t() < partEndTime) {
        PMTHit hit = fNoiseEventHits[fCurrentHitID];
        hit += (fNoiseStartTime - partStartTime);
        signalHits->Append(hit);
        fCurrentHitID++;
    }

    signalHits->ApplyDeadtime(fPMTDeadtime);
    signalHits->Sort();
    fPartID++;
}