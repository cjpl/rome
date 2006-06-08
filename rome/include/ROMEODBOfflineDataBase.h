/********************************************************************
  ROMEODBOfflineDataBase.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMEODBOfflineDataBase_H
#define ROMEODBOfflineDataBase_H

#include "ROMEStr2DArray.h"
#include "ROMEXML.h"
#include "ROMEDataBase.h"

class ROMEODBOfflineDataBase : public ROMEDataBase
{
protected:
   ROMEXML      *xml;

public:
   ROMEODBOfflineDataBase();
   ~ROMEODBOfflineDataBase();

   Bool_t  Init(const char* name,const char* path,const char* connection);
   Bool_t  Read(ROMEStr2DArray *values,const char *path,Long64_t runNumber,Long64_t eventNumber);
   Bool_t  Write(ROMEStr2DArray* values,const char *path,Long64_t runNumber,Long64_t eventNumber);
   char   *GetType() { return "odb"; };
   char   *GetDescription() { return "Midas ODB database for offline use"; };
   Bool_t  SetBuffer(char *buffer);

   ClassDef(ROMEODBOfflineDataBase, 0) // Interface to ODB in offline
};

#endif   // ROMEXMLDataBase_H
