/********************************************************************
  ROMETask.h, M. Schneebeli PSI

  $Log$
  Revision 1.13  2004/11/23 09:22:21  schneebeli_m
  User called Root Interpreter

  Revision 1.12  2004/11/17 16:13:00  schneebeli_m
  remove executetask

  Revision 1.11  2004/11/16 16:14:00  schneebeli_m
  implemented task hierarchy

  Revision 1.10  2004/09/25 01:34:48  schneebeli_m
  implemented FW dependent EventLoop and DataBase classes

********************************************************************/
#ifndef ROMETask_H
#define ROMETask_H

#include <TTask.h>
#include <ROMEString.h>
#include <TStopwatch.h>
#include <ROMEAnalyzer.h>

class ROMETask : public TTask {
private:
   TStopwatch     fWatch;          // Stopwatch
   ROMEString     fTitle;     // Task title
   ROMEString     fName;      // Task name
   ROMEString     fTimeString; // Elapsed Time in a readable format
   TFolder*       fHistoFolder;    // Histogram Folder of this Task in the Memory
protected:
   Int_t          fVersion;            // Version of Task
   Bool_t         fHasHistograms;      // Flags Tasks containing Histograms
   char           fEventID;            // TriggerID for event method
   ROMEString     fCurrentEventMethod; // Current event method name
public:
   ROMETask() { ; }
   ROMETask(const char *name,const char *title);
   void Exec(Option_t *option="");
   Bool_t   hasHistograms()  { return fHasHistograms; };
   Int_t    GetVersion()     { return fVersion; };
   TFolder* GetHistoFolder() { return fHistoFolder; };
protected:
   virtual void BookHisto() = 0;
   virtual void ResetHisto() = 0;
   virtual void Init() = 0;
   virtual void BeginOfRun() = 0;
   virtual void Event() = 0;
   virtual void EndOfRun() = 0;
   virtual void Terminate() = 0;

   void StartRootInterpreter(const char* message = NULL);

   void  TimeStart();
   void  TimeEnd();
   char* GetTime();

   ClassDef(ROMETask,1)
};

#endif   // ROMETask_H
