/********************************************************************
  ROMENoDataBase.h, M. Schneebeli PSI

  $Log$
  Revision 1.1  2004/09/27 18:25:25  schneebeli_m
  new database classes

********************************************************************/
#ifndef ROMENoDataBase_H
#define ROMENoDataBase_H

#include <TClonesArray.h>

class ROMENoDataBase : public ROMEDataBase {
public:
   ROMENoDataBase() {};
   ~ROMENoDataBase() {};
   bool   Init(const char* path,const char* connection) { return true; };
   void   Read(TObjArray *values,const char *pathI,int runNumber,int numberOfFolders,int numberOfFields=1) { };
   void   Write(const char *path,TObjArray* values) { };
   char*  GetType() { return "none"; }
   char*  GetDescription() { return "No data base connected"; }
};

#endif   // ROMEDataBase_H
