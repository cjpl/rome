/********************************************************************
  ROMEEventLoop.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMEEventLoop_H
#define ROMEEventLoop_H

#include "ROMETask.h"
#include "ROMEAnalyzer.h"

class ROMEEventLoop : public ROMETask {
protected:
   // Stop at
   Long64_t      fStopAtRun;                       //! Stop execution at this run
   Long64_t      fStopAtEvent;                     //! Stop execution at this event

   // Statistics update
   Int_t         fStatisticsTimeOfLastEvent;       //! Time of last Event
   Double_t      fStatisticsLastEvent;             //! Last Event

   // Progress Display
   Int_t         fProgressDelta;                   //! Maximal time difference
   time_t        fProgressTimeOfLastEvent;         //! Last time for display
   Long64_t      fProgressLastEvent;               //! Last Event
   Bool_t        fProgressWrite;                   //! Write flag

   // user input
   time_t        fUserInputLastTime;               //! Last time for user input
   Bool_t        fContinuous;                      //! Continuous Mode

   // Output Tree Files
   ROMETreeInfo *fTreeInfo;                        //! Tree Info Object
   Long64_t      fSequentialNumber;                //! Sequential Number
   Int_t         fTreeUpdateIndex;                 //! Index of the tree (for trees in the same file)

   // Histo File
   TFile        *fHistoFile;                       //! Histo file

   ROMEStopwatch fFileIOWatch;                     //! Records Time used by File IO

   Int_t         fSavedUpdateFrequency;            //! Saved update frequency for wait
public:
   // Static Task Switches Changes Flag
   static Bool_t fTaskSwitchesChanged;               //! Flag Task Switches Changes

public:
   ROMEEventLoop() { ; }
   ROMEEventLoop(const char *name,const char *title);
   void         ExecuteTask(Option_t *option="");
   void         BookHisto() {};
   void         ResetHisto() {};
   void         Init() {};
   void         BeginOfRun() {};
   void         Event() {};
   void         EndOfRun() {};
   void         Terminate() {};

   void         SetContinuousMode(Bool_t mode) { fContinuous = mode; };
   Bool_t       isContinuousMode() { return fContinuous; };

   virtual void AddTreeBranches() = 0;
protected:
   // Run Status
   Bool_t       isRunning()  { return gROME->GetActiveDAQ()->isRunning(); };
   Bool_t       isStopped()  { return gROME->GetActiveDAQ()->isStopped(); };

   void         SetRunning()  { gROME->GetActiveDAQ()->SetRunning(); };
   void         SetStopped()  { gROME->GetActiveDAQ()->SetStopped(); };

   // Event Status
   Bool_t       isAnalyze()    { return gROME->GetActiveDAQ()->isAnalyze();    };
   Bool_t       isContinue()   { return gROME->GetActiveDAQ()->isContinue();   };
   Bool_t       isBeginOfRun() { return gROME->GetActiveDAQ()->isBeginOfRun(); };
   Bool_t       isEndOfRun()   { return gROME->GetActiveDAQ()->isEndOfRun();   };
   Bool_t       isTerminate()  { return gROME->GetActiveDAQ()->isTerminate();  };

   void         SetAnalyze()    { gROME->GetActiveDAQ()->SetAnalyze();    };
   void         SetContinue()   { gROME->GetActiveDAQ()->SetContinue();   };
   void         SetBeginOfRun() { gROME->GetActiveDAQ()->SetBeginOfRun(); };
   void         SetEndOfRun()   { gROME->GetActiveDAQ()->SetEndOfRun();   };
   void         SetTerminate()  { gROME->GetActiveDAQ()->SetTerminate();  };

   // event methods
   Bool_t       DAQInit();
   Bool_t       DAQBeginOfRun(Long64_t runNumber);
   Bool_t       DAQEvent(Long64_t event);
   Bool_t       UserInput();
   Bool_t       WriteEvent();
   Bool_t       Update();
   Bool_t       DAQEndOfRun();
   Bool_t       DAQTerminate(Bool_t quit);

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
