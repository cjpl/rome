/********************************************************************
  ROMENoDataBase.h, M. Schneebeli PSI

  $Log$
  Revision 1.3  2005/01/27 16:21:06  schneebeli_m
  print method & no gROME in path

  Revision 1.2  2004/11/11 12:55:27  schneebeli_m
  Implemented XML database with new path rules

  Revision 1.1  2004/09/27 18:25:25  schneebeli_m
  new database classes

********************************************************************/
#ifndef ROMENoDataBase_H
#define ROMENoDataBase_H

#include <ROMEStr2DArray.h>

class ROMENoDataBase : public ROMEDataBase {
public:
   ROMENoDataBase() {};
   ~ROMENoDataBase() {};
   bool   Init(const char* path,const char* connection) { return true; };
   bool   Read(ROMEStr2DArray *values,const char *path,int runNumber) { return true; };
   bool   Write(ROMEStr2DArray* values,const char *path,int runNumber) { return true; };
   char*  GetType() { return "none"; }
   char*  GetDescription() { return "No data base connected"; }
};

#endif   // ROMEDataBase_H
