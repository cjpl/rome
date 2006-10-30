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

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TSystem.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS
#include "ROMETree.h"
#include "ROME.h"

ClassImp(ROMETree)

ROMETree::ROMETree(TTree *tree, ROMEString fileName, ROMEString configFileName, TFile* file, Int_t fileOption, Bool_t read, Bool_t write, Bool_t fill, Bool_t saveConfig, Int_t compressionLevel, Long64_t maxEntries) {
   fTree = tree;
   fFileName = fileName;
   fConfigFileName = configFileName;
   fFile = file;
   fFileOption = fileOption;
   fSwitches.fRead = read;
   fSwitches.fWrite = write;
   fSwitches.fFill = fill;
   fSwitches.fSaveConfig = saveConfig;
   fSwitches.fCompressionLevel = compressionLevel;
   fSwitches.fMaxEntries = static_cast<Int_t>(maxEntries);
   /* note: use 4byte integer for odb */
#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))
   if (maxEntries>0) fTree->SetCircular(maxEntries);
#endif
   TObjArray *branches = fTree->GetListOfBranches();
   for (Int_t i=0;i<branches->GetEntriesFast();i++)
      ((TBranch*)branches->At(i))->SetCompressionLevel(compressionLevel);
   fSwitchesString =  "Read = BOOL : 0\nWrite = BOOL : 0\nFill = BOOL : 0\nSave Config = BOOL : 0\nCompression Level = INT : 0\nMax Entries = INT : 0\n";
   fBranchActive = 0;
}

ROMETree::~ROMETree() {
   SafeDeleteArray(fBranchActive);
}

void ROMETree::AllocateBranchActive(Int_t n) {
   fNBranchActive = n;
   if (fBranchActive) {
      delete [] fBranchActive;
      fBranchActive;
   }
   fBranchActive = new Bool_t[n];
   for(int i=0;i<n;i++) fBranchActive[i]=kTRUE;
}

Bool_t ROMETree::SaveConfig(const char* xml, const char* filename) {
   if(!fFile->cd())
      return kFALSE;
   TDirectory *configDir;
   fFile->GetObject("configuration", configDir);
   if (!configDir)
      gDirectory->mkdir("configuration", "directory for configuration XML file");
   gDirectory->cd("configuration");
   TNamed *config;
   TString filename_mod = filename;
   filename_mod.ReplaceAll(".xml", "");
   if (!filename) {
      filename_mod = "config";
   }
   else {
      filename_mod = gSystem->BaseName(filename);
      filename_mod.ReplaceAll(".xml", "");
   }
   config = new TNamed(filename_mod.Data(), xml);
   config->Write(config->GetName(), TObject::kOverwrite);
   delete config;
   return kTRUE;
}

Bool_t ROMETree::LoadConfig(TString &xml, const char* filename) {
   if(!fFile)
      return kFALSE;
   TNamed *config;
   TString path = "configuration/";
   TString name = filename;
   if (!name.Length()) {
      fFile->cd();
      gDirectory->cd("configuration");
      if(!gDirectory->GetNkeys())
         return kFALSE;
      name = gDirectory->GetListOfKeys()->At(0)->GetName();
   }
   path += name;
   fFile->GetObject(path.Data(), config);
   if (!config)
      return kFALSE;
   xml = config->GetTitle();
   return kTRUE;
}
