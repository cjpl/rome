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

#include <TArrayI.h>
#include <TROOT.h>
#include <TFolder.h>
#include <ROMEIO.h>
#include "Riostream.h"

#if defined HAVE_MIDAS
#include <midas.h>

void ProcessMessage(int hBuf, int id, EVENT_HEADER * pheader, void *message)
{
   // just print message text which comes after event header
   printf("%s\n", message);
}
#endif


bool ROMEIO::Init() {
   if (this->GetNumberOfRunNumbers()<=0) {
      cout << "No run numbers specified." << endl << endl;
      return false;
   }
   // Tree file Initialisation
   treeFiles = new TFile*[GetTreeObjectEntries()];
   char filename[gFileNameLength];
   char runNumberString[6];
   ROMETree *romeTree;
   TTree *tree;
   GetCurrentRunNumberString(runNumberString);
   for (int j=0;j<GetTreeObjectEntries();j++) {
      romeTree = GetTreeObjectAt(j);
      if (romeTree->isWrite() && this->isTreeAccumulation()) {
         tree = romeTree->GetTree();
         sprintf(filename,"%s%s%s.root",GetOutputDir(),tree->GetName(),runNumberString);
         treeFiles[j] = new TFile(filename,"RECREATE");
         tree->SetDirectory(treeFiles[j]);
      }
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

   if (this->isOnline()&&this->isMidas()) {
#if defined HAVE_MIDAS
      // Connect to the Frontend
      int requestId,status;
      cout << "Program is running online." << endl << endl;
      status = cm_connect_experiment("", "","Consumer", NULL);
      if (status != SUCCESS) {
         cout << "Cannot connect to experiment" << endl;
         return false;
      }

      /* open the "system" buffer, 1M size */
      bm_open_buffer("SYSTEM", EVENT_BUFFER_SIZE, &fMidasBuffer);

      /* set the buffer cache size */
      bm_set_cache_size(fMidasBuffer, 100000, 0);

      /* place a request for a specific event id */
      bm_request_event(fMidasBuffer, 1, TRIGGER_ALL,GET_SOME, &requestId,NULL);

      /* place a request for system messages */
      cm_msg_register(ProcessMessage);

      /*Registers a callback function for run transitions.*/
      if (cm_register_transition(TR_START, NULL) != CM_SUCCESS ||
         cm_register_transition(TR_STOP, NULL) != CM_SUCCESS) {
         cout << "Cannot connect to experiment" << endl;
         return false;
      }
      return true;
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
      return true;
   }
   else if (this->isOffline()&&this->isRoot()) {
      cout << "Program is running offline." << endl << endl;
      return true;
   }
   cout << "Severe program failure." << endl << endl;
   return false;
}
bool ROMEIO::Connect(Int_t runNumber) {
   this->SetAnalyze();
   char runNumberString[6];

   if (this->GetNumberOfRunNumbers()<=runNumber) {
      this->SetTerminate();
      return true;
   }
   fIndexOfCurrentRunNumber = runNumber;
   GetRunNumberStringAt(runNumberString,runNumber);
   fTreeInfo->SetRunNumber(this->GetCurrentRunNumber());

   char filename[gFileNameLength];
   ROMETree *romeTree;
   TTree *tree;
   GetCurrentRunNumberString(runNumberString);
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
      return true;
   }
   else if (this->isOnline()&&this->isRoot()) {
      cout << "Root mode is not supported for online analysis." << endl << endl;
      return false;
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
      return true;
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

      // Get Number of Events for ROOT Mode
      return true;
   }
   cout << "Severe program failure." << endl << endl;
   return false;
}

bool ROMEIO::ReadEvent(Int_t event) {
   this->SetAnalyze();
   this->ClearFolders();
   int timeStamp = 0;

   if (this->isOnline()&&this->isMidas()) {
#if defined HAVE_MIDAS
      int runNumber,trans;
      if (cm_query_transition(&trans, &runNumber, NULL)) {
         cout << trans << endl;
         if (trans == TR_START) {
            printf("---> Run %d started.\n", runNumber);
            fRunStatus = kAnalyze;
         }
         if (trans == TR_STOP) {
            printf("---> Run %d stopped.\n", runNumber);
            fRunStatus = kEndOfRun;
         }
      }
      int size = sizeof(fMidasEvent);
      int status = bm_receive_event(fMidasBuffer, fMidasEvent, &size, ASYNC);
      if (status != BM_SUCCESS) {
         fRunStatus = kContinue;
         return true;
      }
      if (status == RPC_SHUTDOWN || status == SS_ABORT) {
         fRunStatus = kTerminate;
         return true;
      }
      fCurrentEventNumber = ((EVENT_HEADER*)fMidasEvent)->event_id;
      timeStamp = ((EVENT_HEADER*)fMidasEvent)->time_stamp;
      this->InitMidasBanks();
      fTreeInfo->SetEventNumber(fCurrentEventNumber);
      fTreeInfo->SetTimeStamp(timeStamp);
      return true;
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
         fRunStatus = kEndOfRun;
      }

      int eventId = ((EVENT_HEADER*)fMidasEvent)->event_id;
      fCurrentEventNumber = ((EVENT_HEADER*)fMidasEvent)->serial_number;
      timeStamp = ((EVENT_HEADER*)fMidasEvent)->time_stamp;

      if (eventId == EVENTID_EOR || eventId < 0) {
         fRunStatus = kContinue;
      }
      if (eventId == EVENTID_EOR) fRunStatus = kEndOfRun;

      if (fRunStatus==kAnalyze) this->InitMidasBanks();
      fTreeInfo->SetEventNumber(fCurrentEventNumber);
      fTreeInfo->SetTimeStamp(timeStamp);
      return true;
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
         fRunStatus = kEndOfRun;
         return true;
      }
      fTreeInfo->SetEventNumber(fCurrentEventNumber);
      fTreeInfo->SetTimeStamp(timeStamp);
      return true;
   }

   cout << "Severe program failure." << endl << endl;
   return false;
}

bool ROMEIO::WriteEvent() {
   // Fill Trees
   FillTrees();
   return true;
}

bool ROMEIO::UserInput() {
   // User Input
   while (ROMEStatic::ss_kbhit()) {
      char ch = ROMEStatic::ss_getchar(0);
      if (ch == -1)
         ch = getchar();
      if (ch == 's') 
         fRunStatus = kEndOfRun;
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
      return true;
   }
   else if (this->isOnline()&&this->isRoot()) {
      cout << "Root mode is not supported for online analysis." << endl << endl;
      return false;
   }
   else if (this->isOffline()&&this->isMidas()) {
      close(fMidasFileHandle);
      return true;
   }
   else if (this->isOffline()&&this->isRoot()) {
      for (int j=0;j<GetTreeObjectEntries();j++) {
         if (GetTreeObjectAt(j)->isRead()) fRootFiles[j]->Close();
      }
      delete [] fRootFiles;
      return true;
   }
   cout << "Severe program failure." << endl << endl;
   return false;
}

bool ROMEIO::Terminate() {
   if (this->isOnline()&&this->isMidas()) {
#if defined HAVE_MIDAS
      cm_disconnect_experiment();
#else
      cout << "Need Midas support for Online Modus !!" << endl;
      cout << "Please link the midas library into this project." << endl;
      return false;
#endif
      return true;
   }
   else if (this->isOnline()&&this->isRoot()) {
      cout << "Root mode is not supported for online analysis." << endl << endl;
      return false;
   }
   else if (this->isOffline()&&this->isMidas()) {
      return true;
   }
   else if (this->isOffline()&&this->isRoot()) {
      return true;
   }
   cout << "Severe program failure." << endl << endl;
   return false;
}
