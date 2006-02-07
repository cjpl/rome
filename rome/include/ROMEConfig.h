/********************************************************************
  ROMEConfig.h, M. Schneebeli PSI

  $Id$

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

   virtual Bool_t WriteConfigurationFile(const char *file) = 0;
   virtual Bool_t ReadConfigurationFile(const char *file) = 0;
   virtual Bool_t CheckConfiguration(Long64_t runNumber) = 0;
   virtual Bool_t CheckConfiguration(const char *file) = 0;
};

#endif   // ROMEConfig_H
