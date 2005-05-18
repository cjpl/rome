/********************************************************************
  ROMEDAQSystem.h, M. Schneebeli PSI

  $Log$
  Revision 1.3  2005/05/18 11:04:24  schneebeli_m
  daq none

  Revision 1.2  2005/04/01 14:56:23  schneebeli_m
  Histo moved, multiple databases, db-paths moved, InputDataFormat->DAQSystem, GetMidas() to access banks, User DAQ

  Revision 1.1  2005/01/24 15:28:02  schneebeli_m
  Seperated DAQ classes


********************************************************************/
#ifndef ROMEDAQSystem_H
#define ROMEDAQSystem_H

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

   int   GetTimeStamp() { return -1; };
   const char* GetName() { return "none"; };

   bool Initialize() { return true; };
   bool Connect() { return true; };
   bool ReadEvent(int event) { return true; };
   bool Disconnect() { return true; };
   bool Termination() { return true; };
public:
   // Run Status
   bool       isRunning()  { return fRunStatus==kRunning; };
   bool       isStopped()  { return fRunStatus==kStopped; };

   void       SetRunning()  { fRunStatus = kRunning; };
   void       SetStopped()  { fRunStatus = kStopped; };

   // Event Status
   bool       isAnalyze()    { return fEventStatus==kAnalyze;    };
   bool       isContinue()   { return fEventStatus==kContinue;   };
   bool       isBeginOfRun() { return fEventStatus==kBeginOfRun; };
   bool       isEndOfRun()   { return fEventStatus==kEndOfRun;   };
   bool       isTerminate()  { return fEventStatus==kTerminate;  };

   void       SetAnalyze()    { fEventStatus = kAnalyze;    };
   void       SetContinue()   { fEventStatus = kContinue;   };
   void       SetBeginOfRun() { fEventStatus = kBeginOfRun; };
   void       SetEndOfRun()   { fEventStatus = kEndOfRun;   };
   void       SetTerminate()  { fEventStatus = kTerminate;  };
};

#endif   // ROMEDAQSystem_H
