#ifndef ROMETask_H
#define ROMETask_H

#include <TDatime.h>
#include <TTask.h>
#include "ROMEAnalyzer.h"

class ROMETask : public TTask {
private:
   Int_t fStartTime;          // Start time of Task
   Int_t fRunTime;            // Elapsed time of Task
   char fTitle[100];          // Task title
   char fName[100];           // Task name
   char timeString[10];       // Elapsed Time in a readable format
   TFolder *fHistoFolder;     // Histogram Folder of this Task in the Memory
protected:
   ROMEAnalyzer *fAnalyzer;    // Handle to the Analyzer Class (should be overriden by each Task)
   Int_t        fVersion;     // Version of Task
public:
   ROMETask() { ; }
   ROMETask(const char *name,const char *title,ROMEAnalyzer *analyzer);
   void Exec(Option_t *option="");
   virtual void BookHisto() = 0;
   virtual void ResetHisto() = 0;
   virtual void Init() = 0;
   virtual void BeginOfRun() = 0;
   virtual void Event() = 0;
   virtual void EndOfRun() = 0;
   virtual void Terminate() = 0;

   Int_t GetVersion() { return fVersion; };
   TFolder *GetHistoFolder() { return fHistoFolder; };
protected:
   void TimeStart();
   void TimeEnd();
   char* GetTime();

   ClassDef(ROMETask,1)
};

#endif   // ROMETask_H
