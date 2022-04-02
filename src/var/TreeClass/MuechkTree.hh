//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Sep  8 14:59:17 2021 by ROOT version 5.28/00h
// from TTree MuechkTree/MuechkTree
// found on file: ../ntg.root
//////////////////////////////////////////////////////////

#ifndef MuechkTree_h
#define MuechkTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

#include "CandidateCluster.hh"

class MuechkTree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   std::vector<float>   *DWall;
   std::vector<float>   *GateType;
   std::vector<float>   *Goodness;
   std::vector<float>   *Label;
   std::vector<float>   *NHits;
   std::vector<float>   *FitT;
   std::vector<float>   *TagClass;
   std::vector<float>   *TagIndex;
   std::vector<float>   *dirx;
   std::vector<float>   *diry;
   std::vector<float>   *dirz;
   std::vector<float>   *x;
   std::vector<float>   *y;
   std::vector<float>   *z;

   // List of branches
   TBranch        *b_DWall;   //!
   TBranch        *b_GateType;   //!
   TBranch        *b_Goodness;   //!
   TBranch        *b_Label;   //!
   TBranch        *b_NHits;   //!
   TBranch        *b_FitT;   //!
   TBranch        *b_TagClass;   //!
   TBranch        *b_TagIndex;   //!
   TBranch        *b_dirx;   //!
   TBranch        *b_diry;   //!
   TBranch        *b_dirz;   //!
   TBranch        *b_x;   //!
   TBranch        *b_y;   //!
   TBranch        *b_z;   //!

   CandidateCluster cluster;

   MuechkTree(TTree *tree=0);
   virtual ~MuechkTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef MuechkTree_cxx
MuechkTree::MuechkTree(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("../ntg.root");
      if (!f) {
         f = new TFile("../ntg.root");
      }
      tree = (TTree*)gDirectory->Get("MuechkTree");

   }
   Init(tree);
   cluster = CandidateCluster("Early");
   cluster.RegisterFeatureNames({"FitT", "x", "y", "z", "DWall", "dirx", "diry", "dirz",
                                 "NHits", "GateType", "Goodness", "Label", "TagIndex", "TagClass"});
}

MuechkTree::~MuechkTree()
{
   //if (!fChain) return;
   //delete fChain->GetCurrentFile();
}

Int_t MuechkTree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   Int_t entryExists = fChain->GetEntry(entry);

   cluster.Clear();

   for (unsigned int i=0; i<NHits->size(); i++) {
     Candidate candidate;
     candidate.Set("DWall", DWall->at(i));
     candidate.Set("GateType", GateType->at(i));
     candidate.Set("Goodness", Goodness->at(i));
     candidate.Set("Label", Label->at(i));
     candidate.Set("NHits", NHits->at(i));
     candidate.Set("FitT", FitT->at(i));
     candidate.Set("TagClass", TagClass->at(i));
     candidate.Set("TagIndex", TagIndex->at(i));
     candidate.Set("dirx", dirx->at(i));
     candidate.Set("diry", diry->at(i));
     candidate.Set("dirz", dirz->at(i));
     candidate.Set("x", x->at(i));
     candidate.Set("y", y->at(i));
     candidate.Set("z", z->at(i));
     cluster.Append(candidate);
   }

   return entryExists;
}
Long64_t MuechkTree::LoadTree(Long64_t entry)
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

void MuechkTree::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   DWall = 0;
   GateType = 0;
   Goodness = 0;
   Label = 0;
   NHits = 0;
   FitT = 0;
   TagClass = 0;
   TagIndex = 0;
   dirx = 0;
   diry = 0;
   dirz = 0;
   x = 0;
   y = 0;
   z = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("DWall", &DWall, &b_DWall);
   fChain->SetBranchAddress("GateType", &GateType, &b_GateType);
   fChain->SetBranchAddress("Goodness", &Goodness, &b_Goodness);
   fChain->SetBranchAddress("Label", &Label, &b_Label);
   fChain->SetBranchAddress("NHits", &NHits, &b_NHits);
   fChain->SetBranchAddress("FitT", &FitT, &b_FitT);
   fChain->SetBranchAddress("TagClass", &TagClass, &b_TagClass);
   fChain->SetBranchAddress("TagIndex", &TagIndex, &b_TagIndex);
   fChain->SetBranchAddress("dirx", &dirx, &b_dirx);
   fChain->SetBranchAddress("diry", &diry, &b_diry);
   fChain->SetBranchAddress("dirz", &dirz, &b_dirz);
   fChain->SetBranchAddress("x", &x, &b_x);
   fChain->SetBranchAddress("y", &y, &b_y);
   fChain->SetBranchAddress("z", &z, &b_z);
   Notify();
}

Bool_t MuechkTree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void MuechkTree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t MuechkTree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef MuechkTree_cxx
