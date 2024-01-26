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
  fNoiseCut(Form("HEADER.idtgsk & %d || HEADER.idtgsk == %d || HEADER.idtgsk & %d", mRandomWide, mT2KDummy, mNickel)),
  fSKGen(6),
  fNoiseSeed(0),
  fNoiseEventLength(1000e3),
  fNoiseEventMinT(-1000e3), fNoiseEventMaxT(1000e3),
  fNoiseStartTime(1e3), fNoiseEndTime(536e3), fNoiseWindowWidth(536e3),
  fNoiseT0(0),
  fIDMaxN200(60), fODMaxN200(20),
  //fMinHitsLimit(50e3), fMaxHitsLimit(100e3),
  //fMinHitDensity(10e-3), fMaxHitDensity(50e-3), // hits per nanosecond
  fPMTDeadtime(900), fIDDarkRatekHz(7.5), fODDarkRatekHz(4.0),
  fCurrentIDHitIndex(0), fCurrentODHitIndex(0),
  fCurrentEntry(-1), fNEntries(0),
  fPartID(0), fNParts(2),
  fCurrentPartStartTime(-1000e3), fCurrentPartEndTime(1000e3),
	fDoRepeat(true), fDoN200Cut(false), fDoRandomizeFirstEntry(true), fRandomizedFirstEntry(-1),
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
		fMsg.Print(Form("Randomized starting entry? : %s", (fDoRandomizeFirstEntry ? "yes" : "no")));
		fMsg.Print(Form("Dummy trgger starting entry: %d", (fRandomizedFirstEntry == -1 ? 0 : fRandomizedFirstEntry)));
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
    fNoiseTree->SetBranchStatus("*", 0);
    fNoiseTree->SetBranchStatus("TQREAL", 1);
    fNoiseTree->SetBranchStatus("TQAREAL", 1);
    fNoiseTree->SetBranchStatus("HEADER", 1);
    fIDTQReal = 0; fNoiseTree->SetBranchAddress("TQREAL", &fIDTQReal);
    fODTQReal = 0; fNoiseTree->SetBranchAddress("TQAREAL", &fODTQReal);
    fHeader = 0; fNoiseTree->SetBranchAddress("HEADER", &fHeader);
    fNEntries = fNoiseTree->GetEntries();

	if (fDoRandomizeFirstEntry == true)
	{
	    auto random = ranGen.Uniform();
	    long randomized_ent = (long) ((double)ceil(fNEntries*random));
		fRandomizedFirstEntry = randomized_ent-1;
		int ret = fNoiseTree->GetEntry(fRandomizedFirstEntry);
		//fMsg.Print(Form("Randomize TChain starting entry: %d", fDoRandomizeFirstEntry));
		//fMsg.Print(Form("             Total TChain entry: %d", fNEntries));
		//fMsg.Print(Form("          TChain starting entry: %d", fNoiseTree->GetReadEntry()));
		//fMsg.Print(Form("                         random: %g", random));
		//fMsg.Print(Form("                 randomized_ent: %d", randomized_ent));
		if ( ret == 0 )
		{
			fMsg.Print("Something wrong at: NoiseManager::SetNoiseTree()", pERROR);
		}
	}
	else
	{
		fNoiseTree->GetEntry(0);
	}

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

    if (base == "default") base = "sk" + std::to_string(fSKGen);
    std::vector<TString> runDirs = GetListOfSubdirectories(fNoisePath + "/" + base);
    std::vector<TString> fileList;
    if (run != "") fileList = GetListOfFiles(fNoisePath + "/" + base + "/" + run);

    TString dummyFilePath;

    int nTrial = 0;
    while (fNEntries <= /*2**/nRequiredEvents) {
        
        // limit the number of iterations
        nTrial++;
        if (nTrial > 10000) {
            std::cerr << "Could not find enough noise files within iteration limit 10000, aborting...\n";
            abort();
        }

        // multiple runs 
        if (run == "" && !runDirs.empty()) {
            dummyRunPath = PickRandom(runDirs);
            int dummyRunNo = ((TObjString*)((dummyRunPath.Tokenize('/'))->Last()))->GetString().Atoi();
            if (!dummyRunNo) continue; // skip any non-numeric run directory name
            if (dummyRunNo < minRun || maxRun < dummyRunNo ) continue;
            fileList = GetListOfFiles(dummyRunPath, ".root");
        }
        
        // specific run
        if (!fileList.empty())
            dummyFilePath = PickRandom(fileList);
        else continue;
        
        AddNoiseFileToChain(dummyChain, dummyFilePath);
    }

    if (!fNEntries) {
        std::cerr << "Empty noise chain! Aborting...\n";
        abort();
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
        if (option=="noise_cut")     fDoN200Cut   = value;
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
    auto skGen       = SKIO::GetSKGeometry();
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
    //auto pmtDeadtime = settings.GetFloat("PMTDEADTIME", 900);
    float pmtDeadtime = 900;
    auto debug       = settings.GetBool("debug", false);
	auto randomizeNoise = settings.GetBool("RANDOMIZENOISE", true);

    SetSKGeneration(skGen);
    SetSeed(noiseSeed);
    SetNoiseMaxN200(idMaxN200, odMaxN200, doN200Cut);
    SetPMTDeadtime(pmtDeadtime);
	SetRandomizeFirstEntry(randomizeNoise);
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
        int trgType = fHeader->idtgsk;
        if (trgType & mRandomWide || trgType == mT2KDummy || trgType & mNickel)
            SetNoiseEventHits();
        else
            GetNextNoiseEvent();
    }
    else {
        fMsg.Print("Noise tree reached its end!", pWARNING);
        if (fCurrentEntry == fNEntries && fDoRepeat) {
            // start from beginning
            fMsg.Print("Repetition allowed: going back to the first entry in noise tree...", pWARNING);
            fCurrentEntry = -1;
            GetNextNoiseEvent();
            //fNoiseTree->GetEntry(fCurrentEntry);
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

    // make sure noise event is not empty
    if (!fIDTQReal->nhits || !fODTQReal->nhits) {
        fMsg.Print(Form("Skipping an empty noise event..."), pWARNING);
        useThisNoiseEvent = false;
    }

    // populate hit clusters
    if (useThisNoiseEvent) {
        PopulateHitCluster(&fIDNoiseEventHits);
        PopulateHitCluster(&fODNoiseEventHits, true);

        int nHits = fIDNoiseEventHits.GetSize();
        float idMinT = fIDNoiseEventHits.First().t(); float idMaxT = fIDNoiseEventHits.Last().t();
        float odMinT = fODNoiseEventHits.First().t(); float odMaxT = fODNoiseEventHits.Last().t();
        float minT = idMinT>odMinT ? idMinT : odMinT;
        float maxT = idMaxT<odMaxT ? idMaxT : odMaxT;
        fNoiseEventMinT = minT; fNoiseEventMaxT = maxT;

        fNoiseEventLength = maxT - minT;
        fNParts = (int)(fNoiseEventLength / fNoiseWindowWidth);
        auto random = ranGen.Uniform();
        fNoiseT0 = minT + (fNoiseEventLength - fNParts*fNoiseWindowWidth)*random;

        if (fNoiseEventLength < fNoiseWindowWidth) {
            fMsg.Print(Form("Noise event length %3.2f us is smaller than required window width %3.2f us, "
                            "getting next noise event...",
                            fNoiseEventLength*1e-3, fNoiseWindowWidth*1e-3), pWARNING);
            GetNextNoiseEvent();
        }
    }

    else GetNextNoiseEvent();
}

void NoiseManager::AddNoise(PMTHitCluster* signalHits, PMTHitCluster* noiseHits, int& currentHitIndex, float darkRate, bool OD)
{
    // noise from noise files
    if (fNoiseTree) {
        if (fCurrentEntry == -1 || fPartID == fNParts) {
			if (fDoRandomizeFirstEntry && fCurrentEntry == -1 && fRandomizedFirstEntry != -1) {
				fCurrentEntry = fRandomizedFirstEntry - 1;
				fRandomizedFirstEntry = -1; // Change the fCurrentEntry, only for the first time.
			}
            GetNextNoiseEvent();
        }
        //std::cout << "NoiseHitsSize: " << noiseHits->GetSize() << "\n";
        //std::cout << "CurrentHitTime: " << noiseHits->At(currentHitIndex).t() << " Index: " << currentHitIndex << "\n";

        float partStartTime = fNoiseT0 + fPartID * fNoiseWindowWidth;
        float partEndTime = partStartTime + fNoiseWindowWidth;
        fCurrentPartStartTime = partStartTime; fCurrentPartEndTime = partEndTime;
        unsigned long nAvailableHits = noiseHits->GetSize();

        fMsg.Print(Form("Processing %s", (OD?"OD":"ID")), pDEBUG);
        fMsg.Print(Form("Current noise entry: %d, part %d/%d", fCurrentEntry, fPartID+1, fNParts), pDEBUG);
        fMsg.Print(Form("Noise event range: [%3.2f, %3.2f] usec, part %d/%d time range: [%3.2f, %3.2f] usec",
                        fNoiseEventMinT*1e-3, fNoiseEventMaxT*1e-3, fPartID+1, fNParts, partStartTime*1e-3, partEndTime*1e-3), pDEBUG);
        while (noiseHits->At(currentHitIndex).t() < partStartTime) {
            currentHitIndex++;
        }

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
    //fMsg.Print("ApplyDeadtime: ", pDEFAULT, false);
    auto res = signalHits->ApplyDeadtime(fPMTDeadtime, true);
    signalHits->Sort();

    if (res.nRemoved) {
        std::cout << "[NoiseManager] Removed " << res.nRemoved << Form(" ( %d due to signal ) ", res.nRemovedBySignal)
                  << "hits for PMT deadtime " << fPMTDeadtime << " ns\n";
    }

    //signalHits->CheckNaN();
    if (!OD) fPartID++;
}

void NoiseManager::PopulateHitCluster(PMTHitCluster* hitCluster, bool OD)
{
    std::vector<float> t = !OD? fIDTQReal->T      : fODTQReal->T;
    std::vector<float> q = !OD? fIDTQReal->Q      : fODTQReal->Q;
    std::vector<int>   i = !OD? fIDTQReal->cables : fODTQReal->cables;

    unsigned int nRawHits = t.size();

    assert((t.size()==q.size()) && (q.size()==i.size()));

    for (unsigned int j=0; j<nRawHits; j++) {
        if (-1000e3 < t[j] && t[j] < 1000e3) {
            hitCluster->Append({t[j], q[j], i[j]&0x0000FFFF, 2/*in-gate flag*/});
        }
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

void NoiseManager::AddIDODNoise(PMTHitCluster* idSignalHits, PMTHitCluster* odSignalHits)
{
    AddODNoise(odSignalHits);
    AddIDNoise(idSignalHits);
}
