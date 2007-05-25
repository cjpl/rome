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

//______________________________________________________________________________
ROMERootDAQ::ROMERootDAQ()
:ROMEDAQSystem()
,fRootFiles(0)
,fTrees(new TObjArray(1))
{
}

//______________________________________________________________________________
ROMERootDAQ::~ROMERootDAQ() {
   SafeDelete(fTrees);
   SafeDeleteArray(fRootFiles);
}

//______________________________________________________________________________
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

//______________________________________________________________________________
Bool_t ROMERootDAQ::BeginOfRun() {
   int i;
   ROMEString filename;
   const Int_t nInputFile = gROME->GetNumberOfInputFileNames();
   fRootFiles = new TFile*[nInputFile];
   for (i=0;i<nInputFile;i++) {
      gROME->ConstructFilePath(gROME->GetInputDirString(), gROME->GetInputFileNameAt(i), filename);
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

//______________________________________________________________________________
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

//______________________________________________________________________________
Long64_t ROMERootDAQ::Seek(Long64_t event) {
   Bool_t found = kTRUE;

   if (event < 0)
      found = kFALSE;

   int j;
   // read event
   for (j=0;j<fTrees->GetEntriesFast();j++) {
      if(event >= ((TTree*)fTrees->At(j))->GetEntries())
         found = kFALSE;
   }

   if (!found) {
#if defined(R__UNIX)
      Warning("Seek", "Event number %lld was not found.", event);
#else
      Warning("Seek", "Event number %I64d was not found.", event);
#endif
      event = gROME->GetCurrentEventNumber();
      gROME->SetDontReadNextEvent();
      SetContinue();
   }

   return event;
}

//______________________________________________________________________________
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

//______________________________________________________________________________
Bool_t ROMERootDAQ::Terminate() {
   return true;
}



