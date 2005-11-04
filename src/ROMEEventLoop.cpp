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
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS
#include <RConfig.h>
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

#ifdef HAVE_MIDAS
#   include <midas.h>
#endif

#include <ROMEEventLoop.h>
#include <Riostream.h>


TTask *TTask::fgBeginTask  = 0;
TTask *TTask::fgBreakPoint = 0;

// Task switches handle initialization
bool ROMEEventLoop::fTaskSwitchesChanged = false;

ROMEEventLoop::ROMEEventLoop(const char *name,const char *title):ROMETask(name,title)
{
   fTreeInfo = new ROMETreeInfo();
   fSequentialNumber = 0;
   fContinuous = true;
   fUserInputLastTime = 0;
   fTreeUpdateIndex = 0;
}

#include <TBrowser.h>
void ROMEEventLoop::ExecuteTask(Option_t *option)
{
   ROMEString text;
   if (!strcmp(option,"init")) {
      this->InitTrees();
      return;
   }

// Event loop
   if (fgBeginTask) {
      Error("ExecuteTask","Cannot execute task:%s, already running task: %s",GetName(),fgBeginTask->GetName());
      this->Terminate();
      gROME->SetTerminationFlag();
      gROME->Println("\n\nTerminating Program !");
      return;
   }
   if (!IsActive()) {
      this->Terminate();
      gROME->SetTerminationFlag();
      gROME->Println("\n\nTerminating Program !");
      return;
   }

   // Declarations
   //--------------
   int i,ii,eventLoopIndex;

   // Initialisation
   //----------------

   if (!this->DAQInit()) {
      this->Terminate();
      gROME->SetTerminationFlag();
      gROME->Println("\n\nTerminating Program !");
      return;
   }

   ExecuteTasks("i");
   CleanTasks();

   eventLoopIndex = 0;

   // Loop over Runs
   //----------------
   for (ii=0;!this->isTerminate();ii++) {

      if (!this->DAQBeginOfRun(eventLoopIndex)) {
         this->DAQTerminate();
         gROME->SetTerminationFlag();
         gROME->Println("\n\nTerminating Program !");
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
         ExecuteTasks("b");
         CleanTasks();
         eventLoopIndex++;

         // Output
         TimeStart();
         text.SetFormatted("\n\nRun %d started",gROME->GetCurrentRunNumber());
         gROME->Println(text.Data());
      }

      // Loop over Events
      //------------------
      for (i=0;!this->isTerminate()&&!this->isEndOfRun();i++) {
         // User Input
         if (!this->UserInput()) {
            this->DAQTerminate();
            gROME->SetTerminationFlag();
            gROME->Println("\n\nTerminating Program !");
            return;
         }
         if (this->isTerminate()) {
            break;
         }

         // Set Fill Event equal true
         gROME->SetFillEvent();

         // Read Event
         if (!this->DAQEvent(i)) {
            this->Terminate();
            gROME->SetTerminationFlag();
            gROME->Println("\n\nTerminating Program !");
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
         char eventID = gROME->GetEventIDChar();
         ExecuteTasks(&eventID);
         CleanTasks();
         if (gROME->isTerminationFlag()) {
            gROME->Println("\n\nTerminating Program !");
            return;
         }

         // Write Event
         if (!this->WriteEvent() && gROME->isFillEvent()) {
            this->Terminate();
            gROME->SetTerminationFlag();
            gROME->Println("\n\nTerminating Program !");
            return;
         }

         // Update
         if (!this->Update()) {
            this->Terminate();
            gROME->SetTerminationFlag();
            gROME->Println("\n\nTerminating Program !");
            return;
         }
      }
      if (this->isEndOfRun() || this->isTerminate()) {
         if (this->isEndOfRun())
            this->SetBeginOfRun();

         TimeEnd();

         // Show number of processed events
         text.SetFormatted("Run %d stopped                                             \n",gROME->GetCurrentRunNumber());
         gROME->Println(text.Data());
         text.SetFormatted("%d events processed\n",(int)gROME->GetProcessedEvents());
         gROME->Println(text.Data());

         gROME->Print("run time = ");
         gROME->Println(GetTime());
         gROME->Println();

         // End of Run Tasks
         ExecuteTasks("e");
         CleanTasks();

         // Disconnect
         if (!this->DAQEndOfRun()) {
            this->Terminate();
            gROME->SetTerminationFlag();
            gROME->Println("\n\nTerminating Program !");
            return;
         }
      }
   }

   // Terminate Tasks
   ExecuteTasks("t");
   CleanTasks();

   // Root Interpreter
   if (!gROME->isBatchMode()) {
      gROME->GetApplication()->Run(true);
      gROME->Println();
   }

   // Terminate
   if (!this->DAQTerminate()) {
      gROME->SetTerminationFlag();
      gROME->Println("\n\nTerminating Program !");
      return;
   }
}


bool ROMEEventLoop::DAQInit() {
   // Initialize the analyzer. Called before the init tasks.
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
      gROME->Println("No run numbers or input file names specified.\n");
      return false;
   }


   // Initialize DAQ System
   if (!gROME->GetActiveDAQ()->Init())
      return false;

   // Open Output Files
   ROMEString filename;
   ROMETree *romeTree;
   TTree *tree;
   for (j=0;j<gROME->GetTreeObjectEntries();j++) {
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
bool ROMEEventLoop::DAQBeginOfRun(Int_t eventLoopIndex) {
   // Connect the Analyzer to the current run. Called before the BeginOfRun tasks.
   ROMEString runNumberString;
   // Statistics
   Statistics *stat = gROME->GetTriggerStatistics();
   stat->processedEvents = 0;
   stat->eventsPerSecond = 0;
   stat->writtenEvents = 0;
   fStatisticsTimeOfLastEvent = 0;
   fStatisticsLastEvent = 0;
   // Event Number Check
   gROME->InitCheckEventNumber();

   if (gROME->isOffline() && (gROME->IsRunNumberBasedIO() || gROME->IsRunNumberAndFileNameBasedIO())) {
      // run number based IO
      if (gROME->GetNumberOfRunNumbers()<=eventLoopIndex) {
         this->SetTerminate();
         return true;
      }
      // Check Configuration
      gROME->SetCurrentRunNumber(gROME->GetRunNumberAt(eventLoopIndex));
      gROME->GetConfiguration()->CheckConfiguration(gROME->GetCurrentRunNumber());
   }

   // Begin Of Run Of Active DAQ
   if (!gROME->GetActiveDAQ()->BeginOfRun())
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
      gROME->Println("\nError while reading the data base !");
      return false;
   }
   if (eventLoopIndex==0)
      this->InitArrayFolders();
   if (!gROME->ReadArrayDataBaseFolders()) {
      gROME->Println("\nError while reading the data base !");
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
   for (int j=0;j<gROME->GetTreeObjectEntries();j++) {
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

bool ROMEEventLoop::DAQEvent(Int_t event) {
   // Reads an event. Called before the Event tasks.
   Statistics *stat = gROME->GetTriggerStatistics();

   this->SetAnalyze();
   this->ResetFolders();

   if (gROME->IsDontReadNextEvent()) {
      gROME->SetDontReadNextEvent(false);
      return true;
   }
   if (gROME->isOffline()) {
      // check event numbers
      int status = gROME->CheckEventNumber(event);
      if (status==0) {
         this->SetContinue();
         return true;
      }
      if (status==-1) {
         this->SetEndOfRun();
         return true;
      }
   }

   if (!gROME->GetActiveDAQ()->Event(event))
      return false;


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

bool ROMEEventLoop::WriteEvent() {
   // Writes the event. Called after the Event tasks.
   this->CleanUpFolders();
   fTreeInfo->SetEventNumber(gROME->GetCurrentEventNumber());
   this->FillTrees();
   return true;
}
bool ROMEEventLoop::Update()
{
   // Update the Analyzer. Called after the Event tasks.

   ROMEString text;
   // Progress Display
   if (fProgressDelta>1) {
      if ((int)gROME->GetTriggerStatistics()->processedEvents >= fProgressLastEvent + fProgressDelta) {
         time(&fProgressTimeOfLastEvent);
         fProgressLastEvent = (int)gROME->GetTriggerStatistics()->processedEvents;
         fProgressWrite = true;
      }
      else {
         if (time(NULL) > fProgressTimeOfLastEvent+1)
            fProgressDelta /= 10;
      }
   }

   if (!fContinuous || ((fProgressDelta==1 || !((int)gROME->GetTriggerStatistics()->processedEvents%fProgressDelta) && fProgressWrite))) {
      text.SetFormatted("%d events processed                                                    \r",(int)gROME->GetTriggerStatistics()->processedEvents);
      gROME->Printfl(text.Data());

      fProgressWrite = false;
   }
   // ODB update
#if defined( HAVE_MIDAS )
   db_send_changed_records();
#endif

   return true;
}

bool ROMEEventLoop::UserInput()
{
   // Looks for user input. Called before the Event tasks.
   bool wait = false;
   bool first = true;
   bool interpreter = false;
   bool hit = false;
   ROMEString text;

   if ((fStopAtRun==gROME->GetCurrentRunNumber() && fStopAtEvent==gROME->GetCurrentEventNumber()) || (gROME->GetCurrentEventNumber()==0 && !fContinuous)) {
      text.SetFormatted("Stopped at event %d                      \r",gROME->GetCurrentEventNumber());
      gROME->Printfl(text.Data());
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
            return false;
         }
         if (ch == 'e' || gROME->IsUserEventE()) {
            this->SetTerminate();
            wait = false;
         }
         if (ch == 's' || gROME->IsUserEventS()) {
            text.SetFormatted("Stopped at event %d                      \r",gROME->GetCurrentEventNumber());
            gROME->Printfl(text.Data());
            wait = true;
         }
         if (ch == 'r' || gROME->IsUserEventR()) {

            if (fContinuous)
               gROME->Printfl("                                  \r");

            wait = false;
         }
         if (ch == 'o' || gROME->IsUserEventO()) {
            gROME->Println("Step by step mode                 ");
            fContinuous = false;
         }
         if (ch == 'c' || gROME->IsUserEventC()) {
            gROME->Println("Continues mode                    ");
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
               gROME->Printfl("                                  \r");
               gROME->Print("Run number :");
               while (true) {
                  ch = gROME->ss_getchar(0);
                  if (ch == 0)
                     continue;
                  if (ch == 13)
                     break;
                  gROME->Print(ch);
                  number += ch;
               }
               gROME->Printfl("                                  \r");
               int inumber = strtol(number.Data(),&cstop,10);
               if (inumber!=0)
                  fStopAtRun = inumber;
               else
                  fStopAtRun = gROME->GetCurrentRunNumber();
               // event number
               number.Resize(0);
               gROME->Printfl("                                  \r");
               gROME->Print("Event number :");
               while (true) {
                  ch = gROME->ss_getchar(0);
                  if (ch == 0)
                     continue;
                  if (ch == 13)
                     break;
                  gROME->Print(ch);
                  number += ch;
               }
               gROME->Printfl("                                  \r");
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
            wait = false;
         }
         gROME->DeleteUserEvent();
      }
      if (interpreter) {
         text.SetFormatted("\nStart root session at the end of event number %d of run number %d",gROME->GetCurrentEventNumber(),gROME->GetCurrentRunNumber());
         gROME->Println(text.Data());
         gROME->GetApplication()->Run(true);
         gSystem->Init();
         gROME->GetApplication()->ProcessLine("MEGAnalyzer* gAnalyzer = ((MEGAnalyzer*)((TFolder*)gROOT->FindObjectAny(\"ROME\"))->GetListOfFolders()->MakeIterator()->Next());");
      }
      gROME->ss_sleep(10);
   }
   if (hit)
      time(&fProgressTimeOfLastEvent);

   return true;
}

bool ROMEEventLoop::DAQEndOfRun() {
   // Disconnects the current run. Called after the EndOfRun tasks.

   // Write non accumulative output tree files
   ROMEString treename;
   ROMETree *romeTree;
   TTree *tree;
   for (int j=0;j<gROME->GetTreeObjectEntries();j++) {
      romeTree = gROME->GetTreeObjectAt(j);
      if (romeTree->isFill()) {
         tree = romeTree->GetTree();
         if (romeTree->isWrite() && !gROME->isTreeAccumulation()) {
            if (fTreeUpdateIndex>0) {
               gROME->Print("Updating Root-File ");
            }
            else {
               gROME->Print("Writing Root-File ");
            }
            gROME->Println(romeTree->GetFileName());
            romeTree->UpdateFilePointer();
            romeTree->GetFile()->cd();
            tree->Write(0,TObject::kOverwrite);
            tree->SetDirectory(0);
            romeTree->GetFile()->Close();
            romeTree->GetFile()->Delete();
         }
         if (!gROME->isTreeAccumulation())
            tree->Reset();
      }
   }
   gROME->Println();

   // Write Histos
   ROMEString filename;
   ROMEString runNumberString;
   gROME->GetCurrentRunNumberString(runNumberString);
   filename.SetFormatted("%s%s%s.root",gROME->GetOutputDir(),"histos",runNumberString.Data());
   fHistoFile = new TFile(filename.Data(),"RECREATE");
   TFolder *folder = (TFolder*)gROOT->FindObjectAny("histos");
   folder->Write();
   fHistoFile->Close();

   if (!gROME->GetActiveDAQ()->EndOfRun())
      return false;

   return true;
}

bool ROMEEventLoop::DAQTerminate() {
   // Clean up the analyzer. Called after the Terminate tasks.
   // Write accumulative output tree files
   // Close all files
   ROMEString runNumberString;
   ROMEString filename;
   gROME->GetCurrentRunNumberString(runNumberString);
   ROMETree *romeTree;
   TTree *tree;
   for (int j=0;j<gROME->GetTreeObjectEntries();j++) {
      romeTree = gROME->GetTreeObjectAt(j);
      if (romeTree->isWrite() && romeTree->isFill()) {
         if (gROME->isTreeAccumulation()) {
            romeTree->GetFile()->cd();
            tree = romeTree->GetTree();
            tree->Write(0,TObject::kOverwrite);
            gROME->Print("\nWriting Root-File ");
            gROME->Println(romeTree->GetFileName());
         }
         if (romeTree->GetFile()!=NULL) {
            romeTree->GetFile()->Close();
         }
      }
   }

   if (!gROME->GetActiveDAQ()->Terminate())
      return false;

   return true;
}
