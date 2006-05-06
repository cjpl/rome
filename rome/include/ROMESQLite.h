/********************************************************************
  ROMESQLite.h, R. Sawada

  $Id$

********************************************************************/
#ifndef ROMESQLITE_H
#define ROMESQLITE_H

#include <sqlite.h>
#include <ROMESQL.h>
#include <ROMEStrArray.h>
#include <ROMEStr2DArray.h>
const Int_t fileOpenMode = 0;

class ROMESQLite : public ROMESQL
{
protected:
   char           *errmsg;
   Int_t           errnum;
   sqlite         *db;
   sqlite_vm      *vm;
   ROMEStr2DArray  result;
   Int_t           numOfRows;
   Int_t           numOfFields;
   Int_t           currentRow;

public:
   ROMESQLite();
   virtual ~ROMESQLite();
   Bool_t  Connect(const char *server,const char *user,const char *passwd,const char *database,const char *port);
   Bool_t  DisConnect();
   Bool_t  MakeQuery(const char* query,Bool_t store);
   Bool_t  StoreResult();
   Bool_t  NextRow();
   char*   GetField(Int_t fieldNumber);
   Int_t   GetNumberOfRows(){ return numOfRows; }
   Int_t   GetNumberOfFields(){ return numOfFields; }
   void    FreeResult();
   Int_t   GetErrorCode(){ return errnum; };
   char   *GetErrorMessage(){ return errmsg; };
   Bool_t  StartTransaction( const char* option );
   Bool_t  CommitTransaction( const char* option );
   Bool_t  RollbackTransaction( const char* option );

   ClassDef(ROMESQLite, 0) // Interface to SQLite version 2.
};

#endif   // ROMESQLITE_H
