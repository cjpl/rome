/********************************************************************
  ROMESQL.h, R.Sawada

  $Log$
  Revision 1.11  2005/04/27 10:30:45  sawada
  Added SQLite,SQLite3 support.


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
   virtual bool  Connect(const char* server,const char* user,const char* passwd,const char* database,const char* port) = 0;
   virtual bool  DisConnect() = 0;
   virtual bool  MakeQuery(const char* query,bool store) = 0;
   virtual bool  StoreResult() = 0;
   virtual bool  NextRow() = 0;
   virtual char* GetField(int fieldNumber) = 0;
   virtual int   GetNumberOfRows() = 0;
   virtual int   GetNumberOfFields() = 0;
   virtual void  FreeResult() = 0;
   virtual int   GetErrorCode() = 0;
   virtual char* GetErrorMessage() = 0;
   
   // DBMS independent methods
   bool  MakeQuery(TString& query,bool store){ return MakeQuery(query.Data(),store); }
   bool  CreateDataBase(const char* database);
   bool  DeleteDataBase(const char* database);
   bool  CreateTable(const char* table,const char* fields);
   bool  DeleteTable(const char* table);
   bool  ReadField(const char* table,const char* field,const char* constraint);
   bool  InsertRow(const char* table,const char* fields,const char* values);
   bool  DeleteRow(const char* table,const char* constraint);
   bool  ReplaceField(const char* table,const char* field,const char* value,const char* constraint);
   bool  ExistField(const char* table,const char* field);
};

#endif   // ROMESQL_H
