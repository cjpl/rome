// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMEEventLoop                                                        //
//                                                                      //
//  Implements the event loop. This Class must be the root Task of 
//  the Application.
//                                                                      //
//  $Log$
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
#define O_RDONLY_BINARY O_RDONLY
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include <ROMEEventLoop.h>
#include <Riostream.h>

TTask *TTask::fgBeginTask  = 0;
TTask *TTask::fgBreakPoint = 0;

ROMEAnalyzer *gAnalyzer;

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

void ROMEEventLoop::ExecuteTask(Option_t *option)
{
   if (!strcmp(option,"init")) {
      gAnalyzer = (ROMEAnalyzer*)gROME;
      this->InitTrees();
      return;
   }

// Event loop
   if (fgBeginTask) {
      Error("ExecuteTask","Cannot execute task:%s, already running task: %s",GetName(),fgBeginTask->GetName());
      this->Termination();
      gAnalyzer->SetTerminationFlag();
      cout << "\n\nTerminating Program !" << endl;
      return;
   }
   if (!IsActive()) {
      this->Termination();
      gAnalyzer->SetTerminationFlag();
      cout << "\n\nTerminating Program !" << endl;
      return;
   }

   // Declarations
   //--------------
   int i,ii;

// Read Histos
//   sprintf(filename,"%s%s%s.root",gAnalyzer->GetInputDir(),"histos",runNumber);
//   gAnalyzer->SetHistoFileHandle(new TFile(filename,"READ"));

   // Initialisation
   //----------------

   if (!this->Initialize()) {
      this->Termination();
      gAnalyzer->SetTerminationFlag();
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
         gAnalyzer->SetTerminationFlag();
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
      ExecuteTasks("b");
      CleanTasks();
	  
      // Loop over Events
      //------------------
      if (gShowTime) TimeStart();
      cout << "\n\nRun " << gAnalyzer->GetCurrentRunNumber() << " started" << endl; 
      for (i=0;!this->isTerminate()&&!this->isEndOfRun();i++) {

         // User Input
         if (!this->UserInput()) {
            this->Termination();
            gAnalyzer->SetTerminationFlag();
            cout << "\n\nTerminating Program !" << endl;
            return;
         }
         if (this->isTerminate()) {
            break;
         }

         // Set Fill Event equal true
         gAnalyzer->SetFillEvent();

         // Read Event
         if (!this->ReadEvent(i)) {
            this->Termination();
            gAnalyzer->SetTerminationFlag();
            cout << "\n\nTerminating Program !" << endl;
            return;
         }
         if (this->isEndOfRun()) {
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
         char eventID = gAnalyzer->GetEventID();
         ExecuteTasks(&eventID);
         CleanTasks();

         // Write Event
         if (!this->WriteEvent() && gAnalyzer->isFillEvent()) {
            this->Termination();
            gAnalyzer->SetTerminationFlag();
            cout << "\n\nTerminating Program !" << endl;
            return;
         }

         // Update
         if (!this->Update()) {
            this->Termination();
            gAnalyzer->SetTerminationFlag();
            cout << "\n\nTerminating Program !" << endl;
            return;
         }
      }
      if (gShowTime) TimeEnd();

      // Show number of processed events
      cout << "Run " << gAnalyzer->GetCurrentRunNumber() << " stopped                                             " << endl << endl; 
      cout << (int)gAnalyzer->GetProcessedEvents() << " events processed" << endl <<endl;
      if (gShowTime) {
         cout << "run time = " << GetTime() << endl << endl;
      }

      // End of Run Tasks
      ExecuteTasks("e");
      CleanTasks();

      // Disconnect
      if (!this->Disconnect()) {
         this->Termination();
         gAnalyzer->SetTerminationFlag();
         cout << "\n\nTerminating Program !" << endl;
         return;
      }
   }

   // Terminate Tasks
   ExecuteTasks("t");
   CleanTasks();

   // Terminate
   if (!this->Termination()) {
      gAnalyzer->SetTerminationFlag();
      cout << "\n\nTerminating Program !" << endl;
      return;
   }
}


bool ROMEEventLoop::Initialize() {
   // Initialize the analyzer. Called before the init tasks.
   int j;
   this->InitFolders();
   this->InitTaskSwitches();

   // Tree file Initialisation
   fTreeFiles = new TFile*[gAnalyzer->GetTreeObjectEntries()];
   ROMEString filename;
   ROMEString runNumberString;
   ROMETree *romeTree;
   TTree *tree;
   gAnalyzer->GetCurrentRunNumberString(runNumberString);
   for (j=0;j<gAnalyzer->GetTreeObjectEntries();j++) {
      fTreeFiles[j] = NULL;
      romeTree = gAnalyzer->GetTreeObjectAt(j);
      if (romeTree->isWrite() && gAnalyzer->isTreeAccumulation()) {
         tree = romeTree->GetTree();
         filename = gAnalyzer->GetOutputDir();
         filename.Append(tree->GetName());         
         filename.Append(runNumberString.Data());
         filename.Append(".root");
         fTreeFiles[j] = new TFile(filename.Data(),"RECREATE");
         tree->SetDirectory(fTreeFiles[j]);
      }
   }

   if (gAnalyzer->isOnline()&&gAnalyzer->isMidas()) {
#if defined HAVE_MIDAS
      // Connect to the Frontend
      int requestId,i;

      cout << "Program is running online." << endl << endl;

      // Connect to the experiment
      if (cm_connect_experiment(gAnalyzer->GetOnlineHost(), gAnalyzer->GetOnlineExperiment(),gAnalyzer->GetProgramName(), NULL) != SUCCESS) {
         cout << "Cannot connect to experiment" << endl;
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
      if (cm_register_transition(TR_START, NULL) != CM_SUCCESS ||
         cm_register_transition(TR_STOP, NULL) != CM_SUCCESS) {
         cout << "Cannot connect to experiment" << endl;
         return false;
      }

      // Connect to the online database
      if (cm_get_experiment_database(gAnalyzer->GetMidasOnlineDataBasePointer(), NULL)!= CM_SUCCESS) {
         cout << "Cannot connect to the online database" << endl;
         return false;
      }

      // Get Runnumber
      int runNumber = 0;
      int size = sizeof(runNumber);
      if (db_get_value(gAnalyzer->GetMidasOnlineDataBase(),0,"/Runinfo/Run number",&runNumber,&size,TID_INT,false)!= CM_SUCCESS) {
         cout << "Cannot read runnumber from the online database" << endl;
         return false;
      }
      gAnalyzer->SetCurrentRunNumber(runNumber);

      // Initialize the online database
      HNDLE hKey;
      ROMEString str;
      str = "//Trigger/Statistics";
      str.Insert(1,gAnalyzer->GetProgramName());
      // Trigger Statistics
      char *triggerStatisticsString =  "Events received = DOUBLE : 0\nEvents per sec. = DOUBLE : 0\nEvents written = DOUBLE : 0\n";
      db_check_record(gAnalyzer->GetMidasOnlineDataBase(), 0, (char*)str.Data(), triggerStatisticsString, TRUE);
      db_find_key(gAnalyzer->GetMidasOnlineDataBase(), 0, (char*)str.Data(), &hKey);
      if (db_open_record(gAnalyzer->GetMidasOnlineDataBase(), hKey, gAnalyzer->GetTriggerStatistics(), sizeof(Statistics), MODE_WRITE, NULL, NULL) != DB_SUCCESS) {
         cout << "Cannot open trigger statistics record, probably other analyzer is using it" << endl;
         return false;
      }

      // Scaler Statistics
      char *fScalerStatisticsString =  "Events received = DOUBLE : 0\nEvents per sec. = DOUBLE : 0\nEvents written = DOUBLE : 0\n";
      str="//Scaler/Statistics";
      str.Insert(1,gAnalyzer->GetProgramName());
      db_check_record(gAnalyzer->GetMidasOnlineDataBase(), 0, (char*)str.Data(), fScalerStatisticsString, TRUE);
      db_find_key(gAnalyzer->GetMidasOnlineDataBase(), 0, (char*)str.Data(), &hKey);
      if (db_open_record(gAnalyzer->GetMidasOnlineDataBase(), hKey, gAnalyzer->GetScalerStatistics(), sizeof(Statistics), MODE_WRITE, NULL, NULL) != DB_SUCCESS) {
         cout << "Cannot open scaler statistics record, probably other analyzer is using it" << endl;
         return false;
      }

      // Tree Switches
      for (i=0;i<gAnalyzer->GetTreeObjectEntries();i++) {
         str="//Tree switches/";
         str.Insert(1,gAnalyzer->GetProgramName());
         str.Append(gAnalyzer->GetTreeObjectAt(i)->GetTree()->GetName());
         db_check_record(gAnalyzer->GetMidasOnlineDataBase(), 0, (char*)str.Data(), gAnalyzer->GetTreeObjectAt(i)->GetSwitchesString(), TRUE);
         db_find_key(gAnalyzer->GetMidasOnlineDataBase(), 0, (char*)str.Data(), &hKey);
         if (db_set_record(gAnalyzer->GetMidasOnlineDataBase(),hKey,gAnalyzer->GetTreeObjectAt(i)->GetSwitches(),gAnalyzer->GetTreeObjectAt(i)->GetSwitchesSize(),0) != DB_SUCCESS) {
            cout << "Cannot write to tree switches record." << endl;
            return false;
         }
         if (db_open_record(gAnalyzer->GetMidasOnlineDataBase(), hKey, gAnalyzer->GetTreeObjectAt(i)->GetSwitches(), gAnalyzer->GetTreeObjectAt(i)->GetSwitchesSize(), MODE_READ, NULL, NULL) != DB_SUCCESS) {
            cout << "Cannot open tree switches record, probably other analyzer is using it" << endl;
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
   else if (gAnalyzer->isOnline()&&gAnalyzer->isRoot()) {
      cout << "Root mode is not supported for online analysis." << endl << endl;
      return false;
   }
   else if (gAnalyzer->isOffline()&&gAnalyzer->isMidas()) {
      cout << "Program is running offline." << endl << endl;
   }
   else if (gAnalyzer->isOffline()&&gAnalyzer->isRoot()) {
      cout << "Program is running offline." << endl << endl;
   }
   else {
      cout << "Severe program failure." << endl << endl;
      return false;
   }

   if (gAnalyzer->isOffline()&&gAnalyzer->GetNumberOfRunNumbers()<=0) {
      cout << "No run numbers specified." << endl << endl;
      return false;
   }

   // Data Base Initialisation
   char* xx = gAnalyzer->GetDataBaseConnection();
   if (!gAnalyzer->GetDataBase()->Init(gAnalyzer->GetDataBaseDir(),gAnalyzer->GetDataBaseConnection()))
      return false;

   return true;
}
bool ROMEEventLoop::Connect(Int_t runNumberIndex) {
   // Connect the Analyzer to the current run. Called before the BeginOfRun tasks.
   ROMEString runNumberString;
   // Statistics
   Statistics *stat = gAnalyzer->GetTriggerStatistics();
   stat->processedEvents = 0;
   stat->eventsPerSecond = 0;
   stat->writtenEvents = 0;
   fStatisticsTimeOfLastEvent = 0;
   fStatisticsLastEvent = 0;
   // Progress Display
   fProgressDelta = 10000;
   fProgressTimeOfLastEvent = time(NULL);
   fProgressLastEvent = 0;
   fProgressWrite = false;
   // Status
   fRunStatus = kRunning;
   fEventStatus = kAnalyze;

   if (gAnalyzer->isOffline()) {
      if (gAnalyzer->GetNumberOfRunNumbers()<=runNumberIndex) {
         this->SetTerminate();
         return true;
      }
      gAnalyzer->SetCurrentRunNumber(gAnalyzer->GetRunNumberAt(runNumberIndex));
   }
   fTreeInfo->SetRunNumber(gAnalyzer->GetCurrentRunNumber());
   gAnalyzer->GetCurrentRunNumberString(runNumberString);

   ROMEString filename;
   ROMETree *romeTree;
   TTree *tree;
   for (int j=0;j<gAnalyzer->GetTreeObjectEntries();j++) {
      romeTree = gAnalyzer->GetTreeObjectAt(j);
      if (romeTree->isWrite() && !gAnalyzer->isTreeAccumulation()) {
         tree = romeTree->GetTree();
         filename.SetFormatted("%s%s%s.root",gAnalyzer->GetOutputDir(),tree->GetName(),runNumberString.Data());
         fTreeFiles[j] = new TFile(filename.Data(),"RECREATE");
         tree->SetDirectory(fTreeFiles[j]);
      }
   }

   // Update Data Base
   if (!gAnalyzer->ReadDataBase())
      return false;

   if (gAnalyzer->isOnline()&&gAnalyzer->isMidas()) {
   }
   else if (gAnalyzer->isOffline()&&gAnalyzer->isMidas()) {
      // Open Midas File
      filename.SetFormatted("%srun%s.mid",gAnalyzer->GetInputDir(),runNumberString.Data());
      fMidasFileHandle = open(filename.Data(),O_RDONLY_BINARY);
      if (fMidasFileHandle==-1) {
         cout << "Inputfile '" << filename.Data() << "' not found." << endl;
         return false;
      }
      cout << "Reading Midas-File run" << runNumberString.Data() << ".mid" << endl;
   }
   else if (gAnalyzer->isOffline()&&gAnalyzer->isRoot()) {
      // Read Trees
      fRootFiles = new TFile*[gAnalyzer->GetTreeObjectEntries()];
      TTree *tree;
      ROMETree *romeTree;
      ROMEString runNumberString;
      gAnalyzer->GetCurrentRunNumberString(runNumberString);
      bool treeRead = false;
      fTreePosition = new int[gAnalyzer->GetTreeObjectEntries()];
      fTreeNextSeqNumber = new int[gAnalyzer->GetTreeObjectEntries()];
      for (int j=0;j<gAnalyzer->GetTreeObjectEntries();j++) {
         romeTree = gAnalyzer->GetTreeObjectAt(j);
         tree = romeTree->GetTree();
         if (!gAnalyzer->isTreeAccumulation()) {
            tree->Reset();
         }
         if (romeTree->isRead()) {
            treeRead = true;
            filename.SetFormatted("%s%s%s.root",gAnalyzer->GetInputDir(),tree->GetName(),runNumberString.Data());
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
   Statistics *stat = gAnalyzer->GetTriggerStatistics();
   
   fEventStatus = kAnalyze;
   this->CleanUpFolders();
   int timeStamp = 0;

   if (gAnalyzer->IsDontReadNextEvent()) {
      gAnalyzer->SetDontReadNextEvent(false);
      return true;
   }

   if (gAnalyzer->isOnline()&&gAnalyzer->isMidas()) {
#if defined HAVE_MIDAS
      int runNumber,trans;
      if (cm_query_transition(&trans, &runNumber, NULL)) {
         if (trans == TR_START) {
            gAnalyzer->SetCurrentRunNumber(runNumber);
            fEventStatus = kAnalyze;
            fRunStatus = kRunning;
         }
         if (trans == TR_STOP) {
            fEventStatus = kEndOfRun;
            fRunStatus = kStopped;
            return true;
         }
      }
      int status = cm_yield(100);
      if (status == RPC_SHUTDOWN || status == SS_ABORT) {
         fEventStatus = kTerminate;
         return false;
      }
      if (fRunStatus == kStopped) {
         fEventStatus = kContinue;
         return true;
      }
      int size = gAnalyzer->GetMidasEventSize();
      void* mEvent = gAnalyzer->GetMidasEvent();
      status = bm_receive_event(fMidasBuffer, mEvent, &size, ASYNC);
      if (status != BM_SUCCESS) {
         fEventStatus = kContinue;
         return true;
      }

      gAnalyzer->SetCurrentEventNumber(((EVENT_HEADER*)mEvent)->event_id);
      timeStamp = ((EVENT_HEADER*)mEvent)->time_stamp;
      gAnalyzer->InitMidasBanks();

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
   else if (gAnalyzer->isOffline()&&gAnalyzer->isMidas()) {
      // read event header
      EVENT_HEADER *pevent = (EVENT_HEADER*)gAnalyzer->GetMidasEvent();
      bool readError = false;

      int n = read(fMidasFileHandle,pevent, sizeof(EVENT_HEADER));
      if (n < (int)sizeof(EVENT_HEADER)) readError = true;
      else {
         n = 0;
         if (pevent->data_size <= 0) readError = true;
         else {
            n = read(fMidasFileHandle, pevent+1, pevent->data_size);
            if (n != (int) pevent->data_size) readError = true;
         }
      }
      if (readError) {
         if (n > 0) cout << "Unexpected end of file\n";
         fEventStatus = kEndOfRun;
         return true;
      }

      if (pevent->event_id < 0) {
         fEventStatus = kContinue;
         return true;
      }
      if (pevent->event_id == EVENTID_EOR) {
         fEventStatus = kEndOfRun;
         return true;
      }

      gAnalyzer->SetEventID(pevent->event_id);
      gAnalyzer->SetCurrentEventNumber(pevent->serial_number);
      timeStamp = pevent->time_stamp;

      if (fEventStatus==kAnalyze) gAnalyzer->InitMidasBanks();

      fTreeInfo->SetTimeStamp(timeStamp);
      stat->processedEvents++;
   }
   else if (gAnalyzer->isOffline()&&gAnalyzer->isRoot()) {
      ROMETree *romeTree;
      TTree *tree;
      bool found = false;
      for (int j=0;j<gAnalyzer->GetTreeObjectEntries();j++) {
         romeTree = gAnalyzer->GetTreeObjectAt(j);
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
         fEventStatus = kEndOfRun;
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
   fTreeInfo->SetEventNumber(gAnalyzer->GetCurrentEventNumber());
   this->FillTrees();
   return true;
}
bool ROMEEventLoop::Update() 
{
   // Update the Analyzer. Called after the Event tasks.

   // Progress Display
   if (fProgressDelta>1) {
      if ((int)gAnalyzer->GetTriggerStatistics()->processedEvents >= fProgressLastEvent + fProgressDelta) {
         time(&fProgressTimeOfLastEvent);
         fProgressLastEvent = (int)gAnalyzer->GetTriggerStatistics()->processedEvents;
         fProgressWrite = true;
      } 
      else {
         if (time(NULL) > fProgressTimeOfLastEvent+1)
            fProgressDelta /= 10;
      }
   }
   if ((!fContinuous || fProgressDelta==1 || !((int)gAnalyzer->GetTriggerStatistics()->processedEvents%fProgressDelta) && fProgressWrite)) {
      cout << (int)gAnalyzer->GetTriggerStatistics()->processedEvents << " events processed                                                    \r";
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
*/      gAnalyzer->GetApplication()->ProcessLine(ROMEAnalyzer::LineToProcess);
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
      while (ROMEStatic::ss_kbhit()) {
         char ch = ROMEStatic::ss_getchar(0);
         if (ch == -1) {
            ch = getchar();
         }
         if (ch == 'q') {
            return false;
         }
         if (ch == 'e') {
            fEventStatus = kTerminate;
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
         gAnalyzer->GetApplication()->Run(true);
      }
   }

   return true;
}

bool ROMEEventLoop::Disconnect() {
   // Disconnects the current run. Called before the EndOfRun tasks.

   TFile *f1;
   // Write Trees
   ROMEString filename;
   ROMETree *romeTree;
   TTree *tree;
   ROMEString runNumberString;
   gAnalyzer->GetCurrentRunNumberString(runNumberString);
   for (int j=0;j<gAnalyzer->GetTreeObjectEntries();j++) {
      romeTree = gAnalyzer->GetTreeObjectAt(j);
      if (romeTree->isWrite() && !gAnalyzer->isTreeAccumulation()) {
         tree = romeTree->GetTree();
         cout << "Writing Root-File " << tree->GetName() << runNumberString.Data() << ".root" << endl;
         fTreeFiles[j] = tree->GetCurrentFile();
         fTreeFiles[j]->Write();
         fTreeFiles[j]->Close();
      }
   }
   cout << endl;

   // Write Histos
   TFolder *folder = (TFolder*)gROOT->FindObjectAny("histos");
   filename.SetFormatted("%s%s%s.root",gAnalyzer->GetOutputDir(),"histos",runNumberString.Data());
   f1 = new TFile(filename.Data(),"RECREATE");
   folder->Write();
   f1->Close();

   if (gAnalyzer->isOnline()&&gAnalyzer->isMidas()) {
   }
   else if (gAnalyzer->isOffline()&&gAnalyzer->isMidas()) {
      close(fMidasFileHandle);
   }
   else if (gAnalyzer->isOffline()&&gAnalyzer->isRoot()) {
      for (int j=0;j<gAnalyzer->GetTreeObjectEntries();j++) {
         if (gAnalyzer->GetTreeObjectAt(j)->isRead()) fRootFiles[j]->Close();
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
   // Clean up the analyzer. Called before the Terminate tasks.
   // Write and close Trees
   ROMETree *romeTree;
   for (int j=0;j<gAnalyzer->GetTreeObjectEntries();j++) {
      romeTree = gAnalyzer->GetTreeObjectAt(j);
      if (romeTree->isWrite()) {
         if (fTreeFiles[j]!=NULL)
            fTreeFiles[j]->Close();
      }
   }
   if (gAnalyzer->isOnline()&&gAnalyzer->isMidas()) {
#if defined HAVE_MIDAS
      cm_disconnect_experiment();
#endif
   }
   else if (gAnalyzer->isOffline()&&gAnalyzer->isMidas()) {
   }
   else if (gAnalyzer->isOffline()&&gAnalyzer->isRoot()) {
   }
   else {
      cout << "Severe program failure." << endl << endl;
      return false;
   }
   return true;
}

