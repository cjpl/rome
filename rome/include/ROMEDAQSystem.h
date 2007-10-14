/********************************************************************
  ROMEDAQSystem.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMEDAQSystem_H
#define ROMEDAQSystem_H
#include <Rtypes.h>
#include <TObject.h>
#include "ROME.h"
#include "ROMEStopwatch.h"

class ROMEDAQSystem : public TObject
{
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

   Int_t         fRunStatus;                       //! Run Status flag
   Int_t         fEventStatus;                     //! Event Status flag
   ROMEStopwatch fWatchEvent;                      //! Records time used by DAQ Events
   ROMEStopwatch fWatchAll;                        //! Records time used by DAQ

public:
   ROMEDAQSystem():fRunStatus(0),fEventStatus(0),fWatchEvent(),fWatchAll() {}
   virtual ~ROMEDAQSystem() {}

   Bool_t           InitDAQ();
   Bool_t           BeginOfRunDAQ();
   Bool_t           EventDAQ(Long64_t event);
   Bool_t           EndOfRunDAQ();
   Bool_t           TerminateDAQ();
   Bool_t           TimeDAQ();

   virtual Int_t    GetTimeStamp() = 0;
   virtual Bool_t   Init() = 0;
   virtual Bool_t   BeginOfRun() = 0;
   virtual Bool_t   Event(Long64_t event) = 0;
   virtual Long64_t Seek(Long64_t /* event */) { AbstractMethod("Seek"); return -1; }
   virtual Bool_t   EndOfRun() = 0;
   virtual Bool_t   Terminate() = 0;
   virtual Bool_t   GetAlwaysFillTrees() const { return kFALSE; }

public:
   // Run Status
   Bool_t           isRunning() const  { return (fRunStatus == kRunning); }
   Bool_t           isStopped() const  { return (fRunStatus == kStopped); }

   void             SetRunning()  { fRunStatus = kRunning; }
   void             SetStopped()  { fRunStatus = kStopped; }

   // Event Status
   Bool_t           isAnalyze() const    { return (fEventStatus == kAnalyze);    }
   Bool_t           isContinue() const   { return (fEventStatus == kContinue);   }
   Bool_t           isBeginOfRun() const { return (fEventStatus == kBeginOfRun); }
   Bool_t           isEndOfRun() const   { return (fEventStatus == kEndOfRun);   }
   Bool_t           isTerminate() const  { return (fEventStatus == kTerminate);  }

   void             SetAnalyze()    { fEventStatus = kAnalyze;    }
   void             SetContinue()   { fEventStatus = kContinue;   }
   void             SetBeginOfRun() { fEventStatus = kBeginOfRun; }
   void             SetEndOfRun()   { fEventStatus = kEndOfRun;   }
   void             SetTerminate()  { fEventStatus = kTerminate;  }

   virtual void     UpdateConfigParameters() {}

   ClassDef(ROMEDAQSystem, 0) // Base DAQ class
};

#endif   // ROMEDAQSystem_H
