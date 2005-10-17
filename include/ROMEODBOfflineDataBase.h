/********************************************************************
  ROMEODBOfflineDataBase.h, M. Schneebeli PSI

  $Id:$

********************************************************************/
#ifndef ROMEODBOfflineDataBase_H
#define ROMEODBOfflineDataBase_H

#include <ROMEString.h>
#include <ROMEStrArray.h>
#include <ROMEStr2DArray.h>
#include <ROMEXML.h>
#include <ROMEPath.h>
#include <ROMEDataBase.h>
#include <Riostream.h>

class ROMEODBOfflineDataBase : public ROMEDataBase
{
protected:
   ROMEXML      *xml;

public:
   ROMEODBOfflineDataBase();
   ~ROMEODBOfflineDataBase();

   bool   Init(const char* name,const char* path,const char* connection);
   bool   Read(ROMEStr2DArray *values,const char *path,int runNumber);
   bool   Write(ROMEStr2DArray* values,const char *path,int runNumber);
   char*  GetType() { return "odb"; };
   char*  GetDescription() { return "Midas ODB database for offline use"; };
   bool   SetBuffer(char *buffer);
};

#endif   // ROMEXMLDataBase_H
