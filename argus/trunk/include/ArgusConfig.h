/********************************************************************
  ArgusConfig.h, R. Sawada

  $Log$
  Revision 1.4  2005/05/05 20:08:05  sawada
  code clean up.

  Revision 1.3  2005/03/13 08:40:43  sawada
  modified handling of recursiveDepth.
  removed unused variables.

  Revision 1.2  2005/02/24 15:43:14  sawada
  removed CheckConfiguration

  Revision 1.1.1.1  2005/01/29 22:45:08  sawada
  Advanced Root based GUi monitoring System


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
