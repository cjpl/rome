/********************************************************************
  ROMETree.h, M. Schneebeli PSI

  $Id:$

********************************************************************/
#ifndef ROMETree_H
#define ROMETree_H

#include<TObject.h>
#include<ROMEString.h>
#include<TTree.h>

class ROMETree : public TObject
{
private:
   // File Options
   enum {
      kOverWrite,
      kUpdate
   };
   struct Switches {
      int fRead;              //!   Read Flag
      int fWrite;             //!   Write Flag
      int fFill;              //!   Fill Flag
      int fCompressionLevel;  //!   Compression Level
      int fMaxEntries;        //!   Max number of entries, 0 in case of a non circular tree
   } fSwitches;               //!   Switches Structure

   ROMEString fSwitchesString;   //!   Switches String

protected:
   TTree *fTree;              //    Tree
   ROMEString fFileName;      //!   Name of the File for the Tree Object
   ROMEString fConfigFileName;//!   Name of the File for the Tree Object in the romeConfig file
   TFile*     fFile;          //!   File Handle for the Tree Object
   int        fFileOption;    //!   File Option for the Tree Object

public:
   ROMETree(TTree *tree=NULL,ROMEString fileName="",ROMEString configFileName="",TFile* file=NULL,int fileOption=kOverWrite,Bool_t read=0,Bool_t write=0,Bool_t fill=0,Int_t compressionLevel=0,Int_t maxEntries=0)
   { fTree = tree; fFileName = fileName; fConfigFileName = configFileName; fFile = file; fFileOption = fileOption; fSwitches.fRead = read; fSwitches.fWrite = write; fSwitches.fFill = fill; fSwitches.fCompressionLevel = compressionLevel; fSwitches.fMaxEntries = maxEntries; 
#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))
     if (maxEntries>0) fTree->SetCircular(maxEntries);
#endif 
     TObjArray *branches = fTree->GetListOfBranches();
     for (int i=0;i<branches->GetEntriesFast();i++)
        ((TBranch*)branches->At(i))->SetCompressionLevel(compressionLevel);
     fSwitchesString =  "Read = BOOL : 0\nWrite = BOOL : 0\nFill = BOOL : 0\nCompression Level = INT : 0\nMax Entries = INT : 0\n"; };
   TTree *GetTree() { return fTree; };
   ROMEString &GetFileName() { return fFileName; };
   ROMEString &GetConfigFileName() { return fConfigFileName; };
   TFile *GetFile() { return fFile; };
   Bool_t IsFileOverWrite() { return fFileOption==kOverWrite; };
   Bool_t IsFileUpdate() { return fFileOption==kUpdate; };
   Bool_t isRead() { return fSwitches.fRead!=0; };
   Bool_t isWrite() { return fSwitches.fWrite!=0; };
   Bool_t isFill() { return fSwitches.fFill!=0; };
   Int_t  GetCompressionLevel() { return fSwitches.fCompressionLevel; };
   Bool_t isCircular() { return fSwitches.fMaxEntries!=0; };
   Int_t  GetMaxEntries() { return fSwitches.fMaxEntries; };
   Switches* GetSwitches() { return &fSwitches; };
   int       GetSwitchesSize() { return sizeof(Switches); };
   const char* GetSwitchesString() { return fSwitchesString.Data(); };
   void SetTree(TTree *tree) { fTree = tree; };
   void SetFileName(ROMEString &fileName) { fFileName = fileName; };
   void SetConfigFileName(ROMEString &configFileName) { fConfigFileName = configFileName; };
   void SetFile(TFile *file) { fFile = file; fTree->SetDirectory((TDirectory*)file); };
   void SetFileOverWrite() { fFileOption = kOverWrite; };
   void SetFileUpdate() { fFileOption = kUpdate; };
   void SetRead(Bool_t read) { fSwitches.fRead = read; };
   void SetWrite(Bool_t write) { fSwitches.fWrite = write; };
   void SetFill(Bool_t fill) { fSwitches.fFill = fill; };
   void SetCompressionLevel(Int_t compressionLevel) {
      fSwitches.fCompressionLevel = compressionLevel;
      TObjArray *branches = fTree->GetListOfBranches();
      for (int i=0;i<branches->GetEntriesFast();i++) ((TBranch*)branches->At(i))->SetCompressionLevel(compressionLevel);
   };
   void SetMaxEntries(Int_t maxEntries)
   {  fSwitches.fMaxEntries = maxEntries;
#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))
      if (maxEntries>0) fTree->SetCircular(maxEntries);
#endif 
   };
};

#endif   // ROMETree_H
