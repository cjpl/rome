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
#endif                          // R__VISUAL_CPLUSPLUS
#include <TSystem.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif                          // R__VISUAL_CPLUSPLUS
#include "ROMETree.h"
#include "ROME.h"

ClassImp(ROMETree)

//______________________________________________________________________________
ROMETree::ROMETree(TTree *tree, ROMEString fileName, ROMEString configInputFileName,
                   ROMEString configOutputFileName, TFile *file, Int_t fileOption,
                   Bool_t read, Bool_t write, Bool_t fill, Bool_t saveConfig, Int_t compressionLevel,
                   Long64_t maxEntries)
:TNamed()
,fSwitches()
,fSwitchesString("Read = BOOL : 0\n"
                 "Write = BOOL : 0\n"
                 "Fill = BOOL : 0\n"
                 "Save Config = BOOL : 0\n"
                 "Compression Level = INT : 1\n"
                 "Max Entries = INT : 0\n")
#if (ROOT_VERSION_CODE >= ROOT_VERSION(5,26,0))
,fAutoSaveSize(300000000)
,fLastSaveSize(0)
,fAutoFlushSize(-30000000)
,fCacheSize(-1)
#else
,fAutoSaveSize(100000000)
,fLastSaveSize(0)
,fAutoFlushSize(-30000000)
,fCacheSize(10000000)
#endif
,fBranchActive(0)
,fNBranchActive(0)
,fBranchRead(0)
,fNBranchRead(0)
,fTree(tree)
,fFileName(fileName)
,fConfigInputFileName(configInputFileName)
,fConfigOutputFileName(configOutputFileName)
,fFile(file)
,fFileOption(fileOption)
{
   fSwitches.fRead = read;
   fSwitches.fWrite = write;
   fSwitches.fFill = fill;
   fSwitches.fSaveConfig = saveConfig;
   fSwitches.fCompressionLevel = compressionLevel;
   fSwitches.fMaxEntries = static_cast<Int_t>(maxEntries);
   /* note: use 4byte integer for odb */
   if (maxEntries > 0) {
      fTree->SetCircular(maxEntries);
   }
   TObjArray *branches = fTree->GetListOfBranches();
   for (Int_t i = 0; i < branches->GetEntriesFast(); i++) {
      static_cast <TBranch*>(branches->At(i))->SetCompressionLevel(compressionLevel);
   }
}

//______________________________________________________________________________
ROMETree::~ROMETree()
{
#if 0                           /* this class does not own it */
   SafeDeleteArray(fTree);
   SafeDeleteArray(fFile);
#endif
   SafeDeleteArray(fBranchActive);
   SafeDeleteArray(fBranchRead);
}

//______________________________________________________________________________
void ROMETree::AllocateBranchActive(Int_t n)
{
   fNBranchActive = n;
   SafeDeleteArray(fBranchActive);
   fBranchActive = new Bool_t[n];
   for (int i = 0; i < n; i++) {
      fBranchActive[i] = kTRUE;
   }
}

//______________________________________________________________________________
void ROMETree::AllocateBranchRead(Int_t n)
{
   fNBranchRead = n;
   SafeDeleteArray(fBranchRead);
   fBranchRead = new Bool_t[n];
   for (int i = 0; i < n; i++) {
      fBranchRead[i] = kTRUE;
   }
}

//______________________________________________________________________________
Bool_t ROMETree::CheckConfiguration(const char *inDir, const char *outDir) const
{
   if (fSwitches.fRead &&
       fSwitches.fWrite && fConfigInputFileName == fConfigOutputFileName && strcmp(inDir, outDir) == 0) {
      Error("CheckConfiguration", "It is not possible to read and write one file.\n"
            "Please use different <InputFilePath> and <OutputFilePath> or different <TreeInputFileName> and <TreeOutputFileName> "
            "for '%s' tree.\n", fName.Data());
      return kFALSE;
   }
   return kTRUE;
}

//______________________________________________________________________________
Long64_t ROMETree::AutoSave(Option_t *option)
{
   Long64_t ret = 0;
   if (fAutoSaveSize > 0 && isWrite() && fTree && fTree->GetCurrentFile()) {
      ret = fTree->AutoSave(option);
#if (ROOT_VERSION_CODE > ROOT_VERSION(4,4,2))
      fLastSaveSize = fTree->GetCurrentFile()->GetBytesWritten();
#else
      fLastSaveSize = static_cast<Long64_t>(fTree->GetCurrentFile()->GetBytesWritten());
#endif
      TString opt = option;
      opt.ToLower();
   }
   return ret;
}

//______________________________________________________________________________
Bool_t ROMETree::CheckAutoSave()
{
   Long64_t currentSize;
   if (fAutoSaveSize > 0 && isWrite() && fTree && fTree->GetCurrentFile()) {
#if (ROOT_VERSION_CODE > ROOT_VERSION(4,4,2))
      currentSize = fTree->GetCurrentFile()->GetBytesWritten();
#else
      currentSize = static_cast<Long64_t>(fTree->GetCurrentFile()->GetBytesWritten());
#endif
      if (currentSize < fLastSaveSize) {
         // probably new file is created.
         fLastSaveSize = 0;
      }
      if (currentSize > fLastSaveSize + fAutoSaveSize) {
         return kTRUE;
      }
   }
   return kFALSE;
}
