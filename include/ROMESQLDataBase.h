/********************************************************************
  ROMESQLDataBase.h, M. Schneebeli PSI

  $Log$
  Revision 1.4  2004/11/11 12:55:27  schneebeli_m
  Implemented XML database with new path rules

  Revision 1.3  2004/10/05 07:52:44  schneebeli_m
  dyn. Folders, TRef Objects, XML format changed, ROMEStatic removed

  Revision 1.2  2004/09/30 13:08:21  schneebeli_m
  ...

  Revision 1.1  2004/09/27 18:25:25  schneebeli_m
  new database classes

********************************************************************/
#ifndef ROMESQLDataBase_H
#define ROMESQLDataBase_H

#include <ROMEStr2DArray.h>
#include <ROMESQL.h>
#include <Riostream.h>

#include <ROMEDataBase.h>
#include <ROMEString.h>

class ROMESQLDataBase : public ROMEDataBase
{
protected:
public:
   ROMESQLDataBase();
   ~ROMESQLDataBase();

   bool   Init(const char* path,const char* connection);
   bool   Read(ROMEStr2DArray *values,const char *path);
   bool   Write(ROMEStr2DArray* values,const char *path);
   char*  GetType() { return "sql"; }
   char*  GetDescription() { return "SQL data base using the file system to store tables"; }
};

#endif   // ROMESQLDataBase_H
