// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMETask                                                             //
//                                                                      //
//  The Base Class for all Tasks in ROME.
//  The following methods should be overriden by derived Classes :
//    Init
//    BeginOfRun
//    Event
//    EndOfRun
//    Terminate
//                                                                      //
//  $Id$
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TSystem.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS
#include "ROMETask.h"
#include "ROMEAnalyzer.h"
#include <ROMERint.h>
#include "ROMEiostream.h"

ClassImp(ROMETask)

ROMETask::ROMETask(const char *name,const char *title,int level):TTask(name,title)
{
   // Initialisation of Class
   fTitle = title;
   fName = name;
   fEventID = -1;
   fLevel = level;
}

void ROMETask::Exec(Option_t *option)
{
   // Overrides the TTask::Exec Method to replace the Exec Method with the following Methods :
   // Init
   // BeginOfRun
   // Event
   // EndOfRun
   // Terminate
   char *cstop;
   if (gROME->isTerminationFlag())
      return;
   if (!strncmp(option,"Init",4)) {
      fCurrentEventMethod = "Init";
      fWatchEvent.Reset();
      fWatchAll.Reset();
      BookHisto();
      ROMEPrint::Debug("Executing %s::Init\n", ClassName());
      Init();
   }
   else if (!strncmp(option,"BeginOfRun",10)) {
      fCurrentEventMethod = "BeginOfRun";
      ResetHisto();
      fWatchAll.Start(false);
      ROMEPrint::Debug("Executing %s::BeginOfRun\n", ClassName());
      BeginOfRun();
      fWatchAll.Stop();
   }
   else if (!strncmp(option,"EndOfRun",8)) {
      fCurrentEventMethod = "EndOfRun";
      fWatchAll.Start(false);
      ROMEPrint::Debug("Executing %s::EndOfRun\n", ClassName());
      EndOfRun();
      fWatchAll.Stop();
   }
   else if (!strncmp(option,"Terminate",9)) {
      fCurrentEventMethod = "Terminate";
      ROMEPrint::Debug("Executing %s::Terminate\n", ClassName());
      Terminate();
   }
   else if (!strncmp(option,"Time",4)) {
      int i;
      ROMEString name;
      int nchars;
      nchars = 0;
      for (i=0;i<fLevel;i++)
         ROMEPrint::Print(" ");
      if (fLevel==1) {
         ROMEPrint::Print("Task ");
         nchars = 5;
      }
      else if (fLevel>1) {
         ROMEPrint::Print("SubTask ");
         nchars = 8;
      }
      name = fName;
      if (name.Last('_')>0)
         name = name(0,name.Last('_'));
      ROMEPrint::Print(name.Data());
      for (i=0;i<30-name.Length()-fLevel-nchars;i++)
         ROMEPrint::Print(".");
      ROMEPrint::Print(" : %s", GetTimeOfAll());
      if (fWatchEvent.RealTime()>0) {
         ROMEPrint::Print("  ");
         ROMEPrint::Print("%s\n", GetTimeOfEvents());
      }
      else
         ROMEPrint::Print("\n");
   }
   else if (!strncmp(option,"Event",5) && (strtol(option+5,&cstop,10)==fEventID || fEventID==-1 || strtol(option+5,&cstop,10)==-1)) {
      fCurrentEventMethod = "Event";
      fWatchAll.Start(false);
      fWatchEvent.Start(false);
      if (gROME->isFillEvent()) {
         ROMEPrint::Debug("Executing %s::Event\n", ClassName());
         Event();
      }
      fWatchEvent.Stop();
      fWatchAll.Stop();
   }
}

void ROMETask::StartRootInterpreter(const char* message) {
#if defined( R__VISUAL_CPLUSPLUS )
   ROMEPrint::Print("\nIn method %s of task %s of event number %I64d of run number %I64d\n",fCurrentEventMethod.Data(),fName.Data(),gROME->GetCurrentEventNumber(),gROME->GetCurrentRunNumber());
#else
   ROMEPrint::Print("\nIn method %s of task %s of event number %lld of run number %lld\n",fCurrentEventMethod.Data(),fName.Data(),gROME->GetCurrentEventNumber(),gROME->GetCurrentRunNumber());
#endif
   
   if (message)
      ROMEPrint::Print("%s\n", message);
   ROMEString prompt = gROME->GetProgramName();
   prompt.ToLower();
   prompt += " [%d]";
   ((TRint*)gROME->GetApplication())->SetPrompt(prompt.Data());
   ((TRint*)gROME->GetApplication())->Run(true);
   gSystem->Init();
   gROME->GetApplication()->ProcessLine(gROME->GetCintInitialisation());
}

// Time methods
const char* ROMETask::GetTimeOfEvents()
{
   // Returns the elapsed time in a readable format
   fWatchEvent.GetRealTimeString(fTimeEventString);
   return fTimeEventString.Data();
}
const char* ROMETask::GetTimeOfAll()
{
   // Returns the elapsed time in a readable format
   fWatchAll.GetRealTimeString(fTimeAllString);
   return fTimeAllString.Data();
}