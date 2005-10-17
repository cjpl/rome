/********************************************************************
  ROMEConfig.h, M. Schneebeli PSI

  $Id:$

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
