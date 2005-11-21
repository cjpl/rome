/********************************************************************
  ROMEOrcaDAQ.h, J. Wouters 

  $Id: ROMEOrcaDAQ.h 480 2005-10-17 12:12:54Z schneebeli_m $

********************************************************************/
#ifndef ROMEOrcaDAQ_H
#define ROMEOrcaDAQ_H

#include <ROMEAnalyzer.h>
#include <ROMEDAQSystem.h>

class ROMEOrcaDAQ : public ROMEDAQSystem {

public:
   ROMEOrcaDAQ::ROMEOrcaDAQ();

   // Additional Getters
   int   GetTimeStamp() { return 0; };  // TODO
   const char* GetName() { return "orca"; };

   bool Init();
   bool BeginOfRun();
   bool Event(int event);
   bool EndOfRun();
   bool Terminate();
};

#endif   // ROMEOrcaDAQ_H
