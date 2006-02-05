/********************************************************************
  ROMEDAQSystem.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMEDAQSystem_H
#define ROMEDAQSystem_H
#include <Rtypes.h>

class ROMEDAQSystem {
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

   Int_t      fRunStatus;                       //! Run Status flag
   Int_t      fEventStatus;                     //! Event Status flag
public:
   ROMEDAQSystem::ROMEDAQSystem() {};
   virtual ROMEDAQSystem::~ROMEDAQSystem() {};

   virtual Int_t       GetTimeStamp() = 0;
   virtual const char *GetName() = 0;

   virtual Bool_t      Init() = 0;
   virtual Bool_t      BeginOfRun() = 0;
   virtual Bool_t      Event(Long64_t event) = 0;
   virtual Bool_t      EndOfRun() = 0;
   virtual Bool_t      Terminate() = 0;
public:
   // Run Status
   Bool_t              isRunning()  { return fRunStatus==kRunning; };
   Bool_t              isStopped()  { return fRunStatus==kStopped; };

   void                SetRunning()  { fRunStatus = kRunning; };
   void                SetStopped()  { fRunStatus = kStopped; };

   // Event Status
   Bool_t              isAnalyze()    { return fEventStatus==kAnalyze;    };
   Bool_t              isContinue()   { return fEventStatus==kContinue;   };
   Bool_t              isBeginOfRun() { return fEventStatus==kBeginOfRun; };
   Bool_t              isEndOfRun()   { return fEventStatus==kEndOfRun;   };
   Bool_t              isTerminate()  { return fEventStatus==kTerminate;  };

   void                SetAnalyze()    { fEventStatus = kAnalyze;    };
   void                SetContinue()   { fEventStatus = kContinue;   };
   void                SetBeginOfRun() { fEventStatus = kBeginOfRun; };
   void                SetEndOfRun()   { fEventStatus = kEndOfRun;   };
   void                SetTerminate()  { fEventStatus = kTerminate;  };
};

#endif   // ROMEDAQSystem_H
