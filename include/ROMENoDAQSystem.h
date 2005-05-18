/********************************************************************
  ROMEDAQSystem.h, M. Schneebeli PSI

  $Log$
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

   bool Initialize() { return true; };
   bool Connect() { return true; };
   bool ReadEvent(int event) { return true; };
   bool Disconnect() { return true; };
   bool Termination() { return true; };
};

#endif   // ROMENoDAQSystem_H
