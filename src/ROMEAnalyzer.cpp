// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMEAnalyzer                                                         //
//                                                                      //
//  Basic class in ROME. This class is the base-class
//  for the analysis dependent main class, which should implement the 
//  Folders, Trees and Task definitions.
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#if defined( _MSC_VER )
#include <io.h>
#include <direct.h>
#define O_RDONLY_BINARY O_RDONLY | O_BINARY
#endif
#if defined ( __linux__ )
#include <unistd.h>
#include <stdlib.h>
#include <sys/io.h>
#include <sys/types.h>
#include <TThread.h>
#define O_RDONLY_BINARY O_RDONLY
#endif
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include <TMessage.h>
#include <TSocket.h>
#include <TServerSocket.h>
#include <THtml.h>
#include <TArrayI.h>
#include <TTask.h>
#include <TTree.h>
#include <TFolder.h>
#include <TList.h>
#include <ROMEString.h>
#include <TObjString.h>
#include <TROOT.h>
#include <TBrowser.h>
#include <TH1.h>
#include <TMath.h>
#include <ROMEAnalyzer.h>
#include <ROMEEventLoop.h>
#include <ROMETree.h>
#include <ROMETask.h>
#include <Riostream.h>

void *root_server_loop(void *arg);

#if defined HAVE_MIDAS
#include <midas.h>
#define MIDAS_DEBUG // define if you want to run the analyzer in the debugger

void ProcessMessage(int hBuf, int id, EVENT_HEADER * pheader, void *message)
{
// This method is called, when a system message from the online system occurs
}
#endif


ROMEAnalyzer::ROMEAnalyzer()
{
// Initialisations

   int i=0;
   fSplashScreen = true;
   fBatchMode = false;
   fTerminate = false;
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
   fContinuous = true;
   fOnlineHost = "";
}

ROMEAnalyzer::~ROMEAnalyzer() {
// Write and close Trees
   ROMETree *romeTree;
   for (int j=0;j<GetTreeObjectEntries();j++) {
      romeTree = GetTreeObjectAt(j);
      if (romeTree->isWrite()) {
         if (treeFiles[j]!=NULL)
            treeFiles[j]->Close();
      }
   }
}


bool ROMEAnalyzer::Start(int argc, char **argv) 
{
// Starts the ROME Analyzer

   consoleStartScreen();

   if (!ReadParameters(argc,argv)) return false;

   if (isSplashScreen()) startSplashScreen();

   CreateHistoFolders();

#if defined ( __linux__ )
   /* start socket server */
   TThread *th1 = new TThread("root_server_loop", root_server_loop, NULL);
   th1->Run();
#endif

   fMainTask->ExecuteTask();
   if (fTerminate) return false;

   if (!isBatchMode()) TBrowser *t = new TBrowser();

   return true;
}

bool ROMEAnalyzer::Init() {
   // Initialize the analyzer. Called before the init tasks.
   int j;
   // Tree file Initialisation
   treeFiles = new TFile*[GetTreeObjectEntries()];
   ROMEString filename;
   ROMEString runNumberString;
   ROMETree *romeTree;
   TTree *tree;
   GetCurrentRunNumberString(runNumberString);
   for (j=0;j<GetTreeObjectEntries();j++) {
      treeFiles[j] = NULL;
      romeTree = GetTreeObjectAt(j);
      if (romeTree->isWrite() && this->isTreeAccumulation()) {
         tree = romeTree->GetTree();
         filename = GetOutputDir();
         filename.Append(tree->GetName());         
         filename.Append(runNumberString.Data());
         filename.Append(".root");
         treeFiles[j] = new TFile(filename.Data(),"RECREATE");
         tree->SetDirectory(treeFiles[j]);
      }
   }

   if (this->isOnline()&&this->isMidas()) {
#if defined HAVE_MIDAS
      // Connect to the Frontend
      int requestId,i;

      cout << "Program is running online." << endl << endl;

      // Connect to the experiment
      if (cm_connect_experiment((char*)fOnlineHost.Data(), (char*)fOnlineExperiment.Data(),(char*)fProgramName.Data(), NULL) != SUCCESS) {
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
      ROMEString str;
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
      cout << "Need Midas support for Online Mode !!" << endl;
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
   }

   return true;
}
bool ROMEAnalyzer::Connect(Int_t runNumberIndex) {
   cout << "Connect" << endl;
   // Connect the Analyzer to the current run. Called before the BeginOfRun tasks.
   ROMEString runNumberString;
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

   if (this->isOffline()) {
      if (this->GetNumberOfRunNumbers()<=runNumberIndex) {
         this->SetTerminate();
         return true;
      }
      fCurrentRunNumber = fRunNumber.At(runNumberIndex);
   }
   fTreeInfo->SetRunNumber(this->GetCurrentRunNumber());
   GetCurrentRunNumberString(runNumberString);

   ROMEString filename;
   ROMETree *romeTree;
   TTree *tree;
   for (int j=0;j<GetTreeObjectEntries();j++) {
      romeTree = GetTreeObjectAt(j);
      if (romeTree->isWrite() && !this->isTreeAccumulation()) {
         tree = romeTree->GetTree();
         filename.SetFormatted("%s%s%s.root",GetOutputDir(),tree->GetName(),runNumberString.Data());
         treeFiles[j] = new TFile(filename.Data(),"RECREATE");
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
      filename.SetFormatted("%srun%s.mid",GetInputDir(),runNumberString.Data());
      fMidasFileHandle = open(filename.Data(),O_RDONLY_BINARY);
      if (fMidasFileHandle==-1) {
         cout << "Inputfile '" << filename.Data() << "' not found." << endl;
         return false;
      }
      cout << "Reading Midas-File run" << runNumberString.Data() << ".mid" << endl;
   }
   else if (this->isOffline()&&this->isRoot()) {
      // Read Trees
      fRootFiles = new TFile*[GetTreeObjectEntries()];
      TTree *tree;
      ROMETree *romeTree;
      ROMEString runNumberString;
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
            filename.SetFormatted("%s%s%s.root",GetInputDir(),tree->GetName(),runNumberString.Data());
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
      ConnectTrees();
   }
   else {
	   cout << "Severe program failure." << endl << endl;
	   return false;
   }
   return true;
}

bool ROMEAnalyzer::ReadEvent(Int_t event) {
   // Reads an event. Called before the Event tasks.
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

bool ROMEAnalyzer::WriteEvent() {
   // Writes the event. Called after the Event tasks.
   FillTrees();
   return true;
}
bool ROMEAnalyzer::Update() {
   // Update the Analyzer. Called after the Event tasks.

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
#if defined HAVE_MIDAS
   db_send_changed_records();
#endif

   return true;
}

bool ROMEAnalyzer::UserInput() {
   bool wait = false;
   bool first = true;
   // Looks for user input. Called before the Event tasks.
   while (wait||first) {

      first = false;
      if (!fContinuous)
         wait = true;

      while (ROMEStatic::ss_kbhit()) {
         char ch = ROMEStatic::ss_getchar(0);
         if (ch == -1)
            ch = getchar();
         if (ch == 's')
            fEventStatus = kTerminate;
         if (ch == 'q')
            return false;
         if (ch == 'p')
            wait = true;
         if (ch == 'r')
            wait = false;
         if (ch == 'o')
            fContinuous = false;
         if (ch == 'c')
            fContinuous = true;
      }
   }
   return true;
}

bool ROMEAnalyzer::Disconnect() {
   cout << "Disconnect" << endl;
   // Disconnects the current run. Called before the EndOfRun tasks.

   TFile *f1;
   // Write Trees
   ROMEString filename;
   ROMETree *romeTree;
   TTree *tree;
   ROMEString runNumberString;
   GetCurrentRunNumberString(runNumberString);
   for (int j=0;j<GetTreeObjectEntries();j++) {
      romeTree = GetTreeObjectAt(j);
      if (romeTree->isWrite() && !this->isTreeAccumulation()) {
         tree = romeTree->GetTree();
         cout << "Writing Root-File " << tree->GetName() << runNumberString.Data() << ".root" << endl;
         treeFiles[j] = tree->GetCurrentFile();
         treeFiles[j]->Write();
         treeFiles[j]->Close();
      }
   }
   cout << endl;

   // Write Histos
   TFolder *folder = (TFolder*)gROOT->FindObjectAny("histos");
   filename.SetFormatted("%s%s%s.root",GetOutputDir(),"histos",runNumberString.Data());
   f1 = new TFile(filename.Data(),"RECREATE");
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

bool ROMEAnalyzer::Terminate() {
   cout << "Terminate" << endl;
   // Clean up the analyzer. Called before the Terminate tasks.
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

bool ROMEAnalyzer::ReadParameters(int argc, char *argv[]) 
{
   // Reads the Inputlineparameters
   int i;
   const int workDirLen = 1000;
   ROMEString workDir(workDirLen);
   getcwd((char*)workDir.Data(),workDirLen);
   workDir.Append("/");
   this->SetDataBaseDir(workDir);
   this->SetInputDir(workDir);
   this->SetOutputDir(workDir);

   ROMEString configFile("romeConfig.xml");

   for (i=1;i<argc;i++) {
      if (!strcmp(argv[i],"-h")||!strcmp(argv[i],"-help")) {
         cout << "  -i       Configuration file (default ROMEConfig.xml)" << endl;
         cout << "  -b       Batch Mode (no Argument)" << endl;
         cout << "  -ns      Splash Screen is not displayed (no Argument)" << endl;
         cout << "  -m       Analysing Mode : (online/[offline])" << endl;
         cout << "  -f       Input Data Format : ([midas]/root)" << endl;
         cout << "  -r       Runnumbers" << endl;
         cout << "  -e       Eventnumbers" << endl;
         cout << "  -docu    Generates a Root-Html-Documentation (no Argument)" << endl;
	      return false;
      }
      if (!strcmp(argv[i],"-docu")) {
         THtml html;
         html.MakeAll(true);
         return false;
      }
      if (!strcmp(argv[i],"-i")&&i<argc-1) {
         configFile = argv[i+1];
         i++;
      }
   }
   char answer[10];
   struct stat buf;
   if( stat( configFile.Data(), &buf )) {
      cout << "Configuration file '" << configFile.Data() << "' not found." << endl;
      cout << "Do you like the framework to generate a new configuration file ([y]/n) ? ";
      cin >> answer;
      if (strstr(answer,"n")==NULL) {
         cout << "\nThe framework generated a new configuration file." << endl;
         cout << "Please edit this file and restart the program.\n" << endl;
         if (!WriteROMEConfigXML((char*)configFile.Data())) {
            cout << "\nTerminate program.\n" << endl;
            return false;
         }
      }
      else {
         cout << "\nTerminate program.\n" << endl;
      }
      return false;
   }
   if (!ReadROMEConfigXML((char*)configFile.Data())) {
      cout << "\nTerminate program.\n" << endl;
      return false;
   }
   if (!WriteROMEConfigXML((char*)configFile.Data())) {
      cout << "\nTerminate program.\n" << endl;
      return false;
   }

   for (i=1;i<argc;i++) {
      if (!strcmp(argv[i],"-b")) {
         fBatchMode = true;
      }
      else if (!strcmp(argv[i],"-ns")) {
         fSplashScreen = false;
      }
      else if (!strcmp(argv[i],"-m")) {
         if (!strcmp(argv[i+1],"online")) this->SetOnline();
         else this->SetOffline();
         i++;
      }
      else if (!strcmp(argv[i],"-f")) {
         if (!strcmp(argv[i+1],"root")) this->SetRoot();
         else this->SetMidas();
         i++;
      }
      else if (!strcmp(argv[i],"-r")&&i<argc-1) {
         this->SetRunNumbers(argv[i+1]);
         i++;
      }
      else if (!strcmp(argv[i],"-e")&&i<argc-1) {
         this->SetEventNumbers(argv[i+1]);
         i++;
      }
      else {
         cout << "Inputlineparameter '" << argv[i] << "' not available." << endl;
         cout << "Available inputlineparameter are : " << endl;
         cout << endl;
         cout << "  -i       Configuration file (default ROMEConfig.xml)" << endl;
         cout << "  -b       Batch Mode (no Argument)" << endl;
         cout << "  -ns      Splash Screen is not displayed (no Argument)" << endl;
         cout << "  -m       Analysing Mode : (online/[offline])" << endl;
         cout << "  -f       Input Data Format : ([midas]/root)" << endl;
         cout << "  -r       Runnumbers" << endl;
         cout << "  -e       Eventnumbers" << endl;
         cout << "  -docu    Generates a Root-Html-Documentation (no Argument)" << endl;
	      return false;
      }
   }

   return true;
}

void ROMEAnalyzer::CreateHistoFolders() 
{
   // Creates a Folder for each active Task
   int i,j;
   ROMEString name;
   ROMEString title;
   TFolder *fHistoFolder = fMainFolder->AddFolder("histos","Histogram Folder");
   TList *taskList = fMainTask->GetListOfTasks();
   for (i=0;i<taskList->GetSize();i++) {
      ROMETask *task = (ROMETask*)taskList->At(i);
      if (!task->IsActive() || !task->hasHistograms()) continue;
      name.SetFormatted("%sHistos",task->GetName());
      title.SetFormatted("Histograms of Task '%s'",task->GetName());
      TFolder *folder = fHistoFolder->AddFolder(name.Data(),title.Data());
      TList *subTaskList = task->GetListOfTasks();
      for (j=0;j<subTaskList->GetSize();j++) {
         ROMETask *task = (ROMETask*)subTaskList->At(j);
         if (!task->IsActive() || !task->hasHistograms()) continue;
         name.SetFormatted("%sHistos",task->GetName());
         title.SetFormatted("Histograms of Task '%s'",task->GetName());
         folder->AddFolder(name.Data(),title.Data());
      }
   }
}

ROMEString currentDirectory[100];
int directoryDepth = 0;

void *server_thread(void *arg)
//  Server histograms to remove clients
{
   int i;
   char str[32];

   TSocket *s = (TSocket *) arg;

   do {
      if (s->Recv(str, sizeof(str)) <= 0) {
         printf("Closed connection from %s\n", s->GetInetAddress().GetHostName());
         s->Close();
         delete s;
         return NULL;
      } else {
//         printf("Received \"%s\"\n", str);

         TMessage *mess = new TMessage(kMESS_OBJECT);

         if (strcmp(str, "LIST") == 0) {

            TObject *obj;
            TObjArray *names = new TObjArray(100);

            TCollection *folders = ((TFolder*)gROOT->FindObjectAny(currentDirectory[directoryDepth].Data()))->GetListOfFolders();
            TIterator *iterFolders = folders->MakeIterator();
            while (obj = iterFolders->Next()) {
               ROMEString str;
               str.SetFormatted("%s\n%s",obj->GetName(),obj->ClassName());
               names->Add(new TObjString(str.Data()));
            }

            mess->Reset();
            mess->WriteObject(names);
            s->Send(*mess);

            for (i = 0; i < names->GetLast() + 1; i++)
               delete(TObjString *) names->At(i);

            delete names;
         }

         else if (strncmp(str, "CD", 2) == 0) {
            TObject *obj;

            if (strcmp(str + 3, "..") == 0) {
               directoryDepth--;
               if (directoryDepth<0) 
                  directoryDepth = 0;
            }

            TCollection *folders = ((TFolder*)gROOT->FindObjectAny(currentDirectory[directoryDepth].Data()))->GetListOfFolders();
            TIterator *iterFolders = folders->MakeIterator();
            while (obj = iterFolders->Next()) {
               if (strcmp(str + 3, obj->GetName()) == 0) {
                  directoryDepth++;
                  if (directoryDepth>=100) 
                     directoryDepth = 99;
                  currentDirectory[directoryDepth] = str + 3;
                  break;
               }
            }

            if (!obj) {
               s->Send("Error");
            } else {
               s->Send("OK");
            }
         }

         else if (strncmp(str, "GET", 3) == 0) {
            TObject *obj;

            TCollection *folders = ((TFolder*)gROOT->FindObjectAny(currentDirectory[directoryDepth].Data()))->GetListOfFolders();
            TIterator *iterFolders = folders->MakeIterator();
            while (obj = iterFolders->Next()) {
               if (strcmp(str + 4, obj->GetName()) == 0)
                  break;
            }

            if (!obj) {
               s->Send("Error");
            } else {
               mess->Reset();
               mess->WriteObject(obj);
               s->Send(*mess);
            }
         }

         else if (strncmp(str, "CLEAR", 5) == 0) {
            TObject *obj;

            TCollection *folders = ((TFolder*)gROOT->FindObjectAny(currentDirectory[directoryDepth].Data()))->GetListOfFolders();
            TIterator *iterFolders = folders->MakeIterator();
            while (obj = iterFolders->Next()) {
               if (strcmp(str + 6, obj->GetName()) == 0)
                  break;
            }

            if (!obj) {
               s->Send("Error");
            } else {
#if defined ( __linux__ )
               TThread::Lock();
               ((TH1 *) obj)->Reset();
               TThread::UnLock();
#endif
               s->Send("OK");
            }
         }

         delete mess;
      }

   } while (1);
}

void *root_server_loop(void *arg)
// Server loop listening for incoming network connections on port
// specified by command line option -s. Starts a searver_thread for 
// each connection.
{
   currentDirectory[0] = "histos";
   int port = 9091;
   printf("Root server listening on port %d...\n", port);
   TServerSocket *lsock = new TServerSocket(port, kTRUE);

   do {
      TSocket *s = lsock->Accept();
      s->Send("RMSERV 1.0");
//      printf("New connection from %s\n", s->GetInetAddress().GetHostName());

      // start a new server thread 
#if defined ( __linux__ )
      TThread *th = new TThread("server_thread", server_thread, s);
      th->Run();
#endif
   } while (1);
}


TArrayI ROMEAnalyzer::decodeRunNumbers(ROMEString& str)
{
   char cminus='-';
   char ccomma=',';
   char csemi =';';
   char *pstr = (char*)str.Data();
   int num;
   int na=0;
   int nat=1;
   int arraySize = 1000;
   TArrayI arr(arraySize);
   while (pstr<str.Data()+str.Length()) {
      if (na>=arraySize*nat) {
         nat++;
         arr.Set(arraySize*nat);
      }
      num = strtol(pstr,&pstr,10);
      if (pstr[0]==cminus) {
         arr.AddAt(-num,na);
         na++;
         pstr++;
      }
      else if (pstr[0]==ccomma||pstr[0]==csemi) {
         arr.AddAt(num,na);
         na++;
         pstr++;
      }
      else {
         arr.AddAt(num,na);
         arr.Set(na+1);
         return arr;
      }
   }
   arr.Set(na);
   return arr;
}

