/********************************************************************
  ROMEXMLDataBase.h, M. Schneebeli PSI

  $Log$
  Revision 1.2  2004/11/11 12:55:27  schneebeli_m
  Implemented XML database with new path rules

  Revision 1.1  2004/09/27 18:25:25  schneebeli_m
  new database classes

********************************************************************/
#ifndef ROMEXMLDataBase_H
#define ROMEXMLDataBase_H

#include <ROMEStrArray.h>
#include <ROMEStr2DArray.h>
#include <ROMEXML.h>
#include <Riostream.h>

#include <ROMEDataBase.h>
#include <ROMEString.h>

class ROMEXMLDataBase : public ROMEDataBase
{
protected:
   ROMEString    fDirectoryPath;
   ROMEString    fDataBaseName;
   ROMEStrArray  fOrderTableIDs;
   int           fOrderTableIndex;

   enum {
      kWrite  = 0,
      kRead   = 1
   };
public:
   ROMEXMLDataBase();
   ~ROMEXMLDataBase();

   bool   Init(const char* path,const char* connection);
   bool   Read(ROMEStr2DArray *values,const char *path);
   bool   Write(ROMEStr2DArray* values,const char *path);
   char*  GetType() { return "xml"; }
   char*  GetDescription() { return "XML data base reflecting the structure of an SQL data base"; }
};

#endif   // ROMEXMLDataBase_H
