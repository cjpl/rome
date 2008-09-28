/********************************************************************
  ROMEODBOfflineDataBase.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMEODBOfflineDataBase_H
#define ROMEODBOfflineDataBase_H

#include "ROMEDataBase.h"
class ROMEStr2DArray;
class ROMEXML;

class ROMEODBOfflineDataBase : public ROMEDataBase
{
protected:
   ROMEXML      *fXML;

private:
   ROMEODBOfflineDataBase(const ROMEODBOfflineDataBase &db); // not implemented
   ROMEODBOfflineDataBase &operator=(const ROMEODBOfflineDataBase &rhs); // not implemented

public:
   ROMEODBOfflineDataBase();
   virtual ~ROMEODBOfflineDataBase();

   Bool_t      Init(const char* name, const char* path, const char* connection);
   Bool_t      Read(ROMEStr2DArray *values, const char *path, Long64_t runNumber, Long64_t eventNumber);
   Bool_t      Write(ROMEStr2DArray* values, const char *path, Long64_t runNumber, Long64_t eventNumber);
   const char* GetType() const { return "odb"; }
   const char* GetDescription() const { return "Midas ODB database for offline use"; }
   Bool_t      SetBuffer(const char *buffer) const;
   Bool_t      DumpToXML(const char *filename) const;

   ClassDef(ROMEODBOfflineDataBase, 0) // Interface to ODB in offline
};

#endif   // ROMEODBOfflineDataBase_H
