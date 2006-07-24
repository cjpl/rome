/********************************************************************
  ROMEEventLoop.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMEEventLoop_H
#define ROMEEventLoop_H

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( push )
#   pragma warning( disable : 4800 4244)
#endif // R__VISUAL_CPLUSPLUS
#include <TTimeStamp.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS
#include "ROME.h"
#include "ROMETask.h"

class ROMETreeInfo;
class TFile;
class TTime;

class ROMEEventLoop : public ROMETask {
protected:
   enum {
      kReturn,
      kBreak,
      kContinue
   };
protected:
   Long64_t      fCurrentEvent;                    //! Current Event Number inside Eventloop
   Bool_t        fFirstUserInput;                  //! Flags the first user input
   Bool_t        fStop;                            //! Stop execution

   // Stop at
   Long64_t      fStopAtRun;                       //! Stop execution at this run
   Long64_t      fStopAtEvent;                     //! Stop execution at this event

   // Statistics update
   ULong_t       fStatisticsTimeOfLastEvent;       //! Time of last Event
   Double_t      fStatisticsLastEvent;             //! Last Event

   // Progress Display
   Int_t         fProgressDelta;                   //! Maximal time difference
   ULong_t       fProgressTimeOfLastEvent;         //! Last time for display
   Long64_t      fProgressLastEvent;               //! Last Event
   Bool_t        fProgressWrite;                   //! Write flag

   // user input
   ULong_t       fUserInputLastTime;               //! Last time for user input
   Bool_t        fContinuous;                      //! Continuous Mode

   // Output Tree Files
   ROMETreeInfo *fTreeInfo;                        //! Tree Info Object
   Long64_t      fSequentialNumber;                //! Sequential Number
   Int_t         fTreeUpdateIndex;                 //! Index of the tree (for trees in the same file)

   // Histo File
   TFile        *fHistoFile;                       //! Histo file

   ROMEStopwatch fFileIOWatch;                     //! Records Time used by File IO

   // Monitor
   Bool_t    fUpdateWindow;                           //! Window update activation flag
   ULong_t   fLastUpdateTime;                         //! Time of the last window update
public:
   // Static Task Switches Changes Flag
   static Bool_t fTaskSwitchesChanged;               //! Flag Task Switches Changes

public:
   ROMEEventLoop();
   virtual ~ROMEEventLoop();
   ROMEEventLoop(const char *name,const char *title);
   void         ExecuteTask(Option_t *option="");
   void         BookHisto() {}
   void         ResetHisto() {}
   void         Init() {}
   void         BeginOfRun() {}
   void         Event() {}
   void         EndOfRun() {}
   void         Terminate() {}

   void         SetContinuousMode(Bool_t mode) { fContinuous = mode; }
   Bool_t       isContinuousMode() { return fContinuous; }

   Int_t        RunEvent();
   void         NextEvent();
   void         GotoEvent(Long64_t eventNumber);

   virtual void AddTreeBranches() = 0;
protected:
   // Run Status
   inline Bool_t isRunning();
   inline Bool_t isStopped();

   inline void   SetRunning();
   inline void   SetStopped();

   // Event Status
   inline Bool_t isAnalyze();
   inline Bool_t isContinue();
   inline Bool_t isBeginOfRun();
   inline Bool_t isEndOfRun();
   inline Bool_t isTerminate();

   inline void   SetAnalyze();
   inline void   SetContinue();
   inline void   SetBeginOfRun();
   inline void   SetEndOfRun();
   inline void   SetTerminate();

   // event methods
   Bool_t       DAQInit();
   Bool_t       DAQBeginOfRun(Long64_t runNumber);
   Bool_t       DAQEvent();
   Bool_t       UserInput();
   Bool_t       WriteEvent();
   Bool_t       Update();
   Bool_t       DAQEndOfRun();
   Bool_t       DAQTerminate();

   // virtual methods
   virtual void InitSingleFolders() = 0;
   virtual void InitArrayFolders() = 0;
   virtual void CleanUpFolders() = 0;
   virtual void ResetFolders() = 0;

   virtual void InitTaskSwitches() = 0;
   virtual void UpdateTaskSwitches() = 0;

   virtual void InitTrees() = 0;
   virtual void FillTrees() = 0;
   virtual void GetTreeFileName(ROMEString& buffer,Int_t treeIndex,const char* runNumber="") = 0;

   virtual void ReadHistograms() = 0;

   ClassDef(ROMEEventLoop, 0) // Base event loop class
};

#endif   // ROMEEventLoop_H
