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
      fAnalyzer->Terminate();
      fAnalyzer->SetTerminationFlag();
      cout << "\n\nTerminating Program !" << endl;
      return;
   }
   if (!IsActive()) {
      fAnalyzer->Terminate();
      fAnalyzer->SetTerminationFlag();
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

   if (!fAnalyzer->Init()) {
      fAnalyzer->Terminate();
      fAnalyzer->SetTerminationFlag();
      cout << "\n\nTerminating Program !" << endl;
      return;
   }
 
   ExecuteTasks("i");
   CleanTasks();

   // Loop over Runs
   //----------------

   for (ii=0;!fAnalyzer->isTerminate();ii++) {

      if (!fAnalyzer->Connect(ii)) {
         fAnalyzer->Terminate();
         fAnalyzer->SetTerminationFlag();
         cout << "\n\nTerminating Program !" << endl;
         return;
      }
      if (fAnalyzer->isTerminate()) {
         break;
      }

     // Begin of Run Tasks
      if (ROMEAnalyzer::fTaskSwitchesChanged) {
         fAnalyzer->UpdateTaskSwitches();
         ROMEAnalyzer::fTaskSwitchesChanged = false;
      }
      ExecuteTasks("b");
      CleanTasks();
	  
      // Loop over Events
      //------------------
      if (gShowTime) TimeStart();
      cout << "\n\nRun " << fAnalyzer->GetCurrentRunNumber() << " started" << endl; 
      for (i=0;!fAnalyzer->isTerminate()&&!fAnalyzer->isEndOfRun();i++) {

         // User Input
         if (!fAnalyzer->UserInput()) {
            fAnalyzer->Terminate();
            fAnalyzer->SetTerminationFlag();
            cout << "\n\nTerminating Program !" << endl;
            return;
         }
         if (fAnalyzer->isTerminate()) {
            break;
         }

         // Set Fill Event equal true
         fAnalyzer->SetFillEvent();

         // Read Event
         if (!fAnalyzer->ReadEvent(i)) {
            fAnalyzer->Terminate();
            fAnalyzer->SetTerminationFlag();
            cout << "\n\nTerminating Program !" << endl;
            return;
         }
         if (fAnalyzer->isEndOfRun()) {
            break;
         }
         if (fAnalyzer->isTerminate()) {
            break;
         }
         if (fAnalyzer->isContinue()) {
            continue;
         }

         // Event Tasks
         if (ROMEAnalyzer::fTaskSwitchesChanged) {
            fAnalyzer->UpdateTaskSwitches();
            ROMEAnalyzer::fTaskSwitchesChanged = false;
         }
         char eventID = fAnalyzer->GetEventID();
         ExecuteTasks(&eventID);
         CleanTasks();

         // Write Event
         if (!fAnalyzer->WriteEvent() && fAnalyzer->isFillEvent()) {
            fAnalyzer->Terminate();
            fAnalyzer->SetTerminationFlag();
            cout << "\n\nTerminating Program !" << endl;
            return;
         }

         // Update
         if (!fAnalyzer->Update()) {
            fAnalyzer->Terminate();
            fAnalyzer->SetTerminationFlag();
            cout << "\n\nTerminating Program !" << endl;
            return;
         }
      }
      if (gShowTime) TimeEnd();

      // Show number of processed events
      cout << "Run " << fAnalyzer->GetCurrentRunNumber() << " stopped                                             " << endl << endl; 
      cout << (int)fAnalyzer->GetProcessedEvents() << " events processed" << endl <<endl;
      if (gShowTime) {
         cout << "run time = " << GetTime() << endl << endl;
      }

      // End of Run Tasks
      ExecuteTasks("e");
      CleanTasks();

      // Disconnect
      if (!fAnalyzer->Disconnect()) {
         fAnalyzer->Terminate();
         fAnalyzer->SetTerminationFlag();
         cout << "\n\nTerminating Program !" << endl;
         return;
      }
   }

   // Terminate Tasks
   ExecuteTasks("t");
   CleanTasks();

   // Terminate
   if (!fAnalyzer->Terminate()) {
      fAnalyzer->SetTerminationFlag();
      cout << "\n\nTerminating Program !" << endl;
      return;
   }
}

