/********************************************************************
  ROMETree.h, M. Schneebeli PSI

  $Log$
  Revision 1.7  2004/10/14 09:53:41  schneebeli_m
  ROME configuration file format changed and extended, Folder Getter changed : GetXYZObject -> GetXYZ, tree compression level and fill flag

  Revision 1.6  2004/10/05 14:01:16  schneebeli_m
  Circular Trees

  Revision 1.5  2004/09/25 01:34:48  schneebeli_m
  implemented FW dependent EventLoop and DataBase classes

********************************************************************/
#ifndef ROMETree_H
#define ROMETree_H

#include<TObject.h>
#include<ROMEString.h>
#include<TTree.h>

class ROMETree : public TObject 
{
private:
   struct Switches {
      int fRead;              //!   Read Flag
      int fWrite;             //!   Write Flag
      int fFill;              //!   Fill Flag
      int fCompressionLevel;  //!   Compression Level
      int fMaxEntries;        //!   Max number of entries, 0 in case of a non circular tree
   } fSwitches;               //!   Switches Structure

   ROMEString fSwitchesString;   //!   Switches String

protected:
   TTree *fTree;              //   Tree
public:
   ROMETree(TTree *tree=NULL,Bool_t read=0,Bool_t write=0,Bool_t fill=0,Bool_t compressionLevel=0,Int_t maxEntries=0)
   { fTree = tree; fSwitches.fRead = read; fSwitches.fWrite = write; fSwitches.fFill = fill; fSwitches.fCompressionLevel = compressionLevel; fSwitches.fMaxEntries = maxEntries; 
     if (maxEntries>0) fTree->SetCircular(maxEntries);
     TObjArray *branches = fTree->GetListOfBranches();
     for (int i=0;i<branches->GetEntriesFast();i++) ((TBranch*)branches->At(i))->SetCompressionLevel(compressionLevel);
     fSwitchesString =  "Read = BOOL : 0\nWrite = BOOL : 0\nFill = BOOL : 0\nMax Entries = INT : 0\n"; };
   TTree *GetTree() { return fTree; };
   Bool_t isRead() { return fSwitches.fRead!=0; };
   Bool_t isWrite() { return fSwitches.fWrite!=0; };
   Bool_t isFill() { return fSwitches.fFill!=0; };
   Int_t  GetCompressionLevel() { return fSwitches.fCompressionLevel; };
   Bool_t isCircular() { return fSwitches.fMaxEntries!=0; };
   Int_t  GetMaxEntries() { return fSwitches.fMaxEntries; };
   Switches* GetSwitches() { return &fSwitches; };
   int       GetSwitchesSize() { return sizeof(Switches); };
   char*     GetSwitchesString() { return (char*)fSwitchesString.Data(); };
   void SetTree(TTree *tree) { fTree = tree; };
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
      if (maxEntries>0) fTree->SetCircular(maxEntries);
   };
};

#endif   // ROMETree_H
