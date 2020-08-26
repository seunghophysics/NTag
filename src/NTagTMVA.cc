#include <TMVA/Config.h>
#include <TMVA/Tools.h>
#include <TMVA/Factory.h>
#include <TMVA/Reader.h>

#include <NTagTMVA.hh>

NTagTMVA::NTagTMVA(unsigned int verbose):
fVerbosity(verbose)
{ 
    msg = NTagMessage("TMVA", fVerbosity);
    fVariables = NTagTMVAVariables();
    SetMethods();
    
    SetSigCut("realneutron == 1");
    SetBkgCut("realneutron == 0");
}

NTagTMVA::NTagTMVA(const char* inFileName, const char* outFileName, unsigned int verbose):
fVerbosity(verbose), fInFileName(inFileName), fOutFileName(outFileName)
{
    msg = NTagMessage("TMVA", fVerbosity);
    fVariables = NTagTMVAVariables();
    SetMethods();
    
    SetSigCut("realneutron == 1");
    SetBkgCut("realneutron == 0");
}

NTagTMVA::~NTagTMVA() {}

void NTagTMVA::SetReader(TString methodName, TString weightFileName)
{ 
    fReaderMethodName = methodName + " method";
    fReaderWeightFileName = weightFileName;
    
    fReader = new TMVA::Reader( "!Color:!Silent" );
    fVariables.AddVariablesToReader(fReader);
    fReader->BookMVA(fReaderMethodName, fReaderWeightFileName);
}

void NTagTMVA::SetMethods()
{
    // --- Cut optimisation
    fUse["Cuts"]            = 0;
    fUse["CutsD"]           = 0;
    fUse["CutsPCA"]         = 0;
    fUse["CutsGA"]          = 0;
    fUse["CutsSA"]          = 0;
    //
    // --- 1-dimensional likelihood ("naive Bayes estimator")
    fUse["Likelihood"]      = 1;
    fUse["LikelihoodD"]     = 1; // the "D" extension indicates decorrelated input variables (see option strings)
    fUse["LikelihoodPCA"]   = 0; // the "PCA" extension indicates PCA-transformed input variables (see option strings)
    fUse["LikelihoodKDE"]   = 0;
    fUse["LikelihoodMIX"]   = 0;
    //
    // --- Mutidimensional likelihood and Nearest-Neighbour methods
    fUse["PDERS"]           = 0;
    fUse["PDERSD"]          = 0;
    fUse["PDERSPCA"]        = 0;
    fUse["PDEFoam"]         = 0;
    fUse["PDEFoamBoost"]    = 1; // fUses generalised MVA method boosting
    fUse["KNN"]             = 0; // k-nearest neighbour method
    //
    // --- Linear Discriminant Analysis
    fUse["LD"]              = 1; // Linear Discriminant identical to Fisher
    fUse["Fisher"]          = 0;
    fUse["FisherG"]         = 0;
    fUse["BoostedFisher"]   = 1; // fUses generalised MVA method boosting
    fUse["HMatrix"]         = 0;
    //
    // --- Function Discriminant analysis
    fUse["FDA_GA"]          = 0; // minimisation of fUser-defined function using Genetics Algorithm
    fUse["FDA_SA"]          = 0;
    fUse["FDA_MC"]          = 0;
    fUse["FDA_MT"]          = 0;
    fUse["FDA_GAMT"]        = 0;
    fUse["FDA_MCMT"]        = 0;
    //
    // --- Neural Networks (all are feed-forward Multilayer Perceptrons)
    fUse["MLP"]             = 1; // Recommended ANN
    fUse["MLPBFGS"]         = 0; // Recommended ANN with optional training method
    fUse["MLPBNN"]          = 1; // Recommended ANN with BFGS training method and bayesian regulator
    fUse["MLPBNN10"]        = 0; // Recommended ANN with BFGS training method and bayesian regulator
    fUse["MLPBNN4"]         = 0; // Recommended ANN with BFGS training method and bayesian regulator
    fUse["MLPBNN6"]         = 0; // Recommended ANN with BFGS training method and bayesian regulator
    fUse["CFMlpANN"]        = 0; // Depreciated ANN from ALEPH
    fUse["TMlpANN"]         = 0; // ROOT's own ANN
    //
    // --- Support Vector Machine
    fUse["SVM"]             = 1;
    fUse["SVM1"]            = 0;
    fUse["SVM2"]            = 0;
    fUse["SVM5"]            = 0;
    fUse["SVM10"]           = 0;
    //
    // --- Boosted Decision Trees
    fUse["BDT"]             = 1; // fUses Adaptive Boost
    fUse["BDTG"]            = 0; // fUses Gradient Boost
    fUse["BDTB"]            = 0; // fUses Bagging
    fUse["BDTD"]            = 1; // decorrelation + Adaptive Boost
    fUse["BDTF"]            = 0; // allow usage of fisher discriminant for node splitting
    //
    // --- Friedman's RuleFit method, ie, an optimised series of cuts ("rules")
    fUse["RuleFit"]         = 1;
}

void NTagTMVA::Train()
{
    TMVA::Tools::Instance();
    
    TFile* outFile = TFile::Open( fOutFileName, "RECREATE" );
    TMVA::Factory *fFactory = new TMVA::Factory( "NTagTMVA", outFile,
                                               "!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification" );
    
    (TMVA::gConfig().GetIONames()).fWeightFileDir = "weights/new";
    
    fVariables = NTagTMVAVariables();
    auto varKeys = fVariables.Keys();
    
    for (const auto& key: varKeys) {
        if (key[0] == 'N') fFactory->AddVariable(key, 'I');
        else fFactory->AddVariable(key, 'F');
    }

    TFile *inFile = TFile::Open( fInFileName );
    
    msg.Print(Form("Input name: %s", fInFileName));
    
    TTree *sigTree = (TTree*)inFile->Get("ntvar");
    TTree *bkgTree = (TTree*)inFile->Get("ntvar");
    
    fFactory->AddSignalTree    ( sigTree, 1.0, TMVA::Types::kTraining);
    fFactory->AddBackgroundTree( bkgTree, 1.0, TMVA::Types::kTraining);
    
    fFactory->PrepareTrainingAndTestTree( fSigCut, fBkgCut,
                                        "nTrain_Signal=0:nTrain_Background=0:SplitMode=Random:NormMode=NumEvents:!V" );

    // Cut optimisation
    if (fUse["Cuts"])
        fFactory->BookMethod( TMVA::Types::kCuts, "Cuts",
                           "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart" );

    if (fUse["CutsD"])
        fFactory->BookMethod( TMVA::Types::kCuts, "CutsD",
                           "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart:VarTransform=Decorrelate" );

    if (fUse["CutsPCA"])
        fFactory->BookMethod( TMVA::Types::kCuts, "CutsPCA",
                           "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart:VarTransform=PCA" );

    if (fUse["CutsGA"])
        fFactory->BookMethod( TMVA::Types::kCuts, "CutsGA",
                           "H:!V:FitMethod=GA:CutRangeMin[0]=10:CutRangeMax[0]=50:VarProp=FSmart:EffSel:Steps=30:Cycles=3:PopSize=400:SC_steps=10:SC_rate=5:SC_factor=0.95" );

    if (fUse["CutsSA"])
        fFactory->BookMethod( TMVA::Types::kCuts, "CutsSA",
                           "!H:!V:FitMethod=SA:EffSel:MaxCalls=150000:KernelTemp=IncAdaptive:InitialTemp=1e+6:MinTemp=1e-6:Eps=1e-10:UseDefaultScale" );

    // Likelihood ("naive Bayes estimator")
    if (fUse["Likelihood"])
        fFactory->BookMethod( TMVA::Types::kLikelihood, "Likelihood",
                           "H:!V:TransformOutput:PDFInterpol=Spline2:NSmooth=1:NAvEvtPerBin=50" );

    // Decorrelated likelihood
    if (fUse["LikelihoodD"])
        fFactory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodD",
                           "!H:!V:TransformOutput:PDFInterpol=Spline2:NSmooth=1:NAvEvtPerBin=50:VarTransform=Decorrelate" );

    // PCA-transformed likelihood
    if (fUse["LikelihoodPCA"])
        fFactory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodPCA",
                           "!H:!V:!TransformOutput:PDFInterpol=Spline2:NSmooth=1:NAvEvtPerBin=50:VarTransform=PCA" );

    // Use a kernel density estimator to approximate the PDFs
    if (fUse["LikelihoodKDE"])
        fFactory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodKDE",
                           "!H:!V:!TransformOutput:PDFInterpol=KDE:KDEtype=Gauss:KDEiter=Adaptive:KDEFineFactor=1:KDEborder=Mirror:NAvEvtPerBin=50" );

    // Use a variable-dependent mix of splines and kernel density estimator
    if (fUse["LikelihoodMIX"])
        fFactory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodMIX",
                           "!H:!V:!TransformOutput:PDFInterpolSig[0]=KDE:PDFInterpolBkg[0]=KDE:PDFInterpolSig[1]=KDE:PDFInterpolBkg[1]=KDE:PDFInterpolSig[2]=Spline2:PDFInterpolBkg[2]=Spline2:PDFInterpolSig[3]=Spline2:PDFInterpolBkg[3]=Spline2:KDEtype=Gauss:KDEiter=Nonadaptive:KDEborder=None:NAvEvtPerBin=50" );

    // Test the multi-dimensional probability density estimator
    // here are the options strings for the MinMax and RMS methods, respectively:
    //      "!H:!V:VolumeRangeMode=MinMax:DeltaFrac=0.2:KernelEstimator=Gauss:GaussSigma=0.3" );
    //      "!H:!V:VolumeRangeMode=RMS:DeltaFrac=3:KernelEstimator=Gauss:GaussSigma=0.3" );
    if (fUse["PDERS"])
        fFactory->BookMethod( TMVA::Types::kPDERS, "PDERS",
                           "!H:!V:NormTree=T:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600" );

    if (fUse["PDERSD"])
        fFactory->BookMethod( TMVA::Types::kPDERS, "PDERSD",
                           "!H:!V:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600:VarTransform=Decorrelate" );

    if (fUse["PDERSPCA"])
        fFactory->BookMethod( TMVA::Types::kPDERS, "PDERSPCA",
                           "!H:!V:VolumeRangeMode=Adaptive:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600:VarTransform=PCA" );

    // Multi-dimensional likelihood estimator using self-adapting phase-space binning
    if (fUse["PDEFoam"])
        fFactory->BookMethod( TMVA::Types::kPDEFoam, "PDEFoam",
                           "!H:!V:SigBgSeparate=F:TailCut=0.001:VolFrac=0.0666:nActiveCells=500:nSampl=2000:nBin=5:Nmin=100:Kernel=None:Compress=T" );

    if (fUse["PDEFoamBoost"])
        fFactory->BookMethod( TMVA::Types::kPDEFoam, "PDEFoamBoost",
                           "!H:!V:Boost_Num=30:Boost_Transform=linear:SigBgSeparate=F:MaxDepth=4:UseYesNoCell=T:DTLogic=MisClassificationError:FillFoamWithOrigWeights=F:TailCut=0:nActiveCells=500:nBin=20:Nmin=400:Kernel=None:Compress=T" );

    // K-Nearest Neighbour classifier (KNN)
    if (fUse["KNN"])
        fFactory->BookMethod( TMVA::Types::kKNN, "KNN",
                           "H:nkNN=20:ScaleFrac=0.8:SigmaFact=1.0:Kernel=Gaus:UseKernel=F:UseWeight=T:!Trim" );

    // H-Matrix (chi2-squared) method
    if (fUse["HMatrix"])
        fFactory->BookMethod( TMVA::Types::kHMatrix, "HMatrix", "!H:!V:VarTransform=None" );

    // Linear discriminant (same as Fisher discriminant)
    if (fUse["LD"])
        fFactory->BookMethod( TMVA::Types::kLD, "LD", "H:!V:VarTransform=None:CreateMVAPdfs:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=50:NsmoothMVAPdf=10" );

    // Fisher discriminant (same as LD)
    if (fUse["Fisher"])
        fFactory->BookMethod( TMVA::Types::kFisher, "Fisher", "H:!V:Fisher:VarTransform=None:CreateMVAPdfs:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=50:NsmoothMVAPdf=10" );

    // Fisher with Gauss-transformed input variables
    if (fUse["FisherG"])
        fFactory->BookMethod( TMVA::Types::kFisher, "FisherG", "H:!V:VarTransform=Gauss" );

    // Composite classifier: ensemble (tree) of boosted Fisher classifiers
    if (fUse["BoostedFisher"])
        fFactory->BookMethod( TMVA::Types::kFisher, "BoostedFisher",
                           "H:!V:Boost_Num=20:Boost_Transform=log:Boost_Type=AdaBoost:Boost_AdaBoostBeta=0.2:!Boost_DetailedMonitoring" );

    // Function discrimination analysis (FDA) -- test of various fitters - the recommended one is Minuit (or GA or SA)
    if (fUse["FDA_MC"])
        fFactory->BookMethod( TMVA::Types::kFDA, "FDA_MC",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MC:SampleSize=100000:Sigma=0.1" );

    if (fUse["FDA_GA"]) // can also use Simulated Annealing (SA) algorithm (see Cuts_SA options])
        fFactory->BookMethod( TMVA::Types::kFDA, "FDA_GA",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=GA:PopSize=300:Cycles=3:Steps=20:Trim=True:SaveBestGen=1" );

    if (fUse["FDA_SA"]) // can also use Simulated Annealing (SA) algorithm (see Cuts_SA options])
        fFactory->BookMethod( TMVA::Types::kFDA, "FDA_SA",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=SA:MaxCalls=15000:KernelTemp=IncAdaptive:InitialTemp=1e+6:MinTemp=1e-6:Eps=1e-10:UseDefaultScale" );

    if (fUse["FDA_MT"])
        fFactory->BookMethod( TMVA::Types::kFDA, "FDA_MT",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=2:UseImprove:UseMinos:SetBatch" );

    if (fUse["FDA_GAMT"])
        fFactory->BookMethod( TMVA::Types::kFDA, "FDA_GAMT",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=GA:Converger=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=0:!UseImprove:!UseMinos:SetBatch:Cycles=1:PopSize=5:Steps=5:Trim" );

    if (fUse["FDA_MCMT"])
        fFactory->BookMethod( TMVA::Types::kFDA, "FDA_MCMT",
                           "H:!V:Formula=(0)+(1)*x0+(2)*x1+(3)*x2+(4)*x3:ParRanges=(-1,1);(-10,10);(-10,10);(-10,10);(-10,10):FitMethod=MC:Converger=MINUIT:ErrorLevel=1:PrintLevel=-1:FitStrategy=0:!UseImprove:!UseMinos:SetBatch:SampleSize=20" );

    // TMVA ANN: MLP (recommended ANN) -- all ANNs in TMVA are Multilayer Perceptrons
    if (fUse["MLP"])
        fFactory->BookMethod( TMVA::Types::kMLP, "MLP", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:!UseRegulator" );

    if (fUse["MLPBFGS"])
        fFactory->BookMethod( TMVA::Types::kMLP, "MLPBFGS", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:TrainingMethod=BFGS:!UseRegulator" );

    if (fUse["MLPBNN"])
        fFactory->BookMethod( TMVA::Types::kMLP, "MLPBNN", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:TrainingMethod=BFGS:UseRegulator" ); // BFGS training with bayesian regulators

    if (fUse["MLPBNN10"])
        fFactory->BookMethod( TMVA::Types::kMLP, "MLPBNN10", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+10:TestRate=5:TrainingMethod=BFGS:UseRegulator" ); // BFGS training with bayesian regulators

    if (fUse["MLPBNN4"])
        fFactory->BookMethod( TMVA::Types::kMLP, "MLPBNN4", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+4:TestRate=5:TrainingMethod=BFGS:UseRegulator" ); // BFGS training with bayesian regulators

    if (fUse["MLPBNN6"])
        fFactory->BookMethod( TMVA::Types::kMLP, "MLPBNN6", "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+6:TestRate=5:TrainingMethod=BFGS:UseRegulator" ); // BFGS training with bayesian regulators

    // CF(Clermont-Ferrand)ANN
    if (fUse["CFMlpANN"])
        fFactory->BookMethod( TMVA::Types::kCFMlpANN, "CFMlpANN", "!H:!V:NCycles=2000:HiddenLayers=N+1,N"  ); // n_cycles:#nodes:#nodes:...

    // Tmlp(Root)ANN
    if (fUse["TMlpANN"])
        fFactory->BookMethod( TMVA::Types::kTMlpANN, "TMlpANN", "!H:!V:NCycles=200:HiddenLayers=N+1,N:LearningMethod=BFGS:ValidationFraction=0.3"  ); // n_cycles:#nodes:#nodes:...

    // Support Vector Machine
    if (fUse["SVM"])
        fFactory->BookMethod( TMVA::Types::kSVM, "SVM", "Gamma=0.25:Tol=0.001:VarTransform=Norm" );

    if (fUse["SVM1"])
        fFactory->BookMethod( TMVA::Types::kSVM, "SVM1", "Gamma=0.1:Tol=0.001:VarTransform=Norm" );

    if (fUse["SVM2"])
        fFactory->BookMethod( TMVA::Types::kSVM, "SVM2", "Gamma=0.2:Tol=0.001:VarTransform=Norm" );

    if (fUse["SVM5"])
        fFactory->BookMethod( TMVA::Types::kSVM, "SVM5", "Gamma=0.5:Tol=0.001:VarTransform=Norm" );

    if (fUse["SVM10"])
        fFactory->BookMethod( TMVA::Types::kSVM, "SVM10", "Gamma=1.:Tol=0.001:VarTransform=Norm" );

    // Boosted Decision Trees
    if (fUse["BDTG"]) // Gradient Boost
        fFactory->BookMethod( TMVA::Types::kBDT, "BDTG",
                           "!H:!V:NTrees=1000:MinNodeSize=2.5%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:nCuts=-1:MaxDepth=3" );

    if (fUse["BDT"])  // Adaptive Boost
        fFactory->BookMethod( TMVA::Types::kBDT, "BDT",
                           "!H:!V:NTrees=1000:MinNodeSize=2.5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=-1" );

    if (fUse["BDTB"]) // Bagging
        fFactory->BookMethod( TMVA::Types::kBDT, "BDTB",
                           "!H:!V:NTrees=1000:MinNodeSize=2.5%:MaxDepth=3:BoostType=Bagging:SeparationType=GiniIndex:nCuts=-1" );

    if (fUse["BDTD"]) // Decorrelation + Adaptive Boost
        fFactory->BookMethod( TMVA::Types::kBDT, "BDTD",
                           "!H:!V:NTrees=1000:MinNodeSize=2.5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=-1:VarTransform=Decorrelate" );

    if (fUse["BDTF"])  // Allow Using Fisher discriminant in node splitting for (strong) linearly correlated variables
        fFactory->BookMethod( TMVA::Types::kBDT, "BDTMitFisher",
                           "!H:!V:NTrees=1000:MinNodeSize=2.5%:UseFisherCuts:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=-1" );

    // RuleFit -- TMVA implementation of Friedman's method
    if (fUse["RuleFit"])
        fFactory->BookMethod( TMVA::Types::kRuleFit, "RuleFit",
                           "H:!V:RuleFitModule=RFTMVA:Model=ModRuleLinear:MinImp=0.001:RuleMinDist=0.001:NTrees=20:fEventsMin=0.01:fEventsMax=0.5:GDTau=-1.0:GDTauPrec=0.01:GDStep=0.01:GDNSteps=10000:GDErrScale=1.02" );
    
    fFactory->TrainAllMethods();
    fFactory->TestAllMethods();
    fFactory->EvaluateAllMethods();
    
    outFile->Close();
    
    msg.Print("TMVA evaluation complete!");
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
    
    fVariables = NTagTMVAVariables();
    fReader = new TMVA::Reader( "!Color:!Silent" );
    
    TFile* inFile = TFile::Open(fInFileName);
    TTree* inNtvarTree = (TTree*)inFile->Get("ntvar");
    TTree* inTruthTree = (TTree*)inFile->Get("truth");
    fVariables.SetBranchAddressToTree(inNtvarTree);

    TFile *outFile = new TFile(fOutFileName, "RECREATE");
    TTree* outNtvarTree = inNtvarTree->CloneTree();
    TTree* outTruthTree = 0;
    if (inTruthTree)
        outTruthTree = inTruthTree->CloneTree();
    
    std::vector<float> outputVector;
    outNtvarTree->Branch(methodName + "_TMVAoutput", &outputVector);
    
    msg.Print(Form("Using input file: %s", fInFileName));
    msg.Print("Using MVA method: " + methodName);

    long nEntries = inNtvarTree->GetEntries();
    
    for(long iEntry = 0; iEntry < nEntries; iEntry++){
        
        outputVector.clear();
        
        if (!inNtvarTree->GetEntry(iEntry)) continue;
        
        int nCandidates = fVariables.GetNumberOfCandidates();
        
        for(int iCandidate = 0; iCandidate < nCandidates; iCandidate++) {
            fVariables.SetVariablesForCaptureCandidate(iCandidate);
            outputVector.push_back(fReader->EvaluateMVA(fReaderMethodName));
        }
        
        outNtvarTree->Fill();
    }
    
    outNtvarTree->Write();
    if (outTruthTree)
        outTruthTree->Write();
        
    outFile->Close();
    inFile->Close();
    
    msg.Print("TMVA output generation complete!");
    msg.Print(Form("TMVA output file: %s", fOutFileName));
}