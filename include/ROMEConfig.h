/********************************************************************
  ROMEConfig.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMEConfig_H
#define ROMEConfig_H

#include <Riostream.h>

class ROMEConfig : public TObject
{
protected:
public:
   ROMEConfig() {};
   virtual ~ROMEConfig() {};

   virtual Bool_t WriteConfigurationFile(const char *file) = 0;
   virtual Bool_t ReadConfigurationFile(const char *file) = 0;
   virtual Bool_t CheckConfiguration(Long64_t runNumber) = 0;
   virtual Bool_t CheckConfiguration(const char *file) = 0;

   ClassDef(ROMEConfig, 0) // Base configuration class
};

#endif   // ROMEConfig_H
