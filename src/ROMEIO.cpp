// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMEIO                                                               //
//                                                                      //
//  The Base Class for Input and Output in ROME. 
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#if defined( _MSC_VER )
#include <io.h>
#define O_RDONLY_BINARY O_RDONLY | O_BINARY
#endif
#if defined ( __linux__ )
#include <unistd.h>
#include <stdlib.h>
#include <sys/io.h>
#include <sys/types.h>
#include <sys/stat.h>
#define O_RDONLY_BINARY O_RDONLY
#endif
#include <fcntl.h>
#include <time.h>

#include <TArrayI.h>
#include <TString.h>
#include <TROOT.h>
#include <TTask.h>
#include <TFolder.h>
#include <ROMEIO.h>
#include "Riostream.h"

#if defined HAVE_MIDAS
#include <midas.h>
#define MIDAS_DEBUG // define if you want to run the analyzer in the debugger

void ProcessMessage(int hBuf, int id, EVENT_HEADER * pheader, void *message)
{
//   printf("%s\n", message);
}
#endif

ROMEIO::ROMEIO() { 
   fAnalysisMode = kAnalyzeOffline; 
   fDataFormat = kRoot; 
   fDataBase = kDataBaseNone; 
   fCurrentRunNumber = 0; 
   fEventStatus = kAnalyze;
   fTreeAccumulation = false;
   fTreeObjects = new TObjArray(0);
   fSequentialNumber = 0;
   fTreeInfo = new ROMETreeInfo();
   fEventID = "all";
   fTriggerStatisticsString =  "Events received = DOUBLE : 0\nEvents per sec. = DOUBLE : 0\nEvents written = DOUBLE : 0\n";
   fScalerStatisticsString =  "Events received = DOUBLE : 0\nEvents per sec. = DOUBLE : 0\nEvents written = DOUBLE : 0\n";
}

ROMEIO::~ROMEIO() {
   ROMETree *romeTree;
   for (int j=0;j<GetTreeObjectEntries();j++) {
      romeTree = GetTreeObjectAt(j);
      if (romeTree->isWrite()) {
         if (treeFiles[j]!=NULL)
            treeFiles[j]->Close();
      }
   }
}


bool ROMEIO::Init() {
   // Tree file Initialisation
   treeFiles = new TFile*[GetTreeObjectEntries()];
   char filename[gFileNameLength];
   char runNumberString[6];
   ROMETree *romeTree;
   TTree *tree;
   GetCurrentRunNumberString(runNumberString);
   for (int j=0;j<GetTreeObjectEntries();j++) {
      treeFiles[j] = NULL;
      romeTree = GetTreeObjectAt(j);
      if (romeTree->isWrite() && this->isTreeAccumulation()) {
         tree = romeTree->GetTree();
         sprintf(filename,"%s%s%s.root",GetOutputDir(),tree->GetName(),runNumberString);
         treeFiles[j] = new TFile(filename,"RECREATE");
         tree->SetDirectory(treeFiles[j]);
      }
   }

   if (this->isOnline()&&this->isMidas()) {
#if defined HAVE_MIDAS
      // Connect to the Frontend
      int requestId,i;

      cout << "Program is running online." << endl << endl;

      // Connect to the experiment
      if (cm_connect_experiment("", "",(char*)fProgramName.Data(), NULL) != SUCCESS) {
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
      if (cm_get_experiment_database(&fMidasOnlineDataBase, NULL)!= CM_SUCCESS) {
         cout << "Cannot connect to the online database" << endl;
         return false;
      }

      // Get Runnumber
      int size = sizeof(fCurrentRunNumber);
      if (db_get_value(fMidasOnlineDataBase,0,"/Runinfo/Run number",&fCurrentRunNumber,&size,TID_INT,false)!= CM_SUCCESS) {
         cout << "Cannot read runnumber from the online database" << endl;
         return false;
      }

      // Initialize the online database
      HNDLE hKey;
      TString str;
      str = "//Trigger/Statistics";
      str.Insert(1,fProgramName.Data());
      // Trigger Statistics
      db_check_record(fMidasOnlineDataBase, 0, (char*)str.Data(), (char*)fTriggerStatisticsString.Data(), TRUE);
      db_find_key(fMidasOnlineDataBase, 0, (char*)str.Data(), &hKey);
      if (db_open_record(fMidasOnlineDataBase, hKey, &fTriggerStatistics, sizeof(Statistics), MODE_WRITE, NULL, NULL) != DB_SUCCESS) {
         cout << "Cannot open trigger statistics record, probably other analyzer is using it" << endl;
         return false;
      }

      // Scaler Statistics
      str="//Scaler/Statistics";
      str.Insert(1,fProgramName.Data());
      db_check_record(fMidasOnlineDataBase, 0, (char*)str.Data(), (char*)fScalerStatisticsString.Data(), TRUE);
      db_find_key(fMidasOnlineDataBase, 0, (char*)str.Data(), &hKey);
      if (db_open_record(fMidasOnlineDataBase, hKey, &fScalerStatistics, sizeof(Statistics), MODE_WRITE, NULL, NULL) != DB_SUCCESS) {
         cout << "Cannot open scaler statistics record, probably other analyzer is using it" << endl;
         return false;
      }

      // Tree Switches
      for (i=0;i<GetTreeObjectEntries();i++) {
         str="//Tree switches/";
         str.Insert(1,fProgramName.Data());
         str.Append(GetTreeObjectAt(i)->GetTree()->GetName());
         db_check_record(fMidasOnlineDataBase, 0, (char*)str.Data(), GetTreeObjectAt(i)->GetSwitchesString(), TRUE);
         db_find_key(fMidasOnlineDataBase, 0, (char*)str.Data(), &hKey);
         if (db_set_record(fMidasOnlineDataBase,hKey,GetTreeObjectAt(i)->GetSwitches(),GetTreeObjectAt(i)->GetSwitchesSize(),0) != DB_SUCCESS) {
            cout << "Cannot write to tree switches record." << endl;
            return false;
         }
         if (db_open_record(fMidasOnlineDataBase, hKey, GetTreeObjectAt(i)->GetSwitches(), GetTreeObjectAt(i)->GetSwitchesSize(), MODE_READ, NULL, NULL) != DB_SUCCESS) {
            cout << "Cannot open tree switches record, probably other analyzer is using it" << endl;
            return false;
         }
      }

      // Experiment dependent ODB initializations
      InitODB();

      // Set Event Status
      fEventStatus = kAnalyze;
#else
      cout << "Need Midas support for Online Modus !!" << endl;
      cout << "Please link the midas library into this project." << endl;
      return false;
#endif
   }
   else if (this->isOnline()&&this->isRoot()) {
      cout << "Root mode is not supported for online analysis." << endl << endl;
      return false;
   }
   else if (this->isOffline()&&this->isMidas()) {
      cout << "Program is running offline." << endl << endl;
   }
   else if (this->isOffline()&&this->isRoot()) {
      cout << "Program is running offline." << endl << endl;
   }
   else {
      cout << "Severe program failure." << endl << endl;
      return false;
   }

   if (this->isOffline()&&this->GetNumberOfRunNumbers()<=0) {
      cout << "No run numbers specified." << endl << endl;
      return false;
   }

   // Data Base Initialisation
   if (this->isSQLDataBase()) {
      if (!this->InitSQLDataBase())
         return false;
      if (!this->ReadSQLDataBase())
         return false;
   }
   else if (this->isXMLDataBase()){
      if (!this->ReadXMLDataBase())
         return false;
   }

   return true;
}
bool ROMEIO::Connect(Int_t runNumberIndex) {
   // Statistics
   fTriggerStatistics.processedEvents = 0;
   fTriggerStatistics.eventsPerSecond = 0;
   fTriggerStatistics.writtenEvents = 0;
   fTimeOfLastEvent = 0;
   fLastEvent = 0;
   // Progress Display
   fProgressDelta = 10000;
   fProgressLast = time(NULL);
   fProgressLastEvent = 0;
   fProgressWrite = false;
   // Status
   fRunStatus = kRunning;
   fEventStatus = kAnalyze;
   char runNumberString[6];

   if (this->isOffline()) {
      if (this->GetNumberOfRunNumbers()<=runNumberIndex) {
         this->SetTerminate();
         return true;
      }
      fCurrentRunNumber = fRunNumber.At(runNumberIndex);
   }
   fTreeInfo->SetRunNumber(this->GetCurrentRunNumber());
   GetCurrentRunNumberString(runNumberString);

   char filename[gFileNameLength];
   ROMETree *romeTree;
   TTree *tree;
   for (int j=0;j<GetTreeObjectEntries();j++) {
      romeTree = GetTreeObjectAt(j);
      if (romeTree->isWrite() && !this->isTreeAccumulation()) {
         tree = romeTree->GetTree();
         sprintf(filename,"%s%s%s.root",GetOutputDir(),tree->GetName(),runNumberString);
         treeFiles[j] = new TFile(filename,"RECREATE");
         tree->SetDirectory(treeFiles[j]);
      }
   }

   // Update Data Base
   if (this->isSQLDataBase()) {
      if (!this->ReadSQLDataBase())
         return false;
   }
   else if (this->isXMLDataBase()){
      if (!this->ReadXMLDataBase())
         return false;
   }

   if (this->isOnline()&&this->isMidas()) {
   }
   else if (this->isOffline()&&this->isMidas()) {
      // Open Midas File
      char filename[gFileNameLength];
      sprintf(filename,"%srun%s.mid",GetInputDir(),runNumberString);
      fMidasFileHandle = open(filename,O_RDONLY_BINARY);
      if (fMidasFileHandle==-1) {
         cout << "Inputfile '" << filename << "' not found." << endl;
         return false;
      }
      cout << "Reading Midas-File run" << runNumberString << ".mid" << endl;
   }
   else if (this->isOffline()&&this->isRoot()) {
      // Read Trees
      fRootFiles = new TFile*[GetTreeObjectEntries()];
      TTree *tree;
      ROMETree *romeTree;
      char filename[gFileNameLength];
      char runNumberString[6];
      GetCurrentRunNumberString(runNumberString);
      bool treeRead = false;
      fTreePosition = new int[GetTreeObjectEntries()];
      fTreeNextSeqNumber = new int[GetTreeObjectEntries()];
      for (int j=0;j<GetTreeObjectEntries();j++) {
         romeTree = GetTreeObjectAt(j);
         tree = romeTree->GetTree();
         if (!this->isTreeAccumulation()) {
            tree->Reset();
         }
         if (romeTree->isRead()) {
            treeRead = true;
            sprintf(filename,"%s%s%s.root",GetInputDir(),tree->GetName(),runNumberString);
            fRootFiles[j] = new TFile(filename,"READ");
            if (fRootFiles[j]->IsZombie()) {
               cout << "Inputfile '" << filename << "' not found." << endl;
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
      ConnectTrees();
   }
   else {
	   cout << "Severe program failure." << endl << endl;
	   return false;
   }
   return true;
}

bool ROMEIO::ReadEvent(Int_t event) {
   fEventStatus = kAnalyze;
   this->ClearFolders();
   int timeStamp = 0;

   if (this->isOnline()&&this->isMidas()) {
#if defined HAVE_MIDAS
      int runNumber,trans;
      if (cm_query_transition(&trans, &runNumber, NULL)) {
         if (trans == TR_START) {
            printf("\n\nRun %d started\n", runNumber);
            fEventStatus = kAnalyze;
            fRunStatus = kRunning;
         }
         if (trans == TR_STOP) {
            printf("\n\nRun %d stopped\n", runNumber);
            fEventStatus = kEndOfRun;
            fRunStatus = kStopped;
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
      int size = sizeof(fMidasEvent);
      status = bm_receive_event(fMidasBuffer, fMidasEvent, &size, ASYNC);
      if (status != BM_SUCCESS) {
         fEventStatus = kContinue;
         return true;
      }

      fCurrentEventNumber = ((EVENT_HEADER*)fMidasEvent)->event_id;
      timeStamp = ((EVENT_HEADER*)fMidasEvent)->time_stamp;
      this->InitMidasBanks();
      fTreeInfo->SetEventNumber(fCurrentEventNumber);
      fTreeInfo->SetTimeStamp(timeStamp);

      // Update Statistics
      fTriggerStatistics.processedEvents++;
      int time;
      time = ss_millitime();
      if (fTimeOfLastEvent == 0)
         fTimeOfLastEvent = time;
      if (time - fTimeOfLastEvent != 0)
         fTriggerStatistics.eventsPerSecond = (fTriggerStatistics.processedEvents-fLastEvent)/(time-fTimeOfLastEvent)*1000.0;
      fTimeOfLastEvent = time;
      fLastEvent = fTriggerStatistics.processedEvents;
#endif
   }
   else if (this->isOffline()&&this->isMidas()) {
      // read event header
      EVENT_HEADER *pevent;
      bool readError = false;
      int n = read(fMidasFileHandle,fMidasEvent, sizeof(EVENT_HEADER));
      if (n < (int)sizeof(EVENT_HEADER)) readError = true;
      else {
         pevent = (EVENT_HEADER*)fMidasEvent;
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

      int eventId = ((EVENT_HEADER*)fMidasEvent)->event_id;
      this->SetEventID(eventId);
      fCurrentEventNumber = ((EVENT_HEADER*)fMidasEvent)->serial_number;
      timeStamp = ((EVENT_HEADER*)fMidasEvent)->time_stamp;

      if (eventId < 0) {
         fEventStatus = kContinue;
         return true;
      }
      if (eventId == EVENTID_EOR) {
         fEventStatus = kEndOfRun;
         return true;
      }

      if (fEventStatus==kAnalyze) this->InitMidasBanks();
      fTreeInfo->SetEventNumber(fCurrentEventNumber);
      fTreeInfo->SetTimeStamp(timeStamp);

      fTriggerStatistics.processedEvents++;
   }
   else if (this->isOffline()&&this->isRoot()) {
      ROMETree *romeTree;
      TTree *tree;
      bool found = false;
      for (int j=0;j<GetTreeObjectEntries();j++) {
         romeTree = GetTreeObjectAt(j);
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
      fTreeInfo->SetEventNumber(fCurrentEventNumber);
      fTreeInfo->SetTimeStamp(timeStamp);
   
      fTriggerStatistics.processedEvents++;
   }
   else {
      cout << "Severe program failure." << endl << endl;
      return false;
   }

   return true;
}

bool ROMEIO::WriteEvent() {
   // Fill Trees
   FillTrees();
   return true;
}
bool ROMEIO::Update() {
   // Progress Display
   if (fProgressDelta>1) {
      if ((int)fTriggerStatistics.processedEvents >= fProgressLastEvent + fProgressDelta) {
         time(&fProgressLast);
         fProgressLastEvent = (int)fTriggerStatistics.processedEvents;
         fProgressWrite = true;
      } 
      else {
         if (time(NULL) > fProgressLast+1)
            fProgressDelta /= 10;
      }
   }
   if ((fProgressDelta==1 || !((int)fTriggerStatistics.processedEvents%fProgressDelta) && fProgressWrite)) {
      cout << (int)fTriggerStatistics.processedEvents << " events processed\r";
      fProgressWrite = false;
   }
 
   // ODB update
   db_send_changed_records();
   cout << "update" << endl;;

   return true;
}

bool ROMEIO::UserInput() {
   // User Input
   while (ROMEStatic::ss_kbhit()) {
      char ch = ROMEStatic::ss_getchar(0);
      if (ch == -1)
         ch = getchar();
      if (ch == 's') {
         fEventStatus = kTerminate;
      }
      if (ch == 'q')
         return false;
   }
   return true;
}

bool ROMEIO::Disconnect() {
   // Write Trees
   TFile *f1;
   char filename[gFileNameLength];
   ROMETree *romeTree;
   TTree *tree;
   char runNumberString[6];
   GetCurrentRunNumberString(runNumberString);
   for (int j=0;j<GetTreeObjectEntries();j++) {
      romeTree = GetTreeObjectAt(j);
      if (romeTree->isWrite() && !this->isTreeAccumulation()) {
         tree = romeTree->GetTree();
         cout << "Writing Root-File " << tree->GetName() << runNumberString << ".root" << endl;
         treeFiles[j] = tree->GetCurrentFile();
         treeFiles[j]->Write();
         treeFiles[j]->Close();
      }
   }
   cout << endl;

   // Write Histos
   TFolder *folder = (TFolder*)gROOT->FindObjectAny("histos");
   sprintf(filename,"%s%s%s.root",GetOutputDir(),"histos",runNumberString);
   f1 = new TFile(filename,"RECREATE");
   folder->Write();
   f1->Close();

   if (this->isOnline()&&this->isMidas()) {
   }
   else if (this->isOffline()&&this->isMidas()) {
      close(fMidasFileHandle);
   }
   else if (this->isOffline()&&this->isRoot()) {
      for (int j=0;j<GetTreeObjectEntries();j++) {
         if (GetTreeObjectAt(j)->isRead()) fRootFiles[j]->Close();
      }
      delete [] fRootFiles;
   }
   else {
      cout << "Severe program failure." << endl << endl;
      return false;
   }
   return true;
}

bool ROMEIO::Terminate() {
   if (this->isOnline()&&this->isMidas()) {
#if defined HAVE_MIDAS
      cm_disconnect_experiment();
#endif
   }
   else if (this->isOffline()&&this->isMidas()) {
   }
   else if (this->isOffline()&&this->isRoot()) {
   }
   else {
      cout << "Severe program failure." << endl << endl;
      return false;
   }
   return true;
}
