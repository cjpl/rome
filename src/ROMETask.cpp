// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMETask                                                             //
//                                                                      //
//  The Base Class for all Tasks in ROME. 
//  The following methods should be overriden by derived Classes :
//    Init
//    BeginOfRun
//    Event
//    EndOfRun
//    Terminate
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <ROMETask.h>
#include "Riostream.h"

ClassImp(ROMETask)

ROMETask::ROMETask(const char *name,const char *title,ROMEAnalyzer *analyzer):TTask(name,title)
{
   // Initialisation of Class
   strcpy(fTitle,title);
   strcpy(fName,name);
   fAnalyzer = analyzer;
   strcpy(fEventID,"all");
}
void ROMETask::Exec(Option_t *option)
{
   // Overrides the TTask::Exec Method to replace the Exec Method with the following Methods :
   // Init
   // BeginOfRun
   // Event
   // EndOfRun
   // Terminate
   if (!strcmp(option,gTaskInit)) {
      char foldername[1000];
      sprintf(foldername,"%sHistos",this->GetName());
      fHistoFolder = ((TFolder*)gROOT->FindObjectAny(foldername));
      BookHisto();
      Init();
   }
   else if (!strcmp(option,gTaskBeginOfRun)) {
      ResetHisto();
      BeginOfRun();
   }
   else if (!strcmp(option,gTaskEndOfRun)) {
      EndOfRun();
   }
   else if (!strcmp(option,gTaskTerminate)) {
      Terminate();
      if (gShowTime) {
         cout << "Task '" << fName << "' : run time = " << GetTime() << endl;
      }
   }
   else if (!strcmp(fEventID,"all") || !strcmp(option,fEventID)) {
      if (gShowTime) TimeStart();
      Event();
      if (gShowTime) TimeEnd();
   }
}

// Time methods

void ROMETask::TimeStart()
{
   // Starts the Tasks stopwatch
   fWatch.Start(false);
}
void ROMETask::TimeEnd()
{
   // Ends the Tasks stopwatch
   fWatch.Stop();
}
char* ROMETask::GetTime()
{
   // Returns the elapsed time in a readable format
   int runTime = (int)fWatch.RealTime();
   int milli = (int)((fWatch.RealTime()-runTime)*1000);
   sprintf(fTimeString, "%i%i:%i%i:%i%i:%d", runTime / 36000, (runTime % 36000) / 3600,
           (runTime % 3600) / 600, (runTime % 600) / 60, (runTime % 60) / 10,
           runTime % 10,milli);
   return fTimeString;
}


