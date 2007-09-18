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
,fWatchUserEvent()
,fCpuTimeAllString("")
,fRealTimeAllString("")
,fTimeUserEventString("")
,fVersion(0)
,fHasHistograms(kFALSE)
,fHasGraphs(kFALSE)
,fEventID(-1)
,fCurrentEventMethod("")
,fHistoFolder(0)
,fSkippedEvents(0)
,fMemoryAccumulated(0)
{
   fWatchAll.Reset();
   fWatchUserEvent.Reset();
}

//______________________________________________________________________________
ROMETask::ROMETask(const char *name, const char *title, int level, int version,
                   int eventID, bool hasHisto, bool hasGraph, TFolder* histoFolder)
:TTask(name, title)
,fLevel(level)
,fWatchAll()
,fWatchUserEvent()
,fCpuTimeAllString("")
,fRealTimeAllString("")
,fTimeUserEventString("")
,fVersion(version)
,fHasHistograms(hasHisto)
,fHasGraphs(hasGraph)
,fEventID(eventID)
,fCurrentEventMethod("")
,fHistoFolder(histoFolder)
,fSkippedEvents(0)
,fMemoryAccumulated(0)
{
   fWatchAll.Reset();
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
#if (ROOT_VERSION_CODE >= ROOT_VERSION(5,14,0))
   MemInfo_t mem;
   Int_t usedMemoryOld = 0;
   Int_t usedMemoryDifference;
#endif
   if (!strncmp(option, "Terminate", 9)) {
      fCurrentEventMethod = "Terminate";
      ROMEPrint::Debug("Executing %s::Terminate\n", ClassName());
      Terminate();
   }
   if (gROME->isTerminationFlag() || gROME->IsSkipEvent()) {
      return;
   }
   fWatchAll.Start(false);
   if (!strncmp(option, "Init", 4)) {
      fCurrentEventMethod = "Init";
      BookHisto();
      BookGraph();
      ROMEPrint::Debug("Executing %s::Init\n", ClassName());
      Init();
   } else if (!strncmp(option, "BeginOfRun", 10)) {
      fCurrentEventMethod = "BeginOfRun";
      fSkippedEvents = 0;
#if (ROOT_VERSION_CODE >= ROOT_VERSION(5,14,0))
      fMemoryAccumulated = 0;
      if (ROMEPrint::GetVerboseLevel() >= ROMEPrint::kDebug) {
         gSystem->GetMemInfo(&mem);
         usedMemoryOld = mem.fMemUsed + mem.fSwapUsed;
      }
#endif
      ReBookHisto();
      ReBookGraph();
      ResetHisto();
      ResetGraph();
      ROMEPrint::Debug("Executing %s::BeginOfRun\n", ClassName());
      BeginOfRun();
#if (ROOT_VERSION_CODE >= ROOT_VERSION(5,14,0))
      if (ROMEPrint::GetVerboseLevel() >= ROMEPrint::kDebug) {
         gSystem->GetMemInfo(&mem);
         usedMemoryDifference = mem.fMemUsed + mem.fSwapUsed - usedMemoryOld;
         fMemoryAccumulated += usedMemoryDifference;
         ROMEPrint::Debug("Memory allocated during %-28s               : %4d MB\n", ClassName(), usedMemoryDifference);
         ROMEPrint::Debug("Memory allocated during %-28s (Accumulated) : %4d MB\n", ClassName(), fMemoryAccumulated);
      }
#endif
  } else if (!strncmp(option, "EndOfRun", 8)) {
      fCurrentEventMethod = "EndOfRun";
      ROMEPrint::Debug("Executing %s::EndOfRun\n", ClassName());
#if (ROOT_VERSION_CODE >= ROOT_VERSION(5,14,0))
      if (ROMEPrint::GetVerboseLevel() >= ROMEPrint::kDebug) {
         gSystem->GetMemInfo(&mem);
         usedMemoryOld = mem.fMemUsed + mem.fSwapUsed;
      }
#endif
      EndOfRun();
#if (ROOT_VERSION_CODE >= ROOT_VERSION(5,14,0))
      if (ROMEPrint::GetVerboseLevel() >= ROMEPrint::kDebug) {
         gSystem->GetMemInfo(&mem);
         usedMemoryDifference = mem.fMemUsed + mem.fSwapUsed - usedMemoryOld;
         fMemoryAccumulated += usedMemoryDifference;
         ROMEPrint::Debug("Memory allocated during %-28s               : %4d MB\n", ClassName(), usedMemoryDifference);
         ROMEPrint::Debug("Memory allocated during %-28s (Accumulated) : %4d MB\n", ClassName(), fMemoryAccumulated);
      }
#endif
   } else if (!strncmp(option, "Time", 4)) {
      int i;
      ROMEString name;
      int nchars;
      nchars = 0;
      for (i = 0; i < fLevel; i++) {
         ROMEPrint::Print(" ");
      }
      if (fLevel == 1) {
         ROMEPrint::Print("Task ");
         nchars = 5;
      } else if (fLevel > 1) {
         ROMEPrint::Print("SubTask ");
         nchars = 8;
      }
      name = fName;
      if (name.Last('_') > 0) {
         name = name(0, name.Last('_'));
      }
      ROMEPrint::Print(name.Data());
      for (i = 0; i < 35 - name.Length() - fLevel - nchars; i++) {
         ROMEPrint::Print(".");
      }
      ROMEPrint::Print(" : %s  %s",
                       GetRealTimeOfAll(),
                       GetCpuTimeOfAll());
//      if (fWatchUserEvent.CpuTime() > 0) {
      ROMEPrint::Print("  %s\n", GetTimeOfUserEvents());
//      } else {
//         ROMEPrint::Print("\n");
//      }
   } else if (!strncmp(option, "PrintSkipped", 12)) {
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
   } else if (!strncmp(option, "Event", 5) && (strtol(option + 5, &cstop, 10) == fEventID ||
                                             fEventID == -1 || strtol(option + 5, &cstop, 10) == -1)) {
      fCurrentEventMethod = "Event";
      ROMEPrint::Debug("Executing %s::Event\n", ClassName());
      Bool_t fillEventOld = gROME->isFillEvent();
#if (ROOT_VERSION_CODE >= ROOT_VERSION(5,14,0))
      if (ROMEPrint::GetVerboseLevel() >= ROMEPrint::kDebug) {
         gSystem->GetMemInfo(&mem);
         usedMemoryOld = mem.fMemUsed + mem.fSwapUsed;
      }
#endif
      fWatchUserEvent.Start(false);
      Event();
      fWatchUserEvent.Stop();
#if (ROOT_VERSION_CODE >= ROOT_VERSION(5,14,0))
      if (ROMEPrint::GetVerboseLevel() >= ROMEPrint::kDebug) {
         gSystem->GetMemInfo(&mem);
         usedMemoryDifference = mem.fMemUsed + mem.fSwapUsed - usedMemoryOld;
         fMemoryAccumulated += usedMemoryDifference;
         ROMEPrint::Debug("Memory allocated during %-28s               : %4d MB\n", ClassName(), usedMemoryDifference);
         ROMEPrint::Debug("Memory allocated during %-28s (Accumulated) : %4d MB\n", ClassName(), fMemoryAccumulated);
      }
#endif
      if (fillEventOld && !gROME->isFillEvent()) {
         fSkippedEvents++;
      }
   }
   fWatchAll.Stop();
}

//______________________________________________________________________________
void ROMETask::StartRootInterpreter(const char* message)
{
   ROMEPrint::Print("\nIn method %s of task %s of event number "R_LLD" of run number "R_LLD"\n",
                    fCurrentEventMethod.Data(), fName.Data(), gROME->GetCurrentEventNumber(),
                    gROME->GetCurrentRunNumber());

   if (message) {
      ROMEPrint::Print("%s\n", message);
   }
   ROMEString prompt = gROME->GetProgramName();
   prompt.ToLower();
   prompt += " [%d]";
   static_cast<TRint*>(gROME->GetApplication())->SetPrompt(prompt.Data());
   static_cast<TRint*>(gROME->GetApplication())->Run(true);
   gSystem->Init();
   gROME->GetApplication()->ProcessLine(gROME->GetCintInitialisation());
}

// Time methods
//______________________________________________________________________________
const char* ROMETask::GetRealTimeOfAll()
{
   // Returns the elapsed real time in a readable format
   fWatchAll.GetRealTimeString(fRealTimeAllString);
   return fRealTimeAllString.Data();
}

//______________________________________________________________________________
const char* ROMETask::GetCpuTimeOfAll()
{
   // Returns the elapsed CPU time in a readable format
   fWatchAll.GetCpuTimeString(fCpuTimeAllString);
   return fCpuTimeAllString.Data();
}

//______________________________________________________________________________
const char* ROMETask::GetTimeOfUserEvents()
{
   // Returns the elapsed CPU time in a readable format
   fWatchUserEvent.GetCpuTimeString(fTimeUserEventString);
   return fTimeUserEventString.Data();
}
