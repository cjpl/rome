/********************************************************************
  ROMESQL.h, R.Sawada

  $Id$

********************************************************************/
#ifndef ROMESQL_H
#define ROMESQL_H

#include <Riostream.h>
#include <ROMEString.h>

class ROMESQL
{
public:
   ROMESQL(){};
   virtual ~ROMESQL(){};

   // DBMS dependent methods
   virtual Bool_t  Connect(const char* server,const char* user,const char* passwd,const char* database,const char* port) = 0;
   virtual Bool_t  DisConnect() = 0;
   virtual Bool_t  MakeQuery(const char* query,Bool_t store) = 0;
   virtual Bool_t  StoreResult() = 0;
   virtual Bool_t  NextRow() = 0;
   virtual char   *GetField(Int_t fieldNumber) = 0;
   virtual Int_t   GetNumberOfRows() = 0;
   virtual Int_t   GetNumberOfFields() = 0;
   virtual void    FreeResult() = 0;
   virtual Int_t   GetErrorCode() = 0;
   virtual char   *GetErrorMessage() = 0;
   virtual Bool_t  StartTransaction( const char* option ){ return true; }
   virtual Bool_t  CommitTransaction( const char* option ){ return true; }
   virtual Bool_t  RollbackTransaction( const char* option ){ return true; }

   // DBMS independent methods
   Bool_t          MakeQuery(TString& query,Bool_t store){ return MakeQuery(query.Data(),store); }
   Bool_t          CreateDataBase(const char* database);
   Bool_t          DeleteDataBase(const char* database);
   Bool_t          CreateTable(const char* table,const char* fields);
   Bool_t          DeleteTable(const char* table);
   Bool_t          ReadField(const char* table,const char* field,const char* constraint);
   Bool_t          InsertRow(const char* table,const char* fields,const char* values);
   Bool_t          DeleteRow(const char* table,const char* constraint);
   Bool_t          ReplaceField(const char* table,const char* field,const char* value,const char* constraint);
   Bool_t           ExistField(const char* table,const char* field);
};

#endif   // ROMESQL_H
