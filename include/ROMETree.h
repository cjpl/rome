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
public:
   ROMETree(TTree *tree=NULL,Bool_t read=0,Bool_t write=0)
   { fTree = tree; fRead = read; fWrite = write; };
   TTree *GetTree() { return fTree; };
   Bool_t isRead() { return fRead; };
   Bool_t isWrite() { return fWrite; };
   void SetTree(TTree *tree) { fTree = tree; };
   void SetRead(Bool_t read) { fRead = read; };
   void SetWrite(Bool_t write) { fWrite = write; };
};

#endif   // ROMETree_H
