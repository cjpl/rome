/********************************************************************
  ROMEDataBaseDAQ.h, M. Schneebeli PSI

  $Id: ROMEDataBaseDAQ.h 480 2005-10-17 12:12:54Z schneebeli_m $

********************************************************************/
#ifndef ROMEDataBaseDAQ_H
#define ROMEDataBaseDAQ_H

#include <ROMEAnalyzer.h>
#include <ROMEDAQSystem.h>

class ROMEDataBaseDAQ : public ROMEDAQSystem {
protected:

   int           fTimeStamp;                       //! Current time stamp

public:
   ROMEDataBaseDAQ();
   virtual ~ROMEDataBaseDAQ() {};

   int   GetTimeStamp() { return fTimeStamp; };
   const char* GetName() { return "database"; };

   bool Init();
   bool BeginOfRun();
   bool Event(int event);
   bool EndOfRun();
   bool Terminate();
};

#endif   // ROMEDataBaseDAQ_H
