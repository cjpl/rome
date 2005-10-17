/********************************************************************
  ROMEConfig.h, M. Schneebeli PSI

  $Log$
  Revision 1.6  2005/08/23 15:49:23  sawada
  bug fix of ROMEAnalyzer::SetCurrentInputFileName and ROMEConfig::CheckConfiguration.

  Revision 1.5  2005/08/12 15:37:02  schneebeli_m
  added input file based IO

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
   virtual bool CheckConfiguration(const char *file) = 0;
//   bool CheckConfiguration(ROMEString& file) { return CheckConfiguration(file.Data()); }
};

#endif   // ROMEConfig_H
