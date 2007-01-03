// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMEDAQSystem
//
//  Base class for DAQ systems.
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////

#include "ROMEDAQSystem.h"
#include "ROMEAnalyzer.h"

ClassImp(ROMEDAQSystem)

Bool_t ROMEDAQSystem::InitDAQ()
{
   fWatchEvent.Reset();
   fWatchAll.Reset();
   return Init();
}

Bool_t ROMEDAQSystem::BeginOfRunDAQ()
{
   Bool_t ret;
   fWatchAll.Start(false);
   ret = BeginOfRun();
   fWatchAll.Stop();
   return ret;
}

Bool_t ROMEDAQSystem::EventDAQ(Long64_t event)
{
   Bool_t ret;
   gROME->SetEventID(-1);
   fWatchAll.Start(false);
   fWatchEvent.Start(false);
   ret = Event(event);
   fWatchEvent.Stop();
   fWatchAll.Stop();
   return ret;
}

Bool_t ROMEDAQSystem::EndOfRunDAQ()
{
   Bool_t ret;
   fWatchAll.Start(false);
   ret = EndOfRun();
   fWatchAll.Stop();
   return ret;
}

Bool_t ROMEDAQSystem::TerminateDAQ()
{
   return Terminate();
}

Bool_t ROMEDAQSystem::TimeDAQ()
{
   int i;
   ROMEString str = GetName();
   ROMEPrint::Print(" DAQ %s", str.Data());
   for (i=0;i<30-4-1-str.Length();i++)
      ROMEPrint::Print(".");
   ROMEPrint::Print(" : %s\n", fWatchAll.GetRealTimeString(str), fWatchEvent.GetRealTimeString(str));
   return true;
}
