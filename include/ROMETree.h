#ifndef ROMETree_H
#define ROMETree_H

#include<TObject.h>
#include<TTree.h>

class ROMETree : public TObject 
{
protected:
   TTree *fTree;         //   Tree
   Bool_t fRead;         //   Read Flag
   Bool_t fWrite;        //   Write Flag
   Bool_t fFill;         //   Fill Tree Flag
   Bool_t fAccumulation; //   Accumulation Flag
   Bool_t fFillEvent;    //   Fill Event Flag
public:
   ROMETree(TTree *tree=NULL,Bool_t read=0,Bool_t write=0,Bool_t fill=false,Bool_t accumulation=false,Bool_t fillEvent=true)
   { fTree = tree; fRead = read; fWrite = write; fFill = fill; fAccumulation = accumulation; fFillEvent = fillEvent; };
   TTree *GetTree() { return fTree; };
   Bool_t isRead() { return fRead; };
   Bool_t isWrite() { return fWrite; };
   Bool_t isFill() { return fFill; };
   Bool_t isAccumulation() { return fAccumulation; };
   Bool_t isFillEvent() { return fFillEvent; };
   void SetTree(TTree *tree) { fTree = tree; };
   void SetRead(Bool_t read) { fRead = read; };
   void SetWrite(Bool_t write) { fWrite = write; };
   void SetFill(Bool_t fill) { fFill = fill; };
   void SetAccumulation(Bool_t accumulation) { fAccumulation = accumulation; };
   void SetFillEvent(Bool_t fillEvent) { fFillEvent = fillEvent; };

   ClassDef(ROMETree,1)
};

#endif   // ROMETree_H
