/********************************************************************
  ROMETree.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMETree_H
#define ROMETree_H

#include <TObject.h>
#include <TTree.h>
#include <TFile.h>
#include "ROMEString.h"

class ROMETree : public TNamed
{
private:
   // File Options
   enum {
      kOverWrite,
      kUpdate
   };
   struct Switches {
      Int_t    fRead;              //!   Read Flag
      Int_t    fWrite;             //!   Write Flag
      Int_t    fFill;              //!   Fill Flag
      Int_t    fSaveConfig;        //!   Save Config Flag
      Int_t    fCompressionLevel;  //!   Compression Level
      Int_t    fMaxEntries;        //!   Max number of entries, 0 in case of a non circular tree
     /* note: use 4byte integer for odb */
   } fSwitches;                    //!   Switches Structure

   ROMEString  fSwitchesString;    //!   Switches String
   Bool_t     *fBranchActive;      //!   Flag if brahch is active
   Int_t       fNBranchActive;     //!   Number of branch active
protected:
   TTree      *fTree;              //    Tree
   ROMEString  fFileName;          //!   Name of the File for the Tree Object
   ROMEString  fConfigInputFileName;  //!   Name of the File for the Tree Object in the romeConfig file
   ROMEString  fConfigOutputFileName; //!   Name of the File for the Tree Object in the romeConfig file
   TFile*      fFile;              //!   File Handle for the Tree Object
   Int_t       fFileOption;        //!   File Option for the Tree Object

public:
   ROMETree(TTree *tree=NULL,ROMEString fileName="",ROMEString configInputFileName="",
            ROMEString configOUtputFileName="",TFile* file=NULL,Int_t fileOption=kOverWrite,
            Bool_t read=0,Bool_t write=0,Bool_t fill=0,Bool_t saveConfig=kFALSE,
            Int_t compressionLevel=0,Long64_t maxEntries=0);
   virtual ~ROMETree();

   void        AllocateBranchActive(Int_t n);
   TTree      *GetTree() { return fTree; }
   ROMEString &GetFileName() { return fFileName; }
   ROMEString &GetConfigInputFileName() { return fConfigInputFileName; }
   ROMEString &GetConfigOutputFileName() { return fConfigOutputFileName; }
   void        UpdateFilePointer() { fFile = fTree->GetCurrentFile(); }
   TFile      *GetFile() { return fFile; }
   Bool_t      IsFileOverWrite() { return fFileOption==kOverWrite; }
   Bool_t      IsFileUpdate() { return fFileOption==kUpdate; }
   Bool_t      isRead() { return fSwitches.fRead!=0; }
   Bool_t      isWrite() { return fSwitches.fWrite!=0; }
   Bool_t      isFill() { return fSwitches.fFill!=0; }
   Bool_t      isSaveConfig() { return fSwitches.fSaveConfig!=0; }
   Int_t       GetCompressionLevel() { return fSwitches.fCompressionLevel; }
   Bool_t      isCircular() {
#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))
                  return fSwitches.fMaxEntries!=0;
#else
                  return false;
#endif
               }
   Long64_t    GetMaxEntries() { return fSwitches.fMaxEntries; }
   Switches   *GetSwitches() { return &fSwitches; }
   Int_t       GetSwitchesSize() { return sizeof(Switches); }
   const char *GetSwitchesString() { return fSwitchesString.Data(); }
   Int_t       GetNBranchActive() { return fNBranchActive; }
   Bool_t     *GetBranchActive() { return fBranchActive; }
   Bool_t      GetBranchActiveAt(Int_t i) { return fBranchActive[i]; }
   void        SetTree(TTree *tree) { fTree = tree; }
   void        SetFileName(ROMEString &fileName) { fFileName = fileName; }
   void        SetConfigInputFileName(ROMEString &configFileName) { fConfigInputFileName = configFileName; }
   void        SetConfigOutputFileName(ROMEString &configFileName) { fConfigOutputFileName = configFileName; }
   void        SetFile(TFile *file) { fFile = file; fTree->SetDirectory((TDirectory*)file); }
   void        SetFileOverWrite() { fFileOption = kOverWrite; }
   void        SetFileUpdate() { fFileOption = kUpdate; }
   void        SetRead(Bool_t read) { fSwitches.fRead = read; }
   void        SetWrite(Bool_t write) { fSwitches.fWrite = write; }
   void        SetFill(Bool_t fill) { fSwitches.fFill = fill; }
   void        SetSaveConfig(Bool_t saveConfig) { fSwitches.fSaveConfig = saveConfig; }
   void        SetCompressionLevel(Int_t compressionLevel) {
                  fSwitches.fCompressionLevel = compressionLevel;
                  TObjArray *branches = fTree->GetListOfBranches();
                  for (Int_t i=0;i<branches->GetEntriesFast();i++) ((TBranch*)branches->At(i))->SetCompressionLevel(compressionLevel);
               }
   void        SetMaxEntries(Long64_t maxEntries) {
                  fSwitches.fMaxEntries = static_cast<Int_t>(maxEntries);
                  /* note: use 4byte integer for odb */
                  if (maxEntries>0) fTree->SetCircular(maxEntries);
               }
   void        SetBranchActiveAt(Int_t i, Bool_t active) { fBranchActive[i] = active; }
   Bool_t      CheckConfiguration(const char* inDir, const char* outDir);

   ClassDef(ROMETree,0) // TTree container for ROME
};

#endif   // ROMETree_H
