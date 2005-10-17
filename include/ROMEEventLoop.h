/********************************************************************
  ROMEEventLoop.h, M. Schneebeli PSI

  $Id:$

********************************************************************/
#ifndef ROMEEventLoop_H
#define ROMEEventLoop_H

#include <ROMETask.h>
#include <ROMEAnalyzer.h>

class ROMEEventLoop : public ROMETask {
protected:
   // Stop at
   Int_t         fStopAtRun;                       //! Stop execution at this run
   Int_t         fStopAtEvent;                     //! Stop execution at this event

   // Statistics update
   int           fStatisticsTimeOfLastEvent;       //! Time of last Event
   double        fStatisticsLastEvent;             //! Last Event

   // Progress Display
   int           fProgressDelta;                   //! Maximal time difference
   time_t        fProgressTimeOfLastEvent;         //! Last time for display
   int           fProgressLastEvent;               //! Last Event
   bool          fProgressWrite;                   //! Write flag

   // user input
   time_t        fUserInputLastTime;               //! Last time for user input
   bool          fContinuous;                      //! Continuous Mode

   // Output Tree Files
   ROMETreeInfo* fTreeInfo;                        //! Tree Info Object
   int           fSequentialNumber;                //! Sequential Number
   int           fTreeUpdateIndex;                 //! Index of the tree (for trees in the same file)

   // Histo File
   TFile*        fHistoFile;                       //! Histo file


public:
   // Static Task Switches Changes Flag
   static bool fTaskSwitchesChanged;               //! Flag Task Switches Changes

public:
   ROMEEventLoop() { ; }
   ROMEEventLoop(const char *name,const char *title);
   void ExecuteTask(Option_t *option="");
   void Exec(Option_t *option="") {};
   void BookHisto() {};
   void ResetHisto() {};
   void Init() {};
   void BeginOfRun() {};
   void Event() {};
   void EndOfRun() {};
   void Terminate() {};

   void SetContinuousMode(bool mode) { fContinuous = mode; };
protected:
   // Run Status
   Bool_t     isRunning()  { return gROME->GetActiveDAQ()->isRunning(); };
   Bool_t     isStopped()  { return gROME->GetActiveDAQ()->isStopped(); };

   void       SetRunning()  { gROME->GetActiveDAQ()->SetRunning(); };
   void       SetStopped()  { gROME->GetActiveDAQ()->SetStopped(); };

   // Event Status
   Bool_t     isAnalyze()    { return gROME->GetActiveDAQ()->isAnalyze();    };
   Bool_t     isContinue()   { return gROME->GetActiveDAQ()->isContinue();   };
   Bool_t     isBeginOfRun() { return gROME->GetActiveDAQ()->isBeginOfRun(); };
   Bool_t     isEndOfRun()   { return gROME->GetActiveDAQ()->isEndOfRun();   };
   Bool_t     isTerminate()  { return gROME->GetActiveDAQ()->isTerminate();  };

   void       SetAnalyze()    { gROME->GetActiveDAQ()->SetAnalyze();    };
   void       SetContinue()   { gROME->GetActiveDAQ()->SetContinue();   };
   void       SetBeginOfRun() { gROME->GetActiveDAQ()->SetBeginOfRun(); };
   void       SetEndOfRun()   { gROME->GetActiveDAQ()->SetEndOfRun();   };
   void       SetTerminate()  { gROME->GetActiveDAQ()->SetTerminate();  };

   // event methods
   bool DAQInit();
   bool DAQBeginOfRun(Int_t runNumber);
   bool DAQEvent(Int_t event);
   bool UserInput();
   bool WriteEvent();
   bool Update();
   bool DAQEndOfRun();
   bool DAQTerminate();

   // virtual methods
   virtual void InitSingleFolders() = 0;
   virtual void InitArrayFolders() = 0;
   virtual void CleanUpFolders() = 0;
   virtual void ResetFolders() = 0;

   virtual void InitTaskSwitches() = 0;
   virtual void UpdateTaskSwitches() = 0;

   virtual void InitTrees() = 0;
   virtual void FillTrees() = 0;
   virtual void GetTreeFileName(ROMEString& buffer,int treeIndex,const char* runNumber="") = 0;
};

#endif   // ROMEEventLoop_H
