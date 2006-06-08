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

#include "ArgusWindow.h"

#if defined( HAVE_MIDAS )
#   include "midas.h"
#endif

#include "ROMEEventLoop.h"
#include <Riostream.h>

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
}

void ROMEEventLoop::ExecuteTask(Option_t *option)
{
   bool firstUserInput = true;
   ROMEString text;
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
      gROME->PrintLine("\n\nTerminating Program !");
      return;
   }
   if (!IsActive()) {
      this->Terminate();
      gROME->SetTerminationFlag();
      gROME->PrintLine("\n\nTerminating Program !");
      return;
   }

   // Declarations
   //--------------
   Long64_t i,ii,eventLoopIndex;

   // Initialisation
   //----------------

   if (!this->DAQInit()) {
      this->Terminate();
      gROME->SetTerminationFlag();
      gROME->PrintLine("\n\nTerminating Program !");
      return;
   }

   if (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()) {
      gROME->PrintVerbose("Executing Init tasks");
      ExecuteTasks("Init");
      CleanTasks();
   }

   // Read Histograms
   if (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()) {
      gROME->PrintVerbose("Reading histograms");
      if (gROME->IsHistosRead()) {
         ReadHistograms();
      }
   }

   eventLoopIndex = 0;

   // Loop over Runs
   //----------------
   gROME->PrintVerbose("Entering run loop");
   for (ii=0;!this->isTerminate();ii++) {
      if (!this->DAQBeginOfRun(eventLoopIndex)) {
         this->DAQTerminate(true);
         gROME->SetTerminationFlag();
         gROME->PrintLine("\n\nTerminating Program !");
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
            gROME->PrintVerbose("Executing BeginOfRun tasks");
            ExecuteTasks("BeginOfRun");
            CleanTasks();
         }
         eventLoopIndex++;

         // Output
         if (gROME->IsShowRunStat()) {
#if defined( R__VISUAL_CPLUSPLUS )
            text.SetFormatted("\n\nRun %I64d started",gROME->GetCurrentRunNumber());
#else
            text.SetFormatted("\n\nRun %lld started",gROME->GetCurrentRunNumber());
#endif
            gROME->PrintLine(text.Data());
         }
      }

      // Start ARGUS
      //-------------
      if (ii==0 && (gROME->IsStandAloneARGUS() || gROME->IsROMEAndARGUS())) {
         gROME->PrintVerbose("Starting main window");
         gROME->StartWindow();
      }

      // Loop over Events
      //------------------
      firstUserInput = true;
      gROME->PrintVerbose("Entering event loop");
      for (i=0;!this->isTerminate()&&!this->isEndOfRun();i++) {
         // set terminal in every events because it is necessary when
         // program resumes from suspend.
/*         if (gROME->isBatchMode())
            gROME->redirectOutput();
         else
            gROME->ss_getchar(0);*/
// ---> This code killes ss_kbhit() please fix it   Matthias

         if (gROME->IsWindowClosed()) {
            if (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()) {
               this->DAQTerminate(true);
               gROME->SetTerminationFlag();
               gROME->PrintLine("\n\nTerminating Program !");
               return;
            }
            this->SetStopped();
            this->SetTerminate();
            break;
         }
         if (gROME->isOffline()) {
            // check event numbers
            int status = gROME->CheckEventNumber(i);
            if (status==0) {
               continue;
            }
            if (status==-1) {
               this->SetStopped();
               this->SetEndOfRun();
               break;
            }
         }
         // Update
         if (!this->Update()) {
            this->Terminate();
            gROME->SetTerminationFlag();
            gROME->PrintLine("\n\nTerminating Program !");
            return;
         }

         // User Input
         if (!gROME->IsStandAloneARGUS()) {
            if (!firstUserInput) {
               if (!this->UserInput()) {
                  this->DAQTerminate(true);
                  gROME->SetTerminationFlag();
                  gROME->PrintLine("\n\nTerminating Program !");
                  return;
               }
               if (this->isTerminate()) {
                  break;
               }
            }
            firstUserInput = false;
         }

         // Set Fill Event equal true
         gROME->SetFillEvent();

         // Read Event
         if (!this->DAQEvent(i)) {
            this->Terminate();
            gROME->SetTerminationFlag();
            gROME->PrintLine("\n\nTerminating Program !");
            return;
         }
         if (this->isEndOfRun()) {
            this->SetStopped();
            break;
         }
         if (this->isBeginOfRun()) {
            this->SetAnalyze();
            break;
         }
         if (this->isTerminate()) {
            break;
         }
         if (this->isContinue()) {
            continue;
         }

         // Event Tasks
         if (ROMEEventLoop::fTaskSwitchesChanged) {
            this->UpdateTaskSwitches();
            ROMEEventLoop::fTaskSwitchesChanged = false;
         }
         if (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()) {
            text.SetFormatted("Event%d",gROME->GetEventID());
            gROME->PrintVerbose("Executing Event tasks (option = %s)", text.Data());
            ExecuteTasks(text.Data());
            CleanTasks();
         }
         if (gROME->isTerminationFlag()) {
            gROME->PrintLine("\n\nTerminating Program !");
            return;
         }
         if (this->isEndOfRun())
            break;

         // Write Event
         if (!this->WriteEvent() && gROME->isFillEvent()) {
            this->Terminate();
            gROME->SetTerminationFlag();
            gROME->PrintLine("\n\nTerminating Program !");
            return;
         }
      }

      if (this->isEndOfRun() || this->isTerminate()) {
         if (this->isEndOfRun())
            this->SetBeginOfRun();

         // Show number of processed events
         if (gROME->IsShowRunStat()) {
#if defined( R__VISUAL_CPLUSPLUS )
            text.SetFormatted("Run %I64d stopped                                             \n",gROME->GetCurrentRunNumber());
            gROME->PrintLine(text.Data());
            text.SetFormatted("%I64d events processed\n",(Long64_t)(gROME->GetProcessedEvents()+0.5));
            gROME->PrintLine(text.Data());
#else
            text.SetFormatted("Run %lld stopped                                             \n",gROME->GetCurrentRunNumber());
            gROME->PrintLine(text.Data());
            text.SetFormatted("%lld events processed\n",(Long64_t)(gROME->GetProcessedEvents()+0.5));
            gROME->PrintLine(text.Data());
#endif
         }

         // End of Run Tasks
         if (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()) {
            gROME->PrintVerbose("Executing EndOfRun tasks");
            ExecuteTasks("EndOfRun");
            CleanTasks();
         }
         // Disconnect
         if (!this->DAQEndOfRun()) {
            this->Terminate();
            gROME->SetTerminationFlag();
            gROME->PrintLine("\n\nTerminating Program !");
            return;
         }
      }
   }

   if (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()) {
      fWatchAll.Stop();
      if (gROME->IsShowRunStat()) {
         gROME->PrintLine("run times :                      All Methods   Event Methods");
         gROME->PrintLine("-----------                      ------------  -------------");
         Exec("t");
         gROME->PrintLine("");
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
         gROME->PrintLine();
      }
   }

   // Terminate
   if (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()) {
      if (!this->DAQTerminate(false)) {
         gROME->SetTerminationFlag();
         gROME->PrintLine("\n\nTerminating Program !");
         return;
      }
      gROME->PrintVerbose("Executing Terminate tasks");
      ExecuteTasks("Terminate");
      CleanTasks();
   }
}

Bool_t ROMEEventLoop::DAQInit()
{
   // Initialize the analyzer. Called before the init tasks.
   gROME->PrintVerbose("Executing DAQ Init");

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
            gROME->PrintLine("No run numbers or input file names specified.\n");
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
   }
   return true;
}

Bool_t ROMEEventLoop::DAQBeginOfRun(Long64_t eventLoopIndex)
{
   // Connect the Analyzer to the current run. Called before the BeginOfRun tasks.
   gROME->PrintVerbose("Executing DAQ BeginOfRun");

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
      gROME->PrintLine("\nError while reading the data base !");
      return false;
   }
   if (eventLoopIndex==0)
      this->InitArrayFolders();
   if (!gROME->ReadArrayDataBaseFolders()) {
      gROME->PrintLine("\nError while reading the data base !");
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
      }
   }
   return true;
}

Bool_t ROMEEventLoop::DAQEvent(Long64_t event)
{
   // Reads an event. Called before the Event tasks.
   gROME->PrintVerbose("Executing DAQ Event");

   Statistics *stat = gROME->GetTriggerStatistics();

   gROME->SetEventFilled(false);
   this->SetAnalyze();
   this->ResetFolders();

   if (gROME->IsDontReadNextEvent()) {
      gROME->SetDontReadNextEvent(false);
      return true;
   }

   if (!gROME->GetActiveDAQ()->EventDAQ(event))
      return false;
   if (this->isContinue()) {
      return true;
   }
   gROME->SetEventFilled(true);

   if (gROME->IsEventBasedDataBase()) {
      if (!gROME->ReadSingleDataBaseFolders()) {
         gROME->PrintLine("\nError while reading the data base !");
         return false;
      }
      if (!gROME->ReadArrayDataBaseFolders()) {
         gROME->PrintLine("\nError while reading the data base !");
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

   if (!fContinuous || ((fProgressDelta==1 || !((Long64_t)(gROME->GetTriggerStatistics()->processedEvents+0.5)%fProgressDelta) && fProgressWrite))) {
      if (gROME->IsStandAloneROME() || gROME->IsROMEAndARGUS()) {
#if defined( R__VISUAL_CPLUSPLUS )
         text.SetFormatted("%I64d events processed                                                    \r",(Long64_t)gROME->GetTriggerStatistics()->processedEvents);
#else
         text.SetFormatted("%lld events processed                                                    \r",(Long64_t)gROME->GetTriggerStatistics()->processedEvents);
#endif
         gROME->PrintFlush(text.Data());
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
   // Looks for user input. Called before the Event tasks.
   bool wait = false;
   bool first = true;
   bool interpreter = false;
   bool hit = false;
   ROMEString text;

   if ((fStopAtRun==gROME->GetCurrentRunNumber() && fStopAtEvent==gROME->GetCurrentEventNumber()) || (gROME->GetCurrentEventNumber()==0 && !fContinuous)) {
#if defined( R__VISUAL_CPLUSPLUS )
      text.SetFormatted("Stopped at event %I64d                      \r",gROME->GetCurrentEventNumber());
#else
      text.SetFormatted("Stopped at event %lld                      \r",gROME->GetCurrentEventNumber());
#endif
      gROME->PrintFlush(text.Data());
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
            gROME->PrintFlush("                                  \r");
            gROME->PrintFlush("Do you really want to quit [y/n] ?");
            ch = 0;
            while (ch==0) {
               while (gROME->ss_kbhit()) {
                  ch = gROME->ss_getchar(0);
               }
            }
            if (ch == 'y' || ch == 'Y')
               return false;
            gROME->PrintFlush("                                  \r");
         }
         if (ch == 'e' || gROME->IsUserEventE()) {
            this->SetTerminate();
            wait = false;
         }
         if (ch == 's' || gROME->IsUserEventS()) {
#if defined( R__VISUAL_CPLUSPLUS )
            text.SetFormatted("Stopped at event %I64d                      \r",gROME->GetCurrentEventNumber());
#else
            text.SetFormatted("Stopped at event %lld                      \r",gROME->GetCurrentEventNumber());
#endif
            gROME->PrintFlush(text.Data());
            wait = true;
         }
         if (ch == 'r' || gROME->IsUserEventR()) {

            if (fContinuous)
               gROME->PrintFlush("                                  \r");
            if (!fContinuous && (gROME->IsStandAloneARGUS() || gROME->IsROMEAndARGUS())) {
               gROME->GetWindow()->TriggerEventHandler();
            }

            wait = false;
         }
         if (ch == 'o' || gROME->IsUserEventO()) {
            gROME->PrintLine("Step by step mode                 ");
            fContinuous = false;
         }
         if (ch == 'c' || gROME->IsUserEventC()) {
            gROME->PrintLine("Continues mode                    ");
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
               gROME->PrintFlush("                                  \r");
               gROME->PrintText("Run number :");
               while (true) {
                  ch = gROME->ss_getchar(0);
                  if (ch == 0)
                     continue;
                  if (ch == 13)
                     break;
                  gROME->PrintText(ch);
                  number += ch;
               }
               gROME->PrintFlush("                                  \r");
               int inumber = strtol(number.Data(),&cstop,10);
               if (inumber!=0)
                  fStopAtRun = inumber;
               else
                  fStopAtRun = gROME->GetCurrentRunNumber();
               // event number
               number.Resize(0);
               gROME->PrintFlush("                                  \r");
               gROME->PrintText("Event number :");
               while (true) {
                  ch = gROME->ss_getchar(0);
                  if (ch == 0)
                     continue;
                  if (ch == 13)
                     break;
                  gROME->PrintText(ch);
                  number += ch;
               }
               gROME->PrintFlush("                                  \r");
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
         text.SetFormatted("\nStart root session at the end of event number %I64d of run number %I64d",gROME->GetCurrentEventNumber(),gROME->GetCurrentRunNumber());
#else
         text.SetFormatted("\nStart root session at the end of event number %lld of run number %lld",gROME->GetCurrentEventNumber(),gROME->GetCurrentRunNumber());
#endif
         gROME->PrintLine(text.Data());
         ROMEString prompt = gROME->GetProgramName();
         prompt.ToLower();
         prompt += " [%d]";
         gROME->GetApplication()->SetPrompt(prompt.Data());
         gROME->GetApplication()->Run(true);
         gSystem->Init();
         gROME->GetApplication()->ProcessLine(gROME->GetCintInitialisation());
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
   gROME->PrintVerbose("Executing DAQ EndOfRun");

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
               gROME->PrintText("Updating Root-File ");
            }
            else {
               gROME->PrintText("Writing Root-File ");
            }
            gROME->PrintLine(romeTree->GetFileName());
            romeTree->UpdateFilePointer();
            romeTree->GetFile()->cd();
            if (tree->Write(0,TObject::kOverwrite)==0) {
               gROME->PrintLine("--> Please check if you have write access to the directory.");
               gROME->PrintLine("--> If you have activated the read flag for this tree you must");
               gROME->PrintLine("    have different input and output directories.");
            }
            tree->SetDirectory(0);
            romeTree->GetFile()->Close();
            romeTree->GetFile()->Delete();
         }
         if (!gROME->isTreeAccumulation())
            tree->Reset();
      }
   }
   gROME->PrintLine();

   // Write Histos
   ROMEString filename;
   ROMEString runNumberString;
   gROME->GetCurrentRunNumberString(runNumberString);
   filename.SetFormatted("%s%s%s.root",gROME->GetOutputDir(),"histos",runNumberString.Data());
   fHistoFile = new TFile(filename.Data(),"RECREATE");
   TFolder *folder = (TFolder*)gROOT->FindObjectAny("histos");
   folder->Write();
   fHistoFile->Close();

   if (!gROME->GetActiveDAQ()->EndOfRunDAQ())
      return false;

   return true;
}

Bool_t ROMEEventLoop::DAQTerminate(Bool_t quit)
{
   // Clean up the analyzer. Called after the Terminate tasks.
   // Write accumulative output tree files
   // Close all files
   gROME->PrintVerbose("Executing DAQ Terminate");

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
            gROME->PrintText("\nWriting Root-File ");
            gROME->PrintLine(romeTree->GetFileName());
            if (tree->Write(0,TObject::kOverwrite)==0) {
               gROME->PrintLine("--> Please check if you have write access to the directory.");
               gROME->PrintLine("--> If you have activated the read flag for this tree you must");
               gROME->PrintLine("    have different input and output directories.");
            }
         }
         if (romeTree->GetFile()!=NULL) {
            romeTree->GetFile()->Close();
         }
      }
   }

   if (!gROME->GetActiveDAQ()->TerminateDAQ(quit))
      return false;

   return true;
}
