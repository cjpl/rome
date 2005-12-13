// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMEDataBaseDAQ
//
//  Interface to a database used as the main input.
//
//  $Id: ROMEDataBaseDAQ.cpp 680 2005-10-31 11:32:46Z ritt $
//
//////////////////////////////////////////////////////////////////////////

#include <ROMEDataBaseDAQ.h>
#include <ROMEAnalyzer.h>

ROMEDataBaseDAQ::ROMEDataBaseDAQ() {
}

bool ROMEDataBaseDAQ::Init() {
   if (gROME->isOnline()) {
      gROME->PrintLine("Database mode is not supported for online analysis.\n");
      return false;
   }
   if (gROME->isOffline()) {
      gROME->SetEventBasedDataBase();
   }
   return true;
}
bool ROMEDataBaseDAQ::BeginOfRun() {
   if (gROME->isOffline()) {
      gROME->SetCurrentEventNumber(1);
      this->SetAnalyze();
      this->SetRunning();
   }
   return true;
}
bool ROMEDataBaseDAQ::Event(int event) {
   if (gROME->isOffline()) {
      gROME->SetCurrentEventNumber(event);
   }

   return true;
}
bool ROMEDataBaseDAQ::EndOfRun() {
   if (gROME->isOffline()) {
   }
   return true;
}
bool ROMEDataBaseDAQ::Terminate() {
   if (gROME->isOffline()) {
   }
   return true;
}



