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

ROMETask::ROMETask(const char *name,const char *title):TTask(name,title)
{
   // Initialisation of Class
   fTitle = title;
   fName = name;
   fEventID = 'a';
}

void ROMETask::Exec(Option_t *option)
{
   // Overrides the TTask::Exec Method to replace the Exec Method with the following Methods :
   // Init
   // BeginOfRun
   // Event
   // EndOfRun
   // Terminate
   if (gROME->isTerminationFlag())
      return;
   if (!strncmp(option,"i",1)) {
      fCurrentEventMethod = "Init";
      TimeReset();
      BookHisto();
      TimeStart();
      Init();
      TimeEnd();
   }
   else if (!strncmp(option,"b",1)) {
      fCurrentEventMethod = "BeginOfRun";
      ResetHisto();
      TimeStart();
      BeginOfRun();
      TimeEnd();
   }
   else if (!strncmp(option,"e",1)) {
      fCurrentEventMethod = "EndOfRun";
      TimeStart();
      EndOfRun();
      TimeEnd();
   }
   else if (!strncmp(option,"t",1)) {
      fCurrentEventMethod = "Terminate";
      TimeStart();
      Terminate();
      TimeEnd();

      gROME->PrintText("Task '");
      gROME->PrintText(fName.Data());
      gROME->PrintText("' : run time = ");
      gROME->PrintLine(GetTime());
   }
   else if ( strncmp(gROME->GetNameOfActiveDAQ(),"midas",5) ||
             ( !strncmp(&fEventID,"a",1) || !strncmp(option,&fEventID,1) )
      ) {
      fCurrentEventMethod = "Event";
      TimeStart();
      if (gROME->isFillEvent())
         Event();
      TimeEnd();
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

void ROMETask::TimeReset()
{
   // Reset the Tasks stopwatch
   fWatch.Reset();
}

void ROMETask::TimeStart()
{
   // Starts the Tasks stopwatch
   fWatch.Start();
}

void ROMETask::TimeEnd()
{
   // Ends the Tasks stopwatch
   fWatch.Stop();
   fWatch.GetRealTime(fTimeString);
}

const char* ROMETask::GetTime()
{
   // Returns the elapsed time in a readable format
   fWatch.GetRealTime(fTimeString);
   return fTimeString.Data();
}
