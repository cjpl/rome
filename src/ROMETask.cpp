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
   fTitle = title;
   fName = name;
   fAnalyzer = analyzer;
   fEventID = 'a';
}
void ROMETask::Exec(Option_t *option)
{
   // Overrides the TTask::Exec Method to replace the Exec Method with the following Methods :
   // Init
   // BeginOfRun
   // Event
   // EndOfRun
   // Terminate
   if (!strcmp(option,"i")) {
      ROMEString foldername;
      foldername.SetFormatted("%sHistos",this->GetName());
      fHistoFolder = ((TFolder*)gROOT->FindObjectAny(foldername.Data()));
      BookHisto();
      Init();
   }
   else if (!strcmp(option,"b")) {
      ResetHisto();
      BeginOfRun();
   }
   else if (!strcmp(option,"e")) {
      EndOfRun();
   }
   else if (!strcmp(option,"t")) {
      Terminate();
      if (gShowTime) {
         cout << "Task '" << fName.Data() << "' : run time = " << GetTime() << endl;
      }
   }
   else if (!strcmp(&fEventID,"a") || !strcmp(option,&fEventID)) {
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
   fTimeString.SetFormatted("%d%d:%d%d:%d%d:%d", (int)(runTime / 36000), (int)((runTime % 36000) / 3600),
           (int)((runTime % 3600) / 600), (int)((runTime % 600) / 60), (int)((runTime % 60) / 10),
           (int)(runTime % 10),milli);
   return (char*)fTimeString.Data();
}


