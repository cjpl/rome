// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMETree
//
//  Tree container for ROME
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////

#include "ROMETree.h"
#include "ROME.h"

ClassImp(ROMETree)

ROMETree::ROMETree(TTree *tree, ROMEString fileName, ROMEString configFileName, TFile* file, Int_t fileOption, Bool_t read, Bool_t write, Bool_t fill, Int_t compressionLevel, Long64_t maxEntries) {
   fTree = tree;
   fFileName = fileName;
   fConfigFileName = configFileName;
   fFile = file;
   fFileOption = fileOption;
   fSwitches.fRead = read;
   fSwitches.fWrite = write;
   fSwitches.fFill = fill;
   fSwitches.fCompressionLevel = compressionLevel;
   fSwitches.fMaxEntries = static_cast<Int_t>(maxEntries);
   /* note: use 4byte integer for odb */
#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))
   if (maxEntries>0) fTree->SetCircular(maxEntries);
#endif
   TObjArray *branches = fTree->GetListOfBranches();
   for (Int_t i=0;i<branches->GetEntriesFast();i++)
      ((TBranch*)branches->At(i))->SetCompressionLevel(compressionLevel);
   fSwitchesString =  "Read = BOOL : 0\nWrite = BOOL : 0\nFill = BOOL : 0\nCompression Level = INT : 0\nMax Entries = INT : 0\n";
   fBranchActive = 0;
}

ROMETree::~ROMETree() {
   SafeDeleteArray(fBranchActive);
}
