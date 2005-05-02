/********************************************************************
  ROMEEventLoop.h, M. Schneebeli PSI

  $Log$
  Revision 1.16  2005/05/02 16:09:22  schneebeli_m
  -o commandline parameter

  Revision 1.15  2005/04/08 17:08:07  schneebeli_m
  TNetFolderServer changes

  Revision 1.14  2005/04/01 14:56:23  schneebeli_m
  Histo moved, multiple databases, db-paths moved, InputDataFormat->DAQSystem, GetMidas() to access banks, User DAQ

  Revision 1.13  2005/02/06 01:26:55  sawada
  moved byte swapping functions to ROMEMidas

  Revision 1.12  2005/01/24 15:25:06  schneebeli_m
  Seperated DAQ classes

  Revision 1.11  2004/12/03 14:42:08  schneebeli_m
  some minor changes

  Revision 1.10  2004/12/02 17:46:43  sawada
  Macintosh port

  Revision 1.9  2004/10/15 12:30:49  schneebeli_m
  online eventloop logic

  Revision 1.8  2004/10/01 14:33:29  schneebeli_m
  Fixed some tree file problems

  Revision 1.7  2004/10/01 13:11:33  schneebeli_m
  Tree write error removed, Database Number Problem solved, Trees in Folder for TSocket

  Revision 1.6  2004/09/25 01:34:48  schneebeli_m
  implemented FW dependent EventLoop and DataBase classes

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
   TFile**       fTreeFiles;                       //! File Handles for Tree Objects
   ROMETreeInfo* fTreeInfo;                        //! Tree Info Object
   int           fSequentialNumber;                //! Sequential Number

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
   bool Initialize();
   bool Connect(Int_t runNumber);
   bool ReadEvent(Int_t event);
   bool UserInput();
   bool WriteEvent();
   bool Update();
   bool Disconnect();
   bool Termination();

   // virtual methods
   virtual void InitSingleFolders() = 0;
   virtual void InitArrayFolders() = 0;
   virtual void CleanUpFolders() = 0;
   virtual void ResetFolders() = 0;

   virtual void InitTaskSwitches() = 0;
   virtual void UpdateTaskSwitches() = 0;

   virtual void InitTrees() = 0;
   virtual void FillTrees() = 0;
};

#endif   // ROMEEventLoop_H
