//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Sep  8 14:59:12 2021 by ROOT version 5.28/00h
// from TTree TaggableTree/TaggableTree
// found on file: ../ntg.root
//////////////////////////////////////////////////////////

#ifndef TaggableTree_h
#define TaggableTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

#include "TaggableCluster.hh"

class TaggableTree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   std::vector<int>     *Type;
   std::vector<int>     *TaggedType;
   std::vector<float>   *t;
   std::vector<float>   *E;
   std::vector<float>   *tagvx;
   std::vector<float>   *tagvy;
   std::vector<float>   *tagvz;
   std::vector<float>   *DistFromPV;
   std::vector<float>   *DWall;
   std::vector<int>     *EarlyIndex;
   std::vector<int>     *DelayedIndex;

   // List of branches
   TBranch        *b_Type;   //!
   TBranch        *b_TaggedType;   //!
   TBranch        *b_t;   //!
   TBranch        *b_E;   //!
   TBranch        *b_tagvx;   //!
   TBranch        *b_tagvy;   //!
   TBranch        *b_tagvz;   //!
   TBranch        *b_DistFromPV;   //!
   TBranch        *b_DWall;   //!
   TBranch        *b_EarlyIndex;   //!
   TBranch        *b_DelayedIndex;   //!

   TaggableCluster cluster;

   TaggableTree(TTree *tree=0);
   virtual ~TaggableTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef TaggableTree_cxx
TaggableTree::TaggableTree(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("../ntg.root");
      if (!f) {
         f = new TFile("../ntg.root");
      }
      tree = (TTree*)gDirectory->Get("TaggableTree");

   }
   Init(tree);
}

TaggableTree::~TaggableTree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t TaggableTree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   Int_t entryExists = fChain->GetEntry(entry);

   cluster.Clear();

   for (unsigned int i=0; i<Type->size(); i++) {
     Taggable taggable(static_cast<TaggableType>(Type->at(i)),
                       t->at(i), E->at(i), TVector3(tagvx->at(i), tagvy->at(i), tagvz->at(i)));
     cluster.Append(taggable);
   }

   return entryExists;
}
Long64_t TaggableTree::LoadTree(Long64_t entry)
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

void TaggableTree::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   Type = 0;
   TaggedType = 0;
   t = 0;
   E = 0;
   tagvx = 0;
   tagvy = 0;
   tagvz = 0;
   DistFromPV = 0;
   DWall = 0;
   EarlyIndex = 0;
   DelayedIndex = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("Type", &Type, &b_Type);
   fChain->SetBranchAddress("TaggedType", &TaggedType, &b_TaggedType);
   fChain->SetBranchAddress("t", &t, &b_t);
   fChain->SetBranchAddress("E", &E, &b_E);
   fChain->SetBranchAddress("tagvx", &tagvx, &b_tagvx);
   fChain->SetBranchAddress("tagvy", &tagvy, &b_tagvy);
   fChain->SetBranchAddress("tagvz", &tagvz, &b_tagvz);
   fChain->SetBranchAddress("DistFromPV", &DistFromPV, &b_DistFromPV);
   fChain->SetBranchAddress("DWall", &DWall, &b_DWall);
   fChain->SetBranchAddress("EarlyIndex", &EarlyIndex, &b_EarlyIndex);
   fChain->SetBranchAddress("DelayedIndex", &DelayedIndex, &b_DelayedIndex);
   Notify();
}

Bool_t TaggableTree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void TaggableTree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t TaggableTree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef TaggableTree_cxx
