/********************************************************************
  ROMERomeDAQ.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMERomeDAQ_H
#define ROMERomeDAQ_H

#include <TFile.h>
#include "ROMEString.h"
#include "ROMETreeInfo.h"
#include "ROMEDAQSystem.h"

class ROMERomeDAQ : public ROMEDAQSystem {
protected:
   TFile       **fRootFiles;                       //! Input Root files
   Int_t         fTreeIndex;                       //! Index of tree in current file
   Int_t         fInputFileNameIndex;              //! Index of current input file name
   ROMEString    fCurrentTreeName;                 //! Name of the current tree

   // Tree Info
   ROMETreeInfo *fTreeInfo;                        //! Tree Info Object
   Long64_t*     fTreePosition;                    //! Array with tree read positions
   Long64_t*     fTreeNextSeqNumber;               //! Array with the trees next sequential number

   Int_t         fTimeStamp;                       //! Current time stamp

public:
   ROMERomeDAQ();
   virtual ~ROMERomeDAQ() {};

   Int_t        GetTimeStamp() { return fTimeStamp; };
   const char  *GetName() { return "rome"; };

   Bool_t       Init();
   Bool_t       BeginOfRun();
   Bool_t       Event(Long64_t event);
   Bool_t       EndOfRun();
   Bool_t       Terminate();

   virtual void ConnectTrees() = 0;

   ClassDef(ROMERomeDAQ, 0) // Base DAQ class for trees
};

#endif   // ROMERomeDAQ_H
