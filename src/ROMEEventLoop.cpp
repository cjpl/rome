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
      cout << "Terminating Program !" << endl;
      return;
   }
   if (!IsActive()) return;

   // Declarations
   //--------------
   int i,ii;

// Read Histos
//   sprintf(filename,"%s%s%s.root",fAnalyzer->GetInputDir(),"histos",runNumber);
//   fAnalyzer->SetHistoFileHandle(new TFile(filename,"READ"));

   // Initialisation
   //----------------

   fAnalyzer->InitFolders();
   if (!fAnalyzer->GetIO()->Init()) {
      cout << "Terminating Program !" << endl;
      return;
   }
 
   ExecuteTasks(gTaskInit);
   CleanTasks();



   // Loop over Runs
   //----------------
   for (ii=0;!fAnalyzer->GetIO()->isTerminate();ii++) {

      if (!fAnalyzer->GetIO()->Connect(ii)) {
         cout << "Terminating Program !" << endl;
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
      for (i=0;!fAnalyzer->GetIO()->isTerminate()&&!fAnalyzer->GetIO()->isEndOfRun();i++) {
         if (!((i+1)%1000) && fAnalyzer->GetIO()->isOffline()) cout << i+1 << " events processed\r";

         fAnalyzer->SetFillEvent();

         if (!fAnalyzer->GetIO()->ReadEvent(i)) {
            cout << "Terminating Program !" << endl;
            return;
         }
         if (fAnalyzer->GetIO()->isTerminate()||fAnalyzer->GetIO()->isEndOfRun()) {
            break;
         }
         if (fAnalyzer->GetIO()->isContinue()) {
            continue;
         }

         // Event
         ExecuteTasks(gTaskEvent);
         CleanTasks();

         if (!fAnalyzer->GetIO()->WriteEvent() && fAnalyzer->isFillEvent()) {
            cout << "Terminating Program !" << endl;
            return;
         }

         if (!fAnalyzer->GetIO()->UserInput()) {
            cout << "Terminating Program !" << endl;
            return;
         }

      }
      if (gShowTime) TimeEnd();

      cout << i << " events processed" << endl<<endl;

      // EndOfRun
      ExecuteTasks(gTaskEndOfRun);
      CleanTasks();

      if (!fAnalyzer->GetIO()->Disconnect()) {
         cout << "Terminating Program !" << endl;
         return;
      }
   }

   // Terminate Tasks
   ExecuteTasks(gTaskTerminate);
   CleanTasks();

   if (!fAnalyzer->GetIO()->Terminate()) {
      cout << "Terminating Program !" << endl;
      return;
   }
   if (gShowTime) {
      cout << "Program : run time = " << GetTime() << endl;
   }
}

