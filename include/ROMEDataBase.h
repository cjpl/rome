/********************************************************************
  ROMEDataBase.h, M. Schneebeli PSI

  $Log$
  Revision 1.1  2004/09/27 18:25:25  schneebeli_m
  new database classes

********************************************************************/
#ifndef ROMEDataBase_H
#define ROMEDataBase_H

#include <TClonesArray.h>

class ROMEDataBase {
public:
   ROMEDataBase() {};
   ~ROMEDataBase() {};
   virtual bool   Init(const char* path,const char* connection) = 0;
   virtual void   Read(TObjArray *values,const char *pathI,int runNumber,int numberOfFolders,int numberOfFields=1) = 0;
   virtual void   Write(const char *path,TObjArray* values) = 0;
   virtual char*  GetType() = 0;
   virtual char*  GetDescription() = 0;
};

#endif   // ROMEDataBase_H
