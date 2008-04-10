/********************************************************************
  ROMETask.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMETask_H
#define ROMETask_H

#if (ROOT_VERSION_CODE < ROOT_VERSION(5,15,2))
#   include <TDirectory.h>
#else
#   include <TDirectoryFile.h>
#endif
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
   Int_t          fTaskIndex;          //!
   ROMEString     fTaskSuffix;         //!
   ROMEStopwatch  fWatchAll;           //! Records time used by Task
   ROMEStopwatch  fWatchUserEvent;     //! Records time used by the user part of the task's event methods
   ROMEString     fCpuTimeAllString;   //! Elapsed CPU Time in a readable format
   ROMEString     fRealTimeAllString;  //! Elapsed Real Time in a readable format
   ROMEString     fTimeUserEventString;//! Elapsed CPU Time of user events in a readable format
   Int_t          fVersion;            // Version of Task
   int            fEventID;            // TriggerID for event method
   ROMEString     fCurrentEventMethod; // Current event method name
   TObjArray     *fRootFolder;         // Handle to histogram/graph root folder
   TFolder       *fHistoFolder;        // Histogram Folder of this Task in the Memory
   Long64_t       fSkippedEvents;      //! Number of events skipped filling tree by this task
   Int_t          fMemoryAccumulated;  //! Accumulated used memory difference between BOE and EOE.
   Int_t          fNumberOfHistos;     //! Number of histograms in this task
   TObjArray     *fHisto;              //! Handle to histograms
   TObjArray     *fHistoParameter;     //! Handle to histogram parameters class
   ROMEString    *fHistoName;          //!
   ROMEString    *fHistoType;          //!
   Int_t         *fHistoDimension;     //!
   Bool_t        *fHistoArray;         //!
   Bool_t        *fHistoUpToDate;      //!
   Int_t          fNumberOfGraphs;     //! Number of graphs in this task
   TObjArray     *fGraph;              //! Handle to graphs
   TObjArray     *fGraphStorage;       //! Handle to graph storages
   TObjArray     *fGraphParameter;     //! Handle to graph parameters class
   ROMEString    *fGraphName;          //!
   ROMEString    *fGraphType;          //!
   Int_t         *fGraphDimension;     //!
   Bool_t        *fGraphArray;         //!
   Int_t          fInitTerminateState; //!
   Int_t          fBorEorState;        //!

private:
   ROMETask(const ROMETask &task); // not implemented
   ROMETask &operator=(const ROMETask &rhs); // not implemented

public:
   ROMETask();
   ROMETask(const char *name, const char *title, int level, int version, int eventID,
            TFolder* histoFolder, const char* histoSuffix,
            int numberOfHistos, int numberOfGraphs);
   virtual ~ROMETask();

   void         Exec(Option_t *option="");
   Int_t        GetVersion() const { return fVersion; }

   // Task methods
   ROMEString*   GetTaskSuffix() { return &fTaskSuffix; }
   Int_t         GetTaskIndex() { return fTaskIndex; }

   // Histo methods
public:
//   Bool_t       hasHistograms() const { return fNumberOfHistos>0; }
   TFolder     *GetHistoFolder() const { return fHistoFolder; }
   TObject     *GetHistoAt(Int_t i);
   ROMEHisto   *GetHistoParameterAt(Int_t i) { return ((ROMEHisto*)fHistoParameter->At(i)); }
   Int_t        GetNumberOfHistos() { return fNumberOfHistos; }
   ROMEString*  GetHistoNameAt(Int_t i) { return &fHistoName[i]; }
   Bool_t       IsHistoUpToDateAt(Int_t i) { return fHistoUpToDate[i]; }
   void         SetHistoUpToDateAt(Int_t i,Bool_t flag) { fHistoUpToDate[i] = flag; }
   void         ResetHisto();
   void         CopyHistosAndGraphs(TDirectory *d) const;
   void         PropagateDeactivation();

protected:
   void         BookHisto();
   void         ReBookHisto();
   Bool_t       CheckHistoActive(Int_t histoIndex);

   // Graph mothods
public:
//   Bool_t       hasGraphs() const { return fNumberOfGraphs>0; }
   TObject     *GetGraphAt(Int_t i) { return fGraph->At(i); }
   TObject     *GetGraphStorageAt(Int_t i) { return fGraphStorage->At(i); }
   ROMEGraph   *GetGraphParameterAt(Int_t i) { return ((ROMEGraph*)fGraphParameter->At(i)); }
   Int_t        GetNumberOfGraphs() { return fNumberOfGraphs; }
   ROMEString*  GetGraphNameAt(Int_t i) { return &fGraphName[i]; }
   void         ResetGraph();
protected:
   void         BookGraph();
   void         ReBookGraph();
   Bool_t       CheckGraphActive(Int_t histoIndex);

protected:

   // Utility methods
   virtual void        SetOriginalHistoParameters() = 0;
   virtual void        SetOriginalGraphParameters() = 0;
   virtual Int_t       GetObjectInterpreterCode       (const char* objectPath) const = 0;
   virtual Int_t       GetObjectInterpreterIntValue   (Int_t code, Int_t defaultValue) const = 0;
   virtual Double_t    GetObjectInterpreterDoubleValue(Int_t code, Double_t defaultValue) const = 0;
   virtual ROMEString& GetObjectInterpreterCharValue  (Int_t code, ROMEString& defaultValue,ROMEString& buffer) const = 0;

   void         StartRootInterpreter(const char* message = NULL);

   // Time Methods
   const char  *GetTimeOfUserEvents();
   const char  *GetRealTimeOfAll();
   const char  *GetCpuTimeOfAll();

   // Event Methods
   virtual void Init() = 0;
   virtual void BeginOfRun() = 0;
   virtual void Event() = 0;
   virtual void EndOfRun() = 0;
   virtual void Terminate() = 0;

   ClassDef(ROMETask,1) // Customized TTask for ROME
};

#endif   // ROMETask_H
