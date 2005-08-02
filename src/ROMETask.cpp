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
//  $Log$
//  Revision 1.25  2005/08/02 14:44:52  schneebeli_m
//  correct taskHierarchy handling
//
//  Revision 1.24  2005/07/12 06:42:23  sawada
//  Bug fix. Matched the name of method (IsActiveID and IsActiveEventID)
//
//  Revision 1.23  2005/04/04 07:23:59  schneebeli_m
//  Time report
//
//  Revision 1.22  2005/04/01 14:56:23  schneebeli_m
//  Histo moved, multiple databases, db-paths moved, InputDataFormat->DAQSystem, GetMidas() to access banks, User DAQ
//
//  Revision 1.21  2005/01/27 16:21:06  schneebeli_m
//  print method & no gROME in path
//
//  Revision 1.20  2005/01/18 17:41:29  schneebeli_m
//  Termination flag
//
//  Revision 1.19  2005/01/18 09:36:11  schneebeli_m
//  FillEvent flag suppresses later tasks
//
//  Revision 1.18  2004/11/23 09:22:21  schneebeli_m
//  User called Root Interpreter
//
//  Revision 1.17  2004/11/17 16:12:46  schneebeli_m
//  remove executetask
//
//  Revision 1.16  2004/11/16 16:14:01  schneebeli_m
//  implemented task hierarchy
//
//  Revision 1.15  2004/10/05 07:52:44  schneebeli_m
//  dyn. Folders, TRef Objects, XML format changed, ROMEStatic removed
//
//  Revision 1.14  2004/09/25 01:34:48  schneebeli_m
//  implemented FW dependent EventLoop and DataBase classes
//
//                                                                      //
//////////////////////////////////////////////////////////////////////////
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
   else if (!strncmp(&fEventID,"a",1) || !strncmp(option,&fEventID,1)) {
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


