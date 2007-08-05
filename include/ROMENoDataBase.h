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
   Bool_t      Init(const char* name, const char*, const char*) { fName = name; return true; }
   Bool_t      Read(ROMEStr2DArray*, const char*, Long64_t, Long64_t) { return true; }
   Bool_t      Write(ROMEStr2DArray*, const char*, Long64_t, Long64_t) { return true; }
   const char *GetType() const { return "none"; }
   const char *GetDescription() const { return "No data base connected"; }

   ClassDef(ROMENoDataBase, 0) // Empty database class
};

#endif   // ROMEDataBase_H
