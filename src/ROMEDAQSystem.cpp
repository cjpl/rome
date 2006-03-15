// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMEDAQSystem
//
//  Base class for DAQ systems.
//
//  $Id:$
//
//////////////////////////////////////////////////////////////////////////

#include <ROMEDAQSystem.h>
#include <ROMEAnalyzer.h>

Bool_t ROMEDAQSystem::InitDAQ() 
{ 
   fWatchEvent.Reset(); 
   fWatchAll.Reset(); 
   return Init();
};
Bool_t ROMEDAQSystem::BeginOfRunDAQ() 
{ 
   Bool_t ret;
   fWatchAll.Start(false); 
   ret = BeginOfRun(); 
   fWatchAll.Stop(); 
   return ret;
};
Bool_t ROMEDAQSystem::EventDAQ(Long64_t event) 
{ 
   Bool_t ret;
   fWatchAll.Start(false); 
   fWatchEvent.Start(false); 
   ret = Event(event); 
   fWatchEvent.Stop(); 
   fWatchAll.Stop(); 
   return ret;
};
Bool_t ROMEDAQSystem::EndOfRunDAQ() 
{ 
   Bool_t ret;
   fWatchAll.Start(false); 
   ret = EndOfRun(); 
   fWatchAll.Stop(); 
   return ret;
};
Bool_t ROMEDAQSystem::TerminateDAQ() 
{ 
   int i;
   ROMEString str;
   gROME->PrintText(" DAQ ");
   str = GetName();
   gROME->PrintText(str.Data());
   for (i=0;i<30-4-1-str.Length();i++)
      gROME->PrintText(".");
   gROME->PrintText(" : ");
   fWatchAll.GetRealTimeString(str);
   gROME->PrintText(str.Data());
   gROME->PrintText("  ");
   fWatchEvent.GetRealTimeString(str);
   gROME->PrintLine(str.Data());
   return Terminate(); 
};
