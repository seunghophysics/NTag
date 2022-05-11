#include "sys/stat.h"

#include "TFile.h"
#include "TChain.h"

#include <TMVA/Config.h>
#include <TMVA/Tools.h>
#include <TMVA/Factory.h>
#include <TMVA/Reader.h>

#include "Calculator.hh"
#include "NTagGlobal.hh"
#include "NTagTMVAManager.hh"

NTagTMVAManager::NTagTMVAManager()
: fFactory(nullptr), fReader(nullptr), fWeightFilePath(GetENV("NTAGLIBPATH")+"/weights/TMVA_MLP.xml")
{}
NTagTMVAManager::~NTagTMVAManager()
{
    if (fFactory) delete fFactory;
    if (fReader)  delete fReader;
}

void NTagTMVAManager::InitializeReader(std::string weightPath)
{
    // use same weight for bonsai and lowfit
    if (weightPath == "lowfit") weightPath = "bonsai";
    if (weightPath == "bonsai" || weightPath == "trms" || weightPath == "prompt")
        SetWeightPath(GetENV("NTAGLIBPATH")+ "weights/" + weightPath + "/NTagTMVAFactory_MLP.weights.xml");
    else if (!weightPath.empty())
        SetWeightPath(weightPath);

    fReader = new TMVA::Reader();
    
    for (auto const& feature: gTMVAFeatures) {
        fFeatureContainer[feature] = 0;
        fReader->AddVariable(feature, &(fFeatureContainer[feature]));
    }

    fReader->AddSpectator("Label", &(fCandidateLabel));
    fReader->BookMVA("MLP", fWeightFilePath);
}

void NTagTMVAManager::SetMethods(bool turnOn)
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

float NTagTMVAManager::GetTMVAOutput(const Candidate& candidate)
{
    // get features from candidate and fill feature container
    for (auto const& pair: fFeatureContainer) {
        float value = candidate[pair.first];
        fFeatureContainer[pair.first] = value;
    }

    return fReader? fReader->EvaluateMVA("MLP") : 0;
}

void NTagTMVAManager::TrainWeights(const char* inFileName, const char* outFileName, const char* outDirName)
{
    //TMVA::Tools::Instance();

    TFile* outFile = TFile::Open(outFileName, "recreate" );

    TString factoryOption = TString("!V:")
                          + TString("!Silent:")
                          + TString("DrawProgressBar:")
                          + TString("Transformations=I;P;N;U;G:");
    //if (isMultiClass)
    //    factoryOption += TString("AnalysisType=multiclass");
    //else
    factoryOption += TString("AnalysisType=Classification");

    fFactory = new TMVA::Factory("NTagTMVAFactory", outFile, factoryOption);

    auto weightFileDir = GetENV("NTAGLIBPATH") + "weights/" + std::string(outDirName);
    (TMVA::gConfig().GetIONames()).fWeightFileDir = weightFileDir;
    mkdir(weightFileDir.c_str(), S_IRWXU);

    std::cout << "\n" << std::endl;
    for (auto& key: gTMVAFeatures) {
        fMsg.Print(Form("Adding feature variable : %s", key.c_str()));
        fFactory->AddVariable(key, 'F');
    }

    fFactory->AddSpectator("Label", 'I');

    TChain* chain = new TChain("ntag");
    chain->Add(inFileName);

    //long nAllSig   = chain->Draw("CaptureType", "CaptureType > 0", "goff");
    //long nAllBkg   = chain->Draw("CaptureType", fBkgCut, "goff");
    //long nInputSig = chain->Draw("CaptureType", fSigCut, "goff");
    //long nInputBkg = (long) (nAllBkg * nInputSig / nAllSig); // to preserve the imbalance ratio!

    //fMsg.Print(TString("Number of input signal (") + fSigCut.GetTitle() + TString(Form(") candidates: %ld", nInputSig)));
    //fMsg.Print(TString("Number of input background (") + fBkgCut.GetTitle() + TString(Form(") candidates: %ld", nInputBkg)));
    //std::cout << std::endl;

    TString trainingOption = TString(Form("nTrain_Signal=%d:", 0))
                           + TString(Form("nTrain_Background=%d:", 0))
                           + TString("nTest_Signal=1:")
                           + TString("nTest_Background=1:")
                           + TString("SplitMode=Random:")
                           + TString("NormMode=None:")
                           + TString("V:");

    TString varTransOption = TString("VarTransform=N");

    //if (isMultiClass) {
    //    gROOT->cd();
    //    TTree* bTree = (TTree*)chain->CopyTree("CaptureType==0");
    //    TTree* hTree = (TTree*)chain->CopyTree("CaptureType==1");
    //    TTree* gTree = (TTree*)chain->CopyTree("CaptureType==2");
    //
    //    fFactory->AddTree(bTree, "Bkg");
    //    fFactory->AddTree(hTree, "H");
    //    fFactory->AddTree(gTree, "Gd");
    //
    //    fFactory->PrepareTrainingAndTestTree( "", trainingOption );
    //}
    //else {
    auto sigCut = Form("(Label==%d||Label==%d)&&SignalRatio>0.2", lnH, lnGd);
    auto bkgCut = Form("Label==%d&&SignalRatio==0", lNoise);
    fFactory->SetInputTrees(chain, sigCut, bkgCut);
    fFactory->PrepareTrainingAndTestTree(sigCut, bkgCut, trainingOption);
    //if (fUse["Cuts"])
    //    fFactory->BookMethod( TMVA::Types::kCuts, "Cuts",
    //                       "H:V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart" );
    //if (fUse["Likelihood"])
    //    fFactory->BookMethod( TMVA::Types::kLikelihood, "Likelihood",
    //                       "H:V:TransformOutput:PDFInterpol=Spline2:NSmooth=1:NAvEvtPerBin=50" );
    //if (fUse["LikelihoodPCA"])
    //    fFactory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodPCA",
    //                       "H:V:TransformOutput:PDFInterpol=Spline2:NSmooth=1:NAvEvtPerBin=50:VarTransform=P" );
    //if (fUse["LikelihoodKDE"])
    //    fFactory->BookMethod( TMVA::Types::kLikelihood, "LikelihoodKDE",
    //                       "H:V:TransformOutput:PDFInterpol=KDE:KDEtype=Gauss:KDEiter=Adaptive:KDEFineFactor=1:KDEborder=Mirror:NAvEvtPerBin=50:VarTransform=P" );
    //if (fUse["PDERS"])
    //    fFactory->BookMethod( TMVA::Types::kPDERS, "PDERS",
    //                       "H:V:NormTree=T:VolumeRangeMode=MinMax:KernelEstimator=Gauss:GaussSigma=0.3:NEventsMin=400:NEventsMax=600:VarTransform=P,G,P,G" );
    //if (fUse["KNN"])
    //    fFactory->BookMethod( TMVA::Types::kKNN, "KNN",
    //                       Form("H:V:nkNN=20:ScaleFrac=0.8:SigmaFact=1.0:Kernel=Gaus:UseKernel=F:UseWeight=T:!Trim:%s", varTransOption.Data()) );
    //if (fUse["HMatrix"])
    //    fFactory->BookMethod( TMVA::Types::kHMatrix, "HMatrix", Form("!H:!V:%s", varTransOption.Data()) );
    //if (fUse["LD"])
    //    fFactory->BookMethod( TMVA::Types::kLD, "LD", Form("H:!V:CreateMVAPdfs:PDFInterpolMVAPdf=Spline2:NbinsMVAPdf=50:NsmoothMVAPdf=10:VarTransform=P,G,P,G") );
    //if (fUse["BoostedFisher"])
    //    fFactory->BookMethod( TMVA::Types::kFisher, "BoostedFisher",
    //                       Form("H:V:Boost_Num=20:Boost_Transform=log:Boost_Type=AdaBoost:Boost_AdaBoostBeta=0.2:!Boost_DetailedMonitoring:%s", varTransOption.Data()) );
    //if (fUse["SVM"])
    //    fFactory->BookMethod( TMVA::Types::kSVM, "SVM", Form("H:V:Gamma=0.2:C=20:Tol=0.01:%s", varTransOption.Data()) );
    //if (fUse["AdaBDT"])
    //    fFactory->BookMethod( TMVA::Types::kBDT, "AdaBDT",
    //                       Form("H:V:NTrees=1000:MinNodeSize=2.5%%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.2:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=CrossEntropy:%s", varTransOption.Data()) );
    //}

    if (fUse["PDEFoam"])
        fFactory->BookMethod( TMVA::Types::kPDEFoam, "PDEFoam",
                           Form("H:V:nActiveCells=500:nBin=5:Nmin=10:Kernel=Gauss:Compress=T:%s", varTransOption.Data()) );

    if (fUse["MLP"]) {
        fFactory->BookMethod( TMVA::Types::kMLP, "MLP", "H:V:NeuronType=sigmoid:NCycles=300:HiddenLayers=N+1,N-1:TestRate=5:UseRegulator:EstimatorType=CE:VarTransform=N" );
    }

    if (fUse["GradBDT"])
        fFactory->BookMethod( TMVA::Types::kBDT, "GradBDT",
                           Form("H:V:NTrees=1000:MinNodeSize=2%%:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.5:MaxDepth=3:NegWeightTreatment=IgnoreNegWeightsInTraining:%s", varTransOption.Data()) );

    fFactory->TrainAllMethods();
    fFactory->TestAllMethods();
    fFactory->EvaluateAllMethods();

    outFile->Close();

    fMsg.Print("TMVA evaluation complete!");
}

/*
void NTagTMVAManager::ApplyWeights(const char* inFileName, const char* outFileName)
{
    //SetReader(methodName, weightFileName);
    //InstantiateReader();
    InitializeReader();
    //DumpReaderCutRange();

    TFile* inFile = TFile::Open(inFileName);
    TTree* inNtvarTree = (TTree*)inFile->Get("ntvar");
    TTree* inTruthTree = (TTree*)inFile->Get("truth");

    if (inNtvarTree->GetListOfBranches()->FindObject("TagOut"))
        inNtvarTree->SetBranchStatus("TagOut", 0);
    fVariables.SetBranchAddressToTree(inNtvarTree);

    TFile* outFile = new TFile(outFileName, "recreate");
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

    msg.Print("TMVA output generation complete!");
}
*/