/********************************************************************
  ROMESQLDataBase.h, M. Schneebeli PSI

  $Log$
  Revision 1.1  2004/09/27 18:25:25  schneebeli_m
  new database classes

********************************************************************/
#ifndef ROMESQLDataBase_H
#define ROMESQLDataBase_H

#include <TClonesArray.h>
#include <ROMESQL.h>
#include <Riostream.h>

#include <ROMEDataBase.h>
#include <ROMEString.h>

class ROMESQLDataBase : public ROMEDataBase
{
protected:
   ROMEString openFile;
   ROMEString filePath;
   ROMEString RunTableName;
   ROMESQL *SQLRunTable;
   ROMESQL *SQLDB;
public:
   ROMESQLDataBase();
   ~ROMESQLDataBase();

   bool   Init(const char* path,const char* connection);
   void   Read(TObjArray *values,const char *pathI,int runNumber,int numberOfFolders,int numberOfFields=1);
   void   Write(const char *path,TObjArray* values);
   char*  GetType() { return "sql"; }
   char*  GetDescription() { return "SQL data base using the file system to store tables"; }
};

#endif   // ROMESQLDataBase_H
