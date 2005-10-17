/********************************************************************
  ROMEXMLDataBase.h, M. Schneebeli PSI

  $Id:$

********************************************************************/
#ifndef ROMEXMLDataBase_H
#define ROMEXMLDataBase_H

#include <ROMEString.h>
#include <ROMEStrArray.h>
#include <ROMEStr2DArray.h>
#include <ROMEXML.h>
#include <ROMEPath.h>
#include <ROMEDataBase.h>
#include <Riostream.h>

class ROMEXMLDataBase : public ROMEDataBase
{
protected:
   ROMEString    fDirectoryPath;
   ROMEString    fDataBaseName;
   ROMEStrArray  fOrderTableIDs;
   int           fOrderTableIndex;
   ROMEStrArray  fIDX;
   ROMEStr2DArray fPointerArray;
   ROMEXML      *xml;
   ROMEString    fXMLBase;
   ROMEString    fFileName;


   bool   WriteValue(ROMEXML *xml,ROMEPath *path,ROMEString& basePath,ROMEString& value,int arrayIndex);
   int    SearchTable(ROMEPath *path,ROMEStr2DArray *values,const char* dataBasePath,int runNumber,bool write);
public:
   ROMEXMLDataBase();
   ~ROMEXMLDataBase();

   bool   Init(const char* name,const char* path,const char* connection);
   bool   Read(ROMEStr2DArray *values,const char *path,int runNumber);
   bool   Write(ROMEStr2DArray* values,const char *path,int runNumber);
   char*  GetType() { return "xml"; }
   char*  GetDescription() { return "XML data base reflecting the structure of an SQL data base"; }
};

#endif   // ROMEXMLDataBase_H
