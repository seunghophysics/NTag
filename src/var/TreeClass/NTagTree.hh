//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Sep  8 14:59:21 2021 by ROOT version 5.28/00h
// from TTree NTagTree/NTagTree
// found on file: ../ntg.root
//////////////////////////////////////////////////////////

#ifndef NTagTree_h
#define NTagTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

#include "NTagGlobal.hh"
#include "CandidateCluster.hh"

class NTagTree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   std::map<std::string, std::vector<float>*> fVectorMap;
   
   /*
   std::vector<float>   *Beta1;
   std::vector<float>   *Beta2;
   std::vector<float>   *Beta3;
   std::vector<float>   *Beta4;
   std::vector<float>   *Beta5;
   std::vector<float>   *DPrompt;
   std::vector<float>   *DWall;
   std::vector<float>   *DWallMeanDir;
   std::vector<float>   *FitT;
   std::vector<float>   *Label;
   std::vector<float>   *MeanDirAngleMean;
   std::vector<float>   *MeanDirAngleRMS;
   std::vector<float>   *N1300;
   std::vector<float>   *N200;
   std::vector<float>   *N50;
   std::vector<float>   *NHits;
   std::vector<float>   *OpeningAngleMean;
   std::vector<float>   *OpeningAngleSkew;
   std::vector<float>   *OpeningAngleStdev;
   std::vector<float>   *QSum;
   std::vector<float>   *SignalRatio;
   std::vector<float>   *TRMS;
   std::vector<float>   *TagClass;
   std::vector<float>   *TagIndex;
   std::vector<float>   *TagOut;
   std::vector<float>   *fvx;
   std::vector<float>   *fvy;
   std::vector<float>   *fvz;

   // List of branches
   TBranch        *b_Beta1;   //!
   TBranch        *b_Beta2;   //!
   TBranch        *b_Beta3;   //!
   TBranch        *b_Beta4;   //!
   TBranch        *b_Beta5;   //!
   TBranch        *b_DPrompt;   //!
   TBranch        *b_DWall;   //!
   TBranch        *b_DWallMeanDir;   //!
   TBranch        *b_FitT;   //!
   TBranch        *b_Label;   //!
   TBranch        *b_MeanDirAngleMean;   //!
   TBranch        *b_MeanDirAngleRMS;   //!
   TBranch        *b_N1300;   //!
   TBranch        *b_N200;   //!
   TBranch        *b_N50;   //!
   TBranch        *b_NHits;   //!
   TBranch        *b_OpeningAngleMean;   //!
   TBranch        *b_OpeningAngleSkew;   //!
   TBranch        *b_OpeningAngleStdev;   //!
   TBranch        *b_QSum;   //!
   TBranch        *b_SignalRatio;   //!
   TBranch        *b_TRMS;   //!
   TBranch        *b_TagClass;   //!
   TBranch        *b_TagIndex;   //!
   TBranch        *b_TagOut;   //!
   TBranch        *b_fvx;   //!
   TBranch        *b_fvy;   //!
   TBranch        *b_fvz;   //!
    */
   CandidateCluster cluster;

   NTagTree(TTree *tree=0);
   virtual ~NTagTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef NTagTree_cxx
NTagTree::NTagTree(TTree *tree)
{
   Init(tree);
   cluster = CandidateCluster("Delayed");
   cluster.RegisterFeatureNames(gNTagFeatures);
}

NTagTree::~NTagTree()
{
   //if (!fChain) return;
   //delete fChain->GetCurrentFile();
}

Int_t NTagTree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   Int_t entryExists = fChain->GetEntry(entry);

   cluster.Clear();

   for (unsigned int i=0; i<fVectorMap["NHits"]->size(); i++) {
        Candidate candidate;
        for (auto const& pair: fVectorMap) {
            candidate.Set(pair.first.c_str(), pair.second->at(i));
        }
        //candidate.Set("OpeningAngleMean", OpeningAngleMean->at(i));
        //candidate.Set("OpeningAngleSkew", OpeningAngleSkew->at(i));
        //candidate.Set("OpeningAngleStdev", OpeningAngleStdev->at(i));
        //candidate.Set("Beta1", Beta1->at(i));
        //candidate.Set("Beta2", Beta2->at(i));
        //candidate.Set("Beta3", Beta3->at(i));
        //candidate.Set("Beta4", Beta4->at(i));
        //candidate.Set("Beta5", Beta5->at(i));
        //candidate.Set("DWall", DWall->at(i));
        //candidate.Set("DWallMeanDir", DWallMeanDir->at(i));
        //candidate.Set("Label", Label->at(i));
        //candidate.Set("N1300", N1300->at(i));
        //candidate.Set("N200", N200->at(i));
        //candidate.Set("N50", N50->at(i));
        //candidate.Set("NHits", NHits->at(i));
        //candidate.Set("QSum", QSum->at(i));
        //candidate.Set("FitT", FitT->at(i));
        //candidate.Set("TagOut", TagOut->at(i));
        //candidate.Set("TRMS", TRMS->at(i));
        //candidate.Set("TagClass", TagClass->at(i));
        //candidate.Set("TagIndex", TagIndex->at(i));
        //candidate.Set("fvx", fvx->at(i));
        //candidate.Set("fvy", fvy->at(i));
        //candidate.Set("fvz", fvz->at(i));
        //candidate.Set("SignalRatio", SignalRatio->at(i));
        //candidate.Set("MeanDirAngleMean", MeanDirAngleMean->at(i));
        //candidate.Set("MeanDirAngleRMS", MeanDirAngleRMS->at(i));
        //candidate.Set("DPrompt", DPrompt->at(i));
        cluster.Append(candidate);
    }

    return entryExists;
}
Long64_t NTagTree::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (!fChain->InheritsFrom(TChain::Class()))  return centry;
   TChain *chain = (TChain*)fChain;
   if (chain->GetTreeNumber() != fCurrent) {
      fCurrent = chain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void NTagTree::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   /*
   Beta1 = 0;
   Beta2 = 0;
   Beta3 = 0;
   Beta4 = 0;
   Beta5 = 0;
   DPrompt = 0;
   DWall = 0;
   DWallMeanDir = 0;
   FitT = 0;
   Label = 0;
   MeanDirAngleMean = 0;
   MeanDirAngleRMS = 0;
   N1300 = 0;
   N200 = 0;
   N50 = 0;
   NHits = 0;
   OpeningAngleMean = 0;
   OpeningAngleSkew = 0;
   OpeningAngleStdev = 0;
   QSum = 0;
   SignalRatio = 0;
   TRMS = 0;
   TagClass = 0;
   TagIndex = 0;
   TagOut = 0;
   fvx = 0;
   fvy = 0;
   fvz = 0;
   
   */
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

    // Set object pointer
    for (auto const& key: gNTagFeatures) {
        if (fChain->GetBranchStatus(key.c_str())) {
            fVectorMap[key] = 0;
            fChain->SetBranchAddress(key.c_str(), &fVectorMap[key]);
        }
    }
    /*
   fChain->SetBranchAddress("Beta1", &Beta1, &b_Beta1);
   fChain->SetBranchAddress("Beta2", &Beta2, &b_Beta2);
   fChain->SetBranchAddress("Beta3", &Beta3, &b_Beta3);
   fChain->SetBranchAddress("Beta4", &Beta4, &b_Beta4);
   fChain->SetBranchAddress("Beta5", &Beta5, &b_Beta5);
   fChain->SetBranchAddress("DPrompt", &DPrompt, &b_DPrompt);
   fChain->SetBranchAddress("DWall", &DWall, &b_DWall);
   fChain->SetBranchAddress("DWallMeanDir", &DWallMeanDir, &b_DWallMeanDir);
   fChain->SetBranchAddress("FitT", &FitT, &b_FitT);
   fChain->SetBranchAddress("Label", &Label, &b_Label);
   fChain->SetBranchAddress("MeanDirAngleMean", &MeanDirAngleMean, &b_MeanDirAngleMean);
   fChain->SetBranchAddress("MeanDirAngleRMS", &MeanDirAngleRMS, &b_MeanDirAngleRMS);
   fChain->SetBranchAddress("N1300", &N1300, &b_N1300);
   fChain->SetBranchAddress("N200", &N200, &b_N200);
   fChain->SetBranchAddress("N50", &N50, &b_N50);
   fChain->SetBranchAddress("NHits", &NHits, &b_NHits);
   fChain->SetBranchAddress("OpeningAngleMean", &OpeningAngleMean, &b_OpeningAngleMean);
   fChain->SetBranchAddress("OpeningAngleSkew", &OpeningAngleSkew, &b_OpeningAngleSkew);
   fChain->SetBranchAddress("OpeningAngleStdev", &OpeningAngleStdev, &b_OpeningAngleStdev);
   fChain->SetBranchAddress("QSum", &QSum, &b_QSum);
   fChain->SetBranchAddress("SignalRatio", &SignalRatio, &b_SignalRatio);
   fChain->SetBranchAddress("TRMS", &TRMS, &b_TRMS);
   fChain->SetBranchAddress("TagClass", &TagClass, &b_TagClass);
   fChain->SetBranchAddress("TagIndex", &TagIndex, &b_TagIndex);
   fChain->SetBranchAddress("TagOut", &TagOut, &b_TagOut);
   fChain->SetBranchAddress("fvx", &fvx, &b_fvx);
   fChain->SetBranchAddress("fvy", &fvy, &b_fvy);
   fChain->SetBranchAddress("fvz", &fvz, &b_fvz);
   */
   Notify();
}

Bool_t NTagTree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void NTagTree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t NTagTree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef NTagTree_cxx
