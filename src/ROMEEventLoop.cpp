// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMEEventLoop                                                        //
//                                                                      //
//  Implements the event loop. This Class must be the root Task of 
//  the Application.
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <time.h>

#include "ROMEEventLoop.h"
#include "ROMEStatic.h"
#include <Riostream.h>

TTask *TTask::fgBeginTask  = 0;
TTask *TTask::fgBreakPoint = 0;

ROMEEventLoop::ROMEEventLoop(const char *name,const char *title,ROMEAnalyzer *analyzer):ROMETask(name,title,analyzer)
{
   // Initialisation of Class
   fAnalyzer = analyzer;
}

void ROMEEventLoop::ExecuteTask(Option_t *option)
{
// Event loop
   if (fgBeginTask) {
      Error("ExecuteTask","Cannot execute task:%s, already running task: %s",GetName(),fgBeginTask->GetName());
      fAnalyzer->SetTerminate();
      cout << "\n\nTerminating Program !" << endl;
      return;
   }
   if (!IsActive()) {
      fAnalyzer->SetTerminate();
      cout << "\n\nTerminating Program !" << endl;
      return;
   }

   // Declarations
   //--------------
   int i,ii;
   int delta = 10000;
   time_t last = time(NULL);
   int lastEvent = 0;
   bool write = false;

// Read Histos
//   sprintf(filename,"%s%s%s.root",fAnalyzer->GetInputDir(),"histos",runNumber);
//   fAnalyzer->SetHistoFileHandle(new TFile(filename,"READ"));

   // Initialisation
   //----------------

   fAnalyzer->InitFolders();
   if (!fAnalyzer->GetIO()->Init()) {
      fAnalyzer->SetTerminate();
      cout << "\n\nTerminating Program !" << endl;
      return;
   }
 
   ExecuteTasks(gTaskInit);
   CleanTasks();

   cout << "Stop run with 's'" << endl;
   cout << "Terminate program with 'q'" << endl << endl;


   // Loop over Runs
   //----------------
   for (ii=0;!fAnalyzer->GetIO()->isTerminate();ii++) {

      if (!fAnalyzer->GetIO()->Connect(ii)) {
         fAnalyzer->SetTerminate();
         cout << "\n\nTerminating Program !" << endl;
         return;
      }
      if (fAnalyzer->GetIO()->isTerminate()) {
         break;
      }

      // Begin of Run Tasks
      ExecuteTasks(gTaskBeginOfRun);
      CleanTasks();

      if (gShowTime) TimeStart();
      // Loop over Events
      //------------------
      lastEvent = 0;
      write = false;
      for (i=0;!fAnalyzer->GetIO()->isTerminate()&&!fAnalyzer->GetIO()->isEndOfRun();i++) {
         // Progress Display
  
         if (i >= lastEvent + delta) {
            time(&last);
            lastEvent = i;
            write = true;
         } 
         else {
            if (time(NULL) > last+1)
               delta /= 10;
         }
         if (!(i%delta) && write && fAnalyzer->GetIO()->isOffline()) {
            cout << i << " events processed\r";
            write = false;
         }

         fAnalyzer->SetFillEvent();

         if (!fAnalyzer->GetIO()->ReadEvent(i)) {
            fAnalyzer->SetTerminate();
            cout << "\n\nTerminating Program !" << endl;
            return;
         }
         if (fAnalyzer->GetIO()->isTerminate()||fAnalyzer->GetIO()->isEndOfRun()) {
            break;
         }
         if (fAnalyzer->GetIO()->isContinue()) {
            continue;
         }

         // Event
         ExecuteTasks(fAnalyzer->GetIO()->GetEventID());
         CleanTasks();

         if (!fAnalyzer->GetIO()->WriteEvent() && fAnalyzer->isFillEvent()) {
            fAnalyzer->SetTerminate();
            cout << "\n\nTerminating Program !" << endl;
            return;
         }

         if (!fAnalyzer->GetIO()->UserInput()) {
            fAnalyzer->SetTerminate();
            cout << "\n\nTerminating Program !" << endl;
            return;
         }

      }
      if (gShowTime) TimeEnd();

      cout << i << " events processed" << endl<<endl;

      // EndOfRun
      ExecuteTasks(gTaskEndOfRun);
      CleanTasks();

      if (!fAnalyzer->GetIO()->Disconnect()) {
         fAnalyzer->SetTerminate();
         cout << "\n\nTerminating Program !" << endl;
         return;
      }
   }

   // Terminate Tasks
   ExecuteTasks(gTaskTerminate);
   CleanTasks();

   if (!fAnalyzer->GetIO()->Terminate()) {
      fAnalyzer->SetTerminate();
      cout << "\n\nTerminating Program !" << endl;
      return;
   }
   if (gShowTime) {
      cout << "Program : run time = " << GetTime() << endl;
   }
}

