#include <csignal>
#include <cmath>

#include <TFile.h>

#include <skheadC.h>
#include <skvectC.h>

#include "NTagPath.hh"
#include "NTagIO.hh"
#include "SKLibs.hh"

NTagIO* NTagIO::instance;

NTagIO::NTagIO(const char* inFileName, const char* outFileName, Verbosity verbose)
: NTagEventInfo(verbose), fInFileName(inFileName), fOutFileName(outFileName), lun(10)
{
    instance = this;

    bData = false;
    fVerbosity = verbose;
    candidateVariablesAdded = false;

    fSigTQFile = NULL; fSigTQTree = NULL;

    outFile = new TFile(fOutFileName, "recreate");

    ntvarTree = new TTree("ntvar", "NTag variables");
    CreateBranchesToNtvarTree();

    truthTree = new TTree("truth", "True variables");
    CreateBranchesToTruthTree();

    rawtqTree = new TTree("rawtq", "Raw TQ");
    CreateBranchesToRawTQTree();

    restqTree = new TTree("restq", "Residual TQ");
    CreateBranchesToResTQTree();
}

NTagIO::~NTagIO() {}

void NTagIO::Initialize()
{
    SKInitialize();
    OpenFile();
}

void NTagIO::SKInitialize()
{
    // Set SK options and SK geometry
    const char* skoptn = "31,30,26,25,23"; skoptn_(skoptn, strlen(skoptn));
    msg.PrintBlock("Setting SK geometry...");
    skheadg_.sk_geometry = 5; geoset_();

    // Initialize BONSAI
    msg.PrintBlock("Initializing ZBS...");
    kzinit_();
    std::cout << std::endl;
    msg.PrintBlock("Initializing BONSAI...");
    bonsai_ini_();
}

void NTagIO::ReadFile()
{
    if (bUseTMVA) {
        TMVATools.InstantiateReader();
        TMVATools.DumpReaderCutRange();
    }

    // SIGINT handler
    struct sigaction sigHandler;
    sigHandler.sa_handler = NTagIO::SIGINTHandler;
    sigemptyset(&sigHandler.sa_mask);
    sigHandler.sa_flags = 0;

    sigaction(SIGINT, &sigHandler, NULL);

    // Read data event-by-event
    int readStatus;
    bool bEOF = false;
    auto startTime = std::clock();

    while (!bEOF) {

        readStatus = skread_(&lun);
        CheckMC();

        switch (readStatus) {
            case 0: // event read

                // If MC
                if (!bData) {
                    std::cout << "\n\n" << std::endl;
                    msg.PrintBlock(Form("Processing event #%d...", nProcessedEvents),
                               pEVENT, pDEFAULT, false);

                    int inPMT;
                    skgetv_();
                    inpmt_(skvect_.pos, inPMT);

                    // Skip event with vertex in PMT
                    if (inPMT) {
                        msg.Print(
                            Form("True vertex is in PMT. Skipping event %d...",
                                 nProcessedEvents), pDEBUG);
                        break;
                    }
                }

                ReadEvent();

                break;

            case 1: // read-error
                msg.Print("FILE READ ERROR OCCURED!", pERROR);
                break;

            case 2: // end of input 
				// If the last event was SHE, fill output.
 		   		if (bData && !bForceMC && !IsRawHitVectorEmpty()) {
					msg.Print("Saving SHE without AFT...", pDEBUG);
					SetToFSubtractedTQ();

    	    		// Tagging starts here!
        			SearchCaptureCandidates();
        			SetCandidateVariables();

        			FillTrees();

	        		// DONT'T FORGET TO CLEAR!
    	    		Clear();
    			}
        		std::cout << "\n\n" << std::endl;
                msg.Print(Form("Reached the end of input. Closing file..."), pDEFAULT);
                CloseFile();
                bEOF = true;

                msg.Print(Form("Number of saved events: %d", nProcessedEvents), pDEFAULT);
                msg.Timer("Reading this file", startTime, pDEFAULT);
                break;
        }
    }
}

void NTagIO::ReadEvent()
{
    if (bData & !bForceMC) ReadDataEvent(); // Data
    else                   ReadMCEvent();   // MC
}

void NTagIO::ReadMCEvent()
{
    // DONT'T FORGET TO CLEAR!
    Clear();

    // Prompt-peak info
    trgType = skhead_.idtgsk;
    SetTDiff();
    SetEventHeader();
    SetPromptVertex();
    SetFitInfo();

    // MC-only truth info
    SetMCInfo();

    // Hit info (all hits)
    AppendRawHitInfo();
    SetToFSubtractedTQ();

    // Tagging starts here!
    SearchCaptureCandidates();
    SetCandidateVariables();

    // DONT'T FORGET TO FILL!
    FillTrees();
}

void NTagIO::ReadDataEvent()
{
    // If current event is AFT, append TQ and fill output.
    if (skhead_.idtgsk & 1<<29) {
        msg.Print("Saving SHE+AFT...", pDEBUG);
        ReadAFTEvent();
    }

    // If previous event was SHE without following AFT,
    // just fill output because there's nothing to append.
    else if (!IsRawHitVectorEmpty()) {
        msg.Print("Saving SHE without AFT...", pDEBUG);
        SetToFSubtractedTQ();

        // Tagging starts here!
        SearchCaptureCandidates();
        SetCandidateVariables();

        FillTrees();

        // DONT'T FORGET TO CLEAR!
        Clear();
    }

    // If current event is SHE,
    // save raw hit info and don't fill output.
    if (skhead_.idtgsk & 1<<28) {
        std::cout << "\n\n" << std::endl;
        msg.PrintBlock(Form("Processing event #%d...", nProcessedEvents),
                       pEVENT, pDEFAULT, false);

        msg.Print("Reading SHE...", pDEBUG);
        ReadSHEEvent();
        SetTDiff();
    }

    // If current event is neither SHE nor AFT (e.g. HE etc.),
    // save raw hit info and fill output.
    if (!(skhead_.idtgsk & 1<<28) && !(skhead_.idtgsk & 1<<29)) {
        std::cout << "\n\n" << std::endl;
        msg.PrintBlock(Form("Processing event #%d...", nProcessedEvents),
                       pEVENT, pDEFAULT, false);

        msg.Print("Reading No-SHE...", pDEBUG);
        ReadnoSHEEvent();
    }
}

void NTagIO::ReadSHEEvent()
{
    // DONT'T FORGET TO CLEAR!
    Clear();
    trgType = 1;

    // Prompt-peak info
    SetEventHeader();
    SetPromptVertex();
    SetFitInfo();

    // Hit info (SHE: close-to-prompt hits only)
    AppendRawHitInfo();
}

void NTagIO::ReadnoSHEEvent()
{
    // DONT'T FORGET TO CLEAR!
    Clear();
    trgType = 3;

    // Prompt-peak info
    SetEventHeader();
    SetPromptVertex();
    SetFitInfo();

    // Hit info (HE: close-to-prompt hits only)
    AppendRawHitInfo();
	SetToFSubtractedTQ();
    SetTDiff();

    // Tagging starts here!
    SearchCaptureCandidates();
    SetCandidateVariables();

    // DONT'T FORGET TO FILL!
    FillTrees();

    // DONT'T FORGET TO CLEAR!
    Clear();
}

void NTagIO::ReadAFTEvent()
{
    trgType = 2;

    // Append hit info (AFT: delayed hits after prompt)
    AppendRawHitInfo();
    SetToFSubtractedTQ();

    // Tagging starts here!
    SearchCaptureCandidates();
    SetCandidateVariables();

    // DONT'T FORGET TO FILL!
    FillTrees();

    // DONT'T FORGET TO CLEAR!
    Clear();
}

void NTagIO::WriteOutput()
{
    outFile->cd();

    ntvarTree->Write();
    if (!bData) truthTree->Write();
    if (bSaveTQ) restqTree->AutoSave();
    outFile->Close();

    //bonsai_end_();
}

void NTagIO::DoWhenInterrupted()
{
    WriteOutput();
    std::cout << std::endl;
    msg.Print(Form("Interrupted by SIGINT. Events up to #%d are saved at: %s", nProcessedEvents-1, fOutFileName), pWARNING);
    exit(1);
}

void NTagIO::SIGINTHandler(int sigNo)
{
    instance->DoWhenInterrupted();
}

void NTagIO::CreateBranchesToTruthTree()
{
    truthTree->Branch("NTrueCaptures", &nTrueCaptures);
    truthTree->Branch("CandidateID", &vCandidateID);
    truthTree->Branch("TrueCT", &vTrueCT);
    truthTree->Branch("capvx", &vCapVX);
    truthTree->Branch("capvy", &vCapVY);
    truthTree->Branch("capvz", &vCapVZ);
    truthTree->Branch("NGamma", &vNGamma);
    truthTree->Branch("TotGammaE", &vTotGammaE);
    truthTree->Branch("TrgOffset", &trgOffset);
    truthTree->Branch("NSavedSec", &nSavedSec);
    truthTree->Branch("SecPID", &vSecPID);
    truthTree->Branch("SecIntID", &vSecIntID);
    truthTree->Branch("ParentPID", &vParentPID);
    truthTree->Branch("secvx", &vSecVX);
    truthTree->Branch("secvy", &vSecVY);
    truthTree->Branch("secvz", &vSecVZ);
    truthTree->Branch("SecDWall", &vSecDWall);
    truthTree->Branch("SecT", &vSecT);
    truthTree->Branch("secpx", &vSecPX);
    truthTree->Branch("secpy", &vSecPY);
    truthTree->Branch("secpz", &vSecPZ);
    truthTree->Branch("SecMom", &vSecMom);
    truthTree->Branch("SecCaptureID", &vCapID);
    truthTree->Branch("NeutIntMode", &neutIntMode);
    truthTree->Branch("NVecInNeut", &nVecInNeut);
    truthTree->Branch("NeutVecPID", &vNeutVecPID);
    truthTree->Branch("NnInNeutVec", &nNInNeutVec);
    truthTree->Branch("NeutIntMom", &neutIntMom);
    truthTree->Branch("NVec", &nVec);
    truthTree->Branch("vecx", &vecx);
    truthTree->Branch("vecy", &vecy);
    truthTree->Branch("vecz", &vecz);
    truthTree->Branch("VecPID", &vVecPID);
    truthTree->Branch("vecpx", &vVecPX);
    truthTree->Branch("vecpy", &vVecPY);
    truthTree->Branch("vecpz", &vVecPZ);
    truthTree->Branch("VecMom", &vVecMom);
}

void NTagIO::CreateBranchesToNtvarTree()
{
    ntvarTree->Branch("NCandidates", &nCandidates);
    ntvarTree->Branch("RunNo", &runNo);
    ntvarTree->Branch("SubrunNo", &subrunNo);
    ntvarTree->Branch("EventNo", &eventNo);
    ntvarTree->Branch("TrgType", &trgType);
    ntvarTree->Branch("TDiff", &tDiff);
    ntvarTree->Branch("FirstHitTime_ToF", &firstHitTime_ToF);
    ntvarTree->Branch("FirstHitIndex", &vFirstHitID);
    ntvarTree->Branch("MaxN200", &maxN200);
    ntvarTree->Branch("MaxN200Time", &maxN200Time);
    ntvarTree->Branch("pvx", &pvx);
    ntvarTree->Branch("pvy", &pvy);
    ntvarTree->Branch("pvz", &pvz);
    ntvarTree->Branch("APNrings", &apNRings);
    ntvarTree->Branch("EVis", &evis);
    ntvarTree->Branch("NHITAC", &nhitac);
    ntvarTree->Branch("APNDecays", &apNDecays);
    ntvarTree->Branch("QISMSK", &qismsk);
    ntvarTree->Branch("APNMuE", &apNMuE);
    ntvarTree->Branch("APNRings", &apNRings);
    ntvarTree->Branch("APRingPID", &vAPRingPID);
    ntvarTree->Branch("APMom", &vAPMom);
    ntvarTree->Branch("APMomE", &vAPMomE);
    ntvarTree->Branch("APMomMu", &vAPMomMu);

    ntvarTree->Branch("HitRawTimes", "vector<vector<float>>", &vHitRawTimes);
    ntvarTree->Branch("HitResTimes", "vector<vector<float>>", &vHitResTimes);
    ntvarTree->Branch("HitCableIDs", "vector<vector<int>>", &vHitCableIDs);

    ntvarTree->Branch("NTotalHits", &nTotalHits);
    ntvarTree->Branch("NRemovedHits", &nRemovedHits);

    if (!bData) {
        ntvarTree->Branch("HitSigFlags", "vector<vector<int>>", &vHitSigFlags);
        ntvarTree->Branch("NTotalSigHits", &nTotalSigHits);
        ntvarTree->Branch("NFoundSigHits", &nFoundSigHits);
    }
}

void NTagIO::AddCandidateVariablesToNtvarTree()
{
    if (fCandidateVarMap.size()) {
    
        for (auto& pair: iCandidateVarMap) {
            ntvarTree->Branch(pair.first.c_str(), &(pair.second));
        }
        for (auto& pair: fCandidateVarMap) {
            if (!iCandidateVarMap.count(pair.first))
                ntvarTree->Branch(pair.first.c_str(), &(pair.second));
        }

        candidateVariablesAdded = true;
    }
}

void NTagIO::CreateBranchesToRawTQTree()
{
    rawtqTree->Branch("T", &vTISKZ);
    rawtqTree->Branch("Q", &vQISKZ);
    rawtqTree->Branch("I", &vCABIZ);
}

void NTagIO::CreateBranchesToResTQTree()
{
    restqTree->Branch("T", &vSortedT_ToF);
    restqTree->Branch("Q", &vSortedQ);
    restqTree->Branch("I", &vSortedPMTID);
    restqTree->Branch("IsSignal", &vSortedSigFlag);
}

void NTagIO::FillTrees()
{
    DumpEventVariables();
    if (fVerbosity > pDEFAULT) DumpCandidateVariables();

    if (!candidateVariablesAdded) {
        AddCandidateVariablesToNtvarTree();
    }

    ntvarTree->Fill();
    
    if (!bData) truthTree->Fill();
    if (bSaveTQ) restqTree->Fill();

    nProcessedEvents++;
}

void NTagIO::SetSignalTQ(const char* fSigTQName)
{
    fSigTQFile = TFile::Open(fSigTQName);
    fSigTQTree = (TTree*)fSigTQFile->Get("rawtq");

    vSIGT = 0; vSIGI = 0;
    fSigTQTree->SetBranchAddress("T", &vSIGT);
    fSigTQTree->SetBranchAddress("I", &vSIGI);
}

void NTagIO::CheckMC()
{
    if (skhead_.nrunsk != 999999) {
        bData = true;
    }
    else {
        bData = false;
    }
}
