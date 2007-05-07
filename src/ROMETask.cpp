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

//______________________________________________________________________________
ROMETask::ROMETask()
:TTask()
,fLevel(1)
,fWatchAll()
,fWatchUser()
,fWatchUserEvent()
,fTimeAllString("")
,fTimeUserString("")
,fTimeUserEventString("")
,fVersion(0)
,fHasHistograms(kFALSE)
,fHasGraphs(kFALSE)
,fEventID(-1)
,fCurrentEventMethod("")
,fHistoFolder(0)
,fSkippedEvents(0)
{
   fWatchAll.Reset();
   fWatchUser.Reset();
   fWatchUserEvent.Reset();
}

//______________________________________________________________________________
ROMETask::ROMETask(const char *name,const char *title,int level,int version,int eventID,bool hasHisto,bool hasGraph,TFolder* histoFolder)
:TTask(name,title)
,fLevel(level)
,fWatchAll()
,fWatchUser()
,fWatchUserEvent()
,fTimeAllString("")
,fTimeUserString("")
,fTimeUserEventString("")
,fVersion(version)
,fHasHistograms(hasHisto)
,fHasGraphs(hasGraph)
,fEventID(eventID)
,fCurrentEventMethod("")
,fHistoFolder(histoFolder)
,fSkippedEvents(0)
{
   fWatchAll.Reset();
   fWatchUser.Reset();
   fWatchUserEvent.Reset();
}

//______________________________________________________________________________
void ROMETask::Exec(Option_t *option)
{
   // Overrides the TTask::Exec Method to replace the Exec Method with the following Methods :
   // Init
   // BeginOfRun
   // Event
   // EndOfRun
   // Terminate
   char *cstop;
   if (!strncmp(option,"Terminate",9)) {
      fCurrentEventMethod = "Terminate";
      ROMEPrint::Debug("Executing %s::Terminate\n", ClassName());
      fWatchUser.Start(false);
      Terminate();
      fWatchUser.Stop();
   }
   if (gROME->isTerminationFlag() || gROME->IsSkipEvent()) {
      return;
   }
   fWatchAll.Start(false);
   if (!strncmp(option,"Init",4)) {
      fCurrentEventMethod = "Init";
      BookHisto();
      BookGraph();
      ROMEPrint::Debug("Executing %s::Init\n", ClassName());
      fWatchUser.Start(false);
      Init();
      fWatchUser.Stop();
   } else if (!strncmp(option,"BeginOfRun",10)) {
      fCurrentEventMethod = "BeginOfRun";
      fSkippedEvents = 0;
      ReBookHisto();
      ReBookGraph();
      ResetHisto();
      ResetGraph();
      ROMEPrint::Debug("Executing %s::BeginOfRun\n", ClassName());
      fWatchUser.Start(false);
      BeginOfRun();
      fWatchUser.Stop();
   } else if (!strncmp(option,"EndOfRun",8)) {
      fCurrentEventMethod = "EndOfRun";
      ROMEPrint::Debug("Executing %s::EndOfRun\n", ClassName());
      fWatchUser.Start(false);
      EndOfRun();
      fWatchUser.Stop();
   } else if (!strncmp(option,"Time",4)) {
      int i;
      ROMEString name;
      int nchars;
      nchars = 0;
      for (i=0;i<fLevel;i++) {
         ROMEPrint::Print(" ");
      }
      if (fLevel==1) {
         ROMEPrint::Print("Task ");
         nchars = 5;
      } else if (fLevel>1) {
         ROMEPrint::Print("SubTask ");
         nchars = 8;
      }
      name = fName;
      if (name.Last('_')>0) {
         name = name(0,name.Last('_'));
      }
      ROMEPrint::Print(name.Data());
      for (i=0;i<30-name.Length()-fLevel-nchars;i++) {
         ROMEPrint::Print(".");
      }
      ROMEPrint::Print(" : %s", GetTimeOfAll());
      if (fWatchUser.RealTime()>0) {
         ROMEPrint::Print("  ");
         ROMEPrint::Print("%s", GetTimeOfUser());
         if (fWatchUserEvent.RealTime()>0) {
            ROMEPrint::Print("  ");
            ROMEPrint::Print("%s\n", GetTimeOfUserEvents());
         } else {
            ROMEPrint::Print("\n");
         }
      } else {
         ROMEPrint::Print("\n");
      }
   } else if (!strncmp(option,"PrintSkipped",12)) {
      ROMEString name;
      name = fName;
      if (name.Last('_') > 0) {
         name = name(0, name.Last('_'));
      }
      if (fSkippedEvents) {
#if defined ( R__UNIX )
         ROMEPrint::Print("%8lld events were skipped filling trees by %s.\n", fSkippedEvents, name.Data());
#else
         ROMEPrint::Print("%8I64d events were skipped filling trees by %s.\n", fSkippedEvents, name.Data());
#endif
         fSkippedEvents = 0;
      }
   } else if (!strncmp(option,"Event",5) && (strtol(option+5,&cstop,10)==fEventID ||
                                             fEventID==-1 || strtol(option+5,&cstop,10)==-1)) {
      fCurrentEventMethod = "Event";
      ROMEPrint::Debug("Executing %s::Event\n", ClassName());
      fWatchUser.Start(false);
      Bool_t fillEventOld = gROME->isFillEvent();
      fWatchUserEvent.Start(false);
      Event();
      fWatchUserEvent.Stop();
      if (fillEventOld && !gROME->isFillEvent()) {
         fSkippedEvents++;
      }
      fWatchUser.Stop();
   }
   fWatchAll.Stop();
}

//______________________________________________________________________________
void ROMETask::StartRootInterpreter(const char* message) {
#if defined( R__VISUAL_CPLUSPLUS )
   ROMEPrint::Print("\nIn method %s of task %s of event number %I64d of run number %I64d\n",fCurrentEventMethod.Data(),fName.Data(),gROME->GetCurrentEventNumber(),gROME->GetCurrentRunNumber());
#else
   ROMEPrint::Print("\nIn method %s of task %s of event number %lld of run number %lld\n",fCurrentEventMethod.Data(),fName.Data(),gROME->GetCurrentEventNumber(),gROME->GetCurrentRunNumber());
#endif

   if (message) {
      ROMEPrint::Print("%s\n", message);
   }
   ROMEString prompt = gROME->GetProgramName();
   prompt.ToLower();
   prompt += " [%d]";
   ((TRint*)gROME->GetApplication())->SetPrompt(prompt.Data());
   ((TRint*)gROME->GetApplication())->Run(true);
   gSystem->Init();
   gROME->GetApplication()->ProcessLine(gROME->GetCintInitialisation());
}

// Time methods
//______________________________________________________________________________
const char* ROMETask::GetTimeOfAll()
{
   // Returns the elapsed time in a readable format
   fWatchAll.GetRealTimeString(fTimeAllString);
   return fTimeAllString.Data();
}

//______________________________________________________________________________
const char* ROMETask::GetTimeOfUser()
{
   // Returns the elapsed time in a readable format
   fWatchUser.GetRealTimeString(fTimeUserString);
   return fTimeUserString.Data();
}

//______________________________________________________________________________
const char* ROMETask::GetTimeOfUserEvents()
{
   // Returns the elapsed time in a readable format
   fWatchUserEvent.GetRealTimeString(fTimeUserEventString);
   return fTimeUserEventString.Data();
}
