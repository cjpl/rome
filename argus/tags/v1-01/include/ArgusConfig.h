/********************************************************************
  ArgusConfig.h, R. Sawada

  $Log$
  Revision 1.2  2005/02/24 15:43:14  sawada
  removed CheckConfiguration

  Revision 1.1.1.1  2005/01/29 22:45:08  sawada
  Advanced Root based GUi monitoring System


********************************************************************/
#ifndef ArgusConfig_H
#define ArgusConfig_H

class ArgusConfig
{
protected:
public:
   ArgusConfig() {};
   
   virtual bool WriteConfigurationFile(char *file) = 0;
   virtual bool ReadConfigurationFile(char *file) = 0;
};

#endif   // ArgusConfig_H
