#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TRandom3.h>

#include <tqrealroot.h>
#undef MAXPM
#undef MAXPMA

#include <Calculator.hh>
#include <SKIO.hh>
#include "NoiseManager.hh"

NoiseManager::NoiseManager()
: fNoiseTree(0), fNoiseTreeName("data"),
  fNoiseEventLength(1000e3),
  fNoiseStartTime(0), fNoiseEndTime(535e3), fNoiseWindowWidth(535e3), 
  fNoiseT0(0),
  fPMTDeadtime(900),
  fCurrentHitID(0),
  fCurrentEntry(-1), fNEntries(0),
  fPartID(0), fNParts(2),
  fDoRepeat(false)
{}

NoiseManager::NoiseManager(TString option, int nInputEvents, float tStart, float tEnd, int seed)
: NoiseManager()
{
    SetNoiseTimeRange(tStart, tEnd);
    
    // Read dummy (TChain)
    TChain* dummyChain = new TChain(fNoiseTreeName);
    
    TObjArray* opt = option.Tokenize('/');
    TString base = ((TObjString*)(opt->At(0)))->GetString();
    TString run = "";
    if (option.Contains("/"))
        run = ((TObjString*)(opt->At(1)))->GetString();

    SetSeed(seed);
    TString dummyRunPath = dummyDir + option;
    int nRequiredEvents = nInputEvents / fNParts;
    
    std::vector<TString> runDirs = GetListOfSubdirectories(dummyDir+base);
    std::vector<TString> fileList;
    if (run != "") fileList = GetListOfFiles(dummyDir + option);
    
    TString dummyFilePath;
    
    std::vector<TString> usedFilesList;
    
    while (fNEntries <= nRequiredEvents) {
        if (run == "") {
            dummyRunPath = PickRandom(runDirs);
            fileList = GetListOfFiles(dummyRunPath, ".root");
        }
        if (!fileList.empty())    
            dummyFilePath = PickRandom(fileList);
        AddNoiseFileToChain(dummyChain, dummyFilePath);
    }
    
    SetNoiseTree(dummyChain);
}

NoiseManager::NoiseManager(TTree* tree)
: NoiseManager()
{
    SetNoiseTree(tree);
}

NoiseManager::~NoiseManager()
{
    delete fNoiseTree;
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
        nAddedEntries = tree->GetEntries(dummyCut);
        dummyFile->Close();
    }
    
    if (nAddedEntries && std::find(usedFilesList.begin(), usedFilesList.end(), noiseFilePath) == usedFilesList.end()) {
        std::cout << "[NoiseManager] Adding dummy file at " << noiseFilePath << ": " << nAddedEntries << " entries\n";
        chain->Add(noiseFilePath);
        fNEntries += nAddedEntries;
        usedFilesList.push_back(noiseFilePath);
    }
}

void NoiseManager::SetNoiseTree(TTree* tree)
{
    fNoiseTree = tree;
    fHeader = 0;
    fTQReal = 0;
    fNoiseTree->SetBranchAddress("HEADER", &fHeader);
    fNoiseTree->SetBranchAddress("TQREAL", &fTQReal);
    fNEntries = fNoiseTree->GetEntries();
    
    std::cout << "[NoiseManager] Total entries: " << fNEntries << "\n";
    std::cout << "[NoiseManager] Total noise entries: " << fNoiseTree->GetEntries(dummyCut) << "\n";
}

void NoiseManager::SetNoiseTimeRange(float startTime, float endTime)
{ 
    fNoiseStartTime = startTime; 
    fNoiseEndTime = endTime;
    fNoiseWindowWidth = endTime - startTime;
    fNParts = (int)(fNoiseEventLength / fNoiseWindowWidth);
}

void NoiseManager::GetNextNoiseEvent()
{
    fPartID = 0; fCurrentHitID = 0;
    
    fCurrentEntry++; fNoiseEventHits.Clear();
    if (fCurrentEntry < fNEntries) {
        fNoiseTree->GetEntry(fCurrentEntry);
        if (fHeader->idtgsk != mT2KDummy && fHeader->idtgsk != mRandomWide)
            GetNextNoiseEvent();
        else
            SetNoiseEventHits();
    }
    else {
        std::cerr << "[NoiseManager] Noise tree reached its end!\n";
        if (fCurrentEntry == fNEntries && fDoRepeat) {
            // start from beginning
            std::cerr << "[NoiseManager] Repetition allowed: going back to the first entry in noise tree...\n";
            fCurrentEntry = 0;
            fNoiseTree->GetEntry(fCurrentEntry);
        }
        else {
            // error
            std::cerr << "[NoiseManager] Repetition disallowed. To allow, use NoiseManager::SetRepeat(true). Aborting program...\n";
            exit(-1);
        }
    }
}

void NoiseManager::SetNoiseEventHits()
{
    fT = fTQReal->T; 
    fQ = fTQReal->Q;
    fI = fTQReal->cables;
    int nRawHits = fT.size();
    
    for (unsigned int j=0; j<=nRawHits; j++)
        fNoiseEventHits.Append({fT[j], fQ[j], fI[j]&0x0000FFFF, 2/*in-gate flag*/});
        
    fNoiseEventHits.Sort();
    int nHits = fNoiseEventHits.GetSize();
    fNoiseEventLength = fNoiseEventHits[nHits-1].t() - fNoiseEventHits[0].t();
    fNParts = (int)(fNoiseEventLength / fNoiseWindowWidth);

    if (fNParts == 0) {
        std::cerr << "[NoiseManager] The current entry is too short. Getting the next entry in the noise tree...\n";
        GetNextNoiseEvent();
    }

    fNoiseT0 = fT[0] + (fNoiseEventLength - fNParts*fNoiseWindowWidth)/2.;
}

void NoiseManager::AddNoise(PMTHitCluster* signalHits)
{
    if (fCurrentEntry == -1 || fPartID == fNParts) {
        GetNextNoiseEvent();
        std::cout << "[NoiseManager] Current noise entry: " << fCurrentEntry << "\n";
    }
    
    float partStartTime = fNoiseT0 + fPartID * fNoiseWindowWidth;
    float partEndTime = partStartTime + fNoiseWindowWidth;
    
    while (fNoiseEventHits[fCurrentHitID].t() < partStartTime) fCurrentHitID++;

    while (fNoiseEventHits[fCurrentHitID].t() < partEndTime) {
        PMTHit hit = fNoiseEventHits[fCurrentHitID];
        PMTHit shiftedHit(hit.t() - partStartTime + fNoiseStartTime, hit.q(), hit.i(), hit.f());
        signalHits->Append(shiftedHit);
        fCurrentHitID++;
    }
    
    signalHits->ApplyDeadtime(fPMTDeadtime);
    fPartID++;
}