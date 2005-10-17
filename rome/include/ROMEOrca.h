/********************************************************************
  ROMEOrca.h, J. Wouters 

  $Log$
  Revision 1.1  2005/07/01 08:21:58  schneebeli_m
  added to cvs



********************************************************************/
#ifndef ROMEOrca_H
#define ROMEOrca_H

#include <ROMEAnalyzer.h>
#include <ROMEDAQSystem.h>

class ROMEOrca : public ROMEDAQSystem {

public:
   ROMEOrca::ROMEOrca();

   // Additional Getters
   int   GetTimeStamp() { return 0; };  // TODO
   const char* GetName() { return "orca"; };

   bool Init();
   bool BeginOfRun();
   bool Event(int event);
   bool EndOfRun();
   bool Terminate();
};

#endif   // ROMEOrca_H
