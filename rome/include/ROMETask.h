/********************************************************************
  ROMETask.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMETask_H
#define ROMETask_H

#include <TTask.h>
#include <TFolder.h>
#include <TObjArray.h>
#include "ROMEHisto.h"
#include "ROMEGraph.h"
#include "ROMEString.h"
#include "ROMEStopwatch.h"

class ROMETask : public TTask {
private:
   Int_t          fLevel;              // Level in Task tree

protected:
   ROMEStopwatch  fWatchAll;           //! Records time used by Task
   ROMEStopwatch  fWatchUserEvent;     //! Records time used by the user part of the task's event methods
   ROMEString     fCpuTimeAllString;   //! Elapsed CPU Time in a readable format
   ROMEString     fRealTimeAllString;  //! Elapsed Real Time in a readable format
   ROMEString     fTimeUserEventString;//! Elapsed CPU Time of user events in a readable format
   Int_t          fVersion;            // Version of Task
   Bool_t         fHasHistograms;      // Flags Tasks containing Histograms
   Bool_t         fHasGraphs;          // Flags Tasks containing Histograms
   int            fEventID;            // TriggerID for event method
   ROMEString     fCurrentEventMethod; // Current event method name
   TFolder       *fHistoFolder;        // Histogram Folder of this Task in the Memory
   Long64_t       fSkippedEvents;      //! Number of events skipped filling tree by this task
   Int_t          fMemoryAccumulated;  //! Accumulated used memory difference between BOE and EOE.
   TObjArray     *fHisto;              //! Handle to histograms
   TObjArray     *fHistoParameter;     //! Handle to histogram parameters class
   Int_t          fNumberOfHistos;     //! Number of histograms in this task
   TObjArray     *fGraph;              //! Handle to graphs
   TObjArray     *fGraphParameter;     //! Handle to graph parameters class
   Int_t          fNumberOfGraphs;     //! Number of graphs in this task

private:
   ROMETask(const ROMETask &task); // not implemented
   ROMETask &operator=(const ROMETask &rhs); // not implemented

public:
   ROMETask();
   ROMETask(const char *name,const char *title,int level,int version,int eventID,
            bool hasHisto,bool hasGraph,TFolder* histoFolder);

   virtual ~ROMETask() { SafeDelete(fHistoFolder); }
   void         Exec(Option_t *option="");
   Bool_t       hasHistograms() const { return fHasHistograms; }
   Bool_t       hasGraphs() const { return fHasGraphs; }
   Int_t        GetVersion() const { return fVersion; }
   TFolder     *GetHistoFolder() const { return fHistoFolder; }
   TObject     *GetHistoAt(Int_t i) { return fHisto->At(i); };
   ROMEHisto   *GetHistoParameterAt(Int_t i) { return ((ROMEHisto*)fHistoParameter->At(i)); };
   Int_t        GetNumberOfHistos() { return fNumberOfHistos; };
   TObject     *GetGraphAt(Int_t i) { return fGraph->At(i); };
   ROMEGraph   *GetGraphParameterAt(Int_t i) { return ((ROMEGraph*)fGraphParameter->At(i)); };
   Int_t        GetNumberOfGraphs() { return fNumberOfGraphs; };

protected:
   void         StartRootInterpreter(const char* message = NULL);
   const char  *GetTimeOfUserEvents();
   const char  *GetRealTimeOfAll();
   const char  *GetCpuTimeOfAll();

   virtual void BookHisto() = 0;
   virtual void ReBookHisto() = 0;
   virtual void ResetHisto() = 0;
   virtual void BookGraph() = 0;
   virtual void ReBookGraph() = 0;
   virtual void ResetGraph() = 0;
   virtual void Init() = 0;
   virtual void BeginOfRun() = 0;
   virtual void Event() = 0;
   virtual void EndOfRun() = 0;
   virtual void Terminate() = 0;


   ClassDef(ROMETask,0) // Customized TTask for ROME
};

#endif   // ROMETask_H
