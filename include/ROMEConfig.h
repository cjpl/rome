/********************************************************************
  ROMEConfig.h, M. Schneebeli PSI

  $Log$
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

   virtual bool WriteConfigurationFile() = 0;
   virtual bool ReadConfigurationFile(char *file) = 0;
   virtual bool CheckConfiguration(int runNumber) = 0;
};

#endif   // ROMEConfig_H
