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
#pragma warning( push )
#pragma warning( disable : 4800 )
#include <TSystem.h> 
#pragma warning( pop )
#include <ROMETask.h>
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

      gROME->Print("Task '");
      gROME->Print(fName.Data());
      gROME->Print("' : run time = ");
      gROME->Println(GetTime());
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
   text.SetFormatted("\nIn method %s of task %s of event number %d of run number %d",fCurrentEventMethod.Data(),fName.Data(),gROME->GetCurrentEventNumber(),gROME->GetCurrentRunNumber());
   gROME->Println(text.Data());
   if (message)
      gROME->Println(message);
   gROME->GetApplication()->Run(true);
   gSystem->Init();
   gROME->GetApplication()->ProcessLine("MEGAnalyzer* gAnalyzer = ((MEGAnalyzer*)((TFolder*)gROOT->FindObjectAny(\"ROME\"))->GetListOfFolders()->MakeIterator()->Next());");
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
   fWatch.Start(false);
}
void ROMETask::TimeEnd()
{
   // Ends the Tasks stopwatch
   fWatch.Stop();
}
const char* ROMETask::GetTime()
{
   // Returns the elapsed time in a readable format
   int runTime = (int)fWatch.RealTime();
   int milli = (int)((fWatch.RealTime()-runTime)*1000);
   fTimeString.SetFormatted("%d%d:%d%d:%d%d:%d", (int)(runTime / 36000), (int)((runTime % 36000) / 3600),
           (int)((runTime % 3600) / 600), (int)((runTime % 600) / 60), (int)((runTime % 60) / 10),
           (int)(runTime % 10),milli);
   return fTimeString.Data();
}


