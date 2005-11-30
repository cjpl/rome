/********************************************************************
  ROMEDataBase.h, M. Schneebeli PSI

  $Id$

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
   virtual bool   Read(ROMEStr2DArray *values,const char *path,int runNumber = 0,int eventNumber = 0) = 0;
   virtual bool   Write(ROMEStr2DArray* values,const char *path,int runNumber = 0,int eventNumber = 0) = 0;
   virtual char*  GetType() = 0;
   const char*    GetName() { return fName.Data(); };
   virtual char*  GetDescription() = 0;
};

#endif   // ROMEDataBase_H
