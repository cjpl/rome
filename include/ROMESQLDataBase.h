/********************************************************************
  ROMESQLDataBase.h, M. Schneebeli PSI

  $Log$
  Revision 1.7  2004/11/17 10:36:27  sawada
  argument for Init

  Revision 1.6  2004/11/16 21:59:45  sawada
  read/write field array at once.

  Revision 1.5  2004/11/16 12:11:06  sawada
  SQL Init,Read

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
#include <ROMEPath.h>

class ROMESQLDataBase : public ROMEDataBase
{
protected:
  ROMEString fFieldList;
  ROMEString fFromPhrase;
  ROMEString fWherePhrase;
  ROMEString fOrderPhrase;
  ROMEString fLimitPhrase;
  ROMESQL    *fSQL;
public:
   ROMESQLDataBase();
   ~ROMESQLDataBase();

   bool   Init(const char* dataBase,const char* connection);
   bool   Read(ROMEStr2DArray *values,const char *dataBasePath);
   bool   Write(ROMEStr2DArray* values,const char *dataBasePath);
   char*  GetType() { return "sql"; }
   char*  GetDescription() { return "SQL data base using the file system to store tables"; }
   bool   MakePhrase(ROMEPath *dataBasePath);
   void   ResetPhrase();
   void   Print();
};

#endif   // ROMESQLDataBase_H
