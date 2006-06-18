/********************************************************************
  ROMENoDataBase.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMENoDataBase_H
#define ROMENoDataBase_H

#include "ROMEDataBase.h"
class ROMEStr2DArray;

class ROMENoDataBase : public ROMEDataBase {
public:
   ROMENoDataBase() {}
   virtual ~ROMENoDataBase() {}
   Bool_t  Init(const char* name,const char* path,const char* connection) { fName = name; return true; }
   Bool_t  Read(ROMEStr2DArray *values,const char *path,Long64_t runNumber,Long64_t eventNumber) { return true; }
   Bool_t  Write(ROMEStr2DArray* values,const char *path,Long64_t runNumber,Long64_t eventNumber) { return true; }
   char   *GetType() { return "none"; }
   char   *GetDescription() { return "No data base connected"; }

   ClassDef(ROMENoDataBase, 0) // Empty database class
};

#endif   // ROMEDataBase_H
