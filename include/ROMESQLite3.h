/********************************************************************
  ROMESQLite3.h, R. Sawada

  $Id$

********************************************************************/
#ifndef ROMESQLITE3_H
#define ROMESQLITE3_H

#if defined ( R__VISUAL_CPLUSPLUS )
#   include <Windows4root.h>
//typedef UINT_PTR        SOCKET;
#endif
#include "ROMESQL.h"
#include "ROMEStr2DArray.h"
#include <sqlite3.h>

class ROMESQLite3 : public ROMESQL
{
protected:
   sqlite3*       db;
   sqlite3_stmt*  stmt;
   ROMEStr2DArray result;
   Int_t          numOfFields;
   Int_t          numOfRows;
   Int_t          currentRow;

private:
   ROMESQLite3(const ROMESQLite3 &sql); // not implemented
   ROMESQLite3 &operator=(const ROMESQLite3 &rhs); // not implemented

public:
   ROMESQLite3();
   virtual ~ROMESQLite3();

   Bool_t Connect(const char *server,const char *user,const char *passwd,const char *database,const char *port);
   Bool_t DisConnect();
   Bool_t MakeQuery(const char* query,Bool_t store);
   Bool_t StoreResult();
   Bool_t NextRow();
   const char* GetField(Int_t fieldNumber);
   Int_t  GetNumberOfRows(){ return numOfRows; }
   Int_t  GetNumberOfFields(){ return numOfFields; }
   void   FreeResult();
   Int_t  GetErrorCode(){ return sqlite3_errcode(db); }
   const char* GetErrorMessage(){ return sqlite3_errmsg(db); }
   Bool_t StartTransaction( const char* option );
   Bool_t CommitTransaction( const char* option );
   Bool_t RollbackTransaction( const char* option );

   ClassDef(ROMESQLite3, 0) // Interface to SQLite3 database
};

#endif   // ROMESQLITE3_H
