/********************************************************************
  ROMERootDAQ.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMERootDAQ_H
#define ROMERootDAQ_H

#include <TFile.h>
#include "ROMEString.h"
#include "ROMEDAQSystem.h"

class ROMERootDAQ : public ROMEDAQSystem {
protected:
   TFile       **fRootFiles;  //! Input Root files
   TObjArray   *fTrees;       //! Array of Trees

public:
   ROMERootDAQ();
   virtual ~ROMERootDAQ();

   Int_t        GetTimeStamp() { return 0; }
   const char  *GetName() const { return "root"; }

   Bool_t       Init();
   Bool_t       BeginOfRun();
   Bool_t       Event(Long64_t event);
   Bool_t       EndOfRun();
   Bool_t       Terminate();

   virtual Bool_t ConnectTrees() = 0;
   virtual Bool_t CreateTrees() = 0;

   ClassDef(ROMERootDAQ, 0)
};

#endif   // ROMERootDAQ_H
