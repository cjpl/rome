// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMEEventLoop                                                        //
//                                                                      //
//  Implements the event loop. This Class must be the root Task of 
//  the Application.
//                                                                      //
//  $Log$
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
#if defined( _MSC_VER )
#include <io.h>
#define O_RDONLY_BINARY O_RDONLY | O_BINARY
#endif
#if defined ( __linux__ ) || defined ( __APPLE__ )
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

      if (this->isRunning()) {
         // Task Switches
         if (ROMEAnalyzer::fTaskSwitchesChanged) {
            this->UpdateTaskSwitches();
            ROMEAnalyzer::fTaskSwitchesChanged = false;
         }

         // Begin of Run Tasks
         ExecuteTasks("b");
         CleanTasks();

         // Output
         if (gShowTime) TimeStart();
         cout << "\n\nRun " << gROME->GetCurrentRunNumber() << " started" << endl; 
      }
          
      // Loop over Events
      //------------------
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
         if (ROMEAnalyzer::fTaskSwitchesChanged) {
            this->UpdateTaskSwitches();
            ROMEAnalyzer::fTaskSwitchesChanged = false;
         }
         char eventID = gROME->GetEventID();
         ExecuteTasks(&eventID);
         CleanTasks();
         if (gROME->isTerminationFlag()) {
            cout << "\n\nTerminating Program !" << endl;
            return;
         }

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
      if (this->isEndOfRun() || this->isTerminate()) {
         if (this->isEndOfRun())
            this->SetBeginOfRun();
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

      // Check Run Status
      int state = 0;
      int statesize = sizeof(state);
      if (db_get_value(gROME->GetMidasOnlineDataBase(),0,"/Runinfo/State",&state,&statesize,TID_INT,false)!= CM_SUCCESS) {
         cout << "\nCannot read run status from the online database" << endl;
         return false;
      }
      if (state!=3) {
         this->SetBeginOfRun();
         this->SetStopped();
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
#if defined( __ppc__ )
      //byte swapping
      if(((EVENT_HEADER*)mEvent)->event_id != EVENTID_BOR &&
              ((EVENT_HEADER*)mEvent)->event_id != EVENTID_EOR &&
              ((EVENT_HEADER*)mEvent)->event_id != EVENTID_MESSAGE)
              bk_swap((EVENT_HEADER*)mEvent + 1, 0);
#endif
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
#if defined( __ppc__ )
         //byte swapping
         ByteSwap((UShort_t *)&pevent->event_id);
         ByteSwap((UShort_t *)&pevent->trigger_mask);
         ByteSwap((UInt_t   *)&pevent->serial_number);
         ByteSwap((UInt_t   *)&pevent->time_stamp);
         ByteSwap((UInt_t   *)&pevent->data_size);
#endif
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
#if defined( __ppc__ )
      //byte swapping
      if(pevent->event_id != EVENTID_BOR &&
         pevent->event_id != EVENTID_EOR &&
         pevent->event_id != EVENTID_MESSAGE)
         bk_swap(pevent + 1, 0);
#endif
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

   if (!fContinuous || ((fProgressDelta==1 || !((int)gROME->GetTriggerStatistics()->processedEvents%fProgressDelta) && fProgressWrite))) {
      cout << (int)gROME->GetTriggerStatistics()->processedEvents << " events processed                                                    \r" << flush;

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
      gROME->GetApplication()->ProcessLine(ROMEAnalyzer::LineToProcess);
      ROMEAnalyzer::LineToProcess = NULL;
   }
}
bool ROMEEventLoop::UserInput() 
{
   // Looks for user input. Called before the Event tasks.
   bool wait = false;
   bool first = true;
   bool interpreter = false;
   bool hit = false;

   if (fStopAtRun==gROME->GetCurrentRunNumber() && fStopAtEvent==gROME->GetCurrentEventNumber()) {
      cout << "Stopped at event " << gROME->GetCurrentEventNumber() << "                      \r" << flush;
      wait = true;
   }
   else if (fContinuous && time(NULL) < fUserInputLastTime+0.1)
      return true;
   time(&fUserInputLastTime);

   while (wait||first) {
      first = false;
      if (!fContinuous)
         wait = true;

      CheckLineToProcess();

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
            cout << "Stopped at event " << gROME->GetCurrentEventNumber() << "                      \r" << flush;
            wait = true;
         }
         if (ch == 'r') {

            if (fContinuous)
               cout << "                                  \r" << flush;

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
         if (ch == 'g') {
            char *cstop;
            ROMEString number;
            // run number
            cout << "                                  \r" << flush;
            cout << "Run number :";
            while (true) {
               ch = gROME->ss_getchar(0);
               if (ch == 0)
                  continue;
               if (ch == 13)
                  break;
               cout << ch;
               number += ch;
            }
            cout << "                                  \r" << flush;
            int inumber = strtol(number.Data(),&cstop,10);
            if (inumber!=0) 
               fStopAtRun = inumber;
            else
               fStopAtRun = gROME->GetCurrentRunNumber();
            // event number
            number.Resize(0);
            cout << "                                  \r" << flush;
            cout << "Event number :";
            while (true) {
               ch = gROME->ss_getchar(0);
               if (ch == 0)
                  continue;
               if (ch == 13)
                  break;
               cout << ch;
               number += ch;
            }
            cout << "                                  \r" << flush;
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
         cout << endl << "End of event number " << gROME->GetCurrentEventNumber() << " of run number " << gROME->GetCurrentRunNumber() << endl;
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
         cout << "Writing Root-File " << tree->GetName() << runNumberString.Data() << ".root" << endl;
         fTreeFile->cd();
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



#if defined( __ppc__ )
#if !defined(HAVE_MIDAS)
/**
Swaps bytes from little endian to big endian or vice versa for a whole event.

An event contains a flag which is set by bk_init() to identify
the endian format of an event. If force is FALSE, this flag is evaluated and
the event is only swapped if it is in the "wrong" format for this system.
An event can be swapped to the "wrong" format on purpose for example by a
front-end which wants to produce events in a "right" format for a back-end
analyzer which has different byte ordering.
@param event pointer to data area of event
@param force If TRUE, the event is always swapped, if FALSE, the event
is only swapped if it is in the wrong format.
@return 1==event has been swap, 0==event has not been swapped.
*/
void ROMEEventLoop::bk_swap(void *event, bool force)
{
   BANK_HEADER *pbh;
   BANK *pbk;
   BANK32 *pbk32;
   void *pdata;
   UShort_t type;
   bool b32;
   
   pbh = (BANK_HEADER *) event;
   
   // only swap if flags in high 16-bit
   if (pbh->flags < 0x10000 && !force)
      return;
   
   // swap bank header 
   ByteSwap((UInt_t   *)&pbh->data_size);
   ByteSwap((UInt_t   *)&pbh->flags);   
   
   // check for 32bit banks
   b32 = ((pbh->flags & (1<<4)) > 0);
   
   pbk = (BANK *) (pbh + 1);
   pbk32 = (BANK32 *) pbk;
   
   // scan event
   while ((Seek_t) pbk - (Seek_t) pbh < (Int_t) pbh->data_size + (Int_t) sizeof(BANK_HEADER)) {
      // swap bank header
      if (b32) {
         ByteSwap((UInt_t *)&pbk32->type);
         ByteSwap((UInt_t *)&pbk32->data_size);
         pdata = pbk32 + 1;
         type = (UShort_t) pbk32->type;
      } else {
         ByteSwap((UShort_t *)&pbk->type);
         ByteSwap((UShort_t *)&pbk->data_size);
         pdata = pbk + 1;
         type = pbk->type;
      }
      
      // pbk points to next bank
      if (b32) {
         pbk32 = (BANK32 *) ((char *) (pbk32 + 1) + ALIGN8(pbk32->data_size));
         pbk = (BANK *) pbk32;
      } else {
         pbk = (BANK *) ((char *) (pbk + 1) + ALIGN8(pbk->data_size));
         pbk32 = (BANK32 *) pbk;
      }
      
      switch (type) {
         case TID_WORD:
         case TID_SHORT:
            while ((Seek_t) pdata < (Seek_t) pbk) {
               ByteSwap((UShort_t*)pdata);
               pdata = (void *) (((UShort_t *) pdata) + 1);
            }
            break;
            
         case TID_DWORD:
         case TID_INT:
         case TID_BOOL:
         case TID_FLOAT:
            while ((Seek_t) pdata < (Seek_t) pbk) {
               ByteSwap((UInt_t*)pdata);
               pdata = (void *) (((UInt_t *) pdata) + 1);
            }
            break;
            
         case TID_DOUBLE:
            while ((Seek_t) pdata < (Seek_t) pbk) {
               ByteSwap((ULong64_t*)pdata);
               pdata = (void *) (((ULong64_t *) pdata) + 1);
            }
            break;
      }
   }   
   return;
}
#endif //if !defined(HAVE_MIDAS)

// Byte swapping big endian <-> little endian
void ROMEEventLoop::ByteSwap(UShort_t *x) 
{
   Byte_t _tmp;
   _tmp= *((Byte_t *)(x));
   *((Byte_t *)(x)) = *(((Byte_t *)(x))+1);
   *(((Byte_t *)(x))+1) = _tmp;
}

void ROMEEventLoop::ByteSwap(UInt_t *x) 
{
   Byte_t _tmp;
   _tmp= *((Byte_t *)(x));
   *((Byte_t *)(x)) = *(((Byte_t *)(x))+3);
   *(((Byte_t *)(x))+3) = _tmp;
   _tmp= *(((Byte_t *)(x))+1);
   *(((Byte_t *)(x))+1) = *(((Byte_t *)(x))+2); 
   *(((Byte_t *)(x))+2) = _tmp;
}

void ROMEEventLoop::ByteSwap(ULong64_t *x) { 
   Byte_t _tmp;
   _tmp= *((Byte_t *)(x));
   *((Byte_t *)(x)) = *(((Byte_t *)(x))+7);
   *(((Byte_t *)(x))+7) = _tmp;
   _tmp= *(((Byte_t *)(x))+1);
   *(((Byte_t *)(x))+1) = *(((Byte_t *)(x))+6);
   *(((Byte_t *)(x))+6) = _tmp;
   _tmp= *(((Byte_t *)(x))+2);
   *(((Byte_t *)(x))+2) = *(((Byte_t *)(x))+5);
   *(((Byte_t *)(x))+5) = _tmp;
   _tmp= *(((Byte_t *)(x))+3);
   *(((Byte_t *)(x))+3) = *(((Byte_t *)(x))+4);
   *(((Byte_t *)(x))+4) = _tmp;
}
#endif // if defined( __ppc__ )
