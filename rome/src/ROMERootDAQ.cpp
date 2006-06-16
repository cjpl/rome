// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMERootDAQ
//
//  Interface to root trees.
//
//  $Id: ROMERootDAQ.cpp 1141 2006-06-10 14:54:29Z sawada $
//
//////////////////////////////////////////////////////////////////////////

#include <TBranchElement.h>
#include "ROMEAnalyzer.h"
#include "ROMERootDAQ.h"

ClassImp(ROMERootDAQ)

ROMERootDAQ::ROMERootDAQ() {
   fTrees = new TObjArray(1);
}

Bool_t ROMERootDAQ::Init() {
   if (gROME->isOnline()) {
      gROME->PrintLine("Root mode is not supported for online analysis.\n");
      return false;
   }
   gROME->PrintLine("Program is running offline.\n");
   if ((!gROME->IsFileNameBasedIO() && !gROME->IsRunNumberAndFileNameBasedIO())) {
      gROME->PrintText("No inputfile specified for Root mode.");
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
         gROME->PrintText("Inputfile '");
         gROME->PrintText(filename.Data());
         gROME->PrintLine("' not found.");
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
      if (((TTree*)fTrees->At(j))->GetEntry(event)<=0) {
         this->SetEndOfRun();
         return true;
      }
   }
   return true;
}

Bool_t ROMERootDAQ::EndOfRun() {
   int i;
   ROMEString filename;
   const Int_t nInputFile = gROME->GetNumberOfInputFileNames();
   for (i=0;i<nInputFile;i++) {
      fRootFiles[i]->Close();
   }
   return true;
}

Bool_t ROMERootDAQ::Terminate() {
   return true;
}



