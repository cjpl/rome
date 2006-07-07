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
#include <TSystem.h>
#include <TThread.h>
#include <TFolder.h>
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
#include <time.h>

#include "ROMEAnalyzer.h"
#include "ROMETreeInfo.h"
#include "ROMERint.h"
#include "ROMEConfig.h"
#include "ArgusWindow.h"
#include "ArgusAnalyzerController.h"

#if defined( HAVE_MIDAS )
#   include "midas.h"
#endif

#include "ROMEEventLoop.h"
#include "ROMEiostream.h"

TTask *TTask::fgBeginTask  = 0;
TTask *TTask::fgBreakPoint = 0;

// Task switches handle initialization
bool ROMEEventLoop::fTaskSwitchesChanged = false;

ClassImp(ROMEEventLoop)

ROMEEventLoop::ROMEEventLoop(const char *name,const char *title):ROMETask(name,title,0)
{
   fTreeInfo = new ROMETreeInfo();
   fSequentialNumber = 0;
   fContinuous = true;
   fUserInputLastTime = 0;
   fTreeUpdateIndex = 0;
   fStopAtRun = -1;
   fStopAtEvent = -1;
   fSavedUpdateFrequency = -1;
   fHistoFile = 0;
}

ROMEEventLoop::~ROMEEventLoop()
{
   SafeDelete(fHistoFile);
}

void ROMEEventLoop::ExecuteTask(Option_t *option)
{
   fFirstUserInput = true;
   if (!strcmp(option,"init")) {
      this->InitTrees();
      return;
   }

// Event loop
   fWatchEvent.Reset();
   fWatchAll.Reset();
   fWatchAll.Start(false);
   if (fgBeginTask) {
      Error("ExecuteTask","Cannot execute task:%s, already running task: %s",GetName(),fgBeginTask->GetName());
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

   // Declarations
   //--------------
   Long64_t ii,eventLoopIndex;

   // Initialisation
   //----------------

   if (!this->DAQInit()) {
      this->Terminate();
      gROME->SetTerminationFlag();
      ROMEPrint::Print("\n\nTerminating Program !\n");
      return;
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
   for (ii=0;!this->isTerminate();ii++) {
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
         // Task Switches
         if (ROMEEventLoop::fTaskSwitchesChanged) {
            this->UpdateTaskSwitches();
            ROMEEventLoop::fTaskSwitchesChanged = false;
         }

         // Begin of Run Tasks
         if (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()) {
            ROMEPrint::Debug("Executing BeginOfRun tasks\n");
            ExecuteTasks("BeginOfRun");
            CleanTasks();
         }
         eventLoopIndex++;

         // Output
         if (gROME->IsShowRunStat()) {
#if defined( R__VISUAL_CPLUSPLUS )
            ROMEPrint::Print("\n\nRun %I64d started\n",gROME->GetCurrentRunNumber());
#else
            ROMEPrint::Print("\n\nRun %lld started\n",gROME->GetCurrentRunNumber());
#endif
         }
      }

      // Start ARGUS
      //-------------
      if (ii==0 && (gROME->IsStandAloneARGUS() || gROME->IsROMEAndARGUS())) {
         ROMEPrint::Debug("Starting main window\n");
         gROME->StartWindow();
      }

      // Loop over Events
      //------------------
      fCurrentEvent = -1;
      fFirstUserInput = true;
      ROMEPrint::Debug("Entering event loop\n");
      for (;!this->isTerminate()&&!this->isEndOfRun();) {
         // set terminal in every events because it is necessary when
         // program resumes from suspend.
/*         if (gROME->isBatchMode())
            gROME->redirectOutput();
         else
            gROME->ss_getchar(0);*/
// ---> This code killes ss_kbhit() please fix it   Matthias

         int status = this->RunEvent();
         if (status==kReturn)
            return;
         if (status==kBreak)
            break;
      }

      if (this->isEndOfRun() || this->isTerminate()) {
         if (this->isEndOfRun())
            this->SetBeginOfRun();

         // Show number of processed events
         if (gROME->IsShowRunStat()) {
#if defined( R__VISUAL_CPLUSPLUS )
            ROMEPrint::Print("Run %I64d stopped                                             \n",gROME->GetCurrentRunNumber());
            ROMEPrint::Print("%I64d events processed\n\n",(Long64_t)(gROME->GetProcessedEvents()+0.5));
#else
            ROMEPrint::Print("Run %lld stopped                                             \n",gROME->GetCurrentRunNumber());
            ROMEPrint::Print("%lld events processed\n\n",(Long64_t)(gROME->GetProcessedEvents()+0.5));
#endif
         }

         // End of Run Tasks
         if (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()) {
            ROMEPrint::Debug("Executing EndOfRun tasks\n");
            ExecuteTasks("EndOfRun");
            CleanTasks();
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

   if (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()) {
      fWatchAll.Stop();
      if (gROME->IsShowRunStat()) {
         ROMEPrint::Print("run times :                      All Methods   Event Methods\n");
         ROMEPrint::Print("-----------                      ------------  -------------\n");
         gROME->GetActiveDAQ()->TimeDAQ();
         ExecuteTasks("Time");
         ROMEPrint::Print("\n");
      }
   }
   if (gROME->IsStandAloneARGUS() || gROME->IsROMEAndARGUS()) {
      gROME->GetWindow()->StopEventHandler();
   }

   // Root Interpreter
   if (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()) {
      ROMEString prompt = gROME->GetProgramName();
      prompt.ToLower();
      prompt += " [%d]";
      ((TRint*)gROME->GetApplication())->SetPrompt(prompt.Data());
      if (!gROME->isQuitMode()) {
         gROME->GetApplication()->Run(true);
         ROMEPrint::Print("\n");
      }
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
}

Int_t ROMEEventLoop::RunEvent()
{
   fCurrentEvent++;

   ROMEString text;
   // Run one Event.
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
   if (gROME->isOffline()) {
      // check event numbers
      int status = gROME->CheckEventNumber(fCurrentEvent);
      if (status==0) {
         return kContinue;
      }
      if (status==-1) {
         this->SetStopped();
         this->SetEndOfRun();
         return kBreak;
      }
   }
   // Update
   if (!this->Update()) {
      this->Terminate();
      gROME->SetTerminationFlag();
      ROMEPrint::Print("\n\nTerminating Program !\n");
      return kReturn;
   }

   // User Input
   if (!gROME->IsStandAloneARGUS()) {
      if (!fFirstUserInput) {
         if (!this->UserInput()) {
            this->DAQTerminate();
            gROME->SetTerminationFlag();
            ROMEPrint::Print("\n\nTerminating Program !\n");
            return kReturn;
         }
         if (this->isTerminate()) {
            return kBreak;
         }
      }
      fFirstUserInput = false;
   }

   // Set Fill Event equal true
   gROME->SetFillEvent();

   // Read Event
   if (!this->DAQEvent()) {
      this->Terminate();
      gROME->SetTerminationFlag();
      ROMEPrint::Print("\n\nTerminating Program !\n");
      return kReturn;
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

   // Event Tasks
   if (ROMEEventLoop::fTaskSwitchesChanged) {
      this->UpdateTaskSwitches();
      ROMEEventLoop::fTaskSwitchesChanged = false;
   }
   if (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()) {
      text.SetFormatted("Event%d",gROME->GetEventID());
      ROMEPrint::Debug("Executing Event tasks (option = '%s')\n", text.Data());
      ExecuteTasks(text.Data());
      CleanTasks();
   }
   if (gROME->isTerminationFlag()) {
      ROMEPrint::Print("\n\nTerminating Program !\n");
      return kReturn;
   }
   if (this->isEndOfRun())
      return kBreak;

   // Write Event
   if (!this->WriteEvent() && gROME->isFillEvent()) {
      this->Terminate();
      gROME->SetTerminationFlag();
      ROMEPrint::Print("\n\nTerminating Program !\n");
      return kReturn;
   }

   return kContinue;
}
Bool_t ROMEEventLoop::DAQInit()
{
   // Initialize the analyzer. Called before the init tasks.
   ROMEPrint::Debug("Executing DAQ Init\n");

   int j;
   this->SetRunning();
   this->SetAnalyze();
   gROME->SetCurrentEventNumber(0);

   this->InitTaskSwitches();
   this->InitSingleFolders();

   // Check IO System
   if (gROME->GetNumberOfRunNumbers()>0 && gROME->GetNumberOfInputFileNames()>0)
      gROME->SetIOType(gROME->kRunNumberAndFileNameBased);
   else if (gROME->GetNumberOfRunNumbers()>0)
      gROME->SetIOType(gROME->kRunNumberBased);
   else if (gROME->GetNumberOfInputFileNames()>0)
      gROME->SetIOType(gROME->kFileNameBased);
   else if (gROME->isOffline()) {
      if (gROME->isActiveDAQSet()) {
         if (strcmp(gROME->GetActiveDAQ()->GetName(),"none")) {
            ROMEPrint::Error("No run numbers or input file names specified.\n");
            return false;
         }
      }
   }

   // Initialize DAQ System
   if (!gROME->GetActiveDAQ()->InitDAQ())
      return false;

   // Open Output Files for accumulative output tree files
   ROMEString filename;
   ROMETree *romeTree;
   TTree *tree;
   const Int_t nTree = gROME->GetTreeObjectEntries();
   for (j=0;j<nTree;j++) {
      romeTree = gROME->GetTreeObjectAt(j);
      if (romeTree->isWrite() && romeTree->isFill()) {
         if (gROME->isTreeAccumulation()) {
            tree = romeTree->GetTree();
            GetTreeFileName(filename,j);
            filename.Insert(0,gROME->GetOutputDir());
            romeTree->SetFileName(filename);
            romeTree->SetFile(new TFile(filename.Data(),"RECREATE"));
         }
      }
      else {
         romeTree->SetFile(0);
         filename = "";
         romeTree->SetFileName(filename);
      }
   }
   return true;
}

Bool_t ROMEEventLoop::DAQBeginOfRun(Long64_t eventLoopIndex)
{
   // Connect the Analyzer to the current run. Called before the BeginOfRun tasks.
   ROMEPrint::Debug("Executing DAQ BeginOfRun\n");

   ROMEString runNumberString;
   // Statistics
   Statistics *stat = gROME->GetTriggerStatistics();
   stat->processedEvents = 0;
   stat->eventsPerSecond = 0;
   stat->writtenEvents = 0;
   fStatisticsTimeOfLastEvent = 0;
   fStatisticsLastEvent = 0;

   if (gROME->isOffline() && (gROME->IsRunNumberBasedIO() || gROME->IsRunNumberAndFileNameBasedIO())) {
      // run number based IO
      Long64_t runNumber = gROME->GetNextRunNumber(gROME->GetCurrentRunNumber());
      if (runNumber==-1) {
         this->SetTerminate();
         return true;
      }
      // Check Configuration
      gROME->SetCurrentRunNumber(runNumber);
      gROME->GetConfiguration()->CheckConfiguration(gROME->GetCurrentRunNumber());
   }

   // Begin Of Run Of Active DAQ
   if (!gROME->GetActiveDAQ()->BeginOfRunDAQ())
      return false;
   if (this->isEndOfRun())
      return true;


   // Check Configuration
   if (gROME->isOffline() && gROME->IsFileNameBasedIO()) {
      // file name based IO
      gROME->GetConfiguration()->CheckConfiguration(gROME->GetCurrentInputFileName());
   }

   // Set Run Number
   fTreeInfo->SetRunNumber(gROME->GetCurrentRunNumber());
   gROME->GetCurrentRunNumberString(runNumberString);

   // Reset Sequential Number
   fSequentialNumber = 0;


   // Update Data Base
   if (!gROME->ReadSingleDataBaseFolders()) {
      ROMEPrint::Error("\nError while reading the data base !\n");
      return false;
   }
   if (eventLoopIndex==0)
      this->InitArrayFolders();
   if (!gROME->ReadArrayDataBaseFolders()) {
      ROMEPrint::Error("\nError while reading the data base !\n");
      return false;
   }

   // Progress Display
   fProgressDelta = 10000;
   fProgressTimeOfLastEvent = time(NULL);
   fProgressLastEvent = 0;
   fProgressWrite = false;

   // Open Output Files
   ROMEString filename;
   ROMEString treename;
   ROMETree *romeTree;
   TTree *tree;
   gROME->GetCurrentRunNumberString(runNumberString);
   const Int_t nTree = gROME->GetTreeObjectEntries();
   for (int j=0;j<nTree;j++) {
      romeTree = gROME->GetTreeObjectAt(j);
      if (romeTree->isFill()) {
         tree = romeTree->GetTree();
         if (romeTree->isWrite() && !gROME->isTreeAccumulation()) {
            GetTreeFileName(filename,j,runNumberString.Data());
            filename.Insert(0,gROME->GetOutputDir());
            if (filename==romeTree->GetFileName()) {
               romeTree->SetFile(new TFile(filename.Data(),"UPDATE"));
               romeTree->SetFileUpdate();
               fTreeUpdateIndex++;
               treename.SetFormatted("%s_%d",tree->GetName(),fTreeUpdateIndex);
               tree->SetName(treename.Data());
            }
            else {
               romeTree->SetFile(new TFile(filename.Data(),"RECREATE"));
               romeTree->SetFileOverWrite();
               fTreeUpdateIndex = 0;
            }
            romeTree->SetFileName(filename);
         }
         else {
            romeTree->SetFile(0);
            filename = "";
            romeTree->SetFileName(filename);
         }
      }
   }
   return true;
}

Bool_t ROMEEventLoop::DAQEvent()
{
   // Reads an event. Called before the Event tasks.
   ROMEPrint::Debug("Executing DAQ Event\n");

   Statistics *stat = gROME->GetTriggerStatistics();

   gROME->SetEventFilled(false);
   this->SetAnalyze();
   this->ResetFolders();

   if (gROME->IsDontReadNextEvent()) {
      gROME->SetDontReadNextEvent(false);
      return true;
   }

   if (!gROME->GetActiveDAQ()->EventDAQ(fCurrentEvent))
      return false;
   if (this->isContinue()) {
      return true;
   }
   gROME->SetEventFilled(true);

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
   int time;
   time = gROME->ss_millitime();
   if (fStatisticsTimeOfLastEvent == 0)
      fStatisticsTimeOfLastEvent = time;
   if (time - fStatisticsTimeOfLastEvent != 0)
      stat->eventsPerSecond = (stat->processedEvents-fStatisticsLastEvent)/(time-fStatisticsTimeOfLastEvent)*1000.0;
   fStatisticsTimeOfLastEvent = time;

   fTreeInfo->SetTimeStamp(gROME->GetActiveDAQ()->GetTimeStamp());
   fStatisticsLastEvent = stat->processedEvents;

   return true;
}

Bool_t ROMEEventLoop::WriteEvent()
{
   // Writes the event. Called after the Event tasks.
   this->CleanUpFolders();
   fTreeInfo->SetEventNumber(gROME->GetCurrentEventNumber());
   this->FillTrees();
   return true;
}

Bool_t ROMEEventLoop::Update()
{
   // Update the Analyzer. Called after the Event tasks.

   ROMEString text;
   // Progress Display
   if (fProgressDelta>1) {
      if ((Long64_t)(gROME->GetTriggerStatistics()->processedEvents+0.5) >= fProgressLastEvent + fProgressDelta) {
         time(&fProgressTimeOfLastEvent);
         fProgressLastEvent = (Long64_t)(gROME->GetTriggerStatistics()->processedEvents+0.5);
         fProgressWrite = true;
      }
      else {
         if (time(NULL) > fProgressTimeOfLastEvent+1) {
            fProgressDelta /= 10;
         }
      }
   }

   if (!gROME->isBatchMode() &&
       ( !fContinuous || ((fProgressDelta==1 || !((Long64_t)(gROME->GetTriggerStatistics()->processedEvents+0.5)%fProgressDelta) && fProgressWrite)))) {
      if (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()) {
#if defined( R__VISUAL_CPLUSPLUS )
         ROMEPrint::Print("processed event number %I64d                                              \r",fCurrentEvent);
#else
         ROMEPrint::Print("processed event number %lld                                               \r",fCurrentEvent);
#endif
      }
      if (gROME->IsStandAloneARGUS() || gROME->IsROMEAndARGUS()) {
         if (gROME->GetWindow()->IsControllerActive())
            gROME->GetWindow()->GetAnalyzerController()->Update();
         gSystem->ProcessEvents();
         gSystem->Sleep(10);
      }
      fProgressWrite = false;
   }

   // ODB update
#if defined( HAVE_MIDAS )
   db_send_changed_records();
#endif

   return true;
}

Bool_t ROMEEventLoop::UserInput()
{
   if (gROME->isDaemonMode() || gROME->isBatchMode())
      return kTRUE;

   // Looks for user input. Called before the Event tasks.
   bool wait = false;
   bool first = true;
   bool interpreter = false;
   bool hit = false;
   ROMEString text;

   if ((fStopAtRun==gROME->GetCurrentRunNumber() && fStopAtEvent==gROME->GetCurrentEventNumber()) || (gROME->GetCurrentEventNumber()==0 && !fContinuous)) {
#if defined( R__VISUAL_CPLUSPLUS )
      ROMEPrint::Print("Stopped at event %I64d                      \r",gROME->GetCurrentEventNumber());
#else
      ROMEPrint::Print("Stopped at event %lld                      \r",gROME->GetCurrentEventNumber());
#endif
      wait = true;
   }
   else if (fContinuous && time(NULL) < fUserInputLastTime+0.1)
      return true;
   time(&fUserInputLastTime);

   while (wait||first) {
      first = false;
      if (!fContinuous)
         wait = true;

      interpreter = false;
      while (gROME->ss_kbhit() || gROME->HasUserEvent()) {
         hit = true;
         char ch = gROME->ss_getchar(0);
         if (ch == -1) {
            ch = getchar();
         }
         if (ch == 'q' || gROME->IsUserEventQ()) {
            ROMEPrint::Print("                                  \r");
            ROMEPrint::Print("Do you really want to quit [y/n] ?");
            ch = 0;
            while (ch==0) {
               while (gROME->ss_kbhit()) {
                  ch = gROME->ss_getchar(0);
               }
            }
            if (ch == 'y' || ch == 'Y') {
               ROMEPrint::Print("\r                                                                                \r");
               SetTerminate();
               gROME->SetTerminationFlag();
               if (gROME->IsStandAloneARGUS() || gROME->IsROMEAndARGUS()) {
                  gROME->GetWindow()->StopEventHandler();
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
               return false;
            }
         }
         if (ch == 'e' || gROME->IsUserEventE()) {
            this->SetTerminate();
            wait = false;
         }
         if (ch == 's' || gROME->IsUserEventS()) {
#if defined( R__VISUAL_CPLUSPLUS )
            ROMEPrint::Print("Stopped at event %I64d                      \r",gROME->GetCurrentEventNumber());
#else
            ROMEPrint::Print("Stopped at event %lld                      \r",gROME->GetCurrentEventNumber());
#endif
            wait = true;
         }
         if (ch == 'r' || gROME->IsUserEventR()) {

            if (fContinuous)
               ROMEPrint::Print("                                  \r");

            wait = false;
         }
         if (ch == 'o' || gROME->IsUserEventO()) {
            ROMEPrint::Print("Step by step mode                 \n");
            fContinuous = false;
         }
         if (ch == 'c' || gROME->IsUserEventC()) {
            ROMEPrint::Print("Continues mode                    \n");
            fContinuous = true;
            wait = false;
         }
         if (ch == 'g' || gROME->IsUserEventG()) {
            if (gROME->IsUserEventG()) {
               fStopAtRun = gROME->GetUserEventGRunNumber();
               fStopAtEvent = gROME->GetUserEventGEventNumber();
            }
            else {
               char *cstop;
               ROMEString number;
               // run number
               ROMEPrint::Print("                                  \r");
               ROMEPrint::Print("Run number :");
               while (true) {
                  ch = gROME->ss_getchar(0);
                  if (ch == 0)
                     continue;
                  if (ch == 13)
                     break;
                  ROMEPrint::Print("%c", ch);
                  number += ch;
               }
               ROMEPrint::Print("                                  \r");
               int inumber = strtol(number.Data(),&cstop,10);
               if (inumber!=0)
                  fStopAtRun = inumber;
               else
                  fStopAtRun = gROME->GetCurrentRunNumber();
               // event number
               number.Resize(0);
               ROMEPrint::Print("                                  \r");
               ROMEPrint::Print("Event number :");
               while (true) {
                  ch = gROME->ss_getchar(0);
                  if (ch == 0)
                     continue;
                  if (ch == 13)
                     break;
                  ROMEPrint::Print("%c", ch);
                  number += ch;
               }
               ROMEPrint::Print("                                  \r");
               inumber = strtol(number.Data(),&cstop,10);
               if (inumber!=0)
                  fStopAtEvent = inumber;
               else
                  fStopAtEvent = -1;
            }
            wait = false;
         }
         if (ch == 'i' || gROME->IsUserEventI()) {
            interpreter = true;
         }
         gROME->DeleteUserEvent();
      }
      if (interpreter) {
#if defined( R__VISUAL_CPLUSPLUS )
         ROMEPrint::Print("\nStart root session at the end of event number %I64d of run number %I64d\n",gROME->GetCurrentEventNumber(),gROME->GetCurrentRunNumber());
#else
         ROMEPrint::Print("\nStart root session at the end of event number %lld of run number %lld\n",gROME->GetCurrentEventNumber(),gROME->GetCurrentRunNumber());
#endif
         ROMEString prompt = gROME->GetProgramName();
         prompt.ToLower();
         prompt += " [%d]";
         gROME->GetApplication()->SetPrompt(prompt.Data());
         gROME->GetApplication()->Run(true);
         gSystem->Init();
         gROME->GetApplication()->ProcessLine(gROME->GetCintInitialisation());
         ROMEPrint::Print("\r                                                                                \r");
      }

      if (wait) {
         if (fSavedUpdateFrequency==-1) {
            fSavedUpdateFrequency = gROME->GetUpdateFrequency();
            gROME->SetUpdateFrequency(0);
         }
         gSystem->ProcessEvents();
         gSystem->Sleep(10);
      }
      else {
         if (fSavedUpdateFrequency!=-1) {
            gROME->SetUpdateFrequency(fSavedUpdateFrequency);
            fSavedUpdateFrequency = -1;
         }
      }
      if (gROME->IsWindowClosed()) {
         if (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS())
            return false;
         this->SetTerminate();
         break;
      }
   }
   if (hit)
      time(&fProgressTimeOfLastEvent);

   return true;
}

Bool_t ROMEEventLoop::DAQEndOfRun()
{
   // Disconnects the current run. Called after the EndOfRun tasks.
   // Write non accumulative output tree files
   ROMEPrint::Debug("Executing DAQ EndOfRun\n");

   ROMEString treename;
   ROMETree *romeTree;
   TTree *tree;
   const Int_t nTree = gROME->GetTreeObjectEntries();
   for (int j=0;j<nTree;j++) {
      romeTree = gROME->GetTreeObjectAt(j);
      if (romeTree->isFill()) {
         tree = romeTree->GetTree();
         if (romeTree->isWrite() && !gROME->isTreeAccumulation()) {
            if (fTreeUpdateIndex>0) {
               ROMEPrint::Print("Updating Root-File ");
            }
            else {
               ROMEPrint::Print("Writing Root-File ");
            }
            ROMEPrint::Print("%s\n", romeTree->GetFileName().Data());
            romeTree->GetFile()->cd();
            if (tree->Write(0,TObject::kOverwrite)==0) {
               ROMEPrint::Warning("--> Please check if you have write access to the directory.\n");
               ROMEPrint::Warning("--> If you have activated the read flag for this tree you must\n");
               ROMEPrint::Warning("    have different input and output directories.\n");
            }
            tree->SetDirectory(0);
            romeTree->GetFile()->Close();
            romeTree->GetFile()->Delete();
         }
         if (!gROME->isTreeAccumulation())
            tree->Reset();
      }
   }
   ROMEPrint::Print("\n");

   // Write Histos
   ROMEString filename;
   ROMEString runNumberString;
   gROME->GetCurrentRunNumberString(runNumberString);
   filename.SetFormatted("%s%s%s.root",gROME->GetOutputDir(),"histos",runNumberString.Data());
   fHistoFile = new TFile(filename.Data(),"RECREATE");
   TFolder *folder = (TFolder*)gROOT->FindObjectAny("histos");
   folder->Write();
   fHistoFile->Close();
   SafeDelete(fHistoFile);

   if (!gROME->GetActiveDAQ()->EndOfRunDAQ())
      return false;

   return true;
}

Bool_t ROMEEventLoop::DAQTerminate()
{
   // Clean up the analyzer. Called after the Terminate tasks.
   // Write accumulative output tree files
   // Close all files
   ROMEPrint::Debug("Executing DAQ Terminate\n");

   ROMEString runNumberString;
   ROMEString filename;
   gROME->GetCurrentRunNumberString(runNumberString);
   ROMETree *romeTree;
   TTree *tree;
   const Int_t nTree = gROME->GetTreeObjectEntries();
   for (int j=0;j<nTree;j++) {
      romeTree = gROME->GetTreeObjectAt(j);
      if (romeTree->isWrite() && romeTree->isFill()) {
         if (gROME->isTreeAccumulation()) {
            romeTree->GetFile()->cd();
            tree = romeTree->GetTree();
            ROMEPrint::Print("\nWriting Root-File %s\n", romeTree->GetFileName().Data());
            if (tree->Write(0,TObject::kOverwrite)==0) {
               ROMEPrint::Warning("--> Please check if you have write access to the directory.\n");
               ROMEPrint::Warning("--> If you have activated the read flag for this tree you must\n");
               ROMEPrint::Warning("    have different input and output directories.\n");
            }
         }
         if (romeTree->GetFile()!=NULL) {
            romeTree->GetFile()->Close();
         }
      }
   }

   if (!gROME->GetActiveDAQ()->TerminateDAQ())
      return false;

   return true;
}

// Run Status
Bool_t ROMEEventLoop::isRunning()  { return gROME->GetActiveDAQ()->isRunning(); }
Bool_t ROMEEventLoop::isStopped()  { return gROME->GetActiveDAQ()->isStopped(); }

void ROMEEventLoop::SetRunning()  { gROME->GetActiveDAQ()->SetRunning(); }
void ROMEEventLoop::SetStopped()  { gROME->GetActiveDAQ()->SetStopped(); }

// Event Status
Bool_t ROMEEventLoop::isAnalyze()    { return gROME->GetActiveDAQ()->isAnalyze(); }
Bool_t ROMEEventLoop::isContinue()   { return gROME->GetActiveDAQ()->isContinue(); }
Bool_t ROMEEventLoop::isBeginOfRun() { return gROME->GetActiveDAQ()->isBeginOfRun(); }
Bool_t ROMEEventLoop::isEndOfRun()   { return gROME->GetActiveDAQ()->isEndOfRun(); }
Bool_t ROMEEventLoop::isTerminate()  { return gROME->GetActiveDAQ()->isTerminate(); }

void ROMEEventLoop::SetAnalyze()    { gROME->GetActiveDAQ()->SetAnalyze(); }
void ROMEEventLoop::SetContinue()   { gROME->GetActiveDAQ()->SetContinue(); }
void ROMEEventLoop::SetBeginOfRun() { gROME->GetActiveDAQ()->SetBeginOfRun(); }
void ROMEEventLoop::SetEndOfRun()   { gROME->GetActiveDAQ()->SetEndOfRun(); }
void ROMEEventLoop::SetTerminate()  { gROME->GetActiveDAQ()->SetTerminate(); }
