/********************************************************************
  ROMEConfig.h, M. Schneebeli PSI

  $Log$
  Revision 1.4  2005/04/01 14:56:23  schneebeli_m
  Histo moved, multiple databases, db-paths moved, InputDataFormat->DAQSystem, GetMidas() to access banks, User DAQ

  Revision 1.3  2005/03/13 08:43:14  sawada
  removed or comment out unused variables.
  made virtual destructor of ROMEConfig and ROMEDataBase.

  Revision 1.2  2004/10/15 11:51:28  schneebeli_m
  bugs removed

  Revision 1.1  2004/10/14 09:54:57  schneebeli_m
  ROMEConfig class implemented


********************************************************************/
#ifndef ROMEConfig_H
#define ROMEConfig_H

#include <Riostream.h>

class ROMEConfig
{
protected:
public:
   ROMEConfig() {};
   virtual ~ROMEConfig() {};

   virtual bool WriteConfigurationFile(const char *file) = 0;
   virtual bool ReadConfigurationFile(const char *file) = 0;
   virtual bool CheckConfiguration(int runNumber) = 0;
};

#endif   // ROMEConfig_H
