/********************************************************************
  ROMEDAQSystem.h, M. Schneebeli PSI

  $Id:$

********************************************************************/
#ifndef ROMENoDAQSystem_H
#define ROMENoDAQSystem_H

#include <ROMEDAQSystem.h>

class ROMENoDAQSystem : public ROMEDAQSystem{
public:
   ROMENoDAQSystem::ROMENoDAQSystem() {};

   int   GetTimeStamp() { return -1; };
   const char* GetName() { return "none"; };

   bool Init() { return true; };
   bool BeginOfRun() { return true; };
   bool Event(int event) { return true; };
   bool EndOfRun() { return true; };
   bool Terminate() { return true; };
};

#endif   // ROMENoDAQSystem_H
