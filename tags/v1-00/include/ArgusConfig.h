/********************************************************************
  ArgusConfig.h, R. Sawada

  $Log$
  Revision 1.1  2005/01/29 22:45:08  sawada
  Initial revision


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
   virtual bool CheckConfiguration(int runNumber) = 0;
};

#endif   // ArgusConfig_H
