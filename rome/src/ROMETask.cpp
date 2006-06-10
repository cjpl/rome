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
#include <Riostream.h>

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
      gROME->PrintVerbose("Executing %s::Init", this->ClassName());
      Init();
   }
   else if (!strncmp(option,"BeginOfRun",10)) {
      fCurrentEventMethod = "BeginOfRun";
      ResetHisto();
      fWatchAll.Start(false);
      gROME->PrintVerbose("Executing %s::BeginOfRun", this->ClassName());
      BeginOfRun();
      fWatchAll.Stop();
   }
   else if (!strncmp(option,"EndOfRun",8)) {
      fCurrentEventMethod = "EndOfRun";
      fWatchAll.Start(false);
      gROME->PrintVerbose("Executing %s::EndOfRun", this->ClassName());
      EndOfRun();
      fWatchAll.Stop();
   }
   else if (!strncmp(option,"Terminate",9)) {
      int i;
      ROMEString name;
      int nchars;
      fCurrentEventMethod = "Terminate";
      gROME->PrintVerbose("Executing %s::Terminate", this->ClassName());
      Terminate();

      nchars = 0;
      for (i=0;i<fLevel;i++)
         gROME->PrintText(" ");
      if (fLevel==1) {
         gROME->PrintText("Task ");
         nchars = 5;
      }
      else if (fLevel>1) {
         gROME->PrintText("SubTask ");
         nchars = 8;
      }
      name = fName;
      if (name.Last('_')>0)
         name = name(0,name.Last('_'));
      gROME->PrintText(name.Data());
      for (i=0;i<30-name.Length()-fLevel-nchars;i++)
         gROME->PrintText(".");
      gROME->PrintText(" : ");
      gROME->PrintText(GetTimeOfAll());
      if (fWatchEvent.RealTime()>0) {
         gROME->PrintText("  ");
         gROME->PrintLine(GetTimeOfEvents());
      }
      else
         gROME->PrintLine("");
   }
   else if (!strncmp(option,"Event",5) && (strtol(option+5,&cstop,10)==fEventID || fEventID==-1 || strtol(option+5,&cstop,10)==-1)) {
      fCurrentEventMethod = "Event";
      fWatchAll.Start(false);
      fWatchEvent.Start(false);
      if (gROME->isFillEvent()) {
         gROME->PrintVerbose("Executing %s::Event", this->ClassName());
         Event();
      }
      fWatchEvent.Stop();
      fWatchAll.Stop();
   }
}

void ROMETask::StartRootInterpreter(const char* message) {
   ROMEString text;
#if defined( R__VISUAL_CPLUSPLUS )
   text.SetFormatted("\nIn method %s of task %s of event number %I64d of run number %I64d",fCurrentEventMethod.Data(),fName.Data(),gROME->GetCurrentEventNumber(),gROME->GetCurrentRunNumber());
#else
   text.SetFormatted("\nIn method %s of task %s of event number %lld of run number %lld",fCurrentEventMethod.Data(),fName.Data(),gROME->GetCurrentEventNumber(),gROME->GetCurrentRunNumber());
#endif
   gROME->PrintLine(text.Data());
   if (message)
      gROME->PrintLine(message);
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
