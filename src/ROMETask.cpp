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
//  $Log$
//  Revision 1.16  2004/11/16 16:14:01  schneebeli_m
//  implemented task hierarchy
//
//  Revision 1.15  2004/10/05 07:52:44  schneebeli_m
//  dyn. Folders, TRef Objects, XML format changed, ROMEStatic removed
//
//  Revision 1.14  2004/09/25 01:34:48  schneebeli_m
//  implemented FW dependent EventLoop and DataBase classes
//
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <ROMETask.h>
#include "ROME.h"
#include "Riostream.h"

ClassImp(ROMETask)

ROMETask::ROMETask(const char *name,const char *title):TTask(name,title)
{
   // Initialisation of Class
   fTitle = title;
   fName = name;
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
   if (!strncmp(option,"i",1)) {
      ROMEString foldername;
      foldername.SetFormatted("%sHistos",this->GetName());
      fHistoFolder = ((TFolder*)gROOT->FindObjectAny(foldername.Data()));
      BookHisto();
      Init();
   }
   else if (!strncmp(option,"b",1)) {
      ResetHisto();
      BeginOfRun();
   }
   else if (!strncmp(option,"e",1)) {
      EndOfRun();
   }
   else if (!strncmp(option,"t",1)) {
      Terminate();
      if (gShowTime) {
         cout << "Task '" << fName.Data() << "' : run time = " << GetTime() << endl;
      }
   }
   else if (!strncmp(&fEventID,"a",1) || !strncmp(option,&fEventID,1)) {
      if (gShowTime) TimeStart();
      Event();
      if (gShowTime) TimeEnd();
   }
}

void TTask::ExecuteTasks(Option_t *option)
{
   // Execute all the subtasks of a task.

   fOption = option;
   fOption += ",";
   fOption += this->GetName();

   TIter next(fTasks);
   TTask *task;
   while((task=(TTask*)next())) {
      if (fgBreakPoint) return;
      if (!task->IsActive()) continue;
      if (task->fHasExecuted) {
         task->ExecuteTasks(fOption);
         continue;
      }
      if (task->fBreakin == 1) {
         printf("Break at entry of task: %s\n",task->GetName());
         fgBreakPoint = this;
         task->fBreakin++;
         return;
      }

      if (gDebug > 1) {
         TROOT::IndentLevel();
         cout<<"Execute task:"<<task->GetName()<<" : "<<task->GetTitle()<<endl;
         TROOT::IncreaseDirLevel();
      }
      task->Exec(fOption);
      task->fHasExecuted = kTRUE;
      task->ExecuteTasks(fOption);
      if (gDebug > 1) TROOT::DecreaseDirLevel();
      if (task->fBreakout == 1) {
         printf("Break at exit of task: %s\n",task->GetName());
         fgBreakPoint = this;
         task->fBreakout++;
         return;
      }
   }
}
void ROMETask::GetParentTaskNames(ROMEStrArray &names) 
{
   int is,ie,num=0;
   if ((is = fOption.Index("mainTask",8,0,TString::kExact))==-1)
      return;
   ROMEString str = fOption(is+8,fOption.Length()-is-8);
   ROMEString name;
   if (str.Length()==0)
      return;
   while (str[0]==',') {
      if ((ie = str.Index(",",1,1,TString::kExact))==-1) {
         name = str(1,str.Length()-1);
         names.AddAtAndExpand(name,num);
         return;
      }
      else {
         name = str(1,ie-1);
         str = str(ie,str.Length()-ie);
         names.AddAtAndExpand(name,num);
         num++;
      }
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


