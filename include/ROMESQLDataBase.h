/********************************************************************
  ROMESQLDataBase.h, M. Schneebeli PSI

  $Id:$

********************************************************************/
#ifndef ROMESQLDataBase_H
#define ROMESQLDataBase_H

#include <ROMEStr2DArray.h>
#include <ROMESQL.h>
#if defined( HAVE_MYSQL )
#   include <ROMEMySQL.h>
#endif
#if defined( HAVE_PGSQL )
#   include <ROMEPgSQL.h>
#endif
#if defined( HAVE_SQLITE )
#   include <ROMESQLite.h>
#endif
#if defined( HAVE_SQLITE3 )
#   include <ROMESQLite3.h>
#endif
#include <Riostream.h>

#include <ROMEDataBase.h>
#include <ROMEString.h>
#include <ROMEPath.h>

class ROMESQLDataBase : public ROMEDataBase
{
protected:
  ROMEString fSelectFieldList;
  ROMEString fInsertFieldList;
  ROMEString fSetFieldList;
  ROMEString fFromPhrase;
  ROMEString fWherePhrase;
  ROMEString fAdInsertFields;
  ROMEString fAdInsertValues;
  ROMESQL    *fSQL;
  TString    fDBMSType;

public:
   ROMESQLDataBase();
   ~ROMESQLDataBase();

   char*  GetDBMSType() { return (char*) fDBMSType.Data(); }
   void   LinkError() {
      cout<<"Error: This program is not linked with "<<fDBMSType<<" library."<<endl;
   }

   bool   Init(const char* name,const char* dataBase,const char* connection);
   bool   Read(ROMEStr2DArray *values,const char *dataBasePath,int runNumber);
   bool   Write(ROMEStr2DArray* values,const char *dataBasePath,int runNumber);
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
   void   FreeResult(){fSQL->FreeResult();}
//   bool   DataSeek(my_ulonglong offset);

protected:
   bool   MakePhrase(ROMEPath *dataBasePath,int runNumber);
   bool   DecodeDBConstraint(const char* currentTableName,const char* nextTableName,const char* dbConstraint,int runNumber,const char* currentIdName,const char* currentIdxName);
   void   ResetPhrase();
   bool   InRange(int value,int b1,int b2){return TMath::Min(b1,b2)<=value && value<=TMath::Max(b1,b2);}
};

#endif   // ROMESQLDataBase_H
