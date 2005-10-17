/********************************************************************
  ROMEDAQSystem.h, M. Schneebeli PSI

  $Id:$

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
   virtual ROMEDAQSystem::~ROMEDAQSystem() {};

   virtual int   GetTimeStamp() = 0;
   virtual const char* GetName() = 0;

   virtual bool Init() = 0;
   virtual bool BeginOfRun() = 0;
   virtual bool Event(int event) = 0;
   virtual bool EndOfRun() = 0;
   virtual bool Terminate() = 0;
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
