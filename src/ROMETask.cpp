// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMETask                                                             //
//                                                                      //
//  The Base Class for all Tasks in the Data Analysis Tool. 
//  The following methods should be overriden by derived Classes :
//    Init
//    BeginOfRun
//    Event
//    EndOfRun
//    Terminate
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <TROOT.h>
#include <TFile.h>
#include "ROMETask.h"
#include "ROMEStatic.h"
#include "Riostream.h"

ClassImp(ROMETask)

ROMETask::ROMETask(const char *name,const char *title,ROMEAnalyzer *analyzer):TTask(name,title)
{
   // Initialisation of Class
   strcpy(fTitle,title);
   strcpy(fName,name);
   fAnalyzer = analyzer;
   fRunTime = 0;
}
void ROMETask::Exec(Option_t *option)
{
   // Overrides the TTask::Exec Method to replace the Exec Method with the following Methods :
   // Init
   // BeginOfRun
   // Event
   // EndOfRun
   // Terminate
   char foldername[1000];
   if (!strcmp(option,gTaskInit)) {
      if (gPrintProgress) cout << fName << " (initialisation)" << endl;
      if (gShowTime) TimeStart();
      sprintf(foldername,"%sHistos",this->GetName());
      fHistoFolder = ((TFolder*)gROOT->FindObjectAny(foldername));
      BookHisto();
      Init();
      if (gShowTime) TimeEnd();
   }
   if (!strcmp(option,gTaskBeginOfRun)) {
      if (gPrintProgress) cout << fName << " (begin of run)" << endl;
      if (gShowTime) TimeStart();
      ResetHisto();
      BeginOfRun();
      if (gShowTime) TimeEnd();
   }
   if (!strcmp(option,gTaskEvent)) {
      if (gPrintProgress) cout << fName << " (event)" << endl;
      if (gShowTime) TimeStart();
      Event();
      if (gShowTime) TimeEnd();
   }
   if (!strcmp(option,gTaskEndOfRun)) {
      if (gPrintProgress) cout << fName << " (end of run)" << endl;
      if (gShowTime) TimeStart();
      EndOfRun();
      if (gShowTime) TimeEnd();
   }
   if (!strcmp(option,gTaskTerminate)) {
      if (gShowTime) TimeStart();
      Terminate();
      if (gShowTime) TimeEnd();
      if (gShowTime) {
         cout << "Task '" << fName << "' : run time = " << GetTime() << endl;
      }
   }
}

// Time methods

void ROMETask::TimeStart()
{
   // Starts the Tasks stopwatch
   TDatime fDatime;
   fStartTime = fDatime.GetTime();
}
void ROMETask::TimeEnd()
{
   // Ends the Tasks stopwatch
   TDatime fDatime;
   fRunTime = fRunTime +fDatime.GetTime() -fStartTime;
}
char* ROMETask::GetTime()
{
   // Returns the elapsed time in a readable format
   sprintf(timeString, "%i%i:%i%i:%i%i", fRunTime / 36000, (fRunTime % 36000) / 3600,
           (fRunTime % 3600) / 600, (fRunTime % 600) / 60, (fRunTime % 60) / 10,
           fRunTime % 10);
   return timeString;
}


