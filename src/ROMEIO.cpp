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
   // Data Base Initialisation
   if (this->isSQLDataBase()) {
      if (!this->InitSQLDataBase()) {
         cout << "Termninating Program !" << endl;
         return false;
      }
      if (!this->ReadSQLDataBase()) {
         cout << "Termninating Program !" << endl;
         return false;
      }
   }
   else if (this->isXMLDataBase()){
      this->ReadXMLRunTable();
      this->InitXMLDataBase();
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

   // Update Data Base
   if (this->isSQLDataBase()) {
      this->ReadSQLDataBase();
   }
   else if (this->isXMLDataBase()){
      this->UpdateXMLDataBase();
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
      cout << "Reading Midas-File run" << runNumberString << ".mid" << endl;
      return true;
   }
   else if (this->isOffline()&&this->isRoot()) {
      // Read Trees
      fRootFiles = new TFile*[GetTreeObjectEntries()];
      TTree *tree;
      ROMETree *datTree;
      char filename[gFileNameLength];
      char runNumberString[6];
      GetCurrentRunNumberString(runNumberString);
      if (!this->isTreeAccumulation()) {
         this->fIndexTree->Reset();
         fFillTreeFirst = true;
      }
      for (int j=0;j<GetTreeObjectEntries();j++) {
         datTree = GetTreeObjectAt(j);
         tree = datTree->GetTree();
         if (!this->isTreeAccumulation()) {
            tree->Reset();
         }
         if (datTree->isRead()) {
            sprintf(filename,"%s%s%s.root",GetInputDir(),tree->GetName(),runNumberString);
            fRootFiles[j] = new TFile(filename,"READ");
            tree = (TTree*)fRootFiles[j]->FindObjectAny(tree->GetName());
            datTree->SetTree(tree);
         }
      }
      ConnectTrees();

      // Get Number of Events for ROOT Mode
      int numberOfEntries = 2147483647;
      for (int i=0;i<GetTreeObjectEntries();i++) {
         if (GetTreeObjectAt(i)->isRead()) {
            numberOfEntries = (int)GetTreeObjectAt(i)->GetTree()->GetEntries();
            break;
         }
      }
      return true;
   }
   cout << "Severe program failure." << endl << endl;
   return false;
}

bool ROMEIO::ReadEvent(Int_t eventNumber) {
   this->SetAnalyze();
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
      if (status != BM_SUCCESS) return true;
      if (status == RPC_SHUTDOWN || status == SS_ABORT) {
         fRunStatus = kTerminate;
         return true;
      }
      this->InitMidasBanks();
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
      pevent = (EVENT_HEADER*)fMidasEvent;

      if (pevent->event_id == EVENTID_EOR || pevent->event_id < 0) {
         fRunStatus = kContinue;
      }
      if (pevent->event_id == EVENTID_EOR) fRunStatus = kEndOfRun;

      if (fRunStatus==kAnalyze) this->InitMidasBanks();
      return true;
   }
   else if (this->isOffline()&&this->isRoot()) {
      for (int j=0;j<GetTreeObjectEntries();j++) {
         if (GetTreeObjectAt(j)->isRead()) {
            GetTreeObjectAt(j)->GetTree()->GetEntry(eventNumber);
         }  
      }
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
      if (ch == 's') fRunStatus = kEndOfRun;
      if (ch == 'q') fRunStatus = kTerminate;
   }
   return true;
}

bool ROMEIO::Disconnect() {
   // Write Trees
   TFile *f1;
   char filename[gFileNameLength];
   ROMETree *datTree;
   char runNumberString[6];
   GetCurrentRunNumberString(runNumberString);
   for (int j=0;j<GetTreeObjectEntries();j++) {
      datTree = GetTreeObjectAt(j);
      if (datTree->isWrite() && !this->isTreeAccumulation()) {
         sprintf(filename,"%s%s%s.root",GetOutputDir(),datTree->GetTree()->GetName(),runNumberString);
         f1 = new TFile(filename,"RECREATE");
         datTree->GetTree()->Write();
         f1->Close();
         cout << "Writing Root-File " << datTree->GetTree()->GetName() << runNumberString << ".root" << endl;
      }
   }
   cout << endl;

   // Write Histos
   TFolder *folder = (TFolder*)gROOT->FindObjectAny("histos");
   sprintf(filename,"%s%s%s.root",GetOutputDir(),"histos",runNumberString);
   f1 = new TFile(filename,"RECREATE");
   folder->Write();
   f1->Close();

   // Save Data Base
   if (this->isXMLDataBase()){
      this->SaveXMLRunTable();
   }

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
