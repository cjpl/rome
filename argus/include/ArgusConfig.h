/********************************************************************
  ArgusConfig.h, R. Sawada

  $Id:$

********************************************************************/
#ifndef ArgusConfig_H
#define ArgusConfig_H
#include <Rtypes.h>

class ArgusConfig
{
protected:
public:
   ArgusConfig() {};
   virtual ~ArgusConfig() {};
   
   virtual Bool_t WriteConfigurationFile(const Char_t *file) = 0;
   virtual Bool_t ReadConfigurationFile(const Char_t *file) = 0;
};

#endif   // ArgusConfig_H
