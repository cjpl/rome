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
   ROMEAnalyzer*  fAnalyzer;       // Handle to the Analyzer Class (should be overriden by each Task)
   Int_t          fVersion;        // Version of Task
   Bool_t         fHasHistograms;  // Flags Tasks containing Histograms
   char           fEventID;        // TriggerID for event method
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

   Bool_t   hasHistograms()  { return fHasHistograms; };
   Int_t    GetVersion()     { return fVersion; };
   TFolder* GetHistoFolder() { return fHistoFolder; };
protected:
   void  TimeStart();
   void  TimeEnd();
   char* GetTime();

   ClassDef(ROMETask,1)
};

#endif   // ROMETask_H
