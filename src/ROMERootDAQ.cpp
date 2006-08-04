// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMERootDAQ
//
//  Interface to root trees.
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////

#include <TBranchElement.h>
#include "ROMEAnalyzer.h"
#include "ROMERootDAQ.h"

ClassImp(ROMERootDAQ)

ROMERootDAQ::ROMERootDAQ() {
   fRootFiles = 0;
   fTrees = new TObjArray(1);
}

ROMERootDAQ::~ROMERootDAQ() {
   SafeDelete(fTrees);
   SafeDeleteArray(fRootFiles);
}

Bool_t ROMERootDAQ::Init() {
   if (gROME->isOnline()) {
      ROMEPrint::Error("Root mode is not supported for online analysis.\n");
      return false;
   }
   ROMEPrint::Print("Program is running offline.\n");
   if ((!gROME->IsFileNameBasedIO() && !gROME->IsRunNumberAndFileNameBasedIO())) {
      ROMEPrint::Error("No inputfile specified for Root mode.\n");
      return false;
   }
   return true;
}

Bool_t ROMERootDAQ::BeginOfRun() {
   int i;
   ROMEString filename;
   const Int_t nInputFile = gROME->GetNumberOfInputFileNames();
   fRootFiles = new TFile*[nInputFile];
   for (i=0;i<nInputFile;i++) {
      filename.SetFormatted("%s%s",gROME->GetInputDir(),gROME->GetInputFileNameAt(i).Data());
      fRootFiles[i] = new TFile(filename.Data(),"READ");
      if (fRootFiles[i]->IsZombie()) {
         ROMEPrint::Warning("Inputfile '%s' not found.\n", filename.Data());
         return false;
      }
   }
   if (!CreateTrees())
      return false;
   if (!ConnectTrees())
      return false;
   return true;
}

Bool_t ROMERootDAQ::Event(Long64_t event) {
   int j;
   // read event
   for (j=0;j<fTrees->GetEntriesFast();j++) {
      if (((TTree*)fTrees->At(j))->GetEntry(event) <= 0) {
         this->SetEndOfRun();
         return true;
      }
   }
   gROME->SetCurrentEventNumber(event);
   return true;
}

Long64_t ROMERootDAQ::Seek(Long64_t event) {
   if (event < 0)
      return Seek(0);

   int j;
   // read event
   for (j=0;j<fTrees->GetEntriesFast();j++) {
      if(event >= ((TTree*)fTrees->At(j))->GetEntries())
         event = ((TTree*)fTrees->At(j))->GetEntries() - 1;
   }
   return event;
}

Bool_t ROMERootDAQ::EndOfRun() {
   int i;
   ROMEString filename;
   const Int_t nInputFile = gROME->GetNumberOfInputFileNames();
   for (i=0;i<nInputFile;i++) {
      fRootFiles[i]->Close();
   }
   SafeDeleteArray(fRootFiles);
   return true;
}

Bool_t ROMERootDAQ::Terminate() {
   return true;
}



