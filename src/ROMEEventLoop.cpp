// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMEEventLoop                                                        //
//                                                                      //
//  Implements the event loop. This Class must be the root Task of 
//  the Application.
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
#include <TFile.h>
#include <TTree.h>

#if defined HAVE_MIDAS
#include <midas.h>
#endif

#include "ROMEEventLoop.h"
#include "ROMEStatic.h"
#include <Riostream.h>

ClassImp(ROMEEventLoop)

TTask *TTask::fgBeginTask  = 0;
TTask *TTask::fgBreakPoint = 0;

void ProcessMessage(int hBuf, int id, EVENT_HEADER * pheader, void *message)
{
   /* just print message text which comes after event header */
   printf("%s\n", message);
}

ROMEEventLoop::ROMEEventLoop(const char *name,const char *title,ROMEAnalyzer *analyzer):TTask(name,title)
{
   // Initialisation of Class
   fAnalyzer = analyzer;
}

void ROMEEventLoop::ExecuteTask(Option_t *option)
{
// Event loop
   if (fgBeginTask) {
      Error("ExecuteTask","Cannot execute task:%s, already running task: %s",GetName(),fgBeginTask->GetName());
      return;
   }
   if (!IsActive()) return;

   // Declarations
   TFile *f1;
   char filename[gFileNameLength];
   char runNumber[6];
   int j,i,ii;
   ROMETree *datTree;

#if defined HAVE_MIDAS
   int requestId,hBufEvent,status;
#endif

   // Value Initialisations
   int numberOfTreeObjects = fAnalyzer->GetTreeObjectEntries();

// Read Histos
//   sprintf(filename,"%s%s%s.root",fAnalyzer->GetInputDir(),"histos",runNumber);
//   fAnalyzer->SetHistoFileHandle(new TFile(filename,"READ"));

   // Initialisation Tasks
   ReadRunTable();
   InitDataBase();
   ExecuteTasks(gTaskInit);
   CleanTasks();

   // Connect to the Frontend (Online)
   if (fAnalyzer->isOnline()) {
#if defined HAVE_MIDAS
      status = cm_connect_experiment("", "","Consumer", NULL);
      if (status != SUCCESS) {
         Error("ROMEEventLoop","Cannot connect to experiment");
         return;
      }

      /* open the "system" buffer, 1M size */
      bm_open_buffer("SYSTEM", EVENT_BUFFER_SIZE, &hBufEvent);

      /* set the buffer cache size */
      bm_set_cache_size(hBufEvent, 100000, 0);

      /* place a request for a specific event id */
      bm_request_event(hBufEvent, 1, TRIGGER_ALL,GET_SOME, &requestId,NULL);

      /* place a request for system messages */
      cm_msg_register(ProcessMessage);

      /*Registers a callback function for run transitions.*/
      if (cm_register_transition(TR_START, NULL) != CM_SUCCESS ||
         cm_register_transition(TR_STOP, NULL) != CM_SUCCESS) {
         Error("ROMEEventLoop","Cannot connect to experiment");
      }
#else
      cout << "Need Midas support for Online Modus !!" << endl;
      cout << "Please link the midas library into this project." << endl;
      return;
#endif
   }

   // Loop over Runs
   for (ii=0;(fAnalyzer->isOffline()&&ii<fAnalyzer->GetNumberOfRunNumbers() || 
             fAnalyzer->isOnline()) &&
             !fAnalyzer->isTerminate();ii++) {
      fAnalyzer->SetIndexOfCurrentRunNumber(ii);
      fAnalyzer->GetRunNumberStringAt(runNumber,ii);

      // Read Trees
      ReadRootFiles();
      ConnectTrees();

      // Get Number of Events for ROOT Mode
      int numberOfTreeObjects = fAnalyzer->GetTreeObjectEntries();
      int numberOfEntries = 2147483647;
      if (fAnalyzer->isOffline() && fAnalyzer->isRoot()) {
         for (int i=0;i<numberOfTreeObjects;i++) {
            if (fAnalyzer->GetTreeObjectAt(i)->isRead()) {
               numberOfEntries = (int)fAnalyzer->GetTreeObjectAt(i)->GetTree()->GetEntries();
               break;
            }
         }
      }

      // Open Midas File (Offline)
      if (fAnalyzer->isOffline() && fAnalyzer->isMidas()) {
         sprintf(filename,"%srun%s.mid",fAnalyzer->GetInputDir(),runNumber);
         midFileHandle = open(filename,O_RDONLY_BINARY);
      }

      // BeginOfRun
      UpdateDataBase();
      ExecuteTasks(gTaskBeginOfRun);
      CleanTasks();
      fAnalyzer->BeginOfRun();

      // Loop over Events
      for (i=0;!fAnalyzer->isEndOfRun()&&!fAnalyzer->isTerminate()&&i<numberOfEntries;i++) {
         if (!((i+1)%1000) && fAnalyzer->isOffline()) cout << i+1 << " events processed\r";

         
         // Read Midas Event (Offline)
         if (fAnalyzer->isOffline() && fAnalyzer->isMidas()) {
            // read event header
            EVENT_HEADER *pevent;
            bool readError = false;
            int n = read(midFileHandle,fAnalyzer->GetEventHeader(), sizeof(EVENT_HEADER));
            if (n < (int)sizeof(EVENT_HEADER)) readError = true;
            else {
               pevent = (EVENT_HEADER*)fAnalyzer->GetEventHeader();
               n = 0;
               if (pevent->data_size <= 0) readError = true;
               else {
                  n = read(midFileHandle, pevent+1, pevent->data_size);
                  if (n != (int) pevent->data_size) readError = true;
               }
            }
            if (readError) {
               if (n > 0) cout << "Unexpected end of file\n";
               fAnalyzer->EndOfRun();
               continue;
            }
            pevent = (EVENT_HEADER*)fAnalyzer->GetEventHeader();

            if (pevent->event_id == EVENTID_EOR) fAnalyzer->EndOfRun();
            if (pevent->event_id == EVENTID_EOR || pevent->event_id < 0) {
               continue;
            }
         }
         if (fAnalyzer->isOffline() && fAnalyzer->isRoot()) {
            for (int j=0;j<numberOfTreeObjects;j++) {
               if (fAnalyzer->GetTreeObjectAt(j)->isRead()) {
                  fAnalyzer->GetTreeObjectAt(j)->GetTree()->GetEntry(i);
               }  
            }
         }

         // Read Midas Event (Online)
         if (fAnalyzer->isOnline()) {
#if defined HAVE_MIDAS
            int runNumber,trans;
            if (cm_query_transition(&trans, &runNumber, NULL)) {
               cout << trans << endl;
               if (trans == TR_START) {
                  printf("---> Run %d started.\n", runNumber);
                  fAnalyzer->BeginOfRun();
               }
               if (trans == TR_STOP) {
                  printf("---> Run %d stopped.\n", runNumber);
                  fAnalyzer->EndOfRun();
               }
            }
            int size = gMidasEventSize;
            status = bm_receive_event(hBufEvent, fAnalyzer->GetMidasEvent(), &size, ASYNC);
            if (status != BM_SUCCESS) continue;
            if (status == RPC_SHUTDOWN || status == SS_ABORT) {
               fAnalyzer->Terminate();
               continue;
            }
#else
            cout << "Need Midas support for Online Modus !!" << endl;
            cout << "Please link the midas library into this project." << endl;
            return;
#endif
         }

         // Set FillEvent Flags of all Trees equal true
         for (j=0;j<numberOfTreeObjects;j++) {
            fAnalyzer->GetTreeObjectAt(j)->SetFillEvent(true);
         }

         // Event
         if (fAnalyzer->isMidas()) InitMidasBanks();
         ExecuteTasks(gTaskEvent);
         CleanTasks();

         // Fill Trees
         for (j=0;j<numberOfTreeObjects;j++) {
            datTree = fAnalyzer->GetTreeObjectAt(j);
            if (datTree->isFill()&&datTree->isFillEvent()) datTree->GetTree()->Fill();
         }

         // User Input
         while (ROMEStatic::ss_kbhit()) {
            cout << endl;
            char ch = getchar();
            if (ch == 's') fAnalyzer->EndOfRun();
            if (ch == 'q') fAnalyzer->Terminate();
         }
      }

      cout << i << " events processed" << endl<<endl;

      // EndOfRun
      ExecuteTasks(gTaskEndOfRun);
      CleanTasks();
      SaveRunTable();

      // Close Midas File (Offline)
      if (fAnalyzer->isOffline() && fAnalyzer->isMidas()) {
         close(midFileHandle);
      }


      // Write Trees
      for (j=0;j<numberOfTreeObjects;j++) {
         datTree = fAnalyzer->GetTreeObjectAt(j);
         if (datTree->isWrite()) {
            sprintf(filename,"%s%s%s.root",fAnalyzer->GetOutputDir(),datTree->GetTree()->GetName(),runNumber);
            f1 = new TFile(filename,"RECREATE");
            datTree->GetTree()->Write();
            f1->Close();
         }
      }
      for (int j=0;j<numberOfTreeObjects;j++) {
         if (fAnalyzer->GetTreeObjectAt(j)->isRead()) files[j]->Close();
      }
      delete [] files;

      // Write Histos
      TFolder *folder = (TFolder*)gROOT->FindObjectAny("histos");
      sprintf(filename,"%s%s%s.root",fAnalyzer->GetOutputDir(),"histos",runNumber);
      f1 = new TFile(filename,"RECREATE");
      folder->Write();
      f1->Close();
   }

   // Terminate Tasks
   ExecuteTasks(gTaskTerminate);
   CleanTasks();

   // Disconnect the Frontend
   if (fAnalyzer->isOnline()) {
#if defined HAVE_MIDAS
      cm_disconnect_experiment();
#else
      cout << "Need Midas support for Online Modus !!" << endl;
      cout << "Please link the midas library into this project." << endl;
      return;
#endif
   }
}

void ROMEEventLoop::ReadRootFiles() {
   int numberOfTreeObjects = fAnalyzer->GetTreeObjectEntries();
   files = new TFile*[numberOfTreeObjects];
   TTree *tree;
   ROMETree *datTree;
   char filename[gFileNameLength];
   char runNumber[6];
   fAnalyzer->GetCurrentRunNumberString(runNumber);
   for (int j=0;j<numberOfTreeObjects;j++) {
      datTree = fAnalyzer->GetTreeObjectAt(j);
      tree = datTree->GetTree();
      if (!datTree->isAccumulation()) tree->Reset();
      if (datTree->isRead()) {
         sprintf(filename,"%s%s%s.root",fAnalyzer->GetInputDir(),tree->GetName(),runNumber);
         files[j] = new TFile(filename,"READ");
         tree = (TTree*)files[j]->FindObjectAny(tree->GetName());
         datTree->SetTree(tree);
      }
   }
}

