/********************************************************************
  ROMETask.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMETask_H
#define ROMETask_H

#include <TTask.h>
#include <TFolder.h>
#include "ROMEString.h"
#include "ROMEStopwatch.h"

class ROMETask : public TTask {
private:
   ROMEString     fTitle;              // Task title
   ROMEString     fName;               // Task name
   Int_t          fLevel;              // Level in Task tree
protected:
   ROMEStopwatch  fWatchAll;           //! Records time used by Task
   ROMEStopwatch  fWatchUser;          //! Records time used by the user part of the task
   ROMEStopwatch  fWatchUserEvent;     //! Records time used by the user part of the task's event methods
   ROMEString     fTimeAllString;      //! Elapsed Time of all in a readable format
   ROMEString     fTimeUserString;     //! Elapsed Time of user in a readable format
   ROMEString     fTimeUserEventString;//! Elapsed Time of user events in a readable format
   Int_t          fVersion;            // Version of Task
   Bool_t         fHasHistograms;      // Flags Tasks containing Histograms
   int            fEventID;            // TriggerID for event method
   ROMEString     fCurrentEventMethod; // Current event method name
   TFolder       *fHistoFolder;        // Histogram Folder of this Task in the Memory

public:
   ROMETask() { fHistoFolder = 0; }
   ROMETask(const char *name,const char *title,int level);
   virtual ~ROMETask() { SafeDelete(fHistoFolder); }
   void         Exec(Option_t *option="");
   Bool_t       hasHistograms()  { return fHasHistograms; }
   Int_t        GetVersion()     { return fVersion; }
   TFolder     *GetHistoFolder() { return fHistoFolder; }

protected:
   void         StartRootInterpreter(const char* message = NULL);
   const char  *GetTimeOfUserEvents();
   const char  *GetTimeOfUser();
   const char  *GetTimeOfAll();

   virtual void BookHisto() = 0;
   virtual void ResetHisto() = 0;
   virtual void Init() = 0;
   virtual void BeginOfRun() = 0;
   virtual void Event() = 0;
   virtual void EndOfRun() = 0;
   virtual void Terminate() = 0;


   ClassDef(ROMETask,0) // Customized TTask for ROME
};

#endif   // ROMETask_H