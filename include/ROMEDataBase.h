/********************************************************************
  ROMEDataBase.h, M. Schneebeli PSI

  $Log$
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
public:
   ROMEDataBase() {};
   ~ROMEDataBase() {};
   virtual bool   Init(const char* path,const char* connection) = 0;
   virtual bool   Read(ROMEStr2DArray *values,const char *path,int runNumber) = 0;
   virtual bool   Write(ROMEStr2DArray* values,const char *path,int runNumber) = 0;
   virtual char*  GetType() = 0;
   virtual char*  GetDescription() = 0;
};

#endif   // ROMEDataBase_H
