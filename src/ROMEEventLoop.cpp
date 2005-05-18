// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMEEventLoop                                                        //
//                                                                      //
//  Implements the event loop. This Class must be the root Task of 
//  the Application.
//                                                                      //
//  $Log$
//  Revision 1.56  2005/05/18 09:49:32  schneebeli_m
//  removed run & event number error, implemented FileRead in ROMEString
//
//  Revision 1.55  2005/05/13 23:51:14  sawada
//  code cleanup.
//
//  Revision 1.54  2005/05/02 16:09:27  schneebeli_m
//  -o commandline parameter
//
//  Revision 1.53  2005/04/15 16:44:26  schneebeli_m
//  odb, zlib
//
//  Revision 1.52  2005/04/08 17:08:09  schneebeli_m
//  TNetFolderServer changes
//
//  Revision 1.51  2005/04/04 07:23:59  schneebeli_m
//  Time report
//
//  Revision 1.50  2005/04/01 14:56:23  schneebeli_m
//  Histo moved, multiple databases, db-paths moved, InputDataFormat->DAQSystem, GetMidas() to access banks, User DAQ
//
//  Revision 1.49  2005/03/21 17:29:47  schneebeli_m
//  minor changes
//
//  Revision 1.48  2005/02/21 21:29:07  sawada
//
//  Changed OS specifying macros
//  Support for DEC,Ultrix,FreeBSD,Solaris
//
//  Revision 1.47  2005/02/06 01:26:55  sawada
//  moved byte swapping functions to ROMEMidas
//
//  Revision 1.46  2005/01/27 16:21:06  schneebeli_m
//  print method & no gROME in path
//
//  Revision 1.45  2005/01/24 16:29:40  schneebeli_m
//  last bank access
//
//  Revision 1.44  2005/01/24 15:45:15  schneebeli_m
//  ss_millitime
//
//  Revision 1.43  2005/01/24 15:25:09  schneebeli_m
//  Seperated DAQ classes
//
//  Revision 1.42  2005/01/19 13:29:50  schneebeli_m
//  geteventid
//
//  Revision 1.41  2005/01/18 17:41:29  schneebeli_m
//  Termination flag
//
//  Revision 1.40  2005/01/17 11:13:27  schneebeli_m
//  consol output
//
//  Revision 1.39  2005/01/14 13:23:19  schneebeli_m
//  Screen output
//
//  Revision 1.38  2005/01/05 10:36:20  schneebeli_m
//  Tree file write error
//
//  Revision 1.37  2004/12/07 15:03:12  schneebeli_m
//  online steering
//
//  Revision 1.36  2004/12/06 16:03:02  sawada
//  code cleanup (tab -> space)
//
//  Revision 1.35  2004/12/06 11:16:51  schneebeli_m
//  user input on linux
//
//  Revision 1.34  2004/12/06 09:20:50  schneebeli_m
//  ss_getchar on linux
//
//  Revision 1.33  2004/12/06 09:04:34  schneebeli_m
//  minor changes
//
//  Revision 1.32  2004/12/05 10:10:43  sawada
//
//  code clean up (line feed)
//
//  Revision 1.31  2004/12/03 14:42:08  schneebeli_m
//  some minor changes
//
//  Revision 1.30  2004/12/02 17:46:43  sawada
//  Macintosh port
//
//  Revision 1.29  2004/11/23 09:22:21  schneebeli_m
//  User called Root Interpreter
//
//  Revision 1.28  2004/11/16 16:14:01  schneebeli_m
//  implemented task hierarchy
//
//  Revision 1.27  2004/11/12 17:35:18  schneebeli_m
//  fast xml database
//
//  Revision 1.26  2004/11/11 12:55:27  schneebeli_m
//  Implemented XML database with new path rules
//
//  Revision 1.25  2004/10/20 09:22:16  schneebeli_m
//  bugs removed
//
//  Revision 1.24  2004/10/19 21:18:06  pierre
//  put unistd.h in the right place
//
//  Revision 1.23  2004/10/19 21:11:20  pierre
//  Add unistd.h for read/close under Linux
//
//  Revision 1.22  2004/10/15 12:30:49  schneebeli_m
//  online eventloop logic
//
//  Revision 1.21  2004/10/14 09:53:41  schneebeli_m
//  ROME configuration file format changed and extended, Folder Getter changed : GetXYZObject -> GetXYZ, tree compression level and fill flag
//
//  Revision 1.20  2004/10/05 07:52:44  schneebeli_m
//  dyn. Folders, TRef Objects, XML format changed, ROMEStatic removed
//
//  Revision 1.19  2004/10/01 14:33:29  schneebeli_m
//  Fixed some tree file problems
//
//  Revision 1.18  2004/10/01 13:11:33  schneebeli_m
//  Tree write error removed, Database Number Problem solved, Trees in Folder for TSocket
//
//  Revision 1.17  2004/09/30 10:18:05  schneebeli_m
//  gAnalyzer and gROME
//
//  Revision 1.16  2004/09/25 01:34:48  schneebeli_m
//  implemented FW dependent EventLoop and DataBase classes
//
//                                                                      //
//////////////////////////////////////////////////////////////////////////
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
      this->Termination();
      gROME->SetTerminationFlag();
      gROME->Println("\n\nTerminating Program !");
      return;
   }
   if (!IsActive()) {
      this->Termination();
      gROME->SetTerminationFlag();
      gROME->Println("\n\nTerminating Program !");
      return;
   }

   // Declarations
   //--------------
   int i,ii,runNumberIndex;

   // Initialisation
   //----------------

   if (!this->Initialize()) {
      this->Termination();
      gROME->SetTerminationFlag();
      gROME->Println("\n\nTerminating Program !");
      return;
   }
 
   ExecuteTasks("i");
   CleanTasks();

   runNumberIndex = 0;

   // Loop over Runs
   //----------------
   for (ii=0;!this->isTerminate();ii++) {

      if (!this->Connect(runNumberIndex)) {
         this->Termination();
         gROME->SetTerminationFlag();
         gROME->Println("\n\nTerminating Program !");
         return;
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
         runNumberIndex++;

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
            this->Termination();
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
         if (!this->ReadEvent(i)) {
            this->Termination();
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
            this->Termination();
            gROME->SetTerminationFlag();
            gROME->Println("\n\nTerminating Program !");
            return;
         }

         // Update
         if (!this->Update()) {
            this->Termination();
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
         if (!this->Disconnect()) {
            this->Termination();
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
   if (!this->Termination()) {
      gROME->SetTerminationFlag();
      gROME->Println("\n\nTerminating Program !");
      return;
   }
}


bool ROMEEventLoop::Initialize() {
   // Initialize the analyzer. Called before the init tasks.
   int j;
   this->SetRunning();
   this->SetAnalyze();

   this->InitTaskSwitches();
   this->InitSingleFolders();

   if (!gROME->GetActiveDAQ()->Initialize())
      return false;

   if (gROME->isOffline()&&gROME->GetNumberOfRunNumbers()<=0) {
      gROME->Println("No run numbers specified.\n");
      return false;
   }

   // Accumulative output tree file initialisation
   fTreeFiles = new TFile*[gROME->GetTreeObjectEntries()];
   ROMEString filename;
   ROMEString runNumberString;
   ROMETree *romeTree;
   TTree *tree;
   if (gROME->isOffline())
      gROME->GetRunNumberStringAt(runNumberString,0);
   else
      gROME->GetCurrentRunNumberString(runNumberString);
   for (j=0;j<gROME->GetTreeObjectEntries();j++) {
      fTreeFiles[j] = NULL;
      romeTree = gROME->GetTreeObjectAt(j);
      if (romeTree->isWrite() && gROME->isTreeAccumulation()) {
         tree = romeTree->GetTree();
         filename.SetFormatted("%s%s%s.root",gROME->GetOutputDir(),tree->GetName(),runNumberString.Data());
         fTreeFiles[j] = new TFile(filename.Data(),"RECREATE");
         tree->SetDirectory(fTreeFiles[j]);
      }
   }

   return true;
}
bool ROMEEventLoop::Connect(Int_t runNumberIndex) {
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

   if (gROME->isOffline()) {
      if (gROME->GetNumberOfRunNumbers()<=runNumberIndex) {
         this->SetTerminate();
         return true;
      }
      gROME->SetCurrentRunNumber(gROME->GetRunNumberAt(runNumberIndex));
      gROME->GetConfiguration()->CheckConfiguration(gROME->GetCurrentRunNumber());
   }
   fTreeInfo->SetRunNumber(gROME->GetCurrentRunNumber());
   gROME->GetCurrentRunNumberString(runNumberString);

   // Non accumulative output tree file initialisation
   ROMEString filename;
   ROMETree *romeTree;
   TTree *tree;
   for (int j=0;j<gROME->GetTreeObjectEntries();j++) {
      romeTree = gROME->GetTreeObjectAt(j);
      if (!gROME->isTreeAccumulation()) {
         if (romeTree->isFill()) {
            tree = romeTree->GetTree();
            tree->Reset();
            if (romeTree->isWrite()) {
               filename.SetFormatted("%s%s%s.root",gROME->GetOutputDir(),tree->GetName(),runNumberString.Data());
               fTreeFiles[j] = new TFile(filename.Data(),"RECREATE");
               tree->SetDirectory(fTreeFiles[j]);
            }
         }
      }
   }

   if (!gROME->GetActiveDAQ()->Connect())
      return false;

   // Update Data Base
   if (!gROME->ReadSingleDataBaseFolders()) {
      gROME->Println("\nError while reading the data base !");
      return false;
   }
   if (runNumberIndex==0)
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

   return true;
}

bool ROMEEventLoop::ReadEvent(Int_t event) {
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

   if (!gROME->GetActiveDAQ()->ReadEvent(event))
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
      while (gROME->ss_kbhit()) {
         hit = true;
         char ch = gROME->ss_getchar(0);
         if (ch == -1) {
            ch = getchar();
         }
         if (ch == 'q') {
            return false;
         }
         if (ch == 'e') {
            this->SetTerminate();
            wait = false;
         }
         if (ch == 's') {
            text.SetFormatted("Stopped at event %d                      \r",gROME->GetCurrentEventNumber());
            gROME->Printfl(text.Data());
            wait = true;
         }
         if (ch == 'r') {

            if (fContinuous)
               gROME->Printfl("                                  \r");

            wait = false;
         }
         if (ch == 'o') {
            gROME->Println("Step by step mode                 ");
            fContinuous = false;
         }
         if (ch == 'c') {
            gROME->Println("Continues mode                    ");
            fContinuous = true;
            wait = false;
         }
         if (ch == 'g') {
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
            wait = false;
         }
         if (ch == 'i') {
            interpreter = true;
            wait = false;
         }
      }
      if (interpreter) {
         text.SetFormatted("\nEnd of event number %d of run number %d",gROME->GetCurrentEventNumber(),gROME->GetCurrentRunNumber());
         gROME->Printfl(text.Data());
         gROME->GetApplication()->Run(true);
      }
   }
   if (hit)
      time(&fProgressTimeOfLastEvent);

   return true;
}

bool ROMEEventLoop::Disconnect() {
   // Disconnects the current run. Called after the EndOfRun tasks.

   // Write non accumulative output tree files
   ROMEString filename;
   ROMETree *romeTree;
   TTree *tree;
   TFile *fTreeFile;
   ROMEString runNumberString;
   gROME->GetCurrentRunNumberString(runNumberString);
   for (int j=0;j<gROME->GetTreeObjectEntries();j++) {
      romeTree = gROME->GetTreeObjectAt(j);
      fTreeFile = fTreeFiles[j];
      if (romeTree->isWrite() && !gROME->isTreeAccumulation()) {
         tree = romeTree->GetTree();
         gROME->Print("Writing Root-File ");
         gROME->Print(tree->GetName());
         gROME->Print(runNumberString.Data());
         gROME->Println(".root");
         fTreeFile->cd();
         tree->Write("",TObject::kOverwrite);
      }
   }
   gROME->Println();

   // Write Histos
   filename.SetFormatted("%s%s%s.root",gROME->GetOutputDir(),"histos",runNumberString.Data());
   fHistoFile = new TFile(filename.Data(),"RECREATE");
   TFolder *folder = (TFolder*)gROOT->FindObjectAny("histos");
   folder->Write();
   fHistoFile->Close();

   if (!gROME->GetActiveDAQ()->Disconnect())
      return false;

   return true;
}

bool ROMEEventLoop::Termination() {
   // Clean up the analyzer. Called after the Terminate tasks.
   // Write accumulative output tree files
   // Close all files
   ROMEString runNumberString;
   gROME->GetCurrentRunNumberString(runNumberString);
   ROMETree *romeTree;
   TTree *tree;
   for (int j=0;j<gROME->GetTreeObjectEntries();j++) {
      romeTree = gROME->GetTreeObjectAt(j);
      if (romeTree->isWrite()) {
         if (gROME->isTreeAccumulation()) {
            tree = romeTree->GetTree();
            gROME->Print("\nWriting Root-File ");
            gROME->Print(tree->GetName());
            gROME->Print(runNumberString.Data());
            gROME->Println(".root");
            fTreeFiles[j]->cd();
            tree->Write("",TObject::kOverwrite);
         }
         if (fTreeFiles[j]!=NULL) {
            fTreeFiles[j]->Close();
         }
      }
   }

   if (!gROME->GetActiveDAQ()->Termination())
      return false;

   return true;
}
