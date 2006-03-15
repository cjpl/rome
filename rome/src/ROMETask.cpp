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
#include <ROMETask.h>
#include <ROMEAnalyzer.h>
#include "Riostream.h"

ClassImp(ROMETask)

ROMETask::ROMETask(const char *name,const char *title,int level):TTask(name,title)
{
   // Initialisation of Class
   fTitle = title;
   fName = name;
   fEventID = 'a';
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
   int i;
   ROMEString name;
   int nchars;
   if (gROME->isTerminationFlag())
      return;
   if (!strncmp(option,"i",1)) {
      fCurrentEventMethod = "Init";
      fWatchEvent.Reset();
      fWatchAll.Reset();
      BookHisto();
      Init();
   }
   else if (!strncmp(option,"b",1)) {
      fCurrentEventMethod = "BeginOfRun";
      ResetHisto();
      fWatchAll.Start(false);
      BeginOfRun();
      fWatchAll.Stop();
   }
   else if (!strncmp(option,"e",1)) {
      fCurrentEventMethod = "EndOfRun";
      fWatchAll.Start(false);
      EndOfRun();
      fWatchAll.Stop();
   }
   else if (!strncmp(option,"t",1)) {
      fCurrentEventMethod = "Terminate";
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
   else if ( strncmp(gROME->GetNameOfActiveDAQ(),"midas",5) ||
             ( !strncmp(&fEventID,"a",1) || !strncmp(option,&fEventID,1) )
      ) {
      fCurrentEventMethod = "Event";
      fWatchAll.Start(false);
      fWatchEvent.Start(false);
      if (gROME->isFillEvent())
         Event();
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
