/********************************************************************
  ROMEDAQSystem.h, M. Schneebeli PSI

  $Log$
  Revision 1.1  2005/01/24 15:28:02  schneebeli_m
  Seperated DAQ classes


********************************************************************/
#ifndef ROMEDAQSystem_H
#define ROMEDAQSystem_H

#include <ROMEAnalyzer.h>

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

   int        fRunStatus;                       //! Run Status flag
   int        fEventStatus;                     //! Event Status flag
public:
   ROMEDAQSystem::ROMEDAQSystem() {};

   virtual int   GetTimeStamp() = 0;

   virtual bool Initialize() = 0;
   virtual bool Connect() = 0;
   virtual bool ReadEvent(int event) = 0;
   virtual bool Disconnect() = 0;
   virtual bool Termination() = 0;
public:
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
};

#endif   // ROMEDAQSystem_H
