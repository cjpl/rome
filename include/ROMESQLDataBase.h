/********************************************************************
  ROMESQLDataBase.h, M. Schneebeli PSI

  $Log$
  Revision 1.10  2004/11/21 00:10:41  sawada
  error handling

  Revision 1.9  2004/11/19 16:26:24  sawada
  speed up with reading order array at once.

  Revision 1.8  2004/11/18 15:23:23  sawada

  Modify handling the order of array.
  Enable inverse order.
  Enable to send sql query from user tasks.

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
  ROMESQL    *fSQL;
public:
   ROMESQLDataBase();
   ~ROMESQLDataBase();

   bool   Init(const char* dataBase,const char* connection);
   bool   Read(ROMEStr2DArray *values,const char *dataBasePath);
   bool   Write(ROMEStr2DArray* values,const char *dataBasePath);
   char*  GetType() { return "sql"; }
   char*  GetDescription() { return "SQL data base using the file system to store tables"; }
   void   Print();
/*
   bool Connect(const char *server,const char *user,const char *passwd,const char *database,const char *port){
      return fSQL->Connect(server,user,passwd,database,port);
   }
   bool   DisConnect(){fSQL->DisConnect();}
   bool   CreateDataBase(char* database){return fSQL->CreateDataBase(database);}
   bool   DeleteDataBase(char* database){return fSQL->DeleteDataBase(database);}
   bool   CreateTable(char* table,char* fields){return fSQL->CreateTable(table,fields);}
   bool   DeleteTable(char* table){return fSQL->DeleteTable(table);}
   bool   ReadField(char *table,char* field,char* constraint){return fSQL->ReadField(table,field,constraint);}
   bool   InsertRow(char *table,char* fields,char* values){return fSQL->InsertRow(table,fields,values);}
   bool   DeleteRow(char *table,char* constraint){return fSQL->DeleteRow(table,constraint);}
   bool   ReplaceField(char *table,char* field,char* value,char* constraint){return fSQL->ReplaceField(table,field,value,constraint);}
   bool   ExistField(char *table,char* field){return fSQL->ExistField(table,field);}
*/
   bool   MakeQuery(char* query,bool store){return fSQL->MakeQuery(query,store);}
   int    GetNumberOfRows(){return fSQL->GetNumberOfRows();}
   bool   NextRow(){return fSQL->NextRow();}
   int    GetNumberOfFields(){return fSQL->GetNumberOfFields();}
   char*  GetField(int fieldNumber){return fSQL->GetField(fieldNumber);}
   void   FreeResult(){return fSQL->FreeResult();}
   bool   DataSeek(my_ulonglong offset);

protected:
   bool   MakePhrase(ROMEPath *dataBasePath);
   bool   DecodeDBConstraint(const char* currentTableName,const char* nextTableName,const char* dbConstraint);
   void   ResetPhrase();
   bool   InRange(int value,int b1,int b2){return TMath::Min(b1,b2)<=value && value<=TMath::Max(b1,b2);}
};

#endif   // ROMESQLDataBase_H
