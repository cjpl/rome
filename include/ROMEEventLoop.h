/********************************************************************
  ROMEEventLoop.h, M. Schneebeli PSI

  $Log$
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
   // Run Status
   enum {
      kRunning   = 0,
      kStopped   = 1
   };
   // Event Status
   enum {
      kAnalyze     = 0,
      kContinue    = 1,
      kBeginOfRun  = 2,
      kEndOfRun    = 3,
      kTerminate   = 4
   };

   // Midas
   Int_t         fMidasFileHandle;                 //! Handle to Midas Inputfile
   int           fMidasBuffer;                     //! Midas Online Buffer

   // Status
   Int_t         fRunStatus;                       //! Run Status flag
   Int_t         fEventStatus;                     //! Event Status flag

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

   // Tree Info
   int           fSequentialNumber;                //! Sequential Number
   ROMETreeInfo* fTreeInfo;                        //! Tree Info Object
   int*          fTreePosition;                    //! Array with tree read positions
   int*          fTreeNextSeqNumber;               //! Array with the trees next sequential number

   // Input Root Files
   TFile**       fRootFiles;                       //! Root files

   // Histo File
   TFile*        fHistoFile;                       //! Histo file



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
protected:
   // Run Status
   Bool_t     isRunning()  { return fRunStatus==kRunning; };
   Bool_t     isStopped()  { return fRunStatus==kStopped; };

   void       SetRunning()  { fRunStatus = kRunning; };
   void       SetStopped()  { fRunStatus = kStopped; };

   // Event Status
   Bool_t     isAnalyze()    { return fEventStatus==kAnalyze;    };
   Bool_t     isContinue()   { return fEventStatus==kContinue;   };
   Bool_t     isBeginOfRun() { return fEventStatus==kBeginOfRun; };
   Bool_t     isEndOfRun()   { return fEventStatus==kEndOfRun;   };
   Bool_t     isTerminate()  { return fEventStatus==kTerminate;  };

   void       SetAnalyze()    { fEventStatus = kAnalyze;    };
   void       SetContinue()   { fEventStatus = kContinue;   };
   void       SetBeginOfRun() { fEventStatus = kBeginOfRun; };
   void       SetEndOfRun()   { fEventStatus = kEndOfRun;   };
   void       SetTerminate()  { fEventStatus = kTerminate;  };

   // event methods
   bool Initialize();
   bool Connect(Int_t runNumber);
   bool ReadEvent(Int_t event);
   void CheckLineToProcess();
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

   virtual bool InitODB() = 0;

   virtual void InitTrees() = 0;
   virtual void ConnectTrees() = 0;
   virtual void FillTrees() = 0;
};

#endif   // ROMEEventLoop_H
