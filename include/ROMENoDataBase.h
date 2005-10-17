/********************************************************************
  ROMENoDataBase.h, M. Schneebeli PSI

  $Id:$

********************************************************************/
#ifndef ROMENoDataBase_H
#define ROMENoDataBase_H

#include <ROMEStr2DArray.h>

class ROMENoDataBase : public ROMEDataBase {
public:
   ROMENoDataBase() {};
   ~ROMENoDataBase() {};
   bool   Init(const char* name,const char* path,const char* connection) { fName = name; return true; };
   bool   Read(ROMEStr2DArray *values,const char *path,int runNumber) { return true; };
   bool   Write(ROMEStr2DArray* values,const char *path,int runNumber) { return true; };
   char*  GetType() { return "none"; }
   char*  GetDescription() { return "No data base connected"; }
};

#endif   // ROMEDataBase_H
