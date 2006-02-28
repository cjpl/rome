/********************************************************************
  ROMETask.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMETask_H
#define ROMETask_H

#include <TTask.h>
#include <TFolder.h>
#include <ROMEString.h>
#include <ROMEStopwatch.h>

class ROMETask : public TTask {
private:
   ROMEStopwatch  fWatch;              // Stopwatch
   ROMEString     fTitle;              // Task title
   ROMEString     fName;               // Task name
   ROMEString     fTimeString;         // Elapsed Time in a readable format
protected:
   Int_t          fVersion;            // Version of Task
   Bool_t         fHasHistograms;      // Flags Tasks containing Histograms
   char           fEventID;            // TriggerID for event method
   ROMEString     fCurrentEventMethod; // Current event method name
   TFolder       *fHistoFolder;        // Histogram Folder of this Task in the Memory
public:
   ROMETask() { ; }
   ROMETask(const char *name,const char *title);
   void         Exec(Option_t *option="");
   Bool_t       hasHistograms()  { return fHasHistograms; };
   Int_t        GetVersion()     { return fVersion; };
   TFolder     *GetHistoFolder() { return fHistoFolder; };
protected:
   virtual void BookHisto() = 0;
   virtual void ResetHisto() = 0;
   virtual void Init() = 0;
   virtual void BeginOfRun() = 0;
   virtual void Event() = 0;
   virtual void EndOfRun() = 0;
   virtual void Terminate() = 0;

   void         StartRootInterpreter(const char* message = NULL);

   void         TimeReset();
   void         TimeStart();
   void         TimeEnd();
   const char  *GetTime();

   ClassDef(ROMETask,1)
};

#endif   // ROMETask_H
