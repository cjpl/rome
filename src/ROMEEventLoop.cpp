// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMEEventLoop                                                        //
//                                                                      //
//  Implements the event loop. This Class must be the root Task of 
//  the Application.
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <ROMEEventLoop.h>
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
      fAnalyzer->GetIO()->Terminate();
      fAnalyzer->SetTerminate();
      cout << "\n\nTerminating Program !" << endl;
      return;
   }
   if (!IsActive()) {
      fAnalyzer->GetIO()->Terminate();
      fAnalyzer->SetTerminate();
      cout << "\n\nTerminating Program !" << endl;
      return;
   }

   // Declarations
   //--------------
   int i,ii;

// Read Histos
//   sprintf(filename,"%s%s%s.root",fAnalyzer->GetInputDir(),"histos",runNumber);
//   fAnalyzer->SetHistoFileHandle(new TFile(filename,"READ"));

   // Initialisation
   //----------------

   fAnalyzer->InitFolders();
   fAnalyzer->InitTaskSwitches();
   if (!fAnalyzer->GetIO()->Init()) {
      fAnalyzer->GetIO()->Terminate();
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
         fAnalyzer->GetIO()->Terminate();
         fAnalyzer->SetTerminate();
         cout << "\n\nTerminating Program !" << endl;
         return;
      }
      if (fAnalyzer->GetIO()->isTerminate()) {
         break;
      }

      // Begin of Run Tasks
      if (ROMEIO::fTaskSwitchesChanged) {
         fAnalyzer->UpdateTaskSwitches();
         ROMEIO::fTaskSwitchesChanged = false;
      }
      ExecuteTasks(gTaskBeginOfRun);
      CleanTasks();

      if (gShowTime) TimeStart();
      // Loop over Events
      //------------------
      for (i=0;!fAnalyzer->GetIO()->isTerminate()&&!fAnalyzer->GetIO()->isEndOfRun();i++) {

         // User Input
         if (!fAnalyzer->GetIO()->UserInput()) {
            fAnalyzer->GetIO()->Terminate();
            fAnalyzer->SetTerminate();
            cout << "\n\nTerminating Program !" << endl;
            return;
         }
         if (fAnalyzer->GetIO()->isTerminate()) {
            break;
         }

         // Set Fill Event equal true
         fAnalyzer->SetFillEvent();

         // Read Event
         if (!fAnalyzer->GetIO()->ReadEvent(i)) {
            fAnalyzer->GetIO()->Terminate();
            fAnalyzer->SetTerminate();
            cout << "\n\nTerminating Program !" << endl;
            return;
         }
         if (fAnalyzer->GetIO()->isEndOfRun()) {
            break;
         }
         if (fAnalyzer->GetIO()->isTerminate()) {
            break;
         }
         if (fAnalyzer->GetIO()->isContinue()) {
            continue;
         }

         // Event Tasks
         if (ROMEIO::fTaskSwitchesChanged) {
            fAnalyzer->UpdateTaskSwitches();
            ROMEIO::fTaskSwitchesChanged = false;
         }
         ExecuteTasks(fAnalyzer->GetIO()->GetEventID());
         CleanTasks();

         // Write Event
         if (!fAnalyzer->GetIO()->WriteEvent() && fAnalyzer->isFillEvent()) {
            fAnalyzer->GetIO()->Terminate();
            fAnalyzer->SetTerminate();
            cout << "\n\nTerminating Program !" << endl;
            return;
         }

         // Update
         if (!fAnalyzer->GetIO()->Update()) {
            fAnalyzer->GetIO()->Terminate();
            fAnalyzer->SetTerminate();
            cout << "\n\nTerminating Program !" << endl;
            return;
         }
      }
      if (gShowTime) TimeEnd();

      // Show number of processed events
      cout << (int)fAnalyzer->GetIO()->GetProcessedEvents() << " events processed" << endl<<endl;

      // End of Run Tasks
      ExecuteTasks(gTaskEndOfRun);
      CleanTasks();

      // Disconnect
      if (!fAnalyzer->GetIO()->Disconnect()) {
         fAnalyzer->GetIO()->Terminate();
         fAnalyzer->SetTerminate();
         cout << "\n\nTerminating Program !" << endl;
         return;
      }
   }

   // Terminate Tasks
   ExecuteTasks(gTaskTerminate);
   CleanTasks();

   // Terminate
   if (!fAnalyzer->GetIO()->Terminate()) {
      fAnalyzer->SetTerminate();
      cout << "\n\nTerminating Program !" << endl;
      return;
   }
   if (gShowTime) {
      cout << "Program : run time = " << GetTime() << endl;
   }
}

