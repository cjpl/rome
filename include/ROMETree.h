#ifndef ROMETree_H
#define ROMETree_H

#include<TObject.h>
#include<TString.h>
#include<TTree.h>

class ROMETree : public TObject 
{
private:
   struct Switches {
      int fRead;           //!   Read Flag
      int fWrite;          //!   Write Flag
   } fSwitches;               //!   Switches Structure

   TString fSwitchesString;   //!   Switches String

protected:
   TTree *fTree;              //   Tree
public:
   ROMETree(TTree *tree=NULL,Bool_t read=0,Bool_t write=0)
   { fTree = tree; fSwitches.fRead = read; fSwitches.fWrite = write; 
     fSwitchesString =  "Read = BOOL : 0\nWrite = BOOL : 0\n"; };
   TTree *GetTree() { return fTree; };
   Bool_t isRead() { return fSwitches.fRead!=0; };
   Bool_t isWrite() { return fSwitches.fWrite!=0; };
   Switches* GetSwitches() { return &fSwitches; };
   int       GetSwitchesSize() { return sizeof(Switches); };
   char*     GetSwitchesString() { return (char*)fSwitchesString.Data(); };
   void SetTree(TTree *tree) { fTree = tree; };
   void SetRead(Bool_t read) { fSwitches.fRead = read; };
   void SetWrite(Bool_t write) { fSwitches.fWrite = write; };
};

#endif   // ROMETree_H
