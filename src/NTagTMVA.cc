#include <TH1F.h>
#include <TChain.h>
#include <TROOT.h>

#include <TMVA/Config.h>
#include <TMVA/Tools.h>
#include <TMVA/Factory.h>
#include <TMVA/Reader.h>

#include "NTagPath.hh"
#include "NTagTMVA.hh"
#include "NTagEventInfo.hh"

NTagTMVA::NTagTMVA(Verbosity verbose):
fVerbosity(verbose)
{
    Constructor();
}

NTagTMVA::NTagTMVA(const char* inFileName, const char* outFileName, Verbosity verbose):
fVerbosity(verbose), fInFileName(inFileName), fOutFileName(outFileName)
{
    Constructor();
}

NTagTMVA::~NTagTMVA() {}

void NTagTMVA::Constructor()
{
    msg = NTagMessage("TMVA", fVerbosity);
    fVariables = NTagTMVAVariables(fVerbosity);
    SetMethods(true);

    SetSigCut("CaptureType > 0");
    SetBkgCut("CaptureType == 0");
}

void NTagTMVA::SetMethods(bool turnOn)
{
    // Simple cuts
    fUse["Cuts"]            = true;

    // 1D likelihood
    fUse["Likelihood"]      = true;
    fUse["LikelihoodPCA"]   = true;
    fUse["LikelihoodKDE"]   = true;

    // Multi-dimensional likelihood
    fUse["PDERS"]           = true;
    fUse["KNN"]             = true;

    // Linear discriminant
    fUse["LD"]              = true;
    fUse["BoostedFisher"]   = true;
    fUse["HMatrix"]         = true;

    // MLP
    fUse["MLP"]             = true;

    // SVM
    fUse["SVM"]             = true;

    // BDT
    fUse["AdaBDT"]          = true;
    fUse["GradBDT"]         = true;

    if (!turnOn) {
        for (auto const& pair: fUse)
            fUse[pair.first] = false;
    }
}

void NTagTMVA::UseMethod(const char* methodName, bool turnOn)
{
    if (fUse.count(methodName)) {
        if (turnOn)
            fUse[methodName] = true;
        else
            fUse[methodName] = false;
    }
    else
        msg.Print(Form("MVA method name [%s] not found! Please enter a valid method name.\n", methodName), pERROR);
}

void NTagTMVA::MakeWeights(bool isMultiClass)
{
    TMVA::Tools::Instance();

    TFile* outFile = TFile::Open( fOutFileName, "recreate" );

    TString factoryOption = TString("!V:")
                          + TString("!Silent:")
                          + TString("DrawProgressBar:")
                          + TString("Transformations=I;P;N;U;G:");
    if (isMultiClass)
        factoryOption += TString("AnalysisType=multiclass");
    else
        factoryOption += TString("AnalysisType=Classification");

    TMVA::Factory *fFactory = new TMVA::Factory( "NTagTMVA", outFile, factoryOption );

    (TMVA::gConfig().GetIONames()).fWeightFileDir = GetENV("NTAGPATH") + "weights/new";

    fVariables = NTagTMVAVariables(fVerbosity);
    auto varKeys = fVariables.Keys();

    std::cout << "\n" << std::endl;
    for (auto& key: varKeys) {
        msg.Print(Form("Adding feature variable : %s", key.c_str()));
        if (key[0] == 'N') fFactory->AddVariable(key, 'F');
        else fFactory->AddVariable(key, 'F');
    }

    fFactory->AddSpectator("CaptureType", 'I');
    std::cout << std::endl;

    TChain* chain = new TChain("ntvar");
    chain->Add(fInFileName);

    TString trainingOption = TString("nTrain_Signal=0:")
                           + TString("nTrain_Background=0:")
                           + TString("nTest_Signal=0:")
                           + TString("nTest_Background=0:")
                           + TString("SplitMode=Random:")
                           + TString("NormMode=None:")
                           + TString("V:");

    TString varTransOption = TString("VarTransform=N");

    if (isMultiClass) {
        gROOT->cd();
        TTree* bTree = (TTree*)chain->CopyTree("CaptureType==0");
        TTree* hTree = (TTree*)chain->CopyTree("CaptureType==1");
        TTree* gTree = (TTree*)chain->CopyTree("CaptureType==2");

        fFactory->AddTree(bTree, "Bkg");
        fFactory->AddTree(hTree, "H");
        fFactory->AddTree(gTree, "Gd");

        fFactory->PrepareTrainingAndTestTree( "", trainingOption );
    }
    else {
        fFactory->SetInputTrees( chain, fSigCut, fBkgCut );
        fFactory->PrepareTrainingAndTestTree( fSigCut, fBkgCut, trainingOption );

        if (fUse["Cuts"])
            fFactory->BookMethod( TMVA::Types::kCuts, "Cuts",
                               "H:V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart" );

        if (fUse["Likelihood"])
            fFactory->BookMethod( TMVA::Types::kLikelihood, "Likelihood",
                               "H:V:TransformOutput:PDFInterpol=Spline2:NSmooth=1:NAvEvtPerBin=50" );
        if (fUse["LikelihoodPCA"])
            fFactory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodPCA",
                               "H:V:TransformOutput:PDFInterpol=Spline2:NSmooth=1:NAvEvtPerBin=50:VarTransform=P" );
        if (fUse["LikelihoodKDE"])
            fFactory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodKDE",
                               "H:V:TransformOutput:PDFInterpol=KDE:KDEtype=Gauss:KDEiter=Adaptive:KDEFineFactor=1:KDEborder=Mirror:NAvEvtPerBin=50:VarTransform=P" );

        if (fUse["PDERS"])
            fFactory->BookMethod( TMVA::Types::kPDERS, "PDERS",
                               "H:V:NormTree=T:VolumeRangeMode=MinMax:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600:VarTransform=P,G,P,G" );

        if (fUse["KNN"])
            fFactory->BookMethod( TMVA::Types::kKNN, "KNN",
                               Form("H:V:nkNN=20:ScaleFrac=0.8:SigmaFact=1.0:Kernel=Gaus:UseKernel=F:UseWeight=T:!Trim:%s", varTransOption.Data()) );

        if (fUse["HMatrix"])
            fFactory->BookMethod( TMVA::Types::kHMatrix, "HMatrix", Form("!H:!V:%s", varTransOption.Data()) );

        if (fUse["LD"])
            fFactory->BookMethod( TMVA::Types::kLD, "LD", Form("H:!V:CreateMVAPdfs:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=50:NsmoothMVAPdf=10:VarTransform=P,G,P,G") );

        if (fUse["BoostedFisher"])
            fFactory->BookMethod( TMVA::Types::kFisher, "BoostedFisher",
                               Form("H:V:Boost_Num=20:Boost_Transform=log:Boost_Type=AdaBoost:Boost_AdaBoostBeta=0.2:!Boost_DetailedMonitoring:%s", varTransOption.Data()) );

        if (fUse["SVM"])
            fFactory->BookMethod( TMVA::Types::kSVM, "SVM", Form("H:V:Gamma=0.2:C=20:Tol=0.01:%s", varTransOption.Data()) );

        if (fUse["AdaBDT"])
            fFactory->BookMethod( TMVA::Types::kBDT, "AdaBDT",
                               Form("H:V:NTrees=1000:MinNodeSize=2.5%%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.2:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=CrossEntropy:%s", varTransOption.Data()) );
    }

    if (fUse["PDEFoam"])
        fFactory->BookMethod( TMVA::Types::kPDEFoam, "PDEFoam",
                           Form("H:V:nActiveCells=500:nBin=5:Nmin=10:Kernel=Gauss:Compress=T:%s", varTransOption.Data()) );

    if (fUse["MLP"]) {
        fFactory->BookMethod( TMVA::Types::kMLP, "MLP", "H:V:NeuronType=sigmoid:NCycles=50:HiddenLayers=N+1,N-1:TestRate=10:UseRegulator:EstimatorType=CE:VarTransform=N" );
    }

    if (fUse["GradBDT"])
        fFactory->BookMethod( TMVA::Types::kBDT, "GradBDT",
                           Form("H:V:NTrees=1000:MinNodeSize=2%%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:MaxDepth=3:NegWeightTreatment=IgnoreNegWeightsInTraining:%s", varTransOption.Data()) );

    fFactory->TrainAllMethods();
    fFactory->TestAllMethods();
    fFactory->EvaluateAllMethods();

    outFile->Close();

    msg.Print("TMVA evaluation complete!");
}

void NTagTMVA::SetReader(TString methodName, TString weightFileName)
{
    fReaderMethodName = methodName + " method";
    fReaderWeightFileName = weightFileName;
}

void NTagTMVA::InstantiateReader()
{
    fReader = new TMVA::Reader( "!Color:!Silent" );
    fVariables.AddVariablesToReader(fReader);
    fReader->BookMVA(fReaderMethodName, fReaderWeightFileName);
}

void NTagTMVA::DumpReaderCutRange()
{
    int iCount = 1;
    for (const auto& pair : fRangeMap) {
        msg.Print(Form("TMVA Reader Cut [%d]: %f < %s < %f", iCount, pair.second.first, pair.first, pair.second.second));
        iCount++;
    }
}

bool NTagTMVA::IsInRange(const char* key)
{
    if (key[0] == 'N')
        return fRangeMap[key].first < fVariables.Get<int>(key) && fVariables.Get<int>(key) < fRangeMap[key].second;
    else
        return fRangeMap[key].first < fVariables.Get<float>(key) && fVariables.Get<float>(key) < fRangeMap[key].second;
}

bool NTagTMVA::CandidateCut()
{
    bool cutSum = true;

    for (const auto& pair : fRangeMap) {
        cutSum = cutSum && IsInRange(pair.first);
    }

    return cutSum;
}

float NTagTMVA::GetOutputFromCandidate(int iCandidate)
{
    fVariables.SetVariablesForCaptureCandidate(iCandidate);
    if (CandidateCut()) {
        return fReader->EvaluateMVA(fReaderMethodName);
    }
    else return -9999.;
}

void NTagTMVA::ApplyWeight(TString methodName, TString weightFileName)
{
    SetReader(methodName, weightFileName);
    InstantiateReader();
    DumpReaderCutRange();

    TFile* inFile = TFile::Open(fInFileName);
    TTree* inNtvarTree = (TTree*)inFile->Get("ntvar");
    TTree* inTruthTree = (TTree*)inFile->Get("truth");

    if (inNtvarTree->GetListOfBranches()->FindObject("TMVAOutput"))
        inNtvarTree->SetBranchStatus("TMVAOutput", 0);
    fVariables.SetBranchAddressToTree(inNtvarTree);

    TFile* outFile = new TFile(fOutFileName, "recreate");
    TTree* outNtvarTree = inNtvarTree->CloneTree(-1, "fast");
    TTree* outTruthTree = 0;
    if (inTruthTree)
        outTruthTree = inTruthTree->CloneTree();

    // Replace old output with new one
    std::vector<float> outputVector;
    TBranch* newOutBranch = outNtvarTree->Branch("TMVAOutput", &outputVector);

    std::cout << std::endl;
    msg.Print("Using MVA method: " + methodName);

    long nEntries = inNtvarTree->GetEntries();

    for(long iEntry = 0; iEntry < nEntries; iEntry++){

        msg.Print(Form("Processing entry %ld / %ld...\r", iEntry, nEntries), pDEFAULT, false);
        std::cout << std::flush;

        outputVector.clear();

        if (!inNtvarTree->GetEntry(iEntry)) continue;

        int nCandidates = fVariables.GetNumberOfCandidates();

        for(int iCandidate = 0; iCandidate < nCandidates; iCandidate++) {
            float output = GetOutputFromCandidate(iCandidate);
            outputVector.push_back(output);
        }

        newOutBranch->Fill();
    }

    outNtvarTree->Write();
    if (outTruthTree)
        outTruthTree->Write();

    outFile->Close();
    inFile->Close();

    msg.Print("TMVA output generation complete!      ");
}