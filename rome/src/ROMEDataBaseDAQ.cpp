// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMEDataBaseDAQ
//
//  Interface to a database used as the main input.
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////

#include <ROMEDataBaseDAQ.h>
#include <ROMEAnalyzer.h>

ClassImp(ROMEDataBaseDAQ)

ROMEDataBaseDAQ::ROMEDataBaseDAQ() {
}

Bool_t ROMEDataBaseDAQ::Init() {
   if (gROME->isOnline()) {
      gROME->PrintLine("Database mode is not supported for online analysis.\n");
      return false;
   }
   if (gROME->isOffline()) {
      gROME->SetEventBasedDataBase();
   }
   return true;
}

Bool_t ROMEDataBaseDAQ::BeginOfRun() {
   if (gROME->isOffline()) {
      gROME->SetCurrentEventNumber(1);
      this->SetAnalyze();
      this->SetRunning();
   }
   return true;
}

Bool_t ROMEDataBaseDAQ::Event(Long64_t event) {
   if (gROME->isOffline()) {
      gROME->SetCurrentEventNumber(event);
   }

   return true;
}

Bool_t ROMEDataBaseDAQ::EndOfRun() {
   if (gROME->isOffline()) {
   }
   return true;
}

Bool_t ROMEDataBaseDAQ::Terminate() {
   if (gROME->isOffline()) {
   }
   return true;
}



