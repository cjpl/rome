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
#endif                          // R__VISUAL_CPLUSPLUS
#include <TSystem.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif                          // R__VISUAL_CPLUSPLUS
#include "ROMETask.h"
#include "ROMEAnalyzer.h"
#include <ROMERint.h>
#include "ROMEiostream.h"

#include "TH1.h"
#include "TH1K.h"
#include "TH2.h"
#include "TH3.h"

#include "TCutG.h"
#include "ROMETCutG.h"
#include "ROMETGraphErrors.h"

ClassImp(ROMETask)

//______________________________________________________________________________
ROMETask::ROMETask()
:TTask()
,fLevel(1)
,fTaskIndex(0)
,fTaskSuffix("")
,fWatchAll()
,fWatchUserEvent()
,fCpuTimeAllString("")
,fRealTimeAllString("")
,fTimeUserEventString("")
,fVersion(0)
,fEventID(-1)
,fCurrentEventMethod("")
,fRootFolder(0)
,fHistoFolder(0)
,fSkippedEvents(0)
,fMemoryAccumulated(0)
,fNumberOfHistos(0)
,fHisto(0)
,fHistoParameter(0)
,fHistoName(0)
,fHistoType(0)
,fHistoDimension(0)
,fHistoArray(0)
,fHistoUpToDate(0)
,fNumberOfGraphs(0)
,fGraph(0)
,fGraphStorage(0)
,fGraphParameter(0)
,fGraphName(0)
,fGraphType(0)
,fGraphDimension(0)
,fGraphArray(0)
,fInitTerminateState(0)
,fBorEorState(0)
{
   fWatchAll.Reset();
   fWatchUserEvent.Reset();
}

//______________________________________________________________________________
ROMETask::ROMETask(const char *name, const char *title, Int_t level, Int_t version, Int_t eventID,
                   TFolder *histoFolder, const char *taskSuffix, Int_t numberOfHistos, Int_t numberOfGraphs)
:TTask(name, title)
,fLevel(level)
,fTaskSuffix(taskSuffix)
,fWatchAll()
,fWatchUserEvent()
,fCpuTimeAllString("")
,fRealTimeAllString("")
,fTimeUserEventString("")
,fVersion(version)
,fEventID(eventID)
,fCurrentEventMethod("")
,fRootFolder(new TObjArray(numberOfHistos + numberOfGraphs))
,fHistoFolder(histoFolder)
,fSkippedEvents(0)
,fMemoryAccumulated(0)
,fNumberOfHistos(numberOfHistos)
,fHisto(new TObjArray(numberOfHistos))
,fHistoParameter(new TObjArray(numberOfHistos))
,fNumberOfGraphs(numberOfGraphs)
,fGraph(new TObjArray(numberOfGraphs))
,fGraphStorage(new TObjArray(numberOfGraphs))
,fGraphParameter(new TObjArray(numberOfGraphs))
,fInitTerminateState(0)
,fBorEorState(0)
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
      if (fInitTerminateState != 1) {
#if 0
         ROMEPrint::Warning("Terminate is called twice, or called before Init");
#endif
      } else {
         fInitTerminateState--;
         fCurrentEventMethod = "Terminate";
         ROMEPrint::Debug("Executing %s::Terminate\n", ClassName());
         Terminate();
      }
   }
   if (gROME->isTerminationFlag() || gROME->IsSkipEvent()) {
      return;
   }
   fWatchAll.Start(false);
   if (!strncmp(option, "Init", 4)) {
      if (fInitTerminateState != 0) {
#if 0
         ROMEPrint::Warning("Init is called twice, or called before Terminate");
#endif
      } else {
         fInitTerminateState++;
         fCurrentEventMethod = "Init";
         BookHisto();
         BookGraph();
         ROMEPrint::Debug("Executing %s::Init\n", ClassName());
         Init();
      }
   } else if (!strncmp(option, "BeginOfRun", 10)) {
      if (fBorEorState != 0 || fInitTerminateState != 1) {
         if (fBorEorState != 0) {
#if 0
            ROMEPrint::Warning("BeginOfRun is called twice, or called before EndOfRun");
         } else {
            ROMEPrint::Warning("BeginOfRun is called before Init or after Terminate.");
#endif
         }
      } {
         fBorEorState++;
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
            ROMEPrint::Debug("Memory allocated during %-28s               : %4d MB\n", ClassName(),
                             usedMemoryDifference);
            ROMEPrint::Debug("Memory allocated during %-28s (Accumulated) : %4d MB\n", ClassName(),
                             fMemoryAccumulated);
         }
#endif
      }
   } else if (!strncmp(option, "EndOfRun", 8)) {
      if (fBorEorState != 1 || fInitTerminateState != 1) {
         if (fBorEorState != 1) {
#if 0
            ROMEPrint::Warning("EndOfRun is called twice, or called before BeginOfRun");
         } else {
            ROMEPrint::Warning("EndOfRun is called before Init or after Terminate.");
#endif
         }
      } else {
         fBorEorState--;
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
      }
   } else if (!strncmp(option, "Time", 4)) {
      Int_t i;
      ROMEString name;
      Int_t nchars;
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
      ROMEPrint::Print(" : %s  %s", GetRealTimeOfAll(), GetCpuTimeOfAll());
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
      if (fBorEorState != 1 || fInitTerminateState != 1) {
#if 0
         if (fBorEorState != 1) {
            ROMEPrint::Warning("Event is called before BeginOfRun or after EndOfRun.");
         } else {
            ROMEPrint::Warning("Event is called before Init or after Terminate.");
         }
#endif
      } else {
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
            ROMEPrint::Debug("Memory allocated during %-28s               : %4d MB\n", ClassName(),
                             usedMemoryDifference);
            ROMEPrint::Debug("Memory allocated during %-28s (Accumulated) : %4d MB\n", ClassName(),
                             fMemoryAccumulated);
         }
#endif
         if (fillEventOld && !gROME->isFillEvent()) {
            fSkippedEvents++;
         }
      }
   }
   fWatchAll.Stop();
}

//______________________________________________________________________________
void ROMETask::StartRootInterpreter(const char *message)
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
const char *ROMETask::GetRealTimeOfAll()
{
   // Returns the elapsed real time in a readable format
   fWatchAll.GetRealTimeString(fRealTimeAllString);
   return fRealTimeAllString.Data();
}

//______________________________________________________________________________
const char *ROMETask::GetCpuTimeOfAll()
{
   // Returns the elapsed CPU time in a readable format
   fWatchAll.GetCpuTimeString(fCpuTimeAllString);
   return fCpuTimeAllString.Data();
}

//______________________________________________________________________________
const char *ROMETask::GetTimeOfUserEvents()
{
   // Returns the elapsed CPU time in a readable format
   fWatchUserEvent.GetCpuTimeString(fTimeUserEventString);
   return fTimeUserEventString.Data();
}

// Histo methods
//______________________________________________________________________________
void ROMETask::ResetHisto()
{
   Int_t i, j;
   for (i = 0; i < GetNumberOfHistos(); i++) {
      if (static_cast<ROMEHisto*>(fHistoParameter->At(i))->IsActive() &&
          !static_cast<ROMEHisto*>(fHistoParameter->At(i))->IsAccumulate()) {
         if (!fHistoArray[i]) {
            static_cast<TH1*>(fHisto->At(i))->Reset();
         } else {
            for (j = 0; j < static_cast<TObjArray*>(fHisto->At(i))->GetEntries(); j++) {
               static_cast<TH1*>(static_cast<TObjArray*>(fHisto->At(i))->At(j))->Reset();
            }
         }
      }
   }
}

//______________________________________________________________________________
void ROMETask::BookHisto(void)
{
   if (fNumberOfHistos <= 0) {
      return;
   }

   SetOriginalHistoParameters();
   Int_t      i, j;
   ROMEHisto *histoHandle = 0;
   ROMEString histoArrayName;
   ROMEString histoArrayTitle;
   ROMEString histoTitle;
   ROMEString folderTitle;
   ROMEString xLabel;
   ROMEString yLabel;
   ROMEString zLabel;
   Int_t      arraySize;
   ROMEString arraySizeStr;
   Int_t      arrayStartIndex;
   ROMEString arrayStartIndexStr;
   Int_t      xNbins;
   ROMEString xNbinsStr;
   Double_t   xmin;
   ROMEString xminStr;
   Double_t   xmax;
   ROMEString xmaxStr;
   Int_t      yNbins;
   ROMEString yNbinsStr;
   Double_t   ymin;
   ROMEString yminStr;
   Double_t   ymax;
   ROMEString ymaxStr;
   Int_t      zNbins;
   ROMEString zNbinsStr;
   Double_t   zmin;
   ROMEString zminStr;
   Double_t   zmax;
   ROMEString zmaxStr;
   TH1       *histoPtr = 0;

   for (i = 0; i < fNumberOfHistos; i++) {
      histoHandle = static_cast<ROMEHisto*>(fHistoParameter->At(i));

      if (histoHandle->IsActive()) {
         histoTitle         = histoHandle->GetTitle();
         folderTitle        = histoHandle->GetFolderTitle();
         xLabel             = histoHandle->GetXLabel();
         yLabel             = histoHandle->GetYLabel();
         zLabel             = histoHandle->GetZLabel();
         arraySize          = histoHandle->GetArraySize();
         arraySizeStr       = histoHandle->GetArraySizeString(arraySizeStr);
         arrayStartIndex    = histoHandle->GetArrayStartIndex();
         arrayStartIndexStr = histoHandle->GetArrayStartIndexString(arrayStartIndexStr);
         xNbins             = histoHandle->GetXNbins();
         xNbinsStr          = histoHandle->GetXNbinsString(xNbinsStr);
         xmin               = histoHandle->GetXmin();
         xminStr            = histoHandle->GetXminString(xminStr);
         xmax               = histoHandle->GetXmax();
         xmaxStr            = histoHandle->GetXmaxString(xmaxStr);
         yNbins             = histoHandle->GetYNbins();
         yNbinsStr          = histoHandle->GetYNbinsString(yNbinsStr);
         ymin               = histoHandle->GetYmin();
         yminStr            = histoHandle->GetYminString(yminStr);
         ymax               = histoHandle->GetYmax();
         ymaxStr            = histoHandle->GetYmaxString(ymaxStr);
         zNbins             = histoHandle->GetZNbins();
         zNbinsStr          = histoHandle->GetZNbinsString(zNbinsStr);
         zmin               = histoHandle->GetZmin();
         zminStr            = histoHandle->GetZminString(zminStr);
         zmax               = histoHandle->GetZmax();
         zmaxStr            = histoHandle->GetZmaxString(zmaxStr);

         histoTitle      = GetObjectInterpreterCharValue  (GetObjectInterpreterCode(histoTitle.Data()),  histoTitle,  histoTitle);
         folderTitle     = GetObjectInterpreterCharValue  (GetObjectInterpreterCode(folderTitle),        folderTitle, folderTitle);
         xLabel          = GetObjectInterpreterCharValue  (GetObjectInterpreterCode(xLabel.Data()),      xLabel,      xLabel);
         yLabel          = GetObjectInterpreterCharValue  (GetObjectInterpreterCode(yLabel.Data()),      yLabel,      yLabel);
         zLabel          = GetObjectInterpreterCharValue  (GetObjectInterpreterCode(zLabel.Data()),      zLabel,      zLabel);
         arraySize       = GetObjectInterpreterIntValue   (GetObjectInterpreterCode(arraySizeStr),       arraySize);
         arrayStartIndex = GetObjectInterpreterIntValue   (GetObjectInterpreterCode(arrayStartIndexStr), arrayStartIndex);
         xNbins          = GetObjectInterpreterIntValue   (GetObjectInterpreterCode(xNbinsStr),          xNbins);
         xmin            = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(xminStr),            xmin);
         xmax            = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(xmaxStr),            xmax);
         yNbins          = GetObjectInterpreterIntValue   (GetObjectInterpreterCode(yNbinsStr),          yNbins);
         ymin            = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(yminStr),            ymin);
         ymax            = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(ymaxStr),            ymax);
         zNbins          = GetObjectInterpreterIntValue   (GetObjectInterpreterCode(zNbinsStr),          zNbins);
         zmin            = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(zminStr),            zmin);
         zmax            = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(zmaxStr),            zmax);

         // create histos
         if (!fHistoArray[i]) {
            if (false) {
               //} else if (fHistoType[i] == "TH1") {
               //fHisto->AddAt(new TH1(fHistoName[i]+fTaskSuffix, histoTitle.Data(), xNbins, xmin, xmax));
            } else if (fHistoType[i] == "TH1S") {
               fHisto->AddAt(new TH1S(fHistoName[i] + fTaskSuffix, histoTitle.Data(), xNbins, xmin, xmax), i);
            } else if (fHistoType[i] == "TH1K") {
               fHisto->AddAt(new TH1K(fHistoName[i] + fTaskSuffix, histoTitle.Data(), xNbins, xmin, xmax), i);
            } else if (fHistoType[i] == "TH1I") {
               fHisto->AddAt(new TH1I(fHistoName[i] + fTaskSuffix, histoTitle.Data(), xNbins, xmin, xmax), i);
            } else if (fHistoType[i] == "TH1F") {
               fHisto->AddAt(new TH1F(fHistoName[i] + fTaskSuffix, histoTitle.Data(), xNbins, xmin, xmax), i);
            } else if (fHistoType[i] == "TH1D") {
               fHisto->AddAt(new TH1D(fHistoName[i] + fTaskSuffix, histoTitle.Data(), xNbins, xmin, xmax), i);
            } else if (fHistoType[i] == "TH1C") {
               fHisto->AddAt(new TH1C(fHistoName[i] + fTaskSuffix, histoTitle.Data(), xNbins, xmin, xmax), i);
            } else if (fHistoType[i] == "TH2") {
               fHisto->AddAt(new TH2(fHistoName[i] + fTaskSuffix, histoTitle.Data(), xNbins, xmin, xmax, yNbins, ymin, ymax), i);
            } else if (fHistoType[i] == "TH2S") {
               fHisto->AddAt(new TH2S(fHistoName[i] + fTaskSuffix, histoTitle.Data(), xNbins, xmin, xmax, yNbins, ymin, ymax), i);
            } else if (fHistoType[i] == "TH2I") {
               fHisto->AddAt(new TH2I(fHistoName[i] + fTaskSuffix, histoTitle.Data(), xNbins, xmin, xmax, yNbins, ymin, ymax), i);
            } else if (fHistoType[i] == "TH2F") {
               fHisto->AddAt(new TH2F(fHistoName[i] + fTaskSuffix, histoTitle.Data(), xNbins, xmin, xmax, yNbins, ymin, ymax), i);
            } else if (fHistoType[i] == "TH2D") {
               fHisto->AddAt(new TH2D(fHistoName[i] + fTaskSuffix, histoTitle.Data(), xNbins, xmin, xmax, yNbins, ymin, ymax), i);
            } else if (fHistoType[i] == "TH2C") {
               fHisto->AddAt(new TH2C(fHistoName[i] + fTaskSuffix, histoTitle.Data(), xNbins, xmin, xmax, yNbins, ymin, ymax), i);
            } else if (fHistoType[i] == "TH3") {
               fHisto->AddAt(new TH3(fHistoName[i] + fTaskSuffix, histoTitle.Data(), xNbins, xmin, xmax, yNbins, ymin, ymax,
                                     zNbins, zmin, zmax), i);
            } else if (fHistoType[i] == "TH3S") {
               fHisto->AddAt(new TH3S(fHistoName[i] + fTaskSuffix, histoTitle.Data(), xNbins, xmin, xmax, yNbins, ymin, ymax,
                                      zNbins, zmin, zmax), i);
            } else if (fHistoType[i] == "TH3S") {
               fHisto->AddAt(new TH3S(fHistoName[i] + fTaskSuffix, histoTitle.Data(), xNbins, xmin, xmax, yNbins, ymin, ymax,
                                      zNbins, zmin, zmax), i);
            } else if (fHistoType[i] == "TH3I") {
               fHisto->AddAt(new TH3I(fHistoName[i] + fTaskSuffix, histoTitle.Data(), xNbins, xmin, xmax, yNbins, ymin, ymax,
                                      zNbins, zmin, zmax), i);
            } else if (fHistoType[i] == "TH3F") {
               fHisto->AddAt(new TH3F(fHistoName[i] + fTaskSuffix, histoTitle.Data(), xNbins, xmin, xmax, yNbins, ymin, ymax,
                                      zNbins, zmin, zmax), i);
            } else if (fHistoType[i] == "TH3D") {
               fHisto->AddAt(new TH3D(fHistoName[i] + fTaskSuffix, histoTitle.Data(), xNbins, xmin, xmax, yNbins, ymin, ymax,
                                      zNbins, zmin, zmax), i);
            } else if (fHistoType[i] == "TH3C") {
               fHisto->AddAt(new TH3C(fHistoName[i] + fTaskSuffix, histoTitle.Data(), xNbins, xmin, xmax, yNbins, ymin, ymax,
                                      zNbins, zmin, zmax), i);
            } else if (fHistoType[i] == "TProfile") {
               fHisto->AddAt(new TProfile(fHistoName[i] + fTaskSuffix, histoTitle.Data(), xNbins, xmin, xmax, ymin, ymax), i);
            } else if (fHistoType[i] == "TProfile2D") {
               fHisto->AddAt(new TProfile2D(fHistoName[i] + fTaskSuffix, histoTitle.Data(), xNbins, xmin, xmax, yNbins, ymin, ymax,
                                            zmin, zmax), i);
            } else {
               ROMEPrint::Error("\nYou have tried to use unimplemented histogram %s.\n\nShutting down the program.\n",
                                fHistoType[i].Data());
               gSystem->StackTrace();
               gROME->GetApplication()->Terminate(1);
            }
            histoPtr = static_cast<TH1*>(fHisto->At(i));
            static_cast<TFolder*>(fRootFolder->At(i))->Add(histoPtr);
            histoPtr->GetXaxis()->SetTitle(xLabel.Data());
            histoPtr->GetYaxis()->SetTitle(yLabel.Data());
            histoPtr->GetZaxis()->SetTitle(zLabel.Data());
         } else {
            fHisto->AddAt(new TObjArray(arraySize), i);

            for (j = 0; j < arraySize; j++) {
               histoArrayName = fHistoName[i];
               histoArrayName.AppendFormatted("_%0*d", 3, j + arrayStartIndex);
               histoArrayName += fTaskSuffix;
               histoArrayTitle = fHistoName[i];
               histoArrayTitle.AppendFormatted("_%0*d", 3, j + arrayStartIndex);

               if (false) {
#if 0
               } else if (fHistoType[i] == "TH1") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH1(histoArrayName.Data(), histoArrayTitle[j].Data(),
                                                                        xNbins, xmin, xmax), j);
#endif
               } else if (fHistoType[i] == "TH1S") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH1S(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax), j);
               } else if (fHistoType[i] == "TH1K") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH1K(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax), j);
               } else if (fHistoType[i] == "TH1I") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH1I(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax), j);
               } else if (fHistoType[i] == "TH1F") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH1F(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax), j);
               } else if (fHistoType[i] == "TH1D") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH1D(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax), j);
               } else if (fHistoType[i] == "TH1C") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH1C(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax), j);
               } else if (fHistoType[i] == "TH2") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH2(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                        xNbins, xmin, xmax, yNbins, ymin, ymax), j);
               } else if (fHistoType[i] == "TH2S") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH2S(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax, yNbins, ymin, ymax), j);
               } else if (fHistoType[i] == "TH2I") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH2I(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax, yNbins, ymin, ymax), j);
               } else if (fHistoType[i] == "TH2F") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH2F(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax, yNbins, ymin, ymax), j);
               } else if (fHistoType[i] == "TH2D") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH2D(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax, yNbins, ymin, ymax), j);
               } else if (fHistoType[i] == "TH2C") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH2C(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax, yNbins, ymin, ymax), j);
               } else if (fHistoType[i] == "TH3") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH3(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                        xNbins, xmin, xmax, yNbins, ymin, ymax,
                                                                        zNbins, zmin, zmax), j);
               } else if (fHistoType[i] == "TH3S") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH3S(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax, yNbins, ymin, ymax,
                                                                         zNbins, zmin, zmax), j);
               } else if (fHistoType[i] == "TH3I") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH3I(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax, yNbins, ymin, ymax,
                                                                         zNbins, zmin, zmax), j);
               } else if (fHistoType[i] == "TH3F") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH3F(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax, yNbins, ymin, ymax,
                                                                         zNbins, zmin, zmax), j);
               } else if (fHistoType[i] == "TH3D") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH3D(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax, yNbins, ymin, ymax,
                                                                         zNbins, zmin, zmax), j);
               } else if (fHistoType[i] == "TH3C") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH3C(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax, yNbins, ymin, ymax,
                                                                         zNbins, zmin, zmax), j);
               } else if (fHistoType[i] == "TProfile") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TProfile(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax), j);
               } else if (fHistoType[i] == "TProfile2D") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TProfile2D(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax, yNbins, ymin, ymax), j);
               } else {
                  ROMEPrint::Error("\nYou have tried to use unimplemented histogram %s.\n\nShutting down the program.\n",
                                   fHistoType[i].Data());
                  gSystem->StackTrace();
                  gROME->GetApplication()->Terminate(1);
               }
               histoPtr = static_cast<TH1*>(static_cast<TObjArray*>(fHisto->At(i))->At(j));
               static_cast<TFolder*>(fRootFolder->At(i))->Add(histoPtr);
               histoPtr->GetXaxis()->SetTitle(xLabel.Data());
               histoPtr->GetYaxis()->SetTitle(yLabel.Data());
               histoPtr->GetZaxis()->SetTitle(zLabel.Data());
            }
         }
      }
   }
}

//______________________________________________________________________________
void ROMETask::ReBookHisto(void)
{
   if (fNumberOfHistos < 0) {
      return;
   }

   SetOriginalHistoParameters();
   Int_t      i, j;
   ROMEHisto *histoHandle = 0;
   ROMEString histoArrayName;
   ROMEString histoArrayTitle;
   ROMEString histoTitle;
   ROMEString folderTitle;
   ROMEString xLabel;
   ROMEString yLabel;
   ROMEString zLabel;
   Int_t      arraySize;
   ROMEString arraySizeStr;
   Int_t      arrayStartIndex;
   ROMEString arrayStartIndexStr;
   Int_t      xNbins;
   ROMEString xNbinsStr;
   Double_t   xmin;
   ROMEString xminStr;
   Double_t   xmax;
   ROMEString xmaxStr;
   Int_t      yNbins;
   ROMEString yNbinsStr;
   Double_t   ymin;
   ROMEString yminStr;
   Double_t   ymax;
   ROMEString ymaxStr;
   Int_t      zNbins;
   ROMEString zNbinsStr;
   Double_t   zmin;
   ROMEString zminStr;
   Double_t   zmax;
   ROMEString zmaxStr;
   TH1       *histoPtr = 0;

   for (i = 0; i < fNumberOfHistos; i++) {
      histoHandle = static_cast<ROMEHisto*>(fHistoParameter->At(i));

      if (histoHandle->IsActive()) {
         histoTitle         = histoHandle->GetTitle();
         folderTitle        = histoHandle->GetFolderTitle();
         xLabel             = histoHandle->GetXLabel();
         yLabel             = histoHandle->GetYLabel();
         zLabel             = histoHandle->GetZLabel();
         arraySize          = histoHandle->GetArraySize();
         arraySizeStr       = histoHandle->GetArraySizeString(arraySizeStr);
         arrayStartIndex    = histoHandle->GetArrayStartIndex();
         arrayStartIndexStr = histoHandle->GetArrayStartIndexString(arrayStartIndexStr);
         xNbins             = histoHandle->GetXNbins();
         xNbinsStr          = histoHandle->GetXNbinsString(xNbinsStr);
         xmin               = histoHandle->GetXmin();
         xminStr            = histoHandle->GetXminString(xminStr);
         xmax               = histoHandle->GetXmax();
         xmaxStr            = histoHandle->GetXmaxString(xmaxStr);
         yNbins             = histoHandle->GetYNbins();
         yNbinsStr          = histoHandle->GetYNbinsString(yNbinsStr);
         ymin               = histoHandle->GetYmin();
         yminStr            = histoHandle->GetYminString(yminStr);
         ymax               = histoHandle->GetYmax();
         ymaxStr            = histoHandle->GetYmaxString(ymaxStr);
         zNbins             = histoHandle->GetZNbins();
         zNbinsStr          = histoHandle->GetZNbinsString(zNbinsStr);
         zmin               = histoHandle->GetZmin();
         zminStr            = histoHandle->GetZminString(zminStr);
         zmax               = histoHandle->GetZmax();
         zmaxStr            = histoHandle->GetZmaxString(zmaxStr);

         histoTitle      = GetObjectInterpreterCharValue  (GetObjectInterpreterCode(histoTitle),         histoTitle,  histoTitle);
         folderTitle     = GetObjectInterpreterCharValue  (GetObjectInterpreterCode(folderTitle),        folderTitle, folderTitle);
         xLabel          = GetObjectInterpreterCharValue  (GetObjectInterpreterCode(xLabel),             xLabel,      xLabel);
         yLabel          = GetObjectInterpreterCharValue  (GetObjectInterpreterCode(yLabel),             yLabel,      yLabel);
         zLabel          = GetObjectInterpreterCharValue  (GetObjectInterpreterCode(zLabel),             zLabel,      zLabel);
         arraySize       = GetObjectInterpreterIntValue   (GetObjectInterpreterCode(arraySizeStr),       arraySize);
         arrayStartIndex = GetObjectInterpreterIntValue   (GetObjectInterpreterCode(arrayStartIndexStr), arrayStartIndex);
         xNbins          = GetObjectInterpreterIntValue   (GetObjectInterpreterCode(xNbinsStr),          xNbins);
         xmin            = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(xminStr),            xmin);
         xmax            = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(xmaxStr),            xmax);
         yNbins          = GetObjectInterpreterIntValue   (GetObjectInterpreterCode(yNbinsStr),          yNbins);
         ymin            = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(yminStr),            ymin);
         ymax            = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(ymaxStr),            ymax);
         zNbins          = GetObjectInterpreterIntValue   (GetObjectInterpreterCode(zNbinsStr),          zNbins);
         zmin            = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(zminStr),            zmin);
         zmax            = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(zmaxStr),            zmax);

         // expand histos
         if (!fHistoArray[i]) {
            // do nothing for non-array histograms
         } else {
            Int_t arraySizeOld = static_cast<TObjArray*>(fHisto->At(i))->GetEntries();
            if (arraySize > arraySizeOld) {
               static_cast<TObjArray*>(fHisto->At(i))->Expand(arraySize);
            }

            for (j = arraySizeOld; j < arraySize; j++) {
               histoArrayName = fHistoName[i];
               histoArrayName.AppendFormatted("_%0*d", 3, j + arrayStartIndex);
               histoArrayName += fTaskSuffix;
               histoArrayTitle = fHistoName[i];
               histoArrayTitle.AppendFormatted("_%0*d", 3, j + arrayStartIndex);

               if (false) {
                  //} else if (fHistoType[i] == "TH1") {
                  //((TObjArray*)fHisto->At(i))->AddAt(new TH1(histoArrayName.Data(), histoArrayTitle.Data(), xNbins, xmin, xmax), j);
               } else if (fHistoType[i] == "TH1S") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH1S(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax), j);
               } else if (fHistoType[i] == "TH1K") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH1K(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax), j);
               } else if (fHistoType[i] == "TH1I") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH1I(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax), j);
               } else if (fHistoType[i] == "TH1F") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH1F(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax), j);
               } else if (fHistoType[i] == "TH1D") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH1D(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax), j);
               } else if (fHistoType[i] == "TH1C") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH1C(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax), j);
               } else if (fHistoType[i] == "TH2") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH2(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                        xNbins, xmin, xmax, yNbins, ymin, ymax), j);
               } else if (fHistoType[i] == "TH2S") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH2S(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax, yNbins, ymin, ymax), j);
               } else if (fHistoType[i] == "TH2I") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH2I(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax, yNbins, ymin, ymax), j);
               } else if (fHistoType[i] == "TH2F") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH2F(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax, yNbins, ymin, ymax), j);
               } else if (fHistoType[i] == "TH2D") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH2D(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax, yNbins, ymin, ymax), j);
               } else if (fHistoType[i] == "TH2C") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH2C(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax, yNbins, ymin, ymax), j);
               } else if (fHistoType[i] == "TH3") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH3(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                        xNbins, xmin, xmax, yNbins, ymin, ymax,
                                                                        zNbins, zmin, zmax), j);
               } else if (fHistoType[i] == "TH3S") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH3S(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax, yNbins, ymin, ymax,
                                                                         zNbins, zmin, zmax), j);
               } else if (fHistoType[i] == "TH3I") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH3I(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax, yNbins, ymin, ymax,
                                                                         zNbins, zmin, zmax), j);
               } else if (fHistoType[i] == "TH3F") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH3F(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax, yNbins, ymin, ymax,
                                                                         zNbins, zmin, zmax), j);
               } else if (fHistoType[i] == "TH3D") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH3D(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax, yNbins, ymin, ymax,
                                                                         zNbins, zmin, zmax), j);
               } else if (fHistoType[i] == "TH3C") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TH3C(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax, yNbins, ymin, ymax,
                                                                         zNbins, zmin, zmax), j);
               } else if (fHistoType[i] == "TProfile") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TProfile(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax), j);
               } else if (fHistoType[i] == "TProfile2D") {
                  static_cast<TObjArray*>(fHisto->At(i))->AddAt(new TProfile2D(histoArrayName.Data(), histoArrayTitle.Data(),
                                                                         xNbins, xmin, xmax, yNbins, ymin, ymax), j);
               } else {
                  ROMEPrint::Error("\nYou have tried to use unimplemented histogram %s.\n\nShutting down the program.\n",
                                   fHistoType[i].Data());
                  gSystem->StackTrace();
                  gROME->GetApplication()->Terminate(1);
               }
               histoPtr = static_cast<TH1*>(static_cast<TObjArray*>(fHisto->At(i))->At(j));
               static_cast<TFolder*>(fRootFolder->At(i))->Add(histoPtr);
               histoPtr->GetXaxis()->SetTitle(xLabel.Data());
               histoPtr->GetYaxis()->SetTitle(yLabel.Data());
               histoPtr->GetZaxis()->SetTitle(zLabel.Data());
            }
         }
      }
   }
}

//______________________________________________________________________________
Bool_t ROMETask::CheckHistoActive(Int_t histoIndex)
{
   if (!static_cast<ROMEHisto*>(fHistoParameter->At(histoIndex))->IsActive()) {
      ROMEPrint::Error("histogram %s is deactivated. Please check the state with Is%sActive() in your code.",
                       fHistoName[histoIndex].Data(), fHistoName[histoIndex].Data());
      return false;
   }
   return true;
}

//______________________________________________________________________________
TObject *ROMETask::GetHistoAt(Int_t i)
{
   if (!CheckHistoActive(i)) {
      return 0;
   }
   if (gROME->IsROMEMonitor() && !fHistoUpToDate[i]) {
      ROMEString command;
      command.SetFormatted("Task_%d:Histo_%d", fTaskIndex, i);
      fHisto->AddAt(gROME->GetSocketClientNetFolder()->FindObjectAny(command.Data()), i);
      fHistoUpToDate[i] = true;
   }
   return fHisto->At(i);
}

// Graph methods
//______________________________________________________________________________
void ROMETask::ResetGraph()
{
   Int_t i, j;
   for (i = 0; i < GetNumberOfGraphs(); i++) {
      if (static_cast<ROMEGraph*>(fGraphParameter->At(i))->IsActive()) {
         if (!fGraphArray[i]) {
            static_cast<TGraph*>(fGraph->At(i))->Set(0);
         } else {
            for (j = 0; j < static_cast<TObjArray*>(fGraph->At(i))->GetEntries(); j++) {
               static_cast<TGraph*>(static_cast<TObjArray*>(fGraph->At(i))->At(j))->Set(0);
            }
         }
      }
   }
}

//______________________________________________________________________________
void ROMETask::BookGraph()
{
   if (fNumberOfGraphs <= 0) {
      return;
   }

   SetOriginalGraphParameters();
   Int_t      i, j;
   ROMEGraph *graphHandle = 0;
   ROMEString graphArrayName;
   ROMEString graphArrayTitle;
   ROMEString graphTitle;
   ROMEString folderTitle;
   ROMEString xLabel;
   ROMEString yLabel;
   ROMEString zLabel;
   Int_t      arraySize;
   ROMEString arraySizeStr;
   Int_t      arrayStartIndex;
   ROMEString arrayStartIndexStr;
   Double_t   xmin;
   ROMEString xminStr;
   Double_t   xmax;
   ROMEString xmaxStr;
   Double_t   ymin;
   ROMEString yminStr;
   Double_t   ymax;
   ROMEString ymaxStr;
   Double_t   zmin;
   ROMEString zminStr;
   Double_t   zmax;
   ROMEString zmaxStr;
   TGraph    *graphPtr = 0;

   for (i = 0; i < fNumberOfGraphs; i++) {
      graphHandle = static_cast<ROMEGraph*>(fGraphParameter->At(i));

      if (graphHandle->IsActive()) {
         graphTitle         = graphHandle->GetTitle();
         folderTitle        = graphHandle->GetFolderTitle();
         xLabel             = graphHandle->GetXLabel();
         yLabel             = graphHandle->GetYLabel();
         zLabel             = graphHandle->GetZLabel();
         arraySize          = graphHandle->GetArraySize();
         arraySizeStr       = graphHandle->GetArraySizeString(arraySizeStr);
         arrayStartIndex    = graphHandle->GetArrayStartIndex();
         arrayStartIndexStr = graphHandle->GetArrayStartIndexString(arrayStartIndexStr);
         xmin               = graphHandle->GetXmin();
         xminStr            = graphHandle->GetXminString(xminStr);
         xmax               = graphHandle->GetXmax();
         xmaxStr            = graphHandle->GetXmaxString(xmaxStr);
         ymin               = graphHandle->GetYmin();
         yminStr            = graphHandle->GetYminString(yminStr);
         ymax               = graphHandle->GetYmax();
         ymaxStr            = graphHandle->GetYmaxString(ymaxStr);
         zmin               = graphHandle->GetZmin();
         zminStr            = graphHandle->GetZminString(zminStr);
         zmax               = graphHandle->GetZmax();
         zmaxStr            = graphHandle->GetZmaxString(zmaxStr);

         graphTitle      = GetObjectInterpreterCharValue  (GetObjectInterpreterCode(graphTitle),         graphTitle,  graphTitle);
         folderTitle     = GetObjectInterpreterCharValue  (GetObjectInterpreterCode(folderTitle),        folderTitle, folderTitle);
         xLabel          = GetObjectInterpreterCharValue  (GetObjectInterpreterCode(xLabel),             xLabel,      xLabel);
         yLabel          = GetObjectInterpreterCharValue  (GetObjectInterpreterCode(yLabel),             yLabel,      yLabel);
         zLabel          = GetObjectInterpreterCharValue  (GetObjectInterpreterCode(zLabel),             zLabel,      zLabel);
         arraySize       = GetObjectInterpreterIntValue   (GetObjectInterpreterCode(arraySizeStr),       arraySize);
         arrayStartIndex = GetObjectInterpreterIntValue   (GetObjectInterpreterCode(arrayStartIndexStr), arrayStartIndex);
         xmin            = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(xminStr),            xmin);
         xmax            = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(xmaxStr),            xmax);
         ymin            = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(yminStr),            ymin);
         ymax            = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(ymaxStr),            ymax);
         zmin            = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(zminStr),            zmin);
         zmax            = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(zmaxStr),            zmax);

         // create graphs
         if (!fGraphArray[i]) {
            if (false) {
            } else if (fGraphType[i] == "TCutG") {
               fGraph->AddAt(new TCutG(fGraphName[i] + "_cutg", 1), i);
               fGraphStorage->AddAt(new TCutG(fGraphName[i] + "Storage_cutg", 1), i);
            } else if (fGraphType[i] == "ROMETGraph") {
               fGraph->AddAt(new ROMETGraph(), i);
               fGraphStorage->AddAt(new ROMETGraph(), i);
            } else if (fGraphType[i] == "ROMETGraphErrors") {
               fGraph->AddAt(new ROMETGraphErrors(), i);
               fGraphStorage->AddAt(new ROMETGraphErrors(), i);
            } else if (fGraphType[i] == "ROMETCutG") {
               fGraph->AddAt(new ROMETCutG(), i);
               fGraphStorage->AddAt(new ROMETCutG(), i);
            } else {
               ROMEPrint::Error("\nYou have tried to use unimplemented graph %s.\n\nShutting down the program.\n",
                                fGraphType[i].Data());
               gSystem->StackTrace();
               gROME->GetApplication()->Terminate(1);
            }
            graphPtr = static_cast<TGraph*>(fGraph->At(i));
            static_cast<TFolder*>(fRootFolder->At(i + fNumberOfHistos))->Add(graphPtr);
            graphPtr->SetNameTitle(fGraphName[i] + fTaskSuffix, graphTitle.Data());
            graphPtr->GetXaxis()->SetTitle(xLabel.Data());
            graphPtr->GetYaxis()->SetTitle(yLabel.Data());

         } else {
            fGraph->AddAt(new TObjArray(arraySize), i);
            fGraphStorage->AddAt(new TObjArray(arraySize), i);

            for (j = 0; j < arraySize; j++) {
               graphArrayName = fGraphName[i];
               graphArrayName.AppendFormatted("_%0*d", 3, j + arrayStartIndex);
               graphArrayName += fTaskSuffix;
               graphArrayTitle = fGraphName[i];
               graphArrayTitle.AppendFormatted("_%0*d", 3, j + arrayStartIndex);

               if (false) {
               } else if (fGraphType[i] == "TCutG") {
                  static_cast<TObjArray*>(fGraph->At(i))->AddAt(new TCutG(fGraphName[i] + "_cutg", 1), j);
                  static_cast<TObjArray*>(fGraphStorage->At(i))->AddAt(new TCutG(fGraphName[i] + "Storage_cutg", 1), j);
               } else if (fGraphType[i] == "ROMETGraph") {
                  static_cast<TObjArray*>(fGraph->At(i))->AddAt(new ROMETGraph(), j);
                  static_cast<TObjArray*>(fGraphStorage->At(i))->AddAt(new ROMETGraph(), j);
               } else if (fGraphType[i] == "ROMETGraphErrors") {
                  static_cast<TObjArray*>(fGraph->At(i))->AddAt(new ROMETGraphErrors(), j);
                  static_cast<TObjArray*>(fGraphStorage->At(i))->AddAt(new ROMETGraphErrors(), j);
               } else if (fGraphType[i] == "ROMETCutG") {
                  static_cast<TObjArray*>(fGraph->At(i))->AddAt(new ROMETCutG(), j);
                  static_cast<TObjArray*>(fGraphStorage->At(i))->AddAt(new ROMETCutG(), j);
               } else {
                  ROMEPrint::Error("\nYou have tried to use unimplemented graph %s.\n\nShutting down the program.\n",
                                   fGraphType[i].Data());
                  gSystem->StackTrace();
                  gROME->GetApplication()->Terminate(1);
               }
               graphPtr = static_cast<TGraph*>(static_cast<TObjArray*>(fGraph->At(i))->At(j));
               static_cast<TFolder*>(fRootFolder->At(i + fNumberOfHistos))->Add(graphPtr);
               graphPtr->SetNameTitle(graphArrayName.Data(), graphArrayTitle.Data());
               graphPtr->GetXaxis()->SetTitle(xLabel);
               graphPtr->GetYaxis()->SetTitle(yLabel);
            }
         }
      }
   }
}

//______________________________________________________________________________
void ROMETask::ReBookGraph()
{
   if (fNumberOfGraphs <= 0) {
      return;
   }

   SetOriginalGraphParameters();
   Int_t      i, j;
   ROMEGraph *graphHandle = 0;
   ROMEString graphArrayName;
   ROMEString graphArrayTitle;
   ROMEString graphTitle;
   ROMEString folderTitle;
   ROMEString xLabel;
   ROMEString yLabel;
   ROMEString zLabel;
   Int_t      arraySize;
   ROMEString arraySizeStr;
   Int_t      arrayStartIndex;
   ROMEString arrayStartIndexStr;
   Double_t   xmin;
   ROMEString xminStr;
   Double_t   xmax;
   ROMEString xmaxStr;
   Double_t   ymin;
   ROMEString yminStr;
   Double_t   ymax;
   ROMEString ymaxStr;
   Double_t   zmin;
   ROMEString zminStr;
   Double_t   zmax;
   ROMEString zmaxStr;
   TGraph    *graphPtr;

   for (i = 0; i < fNumberOfGraphs; i++) {
      graphHandle = static_cast<ROMEGraph*>(fGraphParameter->At(i));

      if (graphHandle->IsActive()) {
         graphTitle         = graphHandle->GetTitle();
         folderTitle        = graphHandle->GetFolderTitle();
         xLabel             = graphHandle->GetXLabel();
         yLabel             = graphHandle->GetYLabel();
         zLabel             = graphHandle->GetZLabel();
         arraySize          = graphHandle->GetArraySize();
         arraySizeStr       = graphHandle->GetArraySizeString(arraySizeStr);
         arrayStartIndex    = graphHandle->GetArrayStartIndex();
         arrayStartIndexStr = graphHandle->GetArrayStartIndexString(arrayStartIndexStr);
         xmin               = graphHandle->GetXmin();
         xminStr            = graphHandle->GetXminString(xminStr);
         xmax               = graphHandle->GetXmax();
         xmaxStr            = graphHandle->GetXmaxString(xmaxStr);
         ymin               = graphHandle->GetYmin();
         yminStr            = graphHandle->GetYminString(yminStr);
         ymax               = graphHandle->GetYmax();
         ymaxStr            = graphHandle->GetYmaxString(ymaxStr);
         zmin               = graphHandle->GetZmin();
         zminStr            = graphHandle->GetZminString(zminStr);
         zmax               = graphHandle->GetZmax();
         zmaxStr            = graphHandle->GetZmaxString(zmaxStr);

         graphTitle      = GetObjectInterpreterCharValue  (GetObjectInterpreterCode(graphTitle),         graphTitle,  graphTitle);
         folderTitle     = GetObjectInterpreterCharValue  (GetObjectInterpreterCode(folderTitle),        folderTitle, folderTitle);
         xLabel          = GetObjectInterpreterCharValue  (GetObjectInterpreterCode(xLabel),             xLabel,      xLabel);
         yLabel          = GetObjectInterpreterCharValue  (GetObjectInterpreterCode(yLabel),             yLabel,      yLabel);
         zLabel          = GetObjectInterpreterCharValue  (GetObjectInterpreterCode(zLabel),             zLabel,      zLabel);
         arraySize       = GetObjectInterpreterIntValue   (GetObjectInterpreterCode(arraySizeStr),       arraySize);
         arrayStartIndex = GetObjectInterpreterIntValue   (GetObjectInterpreterCode(arrayStartIndexStr), arrayStartIndex);
         xmin            = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(xminStr), xmin);
         xmax            = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(xmaxStr), xmax);
         ymin            = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(yminStr), ymin);
         ymax            = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(ymaxStr), ymax);
         zmin            = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(zminStr), zmin);
         zmax            = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(zmaxStr), zmax);

         // expand graphs
         if (!fGraphArray[i]) {
            // do nothing for non-array graphs
         } else {
            Int_t arraySizeOld = static_cast<TObjArray*>(fGraph->At(i))->GetEntries();
            if (arraySize > arraySizeOld) {
               static_cast<TObjArray*>(fGraph->At(i))->Expand(arraySize);
               static_cast<TObjArray*>(fGraphStorage->At(i))->Expand(arraySize);
            }

            for (j = arraySizeOld; j < arraySize; j++) {
               graphArrayName = fGraphName[i];
               graphArrayName.AppendFormatted("_%0*d", 3, j + arrayStartIndex);
               graphArrayName += fTaskSuffix;
               graphArrayTitle = fGraphName[i];
               graphArrayTitle.AppendFormatted("_%0*d", 3, j + arrayStartIndex);

               if (false) {
               } else if (fGraphType[i] == "TCutG") {
                  static_cast<TObjArray*>(fGraph->At(i))->AddAt(new TCutG(fGraphName[i] + "_cutg", 1), j);
                  static_cast<TObjArray*>(fGraphStorage->At(i))->AddAt(new TCutG(fGraphName[i] + "Storage_cutg", 1), j);
               } else if (fGraphType[i] == "ROMETGraph") {
                  static_cast<TObjArray*>(fGraph->At(i))->AddAt(new ROMETGraph(), j);
                  static_cast<TObjArray*>(fGraphStorage->At(i))->AddAt(new ROMETGraph(), j);
               } else if (fGraphType[i] == "ROMETGraphErrors") {
                  static_cast<TObjArray*>(fGraph->At(i))->AddAt(new ROMETGraphErrors(), j);
                  static_cast<TObjArray*>(fGraphStorage->At(i))->AddAt(new ROMETGraphErrors(), j);
               } else if (fGraphType[i] == "ROMETCutG") {
                  static_cast<TObjArray*>(fGraph->At(i))->AddAt(new ROMETCutG(), j);
                  static_cast<TObjArray*>(fGraphStorage->At(i))->AddAt(new ROMETCutG(), j);
               } else {
                  ROMEPrint::Error("\nYou have tried to use unimplemented graph %s.\n\nShutting down the program.\n",
                                   fGraphType[i].Data());
                  gSystem->StackTrace();
                  gROME->GetApplication()->Terminate(1);
               }
               graphPtr = static_cast<TGraph*>(static_cast<TObjArray*>(fGraph->At(i))->At(j));
               static_cast<TFolder*>(fRootFolder->At(i + fNumberOfHistos))->Add(graphPtr);
               graphPtr->SetNameTitle(graphArrayName.Data(), graphArrayTitle.Data());
               graphPtr->GetXaxis()->SetTitle(xLabel);
               graphPtr->GetYaxis()->SetTitle(yLabel);
            }
         }
      }
   }
}

//______________________________________________________________________________
Bool_t ROMETask::CheckGraphActive(Int_t graphIndex)
{
   if (!static_cast<ROMEGraph*>(fGraphParameter->At(graphIndex))->IsActive()) {
      ROMEPrint::Error("graph %s is deactivated. Please check the state with Is%sActive() in your code.",
                       fGraphName[graphIndex].Data(), fGraphName[graphIndex].Data());
      return false;
   }
   return true;
}

//______________________________________________________________________________
void ROMETask::CopyHistosAndGraphs(TDirectory *d) const
{
   TDirectory *directory;
   ROMEString folderName;
   ROMEString folderTitle;
   Int_t i, j;

   for (i = 0; i < fNumberOfHistos; i++) {
      ROMEHisto *histoHandle = 0;
      histoHandle = static_cast<ROMEHisto*>(fHistoParameter->At(i));
      if (fHisto->At(i) && histoHandle->IsActive() && histoHandle->IsWrite()) {
         folderName = fRootFolder->At(i)->GetName();
         folderTitle = fRootFolder->At(i)->GetTitle();
         if (folderName != fHistoFolder->GetName()) {
            if (!(directory = static_cast<TDirectory*>(d->FindObject(folderName)))) {
               d->cd();
#if (ROOT_VERSION_CODE < ROOT_VERSION(5,15,2))
               directory = new TDirectory(folderName, folderTitle);
#else
               directory = new TDirectoryFile(folderName, folderTitle);
#endif
            }
         } else {
            directory = d;
         }
         directory->cd();
         if (!fHistoArray[i]) {
            ROMEPrint::Debug("Writing histo %s:%s\n", GetName(), fHisto->At(i)->GetName());
            fHisto->At(i)->Write(0, gROME->GetOutputObjOption());
         } else {
            for (j = 0; j < static_cast<TObjArray*>(fHisto->At(i))->GetEntries(); j++) {
               ROMEPrint::Debug("Writing histo %s:%s\n", GetName(), static_cast<TObjArray*>(fHisto->At(i))->At(j)->GetName());
               static_cast<TObjArray*>(fHisto->At(i))->At(j)->Write(0, gROME->GetOutputObjOption());
            }
         }
      }
   }

  for (i = 0; i < fNumberOfGraphs; i++) {
      ROMEGraph *graphHandle = 0;
      graphHandle = static_cast<ROMEGraph*>(fGraphParameter->At(i));
      if (fGraph->At(i) && graphHandle->IsActive() && graphHandle->IsWrite()) {
         folderName = fRootFolder->At(i + fNumberOfHistos)->GetName();
         folderTitle = fRootFolder->At(i + fNumberOfHistos)->GetTitle();
         if (folderName != fHistoFolder->GetName()) {
            if (!(directory = static_cast<TDirectory*>(d->FindObject(folderName)))) {
               d->cd();
#if (ROOT_VERSION_CODE < ROOT_VERSION(5,15,2))
               directory = new TDirectory(folderName, folderTitle);
#else
               directory = new TDirectoryFile(folderName, folderTitle);
#endif
            }
         } else {
            directory = d;
         }
         directory->cd();
         if (!fGraphArray[i]) {
            ROMEPrint::Debug("Writing graph %s:%s\n", GetName(), fGraph->At(i)->GetName());
            fGraph->At(i)->Write(0, gROME->GetOutputObjOption());
         } else {
            for (j = 0; j < static_cast<TObjArray*>(fGraph->At(i))->GetEntries(); j++) {
               ROMEPrint::Debug("Writing graph %s:%s\n", GetName(), static_cast<TObjArray*>(fGraph->At(i))->At(j)->GetName());
               static_cast<TObjArray*>(fGraph->At(i))->At(j)->Write(0, gROME->GetOutputObjOption());
            }
         }
      }
   }
}

//______________________________________________________________________________
void ROMETask::PropagateDeactivation()
{
   // Deactivate sub tasks if this is deactivated
   TIter next1(fTasks);
   ROMETask *task;
   if (!IsActive()) {
      while((task = static_cast<ROMETask*>(next1()))) {
         task->SetActive(kFALSE);
      }
   }
   TIter next2(fTasks);
   while((task = static_cast<ROMETask*>(next2()))) {
      task->PropagateDeactivation();
   }
}
