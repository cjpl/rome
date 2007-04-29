/********************************************************************
  ROMEXMLDataBase.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMEXMLDataBase_H
#define ROMEXMLDataBase_H

#include "ROMEString.h"
#include "ROMEStrArray.h"
#include "ROMEStr2DArray.h"
#include "ROMEDataBase.h"

class ROMEXML;
class ROMEPath;

class ROMEXMLDataBase : public ROMEDataBase
{
protected:
   ROMEString     fDirectoryPath;
   ROMEString     fDataBaseName;
   ROMEStrArray   fOrderTableIDs;
   Int_t          fOrderTableIndex;
   ROMEStrArray   fIDX;
   ROMEStr2DArray fPointerArray;
   ROMEXML       *xml;
   ROMEString     fXMLBase;
   ROMEString     fFileName;


   Bool_t WriteValue(ROMEXML *xml,ROMEPath *path,ROMEString& basePath,ROMEString& value,Int_t arrayIndex);
   Int_t  SearchTable(ROMEPath *path,ROMEStr2DArray *values,const char* dataBasePath,Long64_t runNumber,Long64_t eventNumber,Bool_t write);

private:
   ROMEXMLDataBase(const ROMEXMLDataBase &db);
   ROMEXMLDataBase &operator=(const ROMEXMLDataBase &rhs);

public:
   ROMEXMLDataBase();
   virtual ~ROMEXMLDataBase();

   Bool_t      Init(const char* name,const char* path,const char* connection);
   Bool_t      Read(ROMEStr2DArray *values,const char *path,Long64_t runNumber,Long64_t eventNumber);
   Bool_t      Write(ROMEStr2DArray* values,const char *path,Long64_t runNumber,Long64_t eventNumber);
   const char *GetType() { return "xml"; }
   const char *GetDescription() { return "XML data base reflecting the structure of an SQL data base"; }

   ClassDef(ROMEXMLDataBase, 0) // XML database access
};

#endif   // ROMEXMLDataBase_H
