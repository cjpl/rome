/********************************************************************
  ROMEDataBase.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMEDataBase_H
#define ROMEDataBase_H

#include <ROMEStr2DArray.h>
#include <ROMEString.h>

class ROMEDataBase : public TObject
{
protected:
   ROMEString fName;
public:
   ROMEDataBase() {};
   virtual ~ROMEDataBase() {};
   virtual Bool_t  Init(const char* name,const char* path,const char* connection) = 0;
   virtual Bool_t  Read(ROMEStr2DArray *values,const char *path,Long64_t runNumber = 0,Long64_t eventNumber = 0) = 0;
   virtual Bool_t  Write(ROMEStr2DArray* values,const char *path,Long64_t runNumber = 0,Long64_t eventNumber = 0) = 0;
   virtual char   *GetType() = 0;
   const char     *GetName() { return fName.Data(); };
   virtual char   *GetDescription() = 0;

   ClassDef(ROMEDataBase, 0) // Base class for database access
};

#endif   // ROMEDataBase_H
