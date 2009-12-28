// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMEEventLoop                                                        //
//                                                                      //
//  Implements the event loop. This Class must be the root Task of
//  the Application.
//                                                                      //
//  $Id$
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TH1.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TThread.h>
#include <TFolder.h>
#include <TTime.h>
#include <TVirtualMutex.h>
#include <TArrayL64.h>
#if (ROOT_VERSION_CODE < ROOT_VERSION(5,15,2))
#   include <TDirectory.h>
#else
#   include <TDirectoryFile.h>
#endif
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS
#if defined( R__VISUAL_CPLUSPLUS )
#   include <io.h>
#endif
#if defined( R__UNIX )
#   include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "ROMEUtilities.h"
#include "ROMETreeInfo.h"
#include "ROMETGraph.h"
#include "ROMETGraphErrors.h"
#include "ROMERint.h"
#include "ROMEConfig.h"
#include "ArgusWindow.h"
#include "ArgusAnalyzerController.h"
#include "ROMENetFolderServer.h"
#include "ROMEMidasDAQ.h"

#if defined( HAVE_MIDAS )
#   include "midas.h"
#endif

#include "ROMEEventLoop.h"
#include "ROMEiostream.h"

TTask *TTask::fgBeginTask  = 0;
TTask *TTask::fgBreakPoint = 0;

// Hot Links handle initialization
#if defined( HAVE_MIDAS )
bool ROMEEventLoop::fHotLinksChanged = kFALSE;
#endif // HAVE_MIDAS

ClassImp(ROMEEventLoop)

extern TVirtualMutex *gObjectStorageMutex; // declared in ROMEAnalyzer.cpp

//______________________________________________________________________________
ROMEEventLoop::ROMEEventLoop(const char *name, const char *title)
:ROMETask(name, title, 0, 0, 0, 0, 0, 0, 0)
,fCurrentEvent(0)
,fFirstUserInput(kFALSE)
,fStop(kFALSE)
,fStopAtRun(-1)
,fStopAtEvent(-1)
,fStatisticsTimeOfLastEvent(0)
,fStatisticsLastEvent(0)
,fProgressDelta(10000)
,fProgressTimeOfLastEvent(0)
,fProgressLastEvent(0)
,fProgressWrite(kFALSE)
,fUserInputLastTime(0)
,fContinuous(kTRUE)
,fTreeInfo(new ROMETreeInfo())
,fTreeUpdateIndex(0)
,fAlwaysFillTrees(kFALSE)
,fHistoFile(0)
,fWatchWriteEvent()
,fUpdateWindow(kFALSE)
,fLastUpdateTime(0)
//,fWindowFirstDraw(kFALSE)
,fUpdateWindowLastEvent(0)
,fBeginOfRunMacro("")
,fBeginOfEventMacro("")
,fEndOfEventMacro("")
,fEndOfRunMacro("")
,fHaveBeginOfRunMacro(kFALSE)
,fHaveBeginOfEventMacro(kFALSE)
,fHaveEndOfEventMacro(kFALSE)
,fHaveEndOfRunMacro(kFALSE)
,fLastNetFolderServerUpdateTime(0)
,fNetFolderServerUpdateThread(0)
{
}

//______________________________________________________________________________
ROMEEventLoop::~ROMEEventLoop()
{
   if (fNetFolderServerUpdateThread) {
      TThread::Delete(fNetFolderServerUpdateThread);
      fNetFolderServerUpdateThread = 0;
   }
   SafeDelete(fHistoFile);
   SafeDeleteArray(fStatisticsTimeOfLastEvent);
   SafeDeleteArray(fStatisticsLastEvent);
   SafeDelete(fTreeInfo);
}

//______________________________________________________________________________
void ROMEEventLoop::ExecuteTask(Option_t *option)
{
   // Declarations
   //--------------
   Long64_t ii, eventLoopIndex;
   Int_t i;

//   ROMEPrint::SetVerboseLevel(ROMEPrint::kDebug);
   fFirstUserInput = true;
   if (!strcmp(option, "init")) {
      this->InitTrees();
      return;
   }
// Event loop
   fWatchUserEvent.Reset();
   fWatchAll.Reset();
   fWatchWriteEvent.Reset();
   fWatchAll.Start(false);
   if (fgBeginTask) {
      Error("ExecuteTask", "Cannot execute task:%s, already running task: %s", GetName(), fgBeginTask->GetName());
      this->Terminate();
      gROME->SetTerminationFlag();
      ROMEPrint::Print("\n\nTerminating Program !\n");
      return;
   }
   if (!IsActive()) {
      this->Terminate();
      gROME->SetTerminationFlag();
      ROMEPrint::Print("\n\nTerminating Program !\n");
      return;
   }
   // Deactivate all tasks if stand alone ARGUS
   if (gROME->IsStandAloneARGUS()) {
      for (i = 0; i < gROME->GetTaskObjectEntries(); i++) {
         static_cast<ROMETask*>(gROME->GetTaskObjectAt(i))->SetActive(false);
      }
   }

   // Initialisation
   //----------------
   if (!gROME->IsROMEMonitor()) {
      if (!this->DAQInit()) {
         this->Terminate();
         gROME->SetTerminationFlag();
         ROMEPrint::Print("\n\nTerminating Program !\n");
         return;
      }
   } else {
      if (gROME->isOnline() && gROME->IsActiveDAQ("midas")) {
#if defined( HAVE_MIDAS )
         static_cast<ROMEMidasDAQ*>(gROME->GetActiveDAQ())->
               StartOnlineCommunication(static_cast<ROMEMidasDAQ*>(gROME->GetActiveDAQ()));
#endif
      }
   }

   if (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()) {
      ROMEPrint::Debug("Executing Init tasks\n");
      ExecuteTasks("Init");
      CleanTasks();
   }

   // Read Histograms
   if (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()) {
      ROMEPrint::Debug("Reading histograms\n");
      if (gROME->IsHistosRead()) {
         ReadHistograms();
      }
   }

   eventLoopIndex = 0;

   // Loop over Runs
   //----------------
   ROMEPrint::Debug("Entering run loop\n");
   for (ii = 0; !this->isTerminate(); ii++) {
      if (!gROME->IsROMEMonitor()) {
         if (!this->DAQBeginOfRun(eventLoopIndex)) {
            this->DAQTerminate();
            gROME->SetTerminationFlag();
            ROMEPrint::Print("\n\nTerminating Program !\n");
            return;
         }
         if (this->isEndOfRun()) {
#if 0 /* increasing eventLoopIndex when an error in DAQBeginOfRun may skip initialization of array folder */
            eventLoopIndex++;
#endif
            continue;
         }
         if (this->isTerminate()) {
            break;
         }

         if (this->isRunning()) {
            // Hot Links
#if defined( HAVE_MIDAS )
            if (ROMEEventLoop::fHotLinksChanged) {
               this->UpdateHotLinks();
               ROMEEventLoop::fHotLinksChanged = false;
            }
#endif // HAVE_MIDAS

            if(!gROME->CheckDependences())
               return;

            // Output
            if (gROME->IsShowRunStat()) {
               ROMEPrint::Print("\n\nRun "R_LLD" started\n", gROME->GetCurrentRunNumber());
            }

            fHaveBeginOfRunMacro   = (fBeginOfRunMacro.Length()   > 0);
            fHaveBeginOfEventMacro = (fBeginOfEventMacro.Length() > 0);
            fHaveEndOfEventMacro   = (fEndOfEventMacro.Length()   > 0);
            fHaveEndOfRunMacro     = (fEndOfRunMacro.Length()     > 0);

            // Begin of Run Tasks
            if (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()) {
               ROMEPrint::Debug("Executing BeginOfRun tasks\n");
               ExecuteTasks("BeginOfRun");
               CleanTasks();
               if (fHaveBeginOfRunMacro) {
                  gROME->GetApplication()->ProcessFile(fBeginOfRunMacro.Data());
               }
            }
            WriteRunHeaders();

            eventLoopIndex++;
         }
      }

      if (ii == 0) {
         // Start ARGUS
         //-------------
         if (gROME->IsStandAloneARGUS() || gROME->IsROMEAndARGUS() || gROME->IsROMEMonitor()) {
            if (IsTerminal()) {
               ROMEPrint::Print("Starting argus monitor ...                            \r");
            }
            gROME->StartWindow();
            ROMEPrint::Print("Argus monitor running                                 \n");
//            fWindowFirstDraw = kTRUE;
         }
      }

      // Loop over Events
      //------------------
      fUpdateWindow = true;
      fCurrentEvent = -1;
      fFirstUserInput = true;
      ROMEPrint::Debug("Entering event loop\n");

      // Initialize event number
      gROME->SetCurrentEventNumber(0);

      for (; !this->isTerminate() && !this->isEndOfRun();) {
         int status = this->RunEvent();
         if (status == kReturn) {
            return;
         } else if (status == kBreak) {
            break;
         }
      }

      if ((this->isEndOfRun() || this->isTerminate()) &&
          !gROME->IsROMEMonitor() && !gROME->IsStandAloneARGUS()) {
         if (this->isEndOfRun()) {
            this->SetBeginOfRun();
         }

         // Store whole Event
         if (gROME->GetNetFolderServer()) {
            gROME->GetNetFolderServer()->SetCopyAll(true);
            StoreEvent(false);
            gROME->GetNetFolderServer()->SetCopyAll(false);
            gROME->GetNetFolderServer()->SetEventStorageAvailable(false);
         }

         // Show number of processed events
         if (gROME->IsShowRunStat()) {
            ROMEPrint::Print(R_LLD" events processed                                             \n",
                             static_cast<Long64_t>(gROME->GetProcessedEvents()+0.5));
         }

         // End of Run Tasks
         if (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()) {
            ROMEPrint::Debug("Executing EndOfRun tasks\n");
            gROME->SetCurrentEventNumber(kEventNumberEndOfRun);
            ExecuteTasks("EndOfRun");
            CleanTasks();
            if (fHaveEndOfRunMacro) {
               gROME->GetApplication()->ProcessFile(fEndOfRunMacro.Data());
            }
         }

         // Show stopped message
         if (gROME->IsShowRunStat()) {
            ROMEPrint::Print("Run "R_LLD" stopped\n\n", gROME->GetCurrentRunNumber());
         }

         // Disconnect
         if (!this->DAQEndOfRun()) {
            this->Terminate();
            gROME->SetTerminationFlag();
            ROMEPrint::Print("\n\nTerminating Program !\n");
            return;
         }
      }
   }

   // Program End
   gROME->SetProgramTerminated();
   if (gROME->GetNetFolderServer()) {
      gROME->GetNetFolderServer()->SetCopyAll(true);
   }

   // Terminate
   if (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS() || gROME->IsStandAloneARGUS()) {
      ROMEPrint::Debug("Executing Terminate tasks\n");
      gROME->SetCurrentEventNumber(kEventNumberTerminate);
      ExecuteTasks("Terminate");
      CleanTasks();
      if (!this->DAQTerminate()) {
         gROME->SetTerminationFlag();
         ROMEPrint::Print("\n\nTerminating Program !\n");
         return;
      }
   }

   if (gROME->IsStandAloneARGUS() || gROME->IsROMEAndARGUS() || gROME->IsROMEMonitor()) {
      fUpdateWindow = false;
//      gROME->GetWindow()->TriggerEventHandler();
   }

   if (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()) {
      fWatchAll.Stop();
      if (gROME->IsShowRunStat()) {
         ROMEString str;
         ROMEPrint::Print("run times :                           RealTime All  CPUTime All   User Events\n");
         ROMEPrint::Print("-----------                           ------------  ------------  ------------\n");
         Exec("Time");
         gROME->GetActiveDAQ()->TimeDAQ();
         ExecuteTasks("Time");
         CleanTasks();
         if (gROME->IsROMEAndARGUS()) {
            ROMEPrint::Print("%s", gROME->GetWindow()->GetTimeStatisticsString(str));
         }
         TimeEventLoop();
         ROMEPrint::Print("\n");
      }
      if (gVerboseLevel >= ROMEPrint::kDebug) {
         ExecuteTasks("PrintSkipped");
      }
      CleanTasks();
   }

   // Root Interpreter
   if (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()) {
      ROMEString prompt = gROME->GetProgramName();
      prompt.ToLower();
      prompt += " [%d] ";
      static_cast<TRint*>(gROME->GetApplication())->SetPrompt(prompt.Data());
      if (!gROME->isQuitMode()) {
         gROME->GetApplication()->Run(true);
         ROMEPrint::PrintAlways("\n");
      }
   }

}

//______________________________________________________________________________
Int_t ROMEEventLoop::RunEvent()
{
   ROMEString text;

   // Run one Event.
   gROME->SetSkipEvent(false);
   fCurrentEvent++;
   ROMEPrint::Debug("ROMEEventLoop::RunEvent()");

   // Update
   if (!this->isContinue() || this->isStopped() ||
       gROME->IsStandAloneARGUS() || gROME->IsROMEMonitor()) {
      ROMEPrint::Debug("ROMEEventLoop::RunEvent() : Update\n");
      if (fCurrentEvent > 0) {
         if (!this->Update()) {
            this->Terminate();
            gROME->SetTerminationFlag();
            ROMEPrint::Print("\n\nTerminating Program !\n");
            return kReturn;
         }
      }
   }

   // Check If Window Closed
   ROMEPrint::Debug("ROMEEventLoop::RunEvent() : IsWindowClosed\n");
   if (gROME->IsWindowClosed()) {
      if (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()) {
         this->DAQTerminate();
         gROME->SetTerminationFlag();
         ROMEPrint::Print("\n\nTerminating Program !\n");
         return kReturn;
      }
      this->SetStopped();
      this->SetTerminate();
      return kBreak;
   }

   // Check Event Numbers
   ROMEPrint::Debug("ROMEEventLoop::RunEvent() : CheckEventNumber\n");
   if (gROME->isOffline() && !gROME->IsROMEMonitor() && !gROME->IsStandAloneARGUS()) {
      int status = gROME->CheckEventNumber(fCurrentEvent);
      if (status == 0) {
         return kContinue;
      } else if (status == -1) {
         this->SetStopped();
         this->SetEndOfRun();
         return kBreak;
      }
   }

   // User Input
   if ((!this->isContinue() || gROME->isOnline()) &&
       !gROME->IsROMEMonitor() && !gROME->IsStandAloneARGUS()) {
      if (!fFirstUserInput && fCurrentEvent > 0) {
         ROMEPrint::Debug("ROMEEventLoop::RunEvent() : UserInput\n");
         if (!this->UserInput()) {
            gROME->SetTerminationFlag();
            if (gROME->IsROMEAndARGUS()) {
               fUpdateWindow = false;
               fUpdateWindowLastEvent = gROME->GetCurrentEventNumber();
               gROME->GetWindow()->TriggerEventHandler();
            }
            // Terminate
            if (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()) {
               ROMEPrint::Debug("Executing Terminate tasks\n");
               gROME->SetCurrentEventNumber(kEventNumberTerminate);
               ExecuteTasks("Terminate");
               CleanTasks();
               if (!this->DAQTerminate()) {
                  gROME->SetTerminationFlag();
                  ROMEPrint::Print("\n\nTerminating Program !\n");
                  return false;
               }
            }
            return kReturn;
         }
         if (this->isTerminate()) {
            return kBreak;
         }
      }
      fFirstUserInput = false;
   }

   // Set Fill Event equal true
   if (!this->isContinue() && !gROME->IsROMEMonitor() && !gROME->IsStandAloneARGUS()) {
      gROME->SetFillEvent();
   }

   // Read Event
   ROMEPrint::Debug("ROMEEventLoop::RunEvent() : DAQEvent\n");
   if (!gROME->IsROMEMonitor() && !gROME->IsStandAloneARGUS()) {
      if (!this->DAQEvent()) {
         this->Terminate();
         gROME->SetTerminationFlag();
         ROMEPrint::Print("\n\nTerminating Program !\n");
         return kReturn;
      }
      if (fHaveBeginOfEventMacro && this->isAnalyze()) {
         gROME->GetApplication()->ProcessFile(fBeginOfEventMacro.Data());
      }
      if (this->isEndOfRun()) {
         this->SetStopped();
         return kBreak;
      }
      if (this->isBeginOfRun()) {
         this->SetAnalyze();
         return kBreak;
      }
      if (this->isTerminate()) {
         return kBreak;
      }
      if (this->isContinue()) {
         return kContinue;
      }
   }

   // Hot Links
#if defined( HAVE_MIDAS )
   if (!gROME->IsROMEMonitor() && !gROME->IsStandAloneARGUS()) {
      if (ROMEEventLoop::fHotLinksChanged) {
         this->UpdateHotLinks();
         ROMEEventLoop::fHotLinksChanged = false;
      }
   }
#endif // HAVE_MIDAS

   // Event Tasks
   if (!gROME->IsROMEMonitor() && !gROME->IsStandAloneARGUS()) {
      ROMEPrint::Debug("ROMEEventLoop::RunEvent() : ExecuteTasks\n");
      if (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()) {
         text.SetFormatted("Event%d", gROME->GetEventID());
         ROMEPrint::Debug("Executing Event tasks (option = '%s')\n", text.Data());
         ExecuteTasks(text.Data());
         CleanTasks();
         if (fHaveEndOfEventMacro && this->isAnalyze()) {
            gROME->GetApplication()->ProcessFile(fEndOfEventMacro.Data());
         }
      }
      if (gROME->isTerminationFlag()) {
         ROMEPrint::Print("\n\nTerminating Program !\n");
         return kReturn;
      }
      if (this->isEndOfRun()) {
         return kBreak;
      }
      if (this->isContinue()) {
         return kContinue;
      }
   }

   // Store Event
   StoreEvent(true);

   // Write Event
   if (!gROME->IsROMEMonitor() && !gROME->IsStandAloneARGUS()) {
      ROMEPrint::Debug("ROMEEventLoop::RunEvent() : WriteEvent\n");
      if (gROME->isFillEvent()) {
         fWatchWriteEvent.Start(false);
         if (!this->WriteEvent()) {
            this->Terminate();
            gROME->SetTerminationFlag();
            ROMEPrint::Print("\n\nTerminating Program !\n");
            return kReturn;
         }
         fWatchWriteEvent.Stop();
      }
   }
//   if (fWindowFirstDraw && (gROME->IsStandAloneARGUS() || gROME->IsROMEAndARGUS() || gROME->IsROMEMonitor())) {
//      fUpdateWindowLastEvent = gROME->GetCurrentEventNumber();
//      gROME->GetWindow()->TriggerEventHandler();
//      fWindowFirstDraw = kFALSE;
//   }

   return kContinue;
}

//______________________________________________________________________________
Bool_t ROMEEventLoop::StoreEvent(Bool_t useThread)
{
   if (!gROME->IsROMEMonitor() && !gROME->IsStandAloneARGUS() &&
       gROME->GetNetFolderServer() && !gROME->IsObjectStorageUpdated()) {
      const ULong_t kInterval = 10; // this should be changed to parameter
      if (static_cast<ULong_t>(gSystem->Now()) > fLastNetFolderServerUpdateTime + kInterval) {
         if (ROME_TRYLOCK(gObjectStorageMutex) == 0) {
            fLastNetFolderServerUpdateTime = static_cast<ULong_t>(gSystem->Now());
            gROME->FillObjectStorage();
            if (fNetFolderServerUpdateThread) {
               TThread::Delete(fNetFolderServerUpdateThread);
               fNetFolderServerUpdateThread = 0;
            }
            gObjectStorageMutex->UnLock();
            if (useThread) {
               fNetFolderServerUpdateThread =
                     new TThread("CopyThread",
                                 reinterpret_cast<THREADTYPE(*)(void*)>(&ROMEAnalyzer::FillObjectsInNetFolderServer),
                                 static_cast<void*>(gROME));
               fNetFolderServerUpdateThread->Run();
            } else {
               gROME->FillObjectsInNetFolderServer(gROME);
            }
         }
      }
   }
   return kTRUE;
}

//______________________________________________________________________________
Bool_t ROMEEventLoop::DAQInit()
{
   // Initialize the analyzer. Called before the init tasks.
   ROMEPrint::Debug("Executing DAQ Init\n");
   gROME->SetCurrentEventNumber(kEventNumberInit);

   int j;
   this->SetRunning();
   this->SetAnalyze();

#if defined( HAVE_MIDAS )
   this->InitHotLinks();
#endif // HAVE_MIDAS
   gROME->FillConfigParametersFolder();

   // Check IO System
   if (gROME->GetNumberOfRunNumbers() > 0 && gROME->GetNumberOfInputFileNames() > 0) {
      gROME->SetIOType(gROME->kRunNumberAndFileNameBased);
   } else if (gROME->GetNumberOfRunNumbers() > 0) {
      gROME->SetIOType(gROME->kRunNumberBased);
   } else if (gROME->GetNumberOfInputFileNames() > 0) {
      gROME->SetIOType(gROME->kFileNameBased);
   } else if (gROME->isOffline()) {
      if (gROME->isActiveDAQSet()) {
         if (strcmp(gROME->GetActiveDAQ()->GetName(), "none")) {
            ROMEPrint::Error("No run numbers or input file names specified.\n");
            return false;
         }
      }
   }

   // Initialize DAQ System
   if (!gROME->GetActiveDAQ()->InitDAQ()) {
      return false;
   }

   // Set run number
   if (gROME->isOffline() && (gROME->IsRunNumberBasedIO() || gROME->IsRunNumberAndFileNameBasedIO())) {
      gROME->SetCurrentRunNumber(gROME->GetNextRunNumber(gROME->GetCurrentRunNumber()));
   }

   // Init Data Base
   if (!gROME->ReadSingleDataBaseFolders()) {
      ROMEPrint::Error("\nError while reading the data base !\n");
      return false;
   }

   // Reset run number
   if (gROME->isOffline() && (gROME->IsRunNumberBasedIO() || gROME->IsRunNumberAndFileNameBasedIO())) {
      gROME->SetCurrentRunNumber(-1);
   }

   // Open Output Files for accumulative output tree files
   ROMEString filename;
   ROMETree *romeTree;
   TTree *tree;
   const Int_t nTree = gROME->GetTreeObjectEntries();
   Int_t k;
   Bool_t identicalFileNameFound;
   TFile *identicalFilePointer;
   TFile* file;
   for (j = 0; j < nTree; j++) {
      identicalFileNameFound = kFALSE;
      identicalFilePointer = 0;
      romeTree = gROME->GetTreeObjectAt(j);
      if (romeTree->isWrite() && romeTree->isFill()) {
         if (gROME->isTreeAccumulation()) {
            tree = romeTree->GetTree();
            GetTreeFileName(filename, j, kFALSE);
            romeTree->SetFileName(filename);
            for(k = 0; k < j; k++) {
               if (filename == gROME->GetTreeObjectAt(k)->GetFileName()) {
                  identicalFileNameFound = kTRUE;
                  identicalFilePointer = gROME->GetTreeObjectAt(k)->GetFile();
                  break;
               }
            }
            if (!identicalFileNameFound) { // file is not open yet
               if (gROME->isMakeOutputDirectory()) {
                  ROMEString path = gSystem->DirName(filename.Data());
                  path += "/"; // trailing "/" is needed to work mkdir() correctly.
                  gSystem->mkdir(path.Data(), kTRUE);
               }
               file = CreateTFile(filename.Data(), gROME->GetOutputFileOption());
               if (!file || file->IsZombie()) {
                  return false;
               }
               romeTree->SetFile(file);
               gROOT->cd();
            } else { // file is already open
               romeTree->SetFile(identicalFilePointer);
            }
         }
      } else {
         romeTree->SetFile(0);
         filename = "";
         romeTree->SetFileName(filename);
      }
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEEventLoop::DAQBeginOfRun(Long64_t eventLoopIndex)
{
   // Connect the Analyzer to the current run. Called before the BeginOfRun tasks.
   ROMEPrint::Debug("Executing DAQ BeginOfRun\n");
   gROME->SetCurrentEventNumber(kEventNumberBeginOfRun);

   // Statistics
   ResetStatistics();

   if (gROME->isOffline() && (gROME->IsRunNumberBasedIO() || gROME->IsRunNumberAndFileNameBasedIO())) {
      // run number based IO
      Long64_t runNumber = gROME->GetNextRunNumber(gROME->GetCurrentRunNumber());
      if (runNumber == -1) {
         this->SetTerminate();
         return true;
      }
      // Check Configuration
      gROME->SetCurrentRunNumber(runNumber);
      gROME->GetConfiguration()->CheckConfiguration(gROME->GetCurrentRunNumber());
   }

   gROME->UpdateConfigParameters();

   // Begin Of Run Of Active DAQ
   if (!gROME->GetActiveDAQ()->BeginOfRunDAQ()) {
      return false;
   }
   if (this->isEndOfRun()) {
      return true;
   }
   if (this->isTerminate()) {
      return true;
   }

   // Set alwasy fill flag
   fAlwaysFillTrees = gROME->GetActiveDAQ()->GetAlwaysFillTrees();

   // Check Configuration
   if (gROME->isOffline() && gROME->IsFileNameBasedIO()) {
      // file name based IO
      gROME->GetConfiguration()->CheckConfiguration(gROME->GetCurrentInputFileName());
   }

   // Set Run Number
   fTreeInfo->SetRunNumber(gROME->GetCurrentRunNumber());

   // read run header in case ROME DAQ.
   ReadRunHeaders();

   // Update Data Base
   if (!gROME->ReadSingleDataBaseFolders()) {
      ROMEPrint::Error("\nError while reading the data base !\n");
      return false;
   }
   if (eventLoopIndex==0) {
      this->InitArrayFolders();
   }
   if (!gROME->ReadArrayDataBaseFolders()) {
      ROMEPrint::Error("\nError while reading the data base !\n");
      return false;
   }

   // Progress Display
   fProgressDelta = 10000;
   fProgressTimeOfLastEvent = static_cast<ULong_t>(gSystem->Now());
   fProgressLastEvent = 0;
   fProgressWrite = false;

   // Open Output Files
   ROMEString filename;
   ROMEString treename;
   ROMETree *romeTree;
   TTree *tree;
   Int_t k;
   Bool_t identicalFileNameFound;
   TFile *identicalFilePointer;
   TFile *file;
   const Int_t nTree = gROME->GetTreeObjectEntries();
   for (int j = 0; j < nTree; j++) {
      identicalFileNameFound = kFALSE;
      identicalFilePointer = 0;
      romeTree = gROME->GetTreeObjectAt(j);
      if (romeTree->isFill()) {
         tree = romeTree->GetTree();
         if (romeTree->isWrite() && !gROME->isTreeAccumulation()) {
            GetTreeFileName(filename, j, kFALSE);
            if (filename == romeTree->GetFileName()) {
               for(k = 0; k < j; k++) {
                  if (filename == gROME->GetTreeObjectAt(k)->GetFileName()) {
                     identicalFileNameFound = kTRUE;
                     identicalFilePointer = gROME->GetTreeObjectAt(k)->GetFile();
                     break;
                  }
               }
               if (!identicalFileNameFound) { // file is not open yet
                  romeTree->SetFile(new TFile(filename.Data(), "UPDATE"));
                  gROOT->cd();
               } else { // file is already open
                  romeTree->SetFile(identicalFilePointer);
               }
               romeTree->SetFileUpdate();
               fTreeUpdateIndex++;
               treename.SetFormatted("%s_%d", tree->GetName(), fTreeUpdateIndex);
               tree->SetName(treename.Data());
            } else {
               for(k = 0; k < j; k++) {
                  if (filename == gROME->GetTreeObjectAt(k)->GetFileName()) {
                     identicalFileNameFound = kTRUE;
                     identicalFilePointer = gROME->GetTreeObjectAt(k)->GetFile();
                     break;
                  }
               }
               if (!identicalFileNameFound) { // file is not open yet
                  if (gROME->isMakeOutputDirectory()) {
                     ROMEString path = gSystem->DirName(filename.Data());
                     path += "/"; // trailing "/" is needed to work mkdir() correctly.
                     gSystem->mkdir(path.Data(), kTRUE);
                  }
                  file = CreateTFile(filename.Data(), gROME->GetOutputFileOption());
                  if (!file || file->IsZombie()) {
                     return false;
                  }
                  romeTree->SetFile(file);
                  gROOT->cd();
               } else { // file is already open
                  romeTree->SetFile(identicalFilePointer);
               }
               romeTree->SetFileOverWrite();
               fTreeUpdateIndex = 0;
            }
            romeTree->SetFileName(filename);
         } else {
            if (!gROME->isTreeAccumulation()) {
               romeTree->SetFile(0);
               filename = "";
               romeTree->SetFileName(filename);
            }
         }
      }
   }

   // Check task tab dependence
   gROME->GetActiveDAQ()->UpdateConfigParameters();
   if (!gROME->CheckDependences()) {
      return false;
   }

   return true;
}

//______________________________________________________________________________
Bool_t ROMEEventLoop::DAQEvent()
{
   // Reads an event. Called before the Event tasks.
   ROMEPrint::Debug("Executing DAQ Event\n");

   if (gROME->IsDontReadNextEvent()) {
      gROME->SetDontReadNextEvent(false);
      fCurrentEvent--;
      return true;
   }
   this->SetAnalyze();

   ROMEPrint::Debug("Reset folders\n");
   this->ResetFolders();

   if (!gROME->GetActiveDAQ()->EventDAQ(fCurrentEvent)) {
      return false;
   }
   if ((gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()) &&
       gROME->isOffline() && gROME->IsActiveDAQ("midas") && gROME->GetEventID() != 1) {
      // event number is not incremented when non-trigger events.
      fCurrentEvent--;
   }
   if (this->isContinue()) {
      return true;
   }
   if (this->isEndOfRun()) {
      return true;
   }

   if (gROME->IsEventBasedDataBase()) {
      if (!gROME->ReadSingleDataBaseFolders()) {
         ROMEPrint::Error("\nError while reading the data base !\n");
         return false;
      }
      if (!gROME->ReadArrayDataBaseFolders()) {
         ROMEPrint::Error("\nError while reading the data base !\n");
         return false;
      }
   }

   // Update Statistics
   Int_t eventIndex = gROME->GetEventID();
   Statistics *stat;
   if (eventIndex == -1){
      eventIndex = 0;
      stat = gROME->GetStatisticsAt(eventIndex);
   } else if(eventIndex < 0 || eventIndex >= gROME->GetMaxEventID()) {
      stat = 0;
   } else {
      eventIndex = gROME->GetEventID() - 1;
      stat = gROME->GetStatisticsAt(eventIndex);
   }
   if (stat) {
      stat->processedEvents++;
      ULong_t time = static_cast<ULong_t>(gSystem->Now());
      if (fStatisticsTimeOfLastEvent[eventIndex] == 0) {
         fStatisticsTimeOfLastEvent[eventIndex] = time;
      }
      if (time - fStatisticsTimeOfLastEvent[eventIndex] != 0) {
         stat->eventsPerSecond = (stat->processedEvents-fStatisticsLastEvent[eventIndex])/(time-fStatisticsTimeOfLastEvent[eventIndex])*1000.0;
      }
      fStatisticsTimeOfLastEvent[eventIndex] = time;
      fStatisticsLastEvent[eventIndex] = stat->processedEvents;
   }

   fTreeInfo->SetTimeStamp(gROME->GetActiveDAQ()->GetTimeStamp());
   fTreeInfo->SetEventNumber(gROME->GetCurrentEventNumber());

   return true;
}

//______________________________________________________________________________
Bool_t ROMEEventLoop::WriteEvent()
{
   // Writes the event. Called after the Event tasks.
#if 0 // Is it necessary ?
   this->CleanUpFolders();
#endif
   this->FillTrees();

   // Histograms snap shot
   static ULong64_t histoSnapShotLastEvent = 1;
   static Int_t snapShotFileNameLen = -1;

   if (snapShotFileNameLen < 0) {
      // check file name only once for speed.
      snapShotFileNameLen = strlen(gROME->GetHistosSnapShotFileName());
   }
   if (snapShotFileNameLen) {
      if (histoSnapShotLastEvent >= gROME->GetHistosSnapShotEvents()) {
         WriteHistograms(kTRUE);
         histoSnapShotLastEvent = 1;
      } else {
         histoSnapShotLastEvent++;
      }
   }

   return true;
}

//______________________________________________________________________________
Bool_t ROMEEventLoop::Update()
{
   // Update the Analyzer. Called after the Event tasks.

   ROMEString text;
   Long64_t newUpdateWindowEvent;
   ULong_t currentTime = static_cast<ULong_t>(gSystem->Now());

   // Progress Display
   ROMEPrint::Debug("ROMEEventLoop::Update() Progress Display");
   if (fProgressDelta > 1) {
      if (static_cast<Long64_t>(gROME->GetTriggerStatistics()->processedEvents + 0.5) >=
          fProgressLastEvent + fProgressDelta) {
         fProgressTimeOfLastEvent = currentTime;
         fProgressLastEvent = static_cast<Long64_t>(gROME->GetTriggerStatistics()->processedEvents + 0.5);
         fProgressWrite = true;
      } else {
         if (currentTime > static_cast<ULong_t>(fProgressTimeOfLastEvent + 1000)) {
            fProgressDelta /= 10;
         }
      }
   }

   AutoSave();

   ROMEPrint::Debug("ROMEEventLoop::Update() TriggerEventHandler");
   if ((gROME->IsStandAloneARGUS() || gROME->IsROMEAndARGUS() || gROME->IsROMEMonitor())) {
      if (gROME->GetActiveDAQ()->isStopped() && gROME->GetWindow()->IsEventHandlingRequested()) {
         fUpdateWindowLastEvent = gROME->GetCurrentEventNumber();
         gROME->GetWindow()->TriggerEventHandler();
      }
   }

   ROMEPrint::Debug("ROMEEventLoop::Update() Update");
   if (!gROME->isBatchMode()  && (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()) &&
       ((!fContinuous) ||
        (fProgressDelta == 1) ||
        (!(static_cast<Long64_t>(gROME->GetTriggerStatistics()->processedEvents + 0.5) % fProgressDelta) && fProgressWrite))) {
      if (IsTerminal()) {
         ROMEPrint::Print("processed event number "R_LLD"                                              \r",
                          gROME->GetCurrentEventNumber());
         ROMEPrint::Debug("\n");
      }
      fProgressWrite = false;
   }

   if (!gROME->isBatchMode() &&
       (gROME->IsStandAloneARGUS() || gROME->IsROMEAndARGUS() || gROME->IsROMEMonitor()) &&
       fUpdateWindow) {
      newUpdateWindowEvent =  gROME->GetCurrentEventNumber();
      if (fUpdateWindowLastEvent != newUpdateWindowEvent ||
          gROME->GetWindow()->IsEventHandlingRequested()) {
         fUpdateWindowLastEvent = newUpdateWindowEvent;
         if ((!fContinuous && !gROME->IsStandAloneARGUS()) ||
             currentTime > static_cast<ULong_t>(fLastUpdateTime + gROME->GetWindowUpdatePeriod()) ||
             gROME->GetWindow()->IsEventHandlingRequested()) {
            if (!this->isStopped()) {
               fUpdateWindowLastEvent = gROME->GetCurrentEventNumber();
               gROME->GetWindow()->TriggerEventHandler();
            }
            fLastUpdateTime = currentTime;
         }
      }
      gROME->GetApplication()->DisableFPETrap();
      gSystem->ProcessEvents();
      gROME->GetApplication()->EnableFPETrap();
      gSystem->Sleep(10);
   }

   return true;
}

//______________________________________________________________________________
Bool_t ROMEEventLoop::UserInput()
{
   if (gROME->isDaemonMode() || gROME->isBatchMode()) {
      return kTRUE;
   }

   if (!IsTerminal()) {
      return kTRUE;
   }

   // Looks for user input. Called before the Event tasks.
   bool wait = false;
   bool first = true;
   bool interpreter = false;
   bool hit = false;
   ROMEString text;
   int inumber;

   if (fStop) {
      wait = true;
      fStop = false;
//      if (gROME->IsStandAloneARGUS() || gROME->IsROMEAndARGUS() || gROME->IsROMEMonitor()) {
//         fUpdateWindowLastEvent = gROME->GetCurrentEventNumber();
//         gROME->GetWindow()->TriggerEventHandler();
//      }
   } else if ((fStopAtRun == gROME->GetCurrentRunNumber() && fStopAtEvent == gROME->GetCurrentEventNumber()) ||
              (gROME->GetCurrentEventNumber() == 0 && !fContinuous)) {
      if (gROME->IsStandAloneARGUS() || gROME->IsROMEAndARGUS() || gROME->IsROMEMonitor()) {
         fUpdateWindowLastEvent = gROME->GetCurrentEventNumber();
         gROME->GetWindow()->TriggerEventHandler();
      }
      ROMEPrint::Print("Stopped after event "R_LLD"                   \r", gROME->GetCurrentEventNumber());
      wait = true;
   } else if (!gROME->HasUserEvent() && fContinuous &&
              static_cast<ULong_t>(gSystem->Now()) < static_cast<ULong_t>(fUserInputLastTime + 300)) {
      return true;
   }
   fUserInputLastTime = static_cast<ULong_t>(gSystem->Now());

   while (wait || first) {
      if ((gROME->IsStandAloneARGUS() || gROME->IsROMEAndARGUS() || gROME->IsROMEMonitor())) {
         if (!first && gROME->GetWindow()->IsEventHandlingRequested()) {
            fUpdateWindowLastEvent = gROME->GetCurrentEventNumber();
            gROME->GetWindow()->TriggerEventHandler();
         }
      }
      first = false;
      if (!fContinuous) {
         wait = true;
      }

      interpreter = false;
      while (gROME->ss_kbhit() || gROME->HasUserEvent()) {
         hit = true;
         char ch = gROME->ss_getchar(0);
         if (ch == -1) {
            ch = getchar();
         }
         if (ch == 'q' || ch == 'Q' || gROME->IsUserEventQ()) {
            if (IsTerminal()) {
               ROMEPrint::Print("\r                                                                      \r");
               ROMEPrint::Print("Do you really want to quit [y/n] ?");
               ch = 0;
               while (ch==0) {
                  while (gROME->ss_kbhit()) {
                     ch = gROME->ss_getchar(0);
                  }
               }
               ROMEPrint::Print("\r                                  \r");
            }
            if (ch == 'y' || ch == 'Y') {
               return false;
            }
         }
         if (ch == 'e' || ch == 'E' || gROME->IsUserEventE()) {
            this->SetTerminate();
            wait = false;
         }
         if (ch == 's' || ch == 'S' || gROME->IsUserEventS()) {
            ROMEPrint::Print("Stopped after event "R_LLD"                   \r", gROME->GetCurrentEventNumber());
//            if (gROME->IsStandAloneARGUS() || gROME->IsROMEAndARGUS() || gROME->IsROMEMonitor()) {
//               fUpdateWindowLastEvent = gROME->GetCurrentEventNumber();
//               gROME->GetWindow()->TriggerEventHandler();
//            }
            wait = true;
         }
         if (ch == 'o' || ch == 'O' || gROME->IsUserEventO()) {
            if (fContinuous) {
               ROMEPrint::Print("Step by step mode                 \n");
               if (!wait) {
                  ROMEPrint::Print("Stopped after event "R_LLD"                   \r", gROME->GetCurrentEventNumber());
               }
               fContinuous = false;
               wait = true;
               fUpdateWindow = false;
               if (gROME->IsStandAloneARGUS() || gROME->IsROMEAndARGUS() || gROME->IsROMEMonitor()) {
                  fUpdateWindowLastEvent = gROME->GetCurrentEventNumber();
                  gROME->GetWindow()->TriggerEventHandler();
               }
            }
         }
         if (ch == 'c' || ch == 'C' || gROME->IsUserEventC()) {
            if (!fContinuous) {
               ROMEPrint::Print("Continues mode                    \n");
               fContinuous = true;
               wait = false;
               fUpdateWindow = true;
               fProgressDelta = 1000;
               fProgressTimeOfLastEvent = static_cast<ULong_t>(gSystem->Now());
               fProgressLastEvent = static_cast<Long64_t>(gROME->GetTriggerStatistics()->processedEvents + 0.5);
            }
         }
         if (ch == 'r' || ch == 'R' || gROME->IsUserEventR()) {
            if (fContinuous) {
               ROMEPrint::Print("                                  \r");
               fProgressDelta = 1000;
               fProgressTimeOfLastEvent = static_cast<ULong_t>(gSystem->Now());
               fProgressLastEvent = static_cast<Long64_t>(gROME->GetTriggerStatistics()->processedEvents + 0.5);
            } else {
               if ((gROME->IsStandAloneARGUS() || gROME->IsROMEAndARGUS() || gROME->IsROMEMonitor())) {
                  gROME->GetWindow()->RequestEventHandling();
               }
            }

            wait = false;
         }
         if (ch == 'j' || ch == 'J' || gROME->IsUserEventJ()) {
            if (gROME->IsUserEventJ()) {
               GotoEvent(gROME->GetUserEventJEventNumber());
               wait = false;
            } else {
               char *cstop;
               ROMEString number;
               // event number
               ROMEPrint::Print("                                  \r");
               ROMEPrint::Print("Event number :");
               while (true) {
                  ch = gROME->ss_getchar(0);
                  if (ch == 0) {
                     continue;
                  }
                  if (ch == 13) {
                     break;
                  }
                  ROMEPrint::Print("%c", ch);
                  number += ch;
               }
               ROMEPrint::Print("\r                                                                      \r");
               inumber = strtol(number.Data(), &cstop, 10);
               GotoEvent(inumber);
               wait = false;
            }
            ch = 'j';
         }
         if (ch == 'g' || ch == 'G' || gROME->IsUserEventG()) {
            if (gROME->IsUserEventG()) {
               fStopAtRun = gROME->GetUserEventGRunNumber();
               fStopAtEvent = gROME->GetUserEventGEventNumber();
            } else {
               char *cstop;
               ROMEString number;
               // run number
               ROMEPrint::Print("\r                                                                      \r");
               ROMEPrint::Print("Run number :");
               while (true) {
                  ch = gROME->ss_getchar(0);
                  if (ch == 0) {
                     continue;
                  }
                  if (ch == 13) {
                     break;
                  }
                  ROMEPrint::Print("%c", ch);
                  number += ch;
               }
               ROMEPrint::Print("\r                                                                      \r");
               inumber = strtol(number.Data(), &cstop, 10);
               if (inumber!=0) {
                  fStopAtRun = inumber;
               } else {
                  fStopAtRun = gROME->GetCurrentRunNumber();
               }
               // event number
               number.Resize(0);
               ROMEPrint::Print("\r                                                                      \r");
               ROMEPrint::Print("Event number :");
               while (true) {
                  ch = gROME->ss_getchar(0);
                  if (ch == 0) {
                     continue;
                  }
                  if (ch == 13) {
                     break;
                  }
                  ROMEPrint::Print("%c", ch);
                  number += ch;
               }
               ROMEPrint::Print("\r                                                                      \r");
               inumber = strtol(number.Data(), &cstop, 10);
               if (inumber!=0) {
                  fStopAtEvent = inumber;
               } else {
                  fStopAtEvent = -1;
               }
            }
            wait = false;
         }
         if (ch == 'i' || ch == 'I' || gROME->IsUserEventI()) {
            interpreter = true;
         }
         gROME->DeleteUserEvent();
      }
      if (interpreter) {
         ROMEPrint::Print("\nStart root session at the end of event number "R_LLD" of run number "R_LLD"\n",
                          gROME->GetCurrentEventNumber(), gROME->GetCurrentRunNumber());
         ROMEString prompt = gROME->GetProgramName();
         prompt.ToLower();
         prompt += " [%d] ";
         gROME->GetApplication()->SetPrompt(prompt.Data());
         gROME->GetApplication()->Run(true);
         ROMEPrint::PrintAlways("\n");
#if defined( R__VISUAL_CPLUSPLUS )
         gSystem->Init();
#endif
         gROME->GetApplication()->ProcessLine(gROME->GetCintInitialisation());
         ROMEPrint::Print("\r                                                                                \r");
      }

      if (wait) {
         StoreEvent(true);
         gROME->GetApplication()->DisableFPETrap();
         gSystem->ProcessEvents();
         gROME->GetApplication()->EnableFPETrap();
         gSystem->Sleep(10);
#if defined( HAVE_MIDAS )
         if (gROME->isOnline() && gROME->IsActiveDAQ("midas")) {
            // not to stop midas server
            static_cast<ROMEMidasDAQ*>(gROME->GetActiveDAQ())->DummyRead();
         }
#endif
      }

      if (gROME->IsWindowClosed()) {
         if (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()) {
            return false;
         }
         this->SetTerminate();
         break;
      }
   }
   if (hit) {
      fProgressTimeOfLastEvent = static_cast<ULong_t>(gSystem->Now());
   }

   return true;
}

//______________________________________________________________________________
Bool_t ROMEEventLoop::WriteHistograms(Bool_t snapShot)
{
   // Write Histos
   ROMEString filename;
   ROMEString runNumberString;
   Int_t iTask;
   ROMETask *task;
   TDirectory *taskHistoDirectory;
   Int_t histDirNumber;

   if (gROME->IsHistosWrite() && !gROME->IsHistosDeactivateAll()) {
      gROME->GetCurrentRunNumberString(runNumberString);
      if (!snapShot) {
         filename.SetFormatted("%s%s", gROME->GetHistosOutputPath(), gROME->GetHistosOutputFileName());
      } else {
         filename.SetFormatted("%s%s", gROME->GetHistosOutputPath(), gROME->GetHistosSnapShotFileName());
      }
      fHistoFile = CreateTFile(filename.Data(), gROME->GetOutputFileOption());
      ROMEString histoDirectoryName;

      ROMEString histDirName = "histos";
      histDirNumber = 1;

      if (fHistoFile && !fHistoFile->IsZombie()) {
         fHistoFile->cd();

         while (fHistoFile->Get(histDirName.Data())) {
            histDirName.SetFormatted("histos-%d", histDirNumber++);
         }
#if (ROOT_VERSION_CODE < ROOT_VERSION(5,15,2))
         TDirectory *directory = new TDirectory(histDirName.Data(), "Histogram Directory");
#else
         TDirectory *directory = new TDirectoryFile(histDirName.Data(), "Histogram Directory");
#endif
         TObjArray *directories = new TObjArray();
         gROME->ConstructHistoDirectories(directory,directories);
         for (iTask = 0; iTask < gROME->GetTaskObjectEntries(); iTask++) {
            task = gROME->GetTaskObjectAt(iTask);
            taskHistoDirectory = ((TDirectory*)directories->At(iTask));
            if (task->IsActive() && taskHistoDirectory) {
               task->CopyHistosAndGraphs(taskHistoDirectory);
            }
         }
         fHistoFile->Write();
         fHistoFile->Close();
         SafeDelete(fHistoFile);
         SafeDelete(directories);
         gROOT->cd();
         delete [] directories;
      }
   }
   return kTRUE;
}

//______________________________________________________________________________
void ROMEEventLoop::AutoSave()
{
   // Trees auto save and flush
   Bool_t saveTrees = kFALSE;
   const Int_t nTree = gROME->GetTreeObjectEntries();
   Int_t iTree;
   for (iTree = 0; iTree < nTree; iTree++) {
      if (gROME->GetTreeObjectAt(iTree)->CheckAutoSave()) {
         saveTrees = kTRUE;
         break;
      }
   }
   if (saveTrees) {
      for (iTree = 0; iTree < nTree; iTree++) {
         gROME->GetTreeObjectAt(iTree)->AutoSave("SaveSelf");
      }
   }

#if (ROOT_VERSION_CODE >= ROOT_VERSION(5,26,0))
   // check if size of TTree does not exceed a limit.
   TTree *tree;
   Int_t nEntries;
   for (iTree = 0; iTree < nTree; iTree++) {
      if (gROME->GetTreeObjectAt(iTree)->isFill() &&
          (tree = gROME->GetTreeObjectAt(iTree)->GetTree()) &&
          tree->GetAutoFlush() < 0 && // OptimizeBaskets not called yet
          (nEntries = tree->GetEntries()) &&
          tree->GetTotBytes() * (1 + 1. / nEntries) >= gROME->GetMaxTreeMemory()) {
         // force flush next Fill()
         // ROME does not call OptimizeBaskets
         // because I can not update fFlushedBytes. 
         // So the actual size can be larger than limit by one event amount.
         gROME->GetTreeObjectAt(iTree)->SetAutoFlushSize(-gROME->GetMaxTreeMemory());
      }
   }
#endif

   // Histograms auto save
   static ULong_t histoSaveLastTime = static_cast<ULong_t>(gSystem->Now());
   ULong_t currentTime = static_cast<ULong_t>(gSystem->Now());
   if (gROME->GetHistosAutoSavePeriod() > 0 &&
       currentTime > histoSaveLastTime + gROME->GetHistosAutoSavePeriod() * 1000 &&
       !strcmp(gROME->GetOutputFileOption(), "RECREATE")) {
      histoSaveLastTime = currentTime;
      WriteHistograms();
   }
}

//______________________________________________________________________________
Bool_t ROMEEventLoop::DAQEndOfRun()
{
   // Disconnects the current run. Called after the EndOfRun tasks.
   // Write non accumulative output tree files
   ROMEPrint::Debug("Executing DAQ EndOfRun\n");
   gROME->SetCurrentEventNumber(kEventNumberEndOfRun);

   // Write histograms
   if (!gROME->IsHistosAccumulateAll()) {
      WriteHistograms();
   }

   // Write trees
   ROMEString treename;
   ROMETree *romeTree;
   TTree *tree;
   const Int_t nTree = gROME->GetTreeObjectEntries();
   Int_t k;
   Bool_t identicalFileNameFound;
   for (int j = 0; j < nTree; j++) {
      identicalFileNameFound = kFALSE;
      romeTree = gROME->GetTreeObjectAt(j);
      if (romeTree->isFill()) {
         tree = romeTree->GetTree();
         if (romeTree->isWrite() && !gROME->isTreeAccumulation()) {
            if (fTreeUpdateIndex > 0) {
               ROMEPrint::Print("Updating Root-File ");
            } else {
               ROMEPrint::Print("Writing Root-File ");
            }
            romeTree->UpdateFilePointer();
            ROMEPrint::Print("%s\n", romeTree->GetFile()->GetName());
            romeTree->GetFile()->cd();
            if (romeTree->isSaveConfig()) {
               gROME->SaveConfigParametersFolder();
            }
#if (ROOT_VERSION_CODE >= ROOT_VERSION(5,26,0))
            // save absolute auto flush size, instead of # of entries.
            if (romeTree->GetAutoFlushSize() < 0) {
               tree->SetAutoFlush(romeTree->GetAutoFlushSize());
            }
#endif
            if (tree->Write(0, gROME->GetOutputObjOption()) == 0) {
               ROMEPrint::Warning("--> Please check if you have write access to the directory.\n");
               ROMEPrint::Warning("--> If you have activated the read flag for this tree you must\n");
               ROMEPrint::Warning("    have different input and output directories.\n");
               ROMEPrint::Warning("    (i.e. trees for reading and writing must be different.)\n");
            }
            tree->SetDirectory(0);
            for(k = nTree - 1; k > j; k--) {
               if (gROME->GetTreeObjectAt(k)->GetFileName() == gROME->GetTreeObjectAt(j)->GetFileName()) {
                  identicalFileNameFound = kTRUE;
                  break;
               }
            }
            if (!identicalFileNameFound) { // file can be closed here
               romeTree->GetFile()->Close();
               romeTree->DeleteFile();
            }
            gROOT->cd();
         }
         if (!gROME->isTreeAccumulation()) {
            tree->Reset();
         }
      }
   }
   ROMEPrint::Print("\n");

   if (!gROME->GetActiveDAQ()->EndOfRunDAQ()) {
      return false;
   }

   return true;
}

//______________________________________________________________________________
Bool_t ROMEEventLoop::DAQTerminate()
{
   // Clean up the analyzer. Called after the Terminate tasks.
   // Write accumulative output tree files
   // Close all files
   ROMEPrint::Debug("Executing DAQ Terminate\n");
   gROME->SetCurrentEventNumber(kEventNumberTerminate);

   // Write histograms
   if (gROME->IsHistosAccumulateAll()) {
      WriteHistograms();
   }

   // Write trees
   ROMEString filename;
   ROMETree *romeTree;
   TTree *tree;
   const Int_t nTree = gROME->GetTreeObjectEntries();
   Int_t k;
   Bool_t identicalFileNameFound;
   for (int j = 0; j < nTree; j++) {
      identicalFileNameFound = kFALSE;
      romeTree = gROME->GetTreeObjectAt(j);
      if (romeTree->isWrite() && romeTree->isFill()) {
         if (gROME->isTreeAccumulation()) {
            romeTree->UpdateFilePointer();
            romeTree->GetFile()->cd();
            tree = romeTree->GetTree();
            ROMEPrint::Print("\nWriting Root-File %s\n", romeTree->GetFileName().Data());
#if (ROOT_VERSION_CODE >= ROOT_VERSION(5,26,0))
            // save absolute auto flush size, instead of # of entries.
            if (romeTree->GetAutoFlushSize() < 0) {
               tree->SetAutoFlush(romeTree->GetAutoFlushSize());
            }
#endif
            if (tree->Write(0, gROME->GetOutputObjOption()) == 0) {
               ROMEPrint::Warning("--> Please check if you have write access to the directory.\n");
               ROMEPrint::Warning("--> If you have activated the read flag for this tree you must\n");
               ROMEPrint::Warning("    have different input and output directories.\n");
            }
            if (romeTree->isSaveConfig()) {
               gROME->SaveConfigParametersFolder();
            }
            for(k = nTree - 1; k > j; k--) {
               if (gROME->GetTreeObjectAt(k)->GetFileName() == gROME->GetTreeObjectAt(j)->GetFileName()) {
                  identicalFileNameFound = kTRUE;
                  break;
               }
            }
            if (!identicalFileNameFound) { // file can be closed here
               if (romeTree->GetFile() != 0) {
                  romeTree->GetFile()->Close();
                  romeTree->DeleteFile();
               }
            }
            gROOT->cd();
         }
      }
   }

   if (!gROME->GetActiveDAQ()->TerminateDAQ()) {
      return false;
   }

   return true;
}

//______________________________________________________________________________
void ROMEEventLoop::NextEvent()
{
   RunEvent();
   ROMEPrint::Print("Executed Event "R_LLD"                                                      \n", gROME->GetCurrentEventNumber());
}

//______________________________________________________________________________
void ROMEEventLoop::GotoEvent(Long64_t eventNumber)
{
   Long64_t oldEventNumber = gROME->GetCurrentEventNumber();
   fCurrentEvent = gROME->GetActiveDAQ()->Seek(eventNumber);

   fStop = true;
   fContinuous = false;

   if (fCurrentEvent != -1) {
      ROMEPrint::Print("Stepped to Event "R_LLD"                                                    \n", fCurrentEvent);
   } else {
      fCurrentEvent = oldEventNumber;
      ROMEPrint::Print("Failed to step                                                           \n");
   }
}

void ROMEEventLoop::ReadHistograms()
{
   Int_t       ii,i,j,k;
   ROMEString  filename;
   ROMEString  name;
   ROMETask   *task;
   ROMEHisto  *histoPar;
   ROMEGraph  *graphPar;
   ROMEString  histoRuns;
   TArrayL64   runNumbers;
   TFile      *file;
   TObject    *tempObj;

   histoRuns = gROME->GetHistosRun();
   gROME->DecodeNumbers(histoRuns, runNumbers);
   ROMEString histDirName;
   Int_t histDirNumber;
   TDirectory *histDirTmp;
   TDirectory *histDir = 0;
   Bool_t firstWarning;

   for (ii = 0; ii < runNumbers.GetSize(); ii++) {
      filename.SetFormatted("%s%s", gROME->GetHistosInputPath(), gROME->GetHistosInputFileName(runNumbers.At(ii)));
      gROME->ReplaceWithRunAndEventNumber(filename);
      file = new TFile(filename.Data(), "READ");
      if (file->IsZombie()) {
          ROMEPrint::Warning("Histograms of run "R_LLD" not available.\n", runNumbers.At(ii));
          ROMEPrint::Warning("Please check the run number and the input path.\n\n");
          ROMEPrint::Warning("No Histogram loaded!\n\n");
          return;
      }

      histDirName = "histos";
      histDirNumber = 1;
      histDirNumber = 0;
      while ((histDirTmp = static_cast<TDirectory*>(file->FindObjectAny(histDirName.Data())))) {
         histDir = histDirTmp;
         histDirName.SetFormatted("histos-%d", histDirNumber++);
      }

      for (i = 0; i < gROME->GetTaskObjectEntries(); i++) {
         task = gROME->GetTaskObjectAt(i);
         if (task->IsActive()) {
            for (j = 0; j < task->GetNumberOfHistos(); j++) {
               histoPar = task->GetHistoParameterAt(j);
               if (histoPar->IsActive() && !gROME->IsHistosDeactivateAll() &&
                   (histoPar->IsAccumulate() || gROME->IsHistosAccumulateAll())) {
                  firstWarning = kTRUE;
                  for (k = 0; k < histoPar->GetArraySize(); k++) {
                     name.SetFormatted("%s%s", task->GetHistoNameAt(j)->Data(), task->GetTaskSuffix()->Data());
                     if (histoPar->GetArraySize() > 1) {
                        name.AppendFormatted("_%0*d", 3, k + histoPar->GetArrayStartIndex());
                     }
                     tempObj = static_cast<TObject*>(histDir->FindObjectAny(name.Data()));
                     if (tempObj == 0) {
                        if (firstWarning) {
                           ROMEPrint::Warning("Histogram '%s' not available in run "R_LLD"!\n",
                                              task->GetHistoNameAt(j)->Data(), runNumbers.At(ii));
                           firstWarning = kFALSE;
                        }
                     } else {
                        if (ii == 0) {
                           if (histoPar->GetArraySize() > 1) {
                              tempObj->Copy(*static_cast<TObjArray*>(task->GetHistoAt(j))->At(k));
                           } else {
                              tempObj->Copy(*task->GetHistoAt(j));
                           }
                        } else {
                           if (histoPar->GetArraySize() > 1) {
                              static_cast<TH1*>(static_cast<TObjArray*>(task->GetHistoAt(j))->At(k))->Add(static_cast<TH1*>(tempObj));
                           } else {
                              static_cast<TH1*>(task->GetHistoAt(j))->Add(static_cast<TH1*>(tempObj));
                           }
                        }
                     }
                  }
               }
            }
            for (j = 0; j < task->GetNumberOfGraphs(); j++) {
               graphPar = task->GetGraphParameterAt(j);
               if (graphPar->IsActive() && !gROME->IsHistosDeactivateAll()
                   && (graphPar->IsAccumulate() || gROME->IsHistosAccumulateAll())) {
                  firstWarning = kTRUE;
                  for (k = 0; k < graphPar->GetArraySize(); k++) {
                     name.SetFormatted("%s%s", task->GetGraphNameAt(j)->Data(), task->GetTaskSuffix()->Data());
                     if (graphPar->GetArraySize() > 1) {
                        name.AppendFormatted("_%0*d", 3, k + graphPar->GetArrayStartIndex());
                     }
                     tempObj = static_cast<TObject*>(histDir->FindObjectAny(name.Data()));
                     if (tempObj == 0) {
                        if (firstWarning) {
                           ROMEPrint::Warning("Graphgram '%s' not available in run "R_LLD"!\n",
                                              task->GetGraphNameAt(j)->Data(), runNumbers.At(ii));
                           firstWarning = kFALSE;
                        }
                     } else {
                        if (ii == 0) {
                           if (graphPar->GetArraySize() > 1) {
                              tempObj->Copy(*static_cast<TObjArray*>(task->GetGraphAt(j))->At(k));
                           } else {
                              tempObj->Copy(*task->GetGraphAt(j));
                           }
                        } else {
                           if (graphPar->GetArraySize() > 1) {
                              if (static_cast<TObjArray*>(task->GetGraphAt(j))->At(k)->InheritsFrom("ROMETGraph")) {
                                 static_cast<ROMETGraph*>(static_cast<TObjArray*>(task->GetGraphAt(j))->At(k))->Add(tempObj);
                              } else if (static_cast<TObjArray*>(task->GetGraphAt(j))->At(k)->InheritsFrom("ROMETGraphErrors")) {
                                 static_cast<ROMETGraphErrors*>(static_cast<TObjArray*>(task->GetGraphAt(j))->At(k))->Add(tempObj);
                              }
                           } else {
                              if (static_cast<TObjArray*>(task->GetGraphAt(j))->At(k)->InheritsFrom("ROMETGraph")) {
                                 static_cast<ROMETGraph*>(task->GetGraphAt(j))->Add(tempObj);
                              } else if (static_cast<TObjArray*>(task->GetGraphAt(j))->At(k)->InheritsFrom("ROMETGraphErrors")) {
                                 static_cast<ROMETGraphErrors*>(task->GetGraphAt(j))->Add(tempObj);
                              }
                           }
                        }
                     }
                  }
               }
            }
         }
      }
   }
}

//______________________________________________________________________________
TFile* ROMEEventLoop::CreateTFile(const char *fname, Option_t *option, const char *ftitle, Int_t compress)
{
   Int_t num = 1;
   Int_t extPos ,pos;

   ROMEString filename = fname;
   ROMEString opt = option;

   if (opt.Contains("NUMBERED")) {
      ROMEString filenameOrg = filename;

      // Find extension
      extPos = 0;
      pos = filenameOrg.Length() - 1;
      while (pos > 0) {
         if (filenameOrg[pos] == '.') {
            extPos = pos;
            break;
         }
         if (filenameOrg[pos] == '/'
#if defined( R__VISUAL_CPLUSPLUS )
             || filenameOrg[pos] == '\\'
#endif
             ) {
            extPos = -1;
            break;
         }
         pos--;
      }

      // Try to find a suitable file name that does not already exist.
      while (!gSystem->AccessPathName(filename.Data())) {
         if (extPos > 0) {
            ROMEString tmp1 = filenameOrg(0, extPos);
            ROMEString tmp2 = filenameOrg(extPos + 1, filenameOrg.Length());
            filename.SetFormatted("%s-%d.%s", tmp1.Data() , num, tmp2.Data());
         } else {
            filename.SetFormatted("%s-%d", filenameOrg.Data(), num);
         }
         num++;
      }
      opt = "RECREATE";
   }

   return new TFile(filename.Data(), opt.Data(), ftitle, compress);
}

//______________________________________________________________________________
Bool_t ROMEEventLoop::TimeEventLoop()
{
   int i;
   ROMEPrint::Print(" WriteEvent");
   for (i=0; i < 35 - 11; i++) {
      ROMEPrint::Print(".");
   }
   ROMEString str1, str2;
   ROMEPrint::Print(" : %s  %s",
                    fWatchWriteEvent.GetRealTimeString(str1),
                    fWatchWriteEvent.GetCpuTimeString(str2));
//   ROMEPrint::Print("  %s\n", fWatchEvent.GetCpuTimeString(str1));
   return true;
}
