/********************************************************************
  ROMEConfig.h, M. Schneebeli PSI

  $Log$
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

   virtual bool WriteConfigurationFile(char *file) = 0;
   virtual bool ReadConfigurationFile(char *file) = 0;
   virtual bool CheckConfiguration(int runNumber) = 0;
};

#endif   // ROMEConfig_H
