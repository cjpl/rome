/********************************************************************
  ROMEDataBase.h, M. Schneebeli PSI

  $Log$
  Revision 1.5  2005/04/01 14:56:23  schneebeli_m
  Histo moved, multiple databases, db-paths moved, InputDataFormat->DAQSystem, GetMidas() to access banks, User DAQ

  Revision 1.4  2005/03/13 08:43:14  sawada
  removed or comment out unused variables.
  made virtual destructor of ROMEConfig and ROMEDataBase.

  Revision 1.3  2005/01/27 16:21:06  schneebeli_m
  print method & no gROME in path

  Revision 1.2  2004/11/11 12:55:27  schneebeli_m
  Implemented XML database with new path rules

  Revision 1.1  2004/09/27 18:25:25  schneebeli_m
  new database classes

********************************************************************/
#ifndef ROMEDataBase_H
#define ROMEDataBase_H

#include <ROMEStr2DArray.h>
#include <ROMEString.h>

class ROMEDataBase {
protected:
   ROMEString fName;
public:
   ROMEDataBase() {};
   virtual ~ROMEDataBase() {};
   virtual bool   Init(const char* name,const char* path,const char* connection) = 0;
   virtual bool   Read(ROMEStr2DArray *values,const char *path,int runNumber) = 0;
   virtual bool   Write(ROMEStr2DArray* values,const char *path,int runNumber) = 0;
   virtual char*  GetType() = 0;
   const char*    GetName() { return fName.Data(); };
   virtual char*  GetDescription() = 0;
};

#endif   // ROMEDataBase_H
