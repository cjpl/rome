/********************************************************************
  ROMEDAQSystem.h, M. Schneebeli PSI

  $Log$
  Revision 1.2  2005/06/13 15:49:04  schneebeli_m
  changed name of DAQ user functions

  Revision 1.1  2005/05/18 11:41:31  schneebeli_m
  added ROMENoDAQSystem

  Revision 1.3  2005/05/18 11:04:24  schneebeli_m
  daq none

  Revision 1.2  2005/04/01 14:56:23  schneebeli_m
  Histo moved, multiple databases, db-paths moved, InputDataFormat->DAQSystem, GetMidas() to access banks, User DAQ

  Revision 1.1  2005/01/24 15:28:02  schneebeli_m
  Seperated DAQ classes


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
