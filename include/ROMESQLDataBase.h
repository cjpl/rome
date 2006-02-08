/********************************************************************
  ROMESQLDataBase.h, M. Schneebeli PSI

  $Id$

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
  ROMEString  fSelectFieldList;
  ROMEString  fInsertFieldList;
  ROMEString  fSetFieldList;
  ROMEString  fFromPhrase;
  ROMEString  fWherePhrase;
  ROMEString  fAdInsertFields;
  ROMEString  fAdInsertValues;
  ROMESQL    *fSQL;
  TString     fDBMSType;

public:
   ROMESQLDataBase();
   ~ROMESQLDataBase();

   char*  GetDBMSType() { return (char*) fDBMSType.Data(); }
   void   LinkError() {
      cout<<"Error: This program is not linked with "<<fDBMSType.Data()<<" library."<<endl;
   }

   Bool_t       Init(const char* name,const char* dataBase,const char* connection);
   Bool_t       Read(ROMEStr2DArray *values,const char *dataBasePath,Long64_t runNumber,Long64_t eventNumber);
   Bool_t       Write(ROMEStr2DArray* values,const char *dataBasePath,Long64_t runNumber,Long64_t eventNumber);
   char        *GetType() { return "sql"; }
   char        *GetDescription() { return "SQL data base using the file system to store tables"; }
   Bool_t       MakeQuery(char* query,Bool_t store){return fSQL->MakeQuery(query,store);}
   Int_t        GetNumberOfRows(){return fSQL->GetNumberOfRows();}
   Bool_t       NextRow(){return fSQL->NextRow();}
   Int_t        GetNumberOfFields(){return fSQL->GetNumberOfFields();}
   char        *GetField(Int_t fieldNumber){return fSQL->GetField(fieldNumber);}
   void         FreeResult(){fSQL->FreeResult();}
   virtual void Print(Option_t *);
//   Bool_t   DataSeek(my_ulonglong offset);
/*
   Bool_t Connect(const char *server,const char *user,const char *passwd,const char *database,const char *port){
      return fSQL->Connect(server,user,passwd,database,port);
   }
   Bool_t   DisConnect(){fSQL->DisConnect();}
   Bool_t   CreateDataBase(char* database){return fSQL->CreateDataBase(database);}
   Bool_t   DeleteDataBase(char* database){return fSQL->DeleteDataBase(database);}
   Bool_t   CreateTable(char* table,char* fields){return fSQL->CreateTable(table,fields);}
   Bool_t   DeleteTable(char* table){return fSQL->DeleteTable(table);}
   Bool_t   ReadField(char *table,char* field,char* constraint){return fSQL->ReadField(table,field,constraint);}
   Bool_t   InsertRow(char *table,char* fields,char* values){return fSQL->InsertRow(table,fields,values);}
   Bool_t   DeleteRow(char *table,char* constraint){return fSQL->DeleteRow(table,constraint);}
   Bool_t   ReplaceField(char *table,char* field,char* value,char* constraint){return fSQL->ReplaceField(table,field,value,constraint);}
   Bool_t   ExistField(char *table,char* field){return fSQL->ExistField(table,field);}
*/

protected:
   Bool_t       MakePhrase(ROMEPath *dataBasePath,Long64_t runNumber,Long64_t eventNumber);
   Bool_t       DecodeDBConstraint(const char* currentTableName,const char* nextTableName,const char* dbConstraint,Long64_t runNumber,Long64_t eventNumber,const char* currentIdName,const char* currentIdxName);
   void         ResetPhrase();
   Bool_t       InRange(Int_t value,Int_t b1,Int_t b2){return TMath::Min(b1,b2)<=value && value<=TMath::Max(b1,b2);}
};

#endif   // ROMESQLDataBase_H
