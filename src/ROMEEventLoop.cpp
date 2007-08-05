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
#include <TROOT.h>
#include <TSystem.h>
#include <TThread.h>
#include <TFolder.h>
#include <TTime.h>
#include <TVirtualMutex.h>
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
#include "ROMERint.h"
#include "ROMEConfig.h"
#include "ArgusWindow.h"
#include "ArgusAnalyzerController.h"
#include "ROMENetFolderServer.h"

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

static TVirtualMutex *fgObjectStorageMutex = 0;

//______________________________________________________________________________
ROMEEventLoop::ROMEEventLoop(const char *name, const char *title)
:ROMETask(name, title, 0, 0, 0, kFALSE, kFALSE, 0)
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
,fFileIOWatch()
,fUpdateWindow(kFALSE)
,fLastUpdateTime(0)
,fWindowFirstDraw(kFALSE)
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
   fWatchUser.Reset();
   fWatchAll.Reset();
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
            eventLoopIndex++;
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

            fHaveBeginOfRunMacro = (fBeginOfRunMacro.Length() > 0);
            fHaveBeginOfEventMacro =  (fBeginOfEventMacro.Length() > 0);
            fHaveEndOfEventMacro =  (fEndOfEventMacro.Length() > 0);
            fHaveEndOfRunMacro =  (fEndOfRunMacro.Length() > 0);

            // Begin of Run Tasks
            ReadRunHeaders();
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

            // Output
            if (gROME->IsShowRunStat()) {
#if defined( R__VISUAL_CPLUSPLUS )
               ROMEPrint::Print("\n\nRun %I64d started\n", gROME->GetCurrentRunNumber());
#else
               ROMEPrint::Print("\n\nRun %lld started\n", gROME->GetCurrentRunNumber());
#endif
            }
         }
      }

      // Start ARGUS
      //-------------
      if (ii==0 && (gROME->IsStandAloneARGUS() || gROME->IsROMEAndARGUS() || gROME->IsROMEMonitor())) {
         if (IsTerminal()) {
            ROMEPrint::Print("Starting argus monitor ...                            \r");
         }
         gROME->StartWindow();
         ROMEPrint::Print("Argus monitor running                                 \n");
         fWindowFirstDraw = kTRUE;
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

      if ((this->isEndOfRun() || this->isTerminate()) && !gROME->IsROMEMonitor()) {
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
#if defined( R__VISUAL_CPLUSPLUS )
            ROMEPrint::Print("Run %I64d stopped                                             \n", gROME->GetCurrentRunNumber());
            ROMEPrint::Print("%I64d events processed\n\n", static_cast<Long64_t>(gROME->GetProcessedEvents()+0.5));
#else
            ROMEPrint::Print("Run %lld stopped                                             \n", gROME->GetCurrentRunNumber());
            ROMEPrint::Print("%lld events processed\n\n", static_cast<Long64_t>(gROME->GetProcessedEvents()+0.5));
#endif
         }

         // End of Run Tasks
         if (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()) {
            ROMEPrint::Debug("Executing EndOfRun tasks\n");
            ExecuteTasks("EndOfRun");
            CleanTasks();
            if (fHaveEndOfRunMacro) {
               gROME->GetApplication()->ProcessFile(fEndOfRunMacro.Data());
            }
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

   if (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()) {
      fWatchAll.Stop();
      if (gROME->IsShowRunStat()) {
         ROMEString str;
         ROMEPrint::Print("run times :                           All      User Methods   User Events\n");
         ROMEPrint::Print("-----------                      ------------  ------------  ------------\n");
         Exec("Time");
         gROME->GetActiveDAQ()->TimeDAQ();
         ExecuteTasks("Time");
         CleanTasks();
         if (gROME->IsStandAloneARGUS() || gROME->IsROMEAndARGUS() || gROME->IsROMEMonitor()) {
            ROMEPrint::Print(gROME->GetWindow()->GetTimeStatisticsString(str));
         }
         ROMEPrint::Print("\n");
      }
      ExecuteTasks("PrintSkipped");
      CleanTasks();
   }
   if (gROME->IsStandAloneARGUS() || gROME->IsROMEAndARGUS() || gROME->IsROMEMonitor()) {
      fUpdateWindow = false;
//      gROME->GetWindow()->TriggerEventHandler();
   }

   // Terminate
   if (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()) {
      if (!this->DAQTerminate()) {
         gROME->SetTerminationFlag();
         ROMEPrint::Print("\n\nTerminating Program !\n");
         return;
      }
      ROMEPrint::Debug("Executing Terminate tasks\n");
      ExecuteTasks("Terminate");
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
   if (!this->isContinue() || this->isStopped() || gROME->IsStandAloneARGUS() || gROME->IsROMEMonitor()) {
      ROMEPrint::Debug("ROMEEventLoop::RunEvent() : Update\n");
      if (fCurrentEvent>0) {
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
   if (gROME->isOffline() && !gROME->IsROMEMonitor()) {
      int status = gROME->CheckEventNumber(fCurrentEvent);
      if (status==0) {
         return kContinue;
      } else if (status==-1) {
         this->SetStopped();
         this->SetEndOfRun();
         return kBreak;
      }
   }

   // User Input
   if (!this->isContinue() && !gROME->IsROMEMonitor()) {
      ROMEPrint::Debug("ROMEEventLoop::RunEvent() : UserInput\n");
      if (!gROME->IsStandAloneARGUS() || gROME->IsROMEMonitor()) {
         if (!fFirstUserInput && fCurrentEvent > 0) {
            if (!this->UserInput()) {
               gROME->SetTerminationFlag();
               if (gROME->IsStandAloneARGUS() || gROME->IsROMEAndARGUS() || gROME->IsROMEMonitor()) {
                  fUpdateWindow = false;
                  fUpdateWindowLastEvent = gROME->GetCurrentEventNumber();
                  gROME->GetWindow()->TriggerEventHandler();
               }
               // Terminate
               if (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()) {
                  if (!this->DAQTerminate()) {
                     gROME->SetTerminationFlag();
                     ROMEPrint::Print("\n\nTerminating Program !\n");
                     return false;
                  }
                  ROMEPrint::Debug("Executing Terminate tasks\n");
                  ExecuteTasks("Terminate");
                  CleanTasks();
               }
               return kReturn;
            }
            if (this->isTerminate()) {
               return kBreak;
            }
         }
         fFirstUserInput = false;
      }
   }

   // Set Fill Event equal true
   if (!this->isContinue() && !gROME->IsROMEMonitor()) {
      gROME->SetFillEvent();
   }

   // Read Event
   ROMEPrint::Debug("ROMEEventLoop::RunEvent() : DAQEvent\n");
   if (!gROME->IsROMEMonitor()) {
      if (!this->DAQEvent()) {
         this->Terminate();
         gROME->SetTerminationFlag();
         ROMEPrint::Print("\n\nTerminating Program !\n");
         return kReturn;
      }
      if (fHaveBeginOfEventMacro)
         gROME->GetApplication()->ProcessFile(fBeginOfEventMacro.Data());
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
   if (!gROME->IsROMEMonitor()) {
      if (ROMEEventLoop::fHotLinksChanged) {
         this->UpdateHotLinks();
         ROMEEventLoop::fHotLinksChanged = false;
      }
   }
#endif // HAVE_MIDAS

   // Event Tasks
   if (!gROME->IsROMEMonitor()) {
      ROMEPrint::Debug("ROMEEventLoop::RunEvent() : ExecuteTasks\n");
      if (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()) {
         text.SetFormatted("Event%d", gROME->GetEventID());
         ROMEPrint::Debug("Executing Event tasks (option = '%s')\n", text.Data());
         ExecuteTasks(text.Data());
         CleanTasks();
         if (fHaveEndOfEventMacro) {
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
   if (!gROME->IsROMEMonitor()) {
      ROMEPrint::Debug("ROMEEventLoop::RunEvent() : WriteEvent\n");
      if (gROME->isFillEvent()) {
         if (!this->WriteEvent()) {
            this->Terminate();
            gROME->SetTerminationFlag();
            ROMEPrint::Print("\n\nTerminating Program !\n");
            return kReturn;
         }
      }
   }
   if (fWindowFirstDraw && (gROME->IsStandAloneARGUS() || gROME->IsROMEAndARGUS() || gROME->IsROMEMonitor())) {
      fUpdateWindowLastEvent = gROME->GetCurrentEventNumber();
      gROME->GetWindow()->TriggerEventHandler();
      fWindowFirstDraw = kFALSE;
   }

   return kContinue;
}

//______________________________________________________________________________
Bool_t ROMEEventLoop::StoreEvent(Bool_t useThread)
{
   if (!gROME->IsROMEMonitor() && gROME->GetNetFolderServer() && !gROME->IsObjectStorageUpdated()) {
      const ULong_t kInterval = 10; // this should be changed to parameter
      if (static_cast<ULong_t>(gSystem->Now()) > fLastNetFolderServerUpdateTime + kInterval) {
         ROME_LOCKGUARD(fgObjectStorageMutex);
         fLastNetFolderServerUpdateTime = static_cast<ULong_t>(gSystem->Now());
         gROME->FillObjectStorage();
         if (fNetFolderServerUpdateThread) {
            TThread::Delete(fNetFolderServerUpdateThread);
            fNetFolderServerUpdateThread = 0;
         }
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
   gROME->SetCurrentEventNumber(0);

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

   // Init Data Base
   if (!gROME->ReadSingleDataBaseFolders()) {
      ROMEPrint::Error("\nError while reading the data base !\n");
      return false;
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
               file = new TFile(filename.Data(), "RECREATE");
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
   Statistics *stat = gROME->GetTriggerStatistics();
   stat->processedEvents = 0;
   stat->eventsPerSecond = 0;
   stat->writtenEvents = 0;
   fStatisticsTimeOfLastEvent = 0;
   fProgressTimeOfLastEvent = 0;
   fLastUpdateTime = 0;
   fStatisticsLastEvent = 0;

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

   // Set alwasy fill flag
   fAlwaysFillTrees = gROME->GetActiveDAQ()->GetAlwaysFillTrees();

   // Check Configuration
   if (gROME->isOffline() && gROME->IsFileNameBasedIO()) {
      // file name based IO
      gROME->GetConfiguration()->CheckConfiguration(gROME->GetCurrentInputFileName());
   }

   // Set Run Number
   fTreeInfo->SetRunNumber(gROME->GetCurrentRunNumber());

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
                  file = new TFile(filename.Data(), "RECREATE");
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

   Statistics *stat = gROME->GetTriggerStatistics();

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
   stat->processedEvents++;
   ULong_t time = static_cast<ULong_t>(gSystem->Now());
   if (fStatisticsTimeOfLastEvent == 0) {
      fStatisticsTimeOfLastEvent = time;
   }
   if (time - fStatisticsTimeOfLastEvent != 0) {
      stat->eventsPerSecond = (stat->processedEvents-fStatisticsLastEvent)/(time-fStatisticsTimeOfLastEvent)*1000.0;
   }
   fStatisticsTimeOfLastEvent = time;

   fTreeInfo->SetTimeStamp(gROME->GetActiveDAQ()->GetTimeStamp());
   fStatisticsLastEvent = stat->processedEvents;

   return true;
}

//______________________________________________________________________________
Bool_t ROMEEventLoop::WriteEvent()
{
   // Writes the event. Called after the Event tasks.
   this->CleanUpFolders();
   fTreeInfo->SetEventNumber(gROME->GetCurrentEventNumber());
   this->FillTrees();
   return true;
}

//______________________________________________________________________________
Bool_t ROMEEventLoop::Update()
{
   // Update the Analyzer. Called after the Event tasks.

   ROMEString text;
   Long64_t newUpdateWindowEvent;

   // Progress Display
   ROMEPrint::Debug("ROMEEventLoop::Update() Progress Display");
   if (fProgressDelta > 1) {
      if (static_cast<Long64_t>(gROME->GetTriggerStatistics()->processedEvents + 0.5) >=
          fProgressLastEvent + fProgressDelta) {
         fProgressTimeOfLastEvent = static_cast<ULong_t>(gSystem->Now());
         fProgressLastEvent = static_cast<Long64_t>(gROME->GetTriggerStatistics()->processedEvents + 0.5);
         fProgressWrite = true;
      } else {
         if (static_cast<ULong_t>(gSystem->Now()) > static_cast<ULong_t>(fProgressTimeOfLastEvent + 1000)) {
            fProgressDelta /= 10;
         }
      }
   }

   ROMEPrint::Debug("ROMEEventLoop::Update() TriggerEventHandler");
   if ((gROME->IsStandAloneARGUS() || gROME->IsROMEAndARGUS() || gROME->IsROMEMonitor())) {
      if (gROME->GetActiveDAQ()->isStopped() && gROME->GetWindow()->IsEventHandlingRequested()) {
         fUpdateWindowLastEvent = gROME->GetCurrentEventNumber();
         gROME->GetWindow()->TriggerEventHandler();
         gROME->GetWindow()->ClearEventHandlingRequest();
      }
   }

   ROMEPrint::Debug("ROMEEventLoop::Update() Update");
   if (!gROME->isBatchMode() &&
       (!fContinuous ||
        (fProgressDelta == 1 ||
         !(static_cast<Long64_t>(gROME->GetTriggerStatistics()->processedEvents + 0.5) % fProgressDelta) &&
         fProgressWrite))) {
      if (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()) {
         if (IsTerminal()) {
#if defined( R__VISUAL_CPLUSPLUS )
            ROMEPrint::Print("processed event number %I64d                                              \r",
                             gROME->GetCurrentEventNumber());
#else
            ROMEPrint::Print("processed event number %lld                                              \r",
                             gROME->GetCurrentEventNumber());
#endif
            ROMEPrint::Debug("\n");
         }
      }
      fProgressWrite = false;
   }

   // Condition of following 'if' statements can be a problem when 'event type !=1' comes frequently.(EventHandler can be called too frequently.)
   if (!gROME->isBatchMode() && (gROME->IsStandAloneARGUS() || gROME->IsROMEAndARGUS() || gROME->IsROMEMonitor())) {
      newUpdateWindowEvent =  gROME->GetCurrentEventNumber();
      if (fUpdateWindowLastEvent!=newUpdateWindowEvent || gROME->GetWindow()->IsEventHandlingRequested() || gROME->GetEventID() != 1) {
         fUpdateWindowLastEvent = newUpdateWindowEvent;
         if ((fUpdateWindow &&
              static_cast<ULong_t>(gSystem->Now()) >
              static_cast<ULong_t>(fLastUpdateTime + gROME->GetWindowUpdateFrequency())) ||
             gROME->GetWindow()->IsEventHandlingRequested() ||
             (gROME->GetEventID() != 1 && (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()))) {
            if (gROME->GetWindow()->IsControllerActive()) {
               if(gROME->GetWindow()->GetAnalyzerController()) {
                  gROME->GetWindow()->GetAnalyzerController()->Update();
               }
            }
            if (!this->isStopped()) {
               fUpdateWindowLastEvent = gROME->GetCurrentEventNumber();
               gROME->GetWindow()->TriggerEventHandler();
            }
            fLastUpdateTime = static_cast<ULong_t>(gSystem->Now());
         }
         gROME->GetWindow()->ClearEventHandlingRequest();
      }
      gROME->GetApplication()->DisableFPETrap();
      gSystem->ProcessEvents();
      gROME->GetApplication()->EnableFPETrap();
      gSystem->Sleep(10);
   }

   // ODB update
   ROMEPrint::Debug("ROMEEventLoop::Update() ODB update");
#if defined( HAVE_MIDAS )
   db_send_changed_records();
#endif

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
      if (gROME->IsStandAloneARGUS() || gROME->IsROMEAndARGUS() || gROME->IsROMEMonitor()) {
         fUpdateWindowLastEvent = gROME->GetCurrentEventNumber();
         gROME->GetWindow()->TriggerEventHandler();
      }
   } else if ((fStopAtRun==gROME->GetCurrentRunNumber() && fStopAtEvent==gROME->GetCurrentEventNumber()) ||
              (gROME->GetCurrentEventNumber()==0 && !fContinuous)) {
#if defined( R__VISUAL_CPLUSPLUS )
      ROMEPrint::Print("Stopped after event %I64d                   \r", gROME->GetCurrentEventNumber());
#else
      ROMEPrint::Print("Stopped after event %lld                   \r", gROME->GetCurrentEventNumber());
#endif
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
            gROME->GetWindow()->ClearEventHandlingRequest();
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
#if defined( R__VISUAL_CPLUSPLUS )
            ROMEPrint::Print("Stopped after event %I64d                   \r", gROME->GetCurrentEventNumber());
#else
            ROMEPrint::Print("Stopped after event %lld                   \r", gROME->GetCurrentEventNumber());
#endif
            if (gROME->IsStandAloneARGUS() || gROME->IsROMEAndARGUS() || gROME->IsROMEMonitor()) {
               fUpdateWindowLastEvent = gROME->GetCurrentEventNumber();
               gROME->GetWindow()->TriggerEventHandler();
            }
            wait = true;
         }
         if (ch == 'o' || ch == 'O' || gROME->IsUserEventO()) {
            if (fContinuous) {
               ROMEPrint::Print("Step by step mode                 \n");
               if (!wait) {
#if defined( R__VISUAL_CPLUSPLUS )
                  ROMEPrint::Print("Stopped after event %I64d                   \r", gROME->GetCurrentEventNumber());
#else
                  ROMEPrint::Print("Stopped after event %lld                   \r", gROME->GetCurrentEventNumber());
#endif
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
               fStop = true;
               fContinuous = false;
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
               fStop = true;
               fContinuous = false;
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
#if defined( R__VISUAL_CPLUSPLUS )
         ROMEPrint::Print("\nStart root session at the end of event number %I64d of run number %I64d\n",
                          gROME->GetCurrentEventNumber(), gROME->GetCurrentRunNumber());
#else
         ROMEPrint::Print("\nStart root session at the end of event number %lld of run number %lld\n",
                          gROME->GetCurrentEventNumber(), gROME->GetCurrentRunNumber());
#endif
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
Bool_t ROMEEventLoop::DAQEndOfRun()
{
   // Disconnects the current run. Called after the EndOfRun tasks.
   // Write non accumulative output tree files
   ROMEPrint::Debug("Executing DAQ EndOfRun\n");
   gROME->SetCurrentEventNumber(kEventNumberEndOfRun);

   // Write Histos
   ROMEString filename;
   ROMEString runNumberString;
   gROME->GetCurrentRunNumberString(runNumberString);

   filename.SetFormatted("%s%s%s.root", gROME->GetOutputDir(), "histos", runNumberString.Data());
   fHistoFile = new TFile(filename.Data(), "RECREATE");
   if (fHistoFile && !fHistoFile->IsZombie()) {
      fHistoFile->cd();
      TFolder *folder = static_cast<TFolder*>(gROOT->FindObjectAny("histos"));
      folder->Write();
      fHistoFile->Write();
      fHistoFile->Close();
      SafeDelete(fHistoFile);
      gROOT->cd();
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
            if (tree->Write(0, TObject::kOverwrite) == 0) {
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
               romeTree->GetFile()->Delete();
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
            if (tree->Write(0, TObject::kOverwrite) == 0) {
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
#if defined( R__VISUAL_CPLUSPLUS )
   ROMEPrint::Print("Executed Event %I64d                                                     \n", gROME->GetCurrentEventNumber());
#else
   ROMEPrint::Print("Executed Event %lld                                                      \n", gROME->GetCurrentEventNumber());
#endif
}

//______________________________________________________________________________
void ROMEEventLoop::GotoEvent(Long64_t eventNumber)
{
   fCurrentEvent = gROME->GetActiveDAQ()->Seek(eventNumber);
   if (fCurrentEvent != -1) {
#if defined( R__VISUAL_CPLUSPLUS )
      ROMEPrint::Print("Stepped to Event %I64d                                                   \n", fCurrentEvent);
#else
      ROMEPrint::Print("Stepped to Event %lld                                                    \n", fCurrentEvent);
#endif
   } else {
      ROMEPrint::Print("Failed to step                                                           \n");
   }
}
