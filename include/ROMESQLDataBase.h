/********************************************************************
  ROMESQLDataBase.h, M. Schneebeli PSI

  $Log$
  Revision 1.3  2004/10/05 07:52:44  schneebeli_m
  dyn. Folders, TRef Objects, XML format changed, ROMEStatic removed

  Revision 1.2  2004/09/30 13:08:21  schneebeli_m
  ...

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
   ROMESQL *fSQL;
public:
   ROMESQLDataBase();
   ~ROMESQLDataBase();

   bool   Init(const char* path,const char* connection);
   void   Read(TObjArray *values,const char *pathI,int runNumber,int numberOfFolders,int numberOfFields=1);
   void   Write(const char *path,TObjArray* values);
   char*  GetType() { return "sql"; }
   char*  GetDescription() { return "SQL data base using the file system to store tables"; }
protected:
   int    DecodePath(char *path,char *start_id_extension,char **table_name,char **id_extension,int  &transition_depth,char *group_name,char *field_name);
   char*  strtok_x(char *str,char* sub,char** tail);
};

#endif   // ROMESQLDataBase_H
