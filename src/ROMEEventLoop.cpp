// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMEEventLoop                                                        //
//                                                                      //
//  Implements the event loop. This Class must be the root Task of 
//  the Application.
//                                                                      //
//  $Log$
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
#if defined( _MSC_VER )
#include <io.h>
#define O_RDONLY_BINARY O_RDONLY | O_BINARY
#endif
#if defined ( __linux__ )
#include <unistd.h>
#define O_RDONLY_BINARY O_RDONLY
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "ROME.h"
#include <ROMEEventLoop.h>
#include <Riostream.h>

TTask *TTask::fgBeginTask  = 0;
TTask *TTask::fgBreakPoint = 0;

#if defined HAVE_MIDAS
#include <midas.h>
#define MIDAS_DEBUG // define if you want to run the analyzer in the debugger
void ProcessMessage(int hBuf, int id, EVENT_HEADER * pheader, void *message)
{
// This method is called, when a system message from the online system occurs
}
#endif

ROMEEventLoop::ROMEEventLoop(const char *name,const char *title):ROMETask(name,title)
{
   this->SetRunning();
   this->SetAnalyze();
   fSequentialNumber = 0;
   fTreeInfo = new ROMETreeInfo();
   fContinuous = true;
   fUserInputLastTime = 0;
}

#include <TBrowser.h>
void ROMEEventLoop::ExecuteTask(Option_t *option)
{
   if (!strcmp(option,"init")) {
      this->InitTrees();
      return;
   }
   if (gROME->isOnline()) {
      this->SetStopped();
   }
   else {
      this->SetRunning();
   }

// Event loop
   if (fgBeginTask) {
      Error("ExecuteTask","Cannot execute task:%s, already running task: %s",GetName(),fgBeginTask->GetName());
      this->Termination();
      gROME->SetTerminationFlag();
      cout << "\n\nTerminating Program !" << endl;
      return;
   }
   if (!IsActive()) {
      this->Termination();
      gROME->SetTerminationFlag();
      cout << "\n\nTerminating Program !" << endl;
      return;
   }

   // Declarations
   //--------------
   int i,ii;

// Read Histos
//   sprintf(filename,"%s%s%s.root",gROME->GetInputDir(),"histos",runNumber);
//   gROME->SetHistoFileHandle(new TFile(filename,"READ"));

   // Initialisation
   //----------------

   if (!this->Initialize()) {
      this->Termination();
      gROME->SetTerminationFlag();
      cout << "\n\nTerminating Program !" << endl;
      return;
   }
 
   ExecuteTasks("i");
   CleanTasks();

   // Loop over Runs
   //----------------
   for (ii=0;!this->isTerminate();ii++) {

      if (!this->Connect(ii)) {
         this->Termination();
         gROME->SetTerminationFlag();
         cout << "\n\nTerminating Program !" << endl;
         return;
      }
      if (this->isTerminate()) {
         break;
      }

     // Begin of Run Tasks
      if (ROMEAnalyzer::fTaskSwitchesChanged) {
         this->UpdateTaskSwitches();
         ROMEAnalyzer::fTaskSwitchesChanged = false;
      }
      if (this->isRunning()) {
         ExecuteTasks("b");
         CleanTasks();
      }
	  
      // Loop over Events
      //------------------
      if (gShowTime) TimeStart();
      cout << "\n\nRun " << gROME->GetCurrentRunNumber() << " started" << endl; 
      for (i=0;!this->isTerminate()&&!this->isEndOfRun();i++) {

         // User Input
         if (!this->UserInput()) {
            this->Termination();
            gROME->SetTerminationFlag();
            cout << "\n\nTerminating Program !" << endl;
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
            cout << "\n\nTerminating Program !" << endl;
            return;
         }
         if (this->isEndOfRun()) {
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
         if (ROMEAnalyzer::fTaskSwitchesChanged) {
            this->UpdateTaskSwitches();
            ROMEAnalyzer::fTaskSwitchesChanged = false;
         }
         char eventID = gROME->GetEventID();
         ExecuteTasks(&eventID);
         CleanTasks();

         // Write Event
         if (!this->WriteEvent() && gROME->isFillEvent()) {
            this->Termination();
            gROME->SetTerminationFlag();
            cout << "\n\nTerminating Program !" << endl;
            return;
         }

         // Update
         if (!this->Update()) {
            this->Termination();
            gROME->SetTerminationFlag();
            cout << "\n\nTerminating Program !" << endl;
            return;
         }
      }
      if (gShowTime) TimeEnd();

      // Show number of processed events
      cout << "Run " << gROME->GetCurrentRunNumber() << " stopped                                             " << endl << endl; 
      cout << (int)gROME->GetProcessedEvents() << " events processed" << endl <<endl;
      if (gShowTime) {
         cout << "run time = " << GetTime() << endl << endl;
      }

      // End of Run Tasks
      ExecuteTasks("e");
      CleanTasks();

      // Disconnect
      if (!this->Disconnect()) {
         this->Termination();
         gROME->SetTerminationFlag();
         cout << "\n\nTerminating Program !" << endl;
         return;
      }
   }

   // Terminate Tasks
   ExecuteTasks("t");
   CleanTasks();

   // Root Interpreter
   if (!gROME->isBatchMode()) {
      gROME->GetApplication()->Run(true);
      cout << endl;
   }

   // Terminate
   if (!this->Termination()) {
      gROME->SetTerminationFlag();
      cout << "\n\nTerminating Program !" << endl;
      return;
   }
}


bool ROMEEventLoop::Initialize() {
   // Initialize the analyzer. Called before the init tasks.
   int j;
   this->InitTaskSwitches();
   this->InitSingleFolders();

   // Accumulative output tree file initialisation
   fTreeFiles = new TFile*[gROME->GetTreeObjectEntries()];
   ROMEString filename;
   ROMEString runNumberString;
   ROMETree *romeTree;
   TTree *tree;
   gROME->GetRunNumberStringAt(runNumberString,0);
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

   if (gROME->isOnline()&&gROME->isMidas()) {
#if defined HAVE_MIDAS
      // Connect to the Frontend
      int requestId,i;

      cout << "Program is running online." << endl << endl;

      // Connect to the experiment
      if (cm_connect_experiment(gROME->GetOnlineHost(), gROME->GetOnlineExperiment(),gROME->GetProgramName(), NULL) != SUCCESS) {
         cout << "\nCannot connect to experiment" << endl;
         return false;
      }

      // open the "system" buffer, 1M size
      bm_open_buffer("SYSTEM", EVENT_BUFFER_SIZE, &fMidasBuffer);

      // set the buffer cache size
      bm_set_cache_size(fMidasBuffer, 100000, 0);

      // place a request for a specific event id
      bm_request_event(fMidasBuffer, 1, TRIGGER_ALL,GET_SOME, &requestId,NULL);

      // place a request for system messages
      cm_msg_register(ProcessMessage);

      // turn off watchdog if in debug mode
#ifdef MIDAS_DEBUG
      cm_set_watchdog_params(TRUE, 0);
#endif

      // Registers a callback function for run transitions.
      if (cm_register_transition(TR_START, NULL ,500) != CM_SUCCESS ||
         cm_register_transition(TR_STOP, NULL, 500) != CM_SUCCESS) {
         cout << "\nCannot connect to experiment" << endl;
         return false;
      }

      // Connect to the online database
      if (cm_get_experiment_database(gROME->GetMidasOnlineDataBasePointer(), NULL)!= CM_SUCCESS) {
         cout << "\nCannot connect to the online database" << endl;
         return false;
      }

      // Get Runnumber
      int runNumber = 0;
      int size = sizeof(runNumber);
      if (db_get_value(gROME->GetMidasOnlineDataBase(),0,"/Runinfo/Run number",&runNumber,&size,TID_INT,false)!= CM_SUCCESS) {
         cout << "\nCannot read runnumber from the online database" << endl;
         return false;
      }
      gROME->SetCurrentRunNumber(runNumber);

      // Initialize the online database
      HNDLE hKey;
      ROMEString str;
      str = "//Trigger/Statistics";
      str.Insert(1,gROME->GetProgramName());
      // Trigger Statistics
      char *triggerStatisticsString =  "Events received = DOUBLE : 0\nEvents per sec. = DOUBLE : 0\nEvents written = DOUBLE : 0\n";
      db_check_record(gROME->GetMidasOnlineDataBase(), 0, (char*)str.Data(), triggerStatisticsString, TRUE);
      db_find_key(gROME->GetMidasOnlineDataBase(), 0, (char*)str.Data(), &hKey);
      if (db_open_record(gROME->GetMidasOnlineDataBase(), hKey, gROME->GetTriggerStatistics(), sizeof(Statistics), MODE_WRITE, NULL, NULL) != DB_SUCCESS) {
         cout << "\nCannot open trigger statistics record, probably other analyzer is using it" << endl;
         return false;
      }

      // Scaler Statistics
      char *fScalerStatisticsString =  "Events received = DOUBLE : 0\nEvents per sec. = DOUBLE : 0\nEvents written = DOUBLE : 0\n";
      str="//Scaler/Statistics";
      str.Insert(1,gROME->GetProgramName());
      db_check_record(gROME->GetMidasOnlineDataBase(), 0, (char*)str.Data(), fScalerStatisticsString, TRUE);
      db_find_key(gROME->GetMidasOnlineDataBase(), 0, (char*)str.Data(), &hKey);
      if (db_open_record(gROME->GetMidasOnlineDataBase(), hKey, gROME->GetScalerStatistics(), sizeof(Statistics), MODE_WRITE, NULL, NULL) != DB_SUCCESS) {
         cout << "\nCannot open scaler statistics record, probably other analyzer is using it" << endl;
         return false;
      }

      // Tree Switches
      for (i=0;i<gROME->GetTreeObjectEntries();i++) {
         str="//Tree switches/";
         str.Insert(1,gROME->GetProgramName());
         str.Append(gROME->GetTreeObjectAt(i)->GetTree()->GetName());
         db_check_record(gROME->GetMidasOnlineDataBase(), 0, (char*)str.Data(), gROME->GetTreeObjectAt(i)->GetSwitchesString(), TRUE);
         db_find_key(gROME->GetMidasOnlineDataBase(), 0, (char*)str.Data(), &hKey);
         if (db_set_record(gROME->GetMidasOnlineDataBase(),hKey,gROME->GetTreeObjectAt(i)->GetSwitches(),gROME->GetTreeObjectAt(i)->GetSwitchesSize(),0) != DB_SUCCESS) {
            cout << "\nCannot write to tree switches record." << endl;
            return false;
         }
         if (db_open_record(gROME->GetMidasOnlineDataBase(), hKey, gROME->GetTreeObjectAt(i)->GetSwitches(), gROME->GetTreeObjectAt(i)->GetSwitchesSize(), MODE_READ, NULL, NULL) != DB_SUCCESS) {
            cout << "\nCannot open tree switches record, probably other analyzer is using it" << endl;
            return false;
         }
      }

      // Experiment dependent ODB initializations
      this->InitODB();

      // Set Event Status
      this->SetAnalyze();
#else
      cout << "Need Midas support for Online Mode !!" << endl;
      cout << "Please link the midas library into this project." << endl;
      return false;
#endif
   }
   else if (gROME->isOnline()&&gROME->isRoot()) {
      cout << "Root mode is not supported for online analysis." << endl << endl;
      return false;
   }
   else if (gROME->isOffline()&&gROME->isMidas()) {
      cout << "Program is running offline." << endl << endl;
   }
   else if (gROME->isOffline()&&gROME->isRoot()) {
      cout << "Program is running offline." << endl << endl;
   }
   else {
      cout << "Severe program failure." << endl << endl;
      return false;
   }

   if (gROME->isOffline()&&gROME->GetNumberOfRunNumbers()<=0) {
      cout << "No run numbers specified." << endl << endl;
      return false;
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
   // Status
   this->SetRunning();
   this->SetAnalyze();

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


   // Update Data Base
   if (!gROME->ReadSingleDataBaseFolders()) {
      cout << "\nError while reading the data base !" << endl;
      return false;
   }
   this->InitArrayFolders();
   if (!gROME->ReadArrayDataBaseFolders()) {
      cout << "\nError while reading the data base !" << endl;
      return false;
   }

   // Progress Display
   fProgressDelta = 10000;
   fProgressTimeOfLastEvent = time(NULL);
   fProgressLastEvent = 0;
   fProgressWrite = false;

   if (gROME->isOnline()&&gROME->isMidas()) {
   }
   else if (gROME->isOffline()&&gROME->isMidas()) {
      // Open Midas File
      filename.SetFormatted("%srun%s.mid",gROME->GetInputDir(),runNumberString.Data());
      fMidasFileHandle = open(filename.Data(),O_RDONLY_BINARY);
      if (fMidasFileHandle==-1) {
         cout << "Inputfile '" << filename.Data() << "' not found." << endl;
         return false;
      }
      cout << "Reading Midas-File run" << runNumberString.Data() << ".mid" << endl;
   }
   else if (gROME->isOffline()&&gROME->isRoot()) {
      // Read Trees
      fRootFiles = new TFile*[gROME->GetTreeObjectEntries()];
      TTree *tree;
      ROMETree *romeTree;
      ROMEString runNumberString;
      gROME->GetCurrentRunNumberString(runNumberString);
      bool treeRead = false;
      fTreePosition = new int[gROME->GetTreeObjectEntries()];
      fTreeNextSeqNumber = new int[gROME->GetTreeObjectEntries()];
      for (int j=0;j<gROME->GetTreeObjectEntries();j++) {
         romeTree = gROME->GetTreeObjectAt(j);
         tree = romeTree->GetTree();
         if (!gROME->isTreeAccumulation()) {
            tree->Reset();
         }
         if (romeTree->isRead()) {
            treeRead = true;
            filename.SetFormatted("%s%s%s.root",gROME->GetInputDir(),tree->GetName(),runNumberString.Data());
            fRootFiles[j] = new TFile(filename.Data(),"READ");
            if (fRootFiles[j]->IsZombie()) {
               cout << "Inputfile '" << filename.Data() << "' not found." << endl;
               return false;
            }
            tree = (TTree*)fRootFiles[j]->FindObjectAny(tree->GetName());
            romeTree->SetTree(tree);
            fTreePosition[j] = 0;
            if (tree->GetEntriesFast()>0) {
               tree->GetBranch("Info")->GetEntry(0);
               fTreeNextSeqNumber[j] = fTreeInfo->GetSequentialNumber();
            }
            else {
               fTreeNextSeqNumber[j] = -1;
            }
         }
      }
      if (!treeRead) {
         cout << "No input root file specified for running in root mode." << endl << endl;
         return false;
      }
      this->ConnectTrees();
   }
   else {
	   cout << "Severe program failure." << endl << endl;
	   return false;
   }
   return true;
}

bool ROMEEventLoop::ReadEvent(Int_t event) {
   // Reads an event. Called before the Event tasks.
   Statistics *stat = gROME->GetTriggerStatistics();
   
   this->SetAnalyze();
   this->ResetFolders();
   int timeStamp = 0;

   if (gROME->IsDontReadNextEvent()) {
      gROME->SetDontReadNextEvent(false);
      return true;
   }

   if (gROME->isOnline()&&gROME->isMidas()) {
#if defined HAVE_MIDAS
      int runNumber,trans;
      if (cm_query_transition(&trans, &runNumber, NULL)) {
         if (trans == TR_START) {
            gROME->SetCurrentRunNumber(runNumber);
            this->SetBeginOfRun();
            this->SetRunning();
            return true;
         }
         if (trans == TR_STOP) {
            this->SetEndOfRun();
            this->SetStopped();
            return true;
         }
      }
      int status = cm_yield(100);
      if (status == RPC_SHUTDOWN || status == SS_ABORT) {
         this->SetTerminate();
         return false;
      }
      if (this->isStopped()) {
         this->SetContinue();
         return true;
      }
      int size = gROME->GetMidasEventSize();
      void* mEvent = gROME->GetMidasEvent();
      status = bm_receive_event(fMidasBuffer, mEvent, &size, ASYNC);
      if (status != BM_SUCCESS) {
         this->SetContinue();
         return true;
      }

      gROME->SetCurrentEventNumber(((EVENT_HEADER*)mEvent)->serial_number);
      gROME->SetEventID(((EVENT_HEADER*)mEvent)->event_id);
      timeStamp = ((EVENT_HEADER*)mEvent)->time_stamp;
      gROME->InitMidasBanks();

      // Update Statistics
      stat->processedEvents++;
      int time;
      time = ss_millitime();
      if (fStatisticsTimeOfLastEvent == 0)
         fStatisticsTimeOfLastEvent = time;
      if (time - fStatisticsTimeOfLastEvent != 0)
         stat->eventsPerSecond = (stat->processedEvents-fStatisticsLastEvent)/(time-fStatisticsTimeOfLastEvent)*1000.0;
      fStatisticsTimeOfLastEvent = time;

      fTreeInfo->SetTimeStamp(timeStamp);
      fStatisticsLastEvent = stat->processedEvents;
#endif
   }
   else if (gROME->isOffline()&&gROME->isMidas()) {
      // read event header
      EVENT_HEADER *pevent = (EVENT_HEADER*)gROME->GetMidasEvent();
      bool readError = false;

      // read event
      int n = read(fMidasFileHandle,pevent, sizeof(EVENT_HEADER));
      if (n < (int)sizeof(EVENT_HEADER)) readError = true;
      else {
         n = 0;
         if (pevent->data_size <= 0) readError = true;
         else {
            n = read(fMidasFileHandle, pevent+1, pevent->data_size);
            if (n != (int) pevent->data_size) readError = true;
//            if ((int) ((BANK_HEADER*)(pevent+1))->data_size <= 0) readError = true;
         }
      }
      // check input
      if (readError) {
         if (n > 0) cout << "Unexpected end of file\n";
         this->SetEndOfRun();
         return true;
      }
      if (pevent->event_id < 0) {
         this->SetContinue();
         return true;
      }
      if (pevent->event_id == EVENTID_EOR) {
         this->SetEndOfRun();
         return true;
      }
      if (pevent->data_size<((BANK_HEADER*)(pevent+1))->data_size) { 
         this->SetContinue();
         return true;
      }

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

      // initalize event
      gROME->SetEventID(pevent->event_id);
      gROME->SetCurrentEventNumber(pevent->serial_number);
      timeStamp = pevent->time_stamp;

      gROME->InitMidasBanks();
      fTreeInfo->SetTimeStamp(timeStamp);
      stat->processedEvents++;
   }
   else if (gROME->isOffline()&&gROME->isRoot()) {
      ROMETree *romeTree;
      TTree *tree;
      bool found = false;
      // read event
      for (int j=0;j<gROME->GetTreeObjectEntries();j++) {
         romeTree = gROME->GetTreeObjectAt(j);
         tree = romeTree->GetTree();
         if (romeTree->isRead()) {
            if (fTreeNextSeqNumber[j]==event) {
               found = true;
               if (tree->GetEntriesFast()>fTreePosition[j]+1) {
                  tree->GetBranch("Info")->GetEntry(fTreePosition[j]+1);
                  fTreeNextSeqNumber[j] = fTreeInfo->GetSequentialNumber();
               }
               else {
                  fTreeNextSeqNumber[j] = -1;
               }
               tree->GetEntry(fTreePosition[j]);
               fTreePosition[j]++;
            }
         }  
      }
      if (!found) {
         this->SetEndOfRun();
         return true;
      }
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
   
      fTreeInfo->SetTimeStamp(timeStamp);
      stat->processedEvents++;
   }
   else {
      cout << "Severe program failure." << endl << endl;
      return false;
   }

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
   if ((!fContinuous || fProgressDelta==1 || !((int)gROME->GetTriggerStatistics()->processedEvents%fProgressDelta) && fProgressWrite)) {
      cout << (int)gROME->GetTriggerStatistics()->processedEvents << " events processed                                                    \r";
      fProgressWrite = false;
   }
 
   // ODB update
#if defined HAVE_MIDAS
   db_send_changed_records();
#endif

   return true;
}

void ROMEEventLoop::CheckLineToProcess()
{
   if (ROMEAnalyzer::LineToProcess){
/*      cout << ROMEAnalyzer::LineToProcess << endl;
*/      gROME->GetApplication()->ProcessLine(ROMEAnalyzer::LineToProcess);
      ROMEAnalyzer::LineToProcess = NULL;
   }
}
bool ROMEEventLoop::UserInput() 
{
   // Looks for user input. Called before the Event tasks.
   bool wait = false;
   bool first = true;
   bool interpreter = false;

   if (fContinuous && time(NULL) < fUserInputLastTime+0.1)
      return true;
   time(&fUserInputLastTime);

   while (wait||first) {

      first = false;
      if (!fContinuous)
         wait = true;

      CheckLineToProcess();

      interpreter = false;
      while (gROME->ss_kbhit()) {
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
            cout << "Stopped                          \r";
            wait = true;
         }
         if (ch == 'r') {
            wait = false;
         }
         if (ch == 'o') {
            cout << "Step by step mode                 " << endl;
            fContinuous = false;
         }
         if (ch == 'c') {
            cout << "Continues mode                    " << endl;
            fContinuous = true;
            wait = false;
         }
         if (ch == 'i') {
            interpreter = true;
            wait = false;
         }
      }
      if (interpreter) {
         gROME->GetApplication()->Run(true);
      }
   }

   return true;
}

bool ROMEEventLoop::Disconnect() {
   // Disconnects the current run. Called after the EndOfRun tasks.

   TFile *f1;
   // Write non accumulative output tree files
   ROMEString filename;
   ROMETree *romeTree;
   TTree *tree;
   ROMEString runNumberString;
   gROME->GetCurrentRunNumberString(runNumberString);
   for (int j=0;j<gROME->GetTreeObjectEntries();j++) {
      romeTree = gROME->GetTreeObjectAt(j);
      if (romeTree->isWrite() && !gROME->isTreeAccumulation()) {
         tree = romeTree->GetTree();
         cout << "Writing Root-File " << tree->GetName() << runNumberString.Data() << ".root" << endl;
         tree->Write("",TObject::kOverwrite);
      }
   }
   cout << endl;

   // Write Histos
   filename.SetFormatted("%s%s%s.root",gROME->GetOutputDir(),"histos",runNumberString.Data());
   fHistoFile = new TFile(filename.Data(),"RECREATE");
   TFolder *folder = (TFolder*)gROOT->FindObjectAny("histos");
   folder->Write();
   fHistoFile->Close();

   if (gROME->isOnline()&&gROME->isMidas()) {
   }
   else if (gROME->isOffline()&&gROME->isMidas()) {
      close(fMidasFileHandle);
   }
   else if (gROME->isOffline()&&gROME->isRoot()) {
      for (int j=0;j<gROME->GetTreeObjectEntries();j++) {
         if (gROME->GetTreeObjectAt(j)->isRead()) fRootFiles[j]->Close();
      }
      delete [] fRootFiles;
   }
   else {
      cout << "Severe program failure." << endl << endl;
      return false;
   }
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
            cout << endl << "Writing Root-File " << tree->GetName() << runNumberString.Data() << ".root" << endl;
            fTreeFiles[j]->cd();
            tree->Write("",TObject::kOverwrite);
         }
         if (fTreeFiles[j]!=NULL) {
            fTreeFiles[j]->Close();
         }
      }
   }
   if (gROME->isOnline()&&gROME->isMidas()) {
#if defined HAVE_MIDAS
      cm_disconnect_experiment();
#endif
   }
   else if (gROME->isOffline()&&gROME->isMidas()) {
   }
   else if (gROME->isOffline()&&gROME->isRoot()) {
   }
   else {
      cout << "Severe program failure." << endl << endl;
      return false;
   }
   return true;
}

