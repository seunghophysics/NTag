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
    // --- Cut optimisation
    fUse["Cuts"]            = true;
    
    // --- 1-dimensional likelihood
    fUse["Likelihood"]      = true;
    fUse["LikelihoodPCA"]   = true;
    fUse["LikelihoodKDE"]   = true;

    // --- Mutidimensional likelihood and Nearest-Neighbour methods
    fUse["PDERS"]           = true;
    fUse["PDEFoam"]         = true;
    fUse["PDEFoamBoost"]    = true;
    fUse["KNN"]             = true;

    // --- Linear Discriminant Analysis
    fUse["LD"]              = true;
    fUse["BoostedFisher"]   = true;
    fUse["HMatrix"]         = true;

    // --- Neural Networks (all are feed-forward Multilayer Perceptrons)
    fUse["MLP"]             = true; // Recommended ANN with BFGS training method and bayesian regulator

    // --- Support Vector Machine
    fUse["SVM"]             = true;

    // --- Boosted Decision Trees
    fUse["AdaBDT"]          = true; // fUses Adaptive Boost
    fUse["GradBDT"]         = true; // fUses Gradient Boost
    
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
                          + TString("Transformations=I;P;U;G,U(N10,N200):");
    if (isMultiClass)
        factoryOption += TString("AnalysisType=multiclass");
    else
        factoryOption += TString("AnalysisType=Classification");
    
    TMVA::Factory *fFactory = new TMVA::Factory( "NTagTMVA", outFile, factoryOption );

    (TMVA::gConfig().GetIONames()).fWeightFileDir = GetENV("NTAGPATH") + "weights/new";

    fVariables = NTagTMVAVariables(fVerbosity);
    auto varKeys = fVariables.Keys();

    std::cout << "\n" << std::endl;
    for (const auto& key: varKeys) {
        msg.Print(Form("Adding feature variable : %s", key));
        if (key[0] == 'N') fFactory->AddVariable(key, 'I');
        else fFactory->AddVariable(key, 'F');
    }
    
    fFactory->AddSpectator("CaptureType", 'I');
    std::cout << std::endl;

    //TFile *inFile = TFile::Open( fInFileName );

    //TTree *evTree = (TTree*)inFile->Get("ntvar");
    //Tree *bkgTree = (TTree*)inFile->Get("ntvar");
    TChain* chain = new TChain("ntvar");
    chain->Add(fInFileName);

    //fFactory->AddSignalTree    ( sigTree, 1.0, TMVA::Types::kTraining);
    //fFactory->AddBackgroundTree( bkgTree, 1.0, TMVA::Types::kTraining);
    //TH1F hH("hH", "hH", 1, 0, 100);
    //chain->Draw("NCandidates>>hH", "CaptureType==0", "goff");
    //int totalH = hH.GetMean() * hH.GetEntries();
    
    if (isMultiClass) {
        gROOT->cd();
        TTree* bTree = (TTree*)chain->CopyTree("CaptureType==0");
        TTree* hTree = (TTree*)chain->CopyTree("CaptureType==1");
        TTree* gTree = (TTree*)chain->CopyTree("CaptureType==2");
        
        fFactory->AddTree(bTree, "Bkg");
        fFactory->AddTree(hTree, "H");
        fFactory->AddTree(gTree, "Gd");
        
        fFactory->PrepareTrainingAndTestTree( "", "SplitMode=Random:NormMode=EqualNumEvents:V" );
    }
    else {
        fFactory->SetInputTrees( chain, fSigCut, fBkgCut );

        //float trainRatio = 0.8;
        TString trainingOption = TString("nTrain_Signal=0:")
                               + TString("nTrain_Background=0:")
                               + TString("nTest_Signal=0:")
                               + TString("nTest_Background=0:")
                               + TString("SplitMode=Random:")
                               + TString("NormMode=EqualNumEvents:")
                               + TString("V:");
                              
        //Form("nTrain_Signal=0:nTrain_Background=0:nTest_Signal=0:nTest_Background=0:SplitMode=Random:MixMode=Random:NormMode=EqualNumEvents:!V");
        //, (int)(trainRatio*totalH), (int)(trainRatio*totalH), (int)((1-trainRatio)*totalH), (int)((1-trainRatio)*totalH));
    
    fFactory->PrepareTrainingAndTestTree( fSigCut, fBkgCut, trainingOption );
    
    // Cut optimisation
    if (fUse["Cuts"])
        fFactory->BookMethod( TMVA::Types::kCuts, "Cuts",
                           "H:V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart" );

    // Likelihood ("naive Bayes estimator")
    if (fUse["Likelihood"])
        fFactory->BookMethod( TMVA::Types::kLikelihood, "Likelihood",
                           "H:V:TransformOutput:PDFInterpol=Spline2:NSmooth=1:NAvEvtPerBin=50" );

    // Decorrelated likelihood
    if (fUse["LikelihoodPCA"])
        fFactory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodPCA",
                           "H:V:TransformOutput:PDFInterpol=Spline2:NSmooth=1:NAvEvtPerBin=50:VarTransform=P,G,P,G,U(N10,N200)" );

    // Use a kernel density estimator to approximate the PDFs
    if (fUse["LikelihoodKDE"])
        fFactory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodKDE",
                           "H:V:TransformOutput:PDFInterpol=KDE:KDEtype=Gauss:KDEiter=Adaptive:KDEFineFactor=1:KDEborder=Mirror:NAvEvtPerBin=50:VarTransform=P,G,P,G,U(N10,N200)" );

    // Test the multi-dimensional probability density estimator
    // here are the options strings for the MinMax and RMS methods, respectively:
    //      "!H:!V:VolumeRangeMode=MinMax:DeltaFrac=0.2:KernelEstimator=Gauss:GaussSigma=0.3" );
    //      "!H:!V:VolumeRangeMode=RMS:DeltaFrac=3:KernelEstimator=Gauss:GaussSigma=0.3" );
    if (fUse["PDERS"])
        fFactory->BookMethod( TMVA::Types::kPDERS, "PDERS",
                           "H:V:NormTree=T:VolumeRangeMode=MinMax:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600:VarTransform=G,U(N10,N200)" );

    if (fUse["PDEFoamBoost"])
        fFactory->BookMethod( TMVA::Types::kPDEFoam, "PDEFoamBoost",
                           "H:V:nSampl=3000:nActiveCells=500:nBin=5:Nmin=10:Kernel=Gauss:Compress=T:VarTransform=G:Boost_Num=100:Boost_Transform=linear:MaxDepth=4" );

    // K-Nearest Neighbour classifier (KNN)
    if (fUse["KNN"])
        fFactory->BookMethod( TMVA::Types::kKNN, "KNN",
                           "H:V:nkNN=20:ScaleFrac=0.8:SigmaFact=1.0:Kernel=Gaus:UseKernel=F:UseWeight=T:!Trim" );

    // H-Matrix (chi2-squared) method
    if (fUse["HMatrix"])
        fFactory->BookMethod( TMVA::Types::kHMatrix, "HMatrix", "!H:!V:VarTransform=None" );

    // Linear discriminant (same as Fisher discriminant)
    if (fUse["LD"])
        fFactory->BookMethod( TMVA::Types::kLD, "LD", "H:!V:VarTransform=None:CreateMVAPdfs:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=50:NsmoothMVAPdf=10" );

    // Composite classifier: ensemble (tree) of boosted Fisher classifiers
    if (fUse["BoostedFisher"])
        fFactory->BookMethod( TMVA::Types::kFisher, "BoostedFisher",
                           "H:V:Boost_Num=20:Boost_Transform=log:Boost_Type=AdaBoost:Boost_AdaBoostBeta=0.2:!Boost_DetailedMonitoring" );

    // Support Vector Machine
    if (fUse["SVM"])
        fFactory->BookMethod( TMVA::Types::kSVM, "SVM", "H:V:Gamma=0.25:Tol=0.001:VarTransform=G,U(N10,N200)" );

    if (fUse["AdaBDT"])  // Adaptive Boost
        fFactory->BookMethod( TMVA::Types::kBDT, "AdaBDT",
                           "H:V:NTrees=1000:MinNodeSize=2.5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.2:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=CrossEntropy" );
    }

    // Multi-dimensional likelihood estimator using self-adapting phase-space binning
    if (fUse["PDEFoam"])
        fFactory->BookMethod( TMVA::Types::kPDEFoam, "PDEFoam",
                           "H:V:nActiveCells=500:nBin=5:Nmin=10:Kernel=Gauss:Compress=T:VarTransform=G,U(N10,N200)" );

    // TMVA ANN: MLP (recommended ANN) -- all ANNs in TMVA are Multilayer Perceptrons
    if (fUse["MLP"])
        fFactory->BookMethod( TMVA::Types::kMLP, "MLP", "H:V:NeuronType=sigmoid:VarTransform=G,U(N10,N200):NCycles=500:HiddenLayers=N+1,N-1:TestRate=10:UseRegulator:EstimatorType=CE" );

    // Boosted Decision Trees
    if (fUse["GradBDT"]) // Gradient Boost
        fFactory->BookMethod( TMVA::Types::kBDT, "GradBDT",
                           "H:V:NTrees=1000:MinNodeSize=2.5%:Boost_num=30:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:MaxDepth=3:NegWeightTreatment=IgnoreNegWeightsInTraining" );

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

    msg.Print(Form("Using input file: %s", fInFileName));
    msg.Print("Using MVA method: " + methodName);

    long nEntries = inNtvarTree->GetEntries();

    for(long iEntry = 0; iEntry < nEntries; iEntry++){

        msg.Print(Form("Processing entry %ld / %ld...", iEntry, nEntries));

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

    msg.Print("TMVA output generation complete!");
    msg.Print(Form("TMVA output file: %s", fOutFileName));
}