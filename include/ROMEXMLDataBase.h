/********************************************************************
  ROMEXMLDataBase.h, M. Schneebeli PSI

  $Log$
  Revision 1.1  2004/09/27 18:25:25  schneebeli_m
  new database classes

********************************************************************/
#ifndef ROMEXMLDataBase_H
#define ROMEXMLDataBase_H

#include <TClonesArray.h>
#include <ROMEXML.h>
#include <Riostream.h>

#include <ROMEDataBase.h>
#include <ROMEString.h>

class ROMEXMLDataBase : public ROMEDataBase
{
protected:
   ROMEString openFile;
   ROMEString filePath;
   ROMEString RunTableName;
   ROMEXML *xmlRunTable;
   ROMEXML *xmlDB;
public:
   ROMEXMLDataBase();
   ~ROMEXMLDataBase();

   bool   Init(const char* path,const char* connection);
   void   Read(TObjArray *values,const char *pathI,int runNumber,int numberOfFolders,int numberOfFields=1);
   void   Write(const char *path,TObjArray* values);
   char*  GetType() { return "xml"; }
   char*  GetDescription() { return "XML data base using the file system to store tables"; }
};

#endif   // ROMEXMLDataBase_H
