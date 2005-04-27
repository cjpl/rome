/********************************************************************
  ROMESQLite3.h, R. Sawada

  $Log$
  Revision 1.1  2005/04/27 10:30:45  sawada
  Added SQLite,SQLite3 support.


********************************************************************/
#ifndef ROMESQLITE3_H
#define ROMESQLITE3_H

#if defined ( R__VISUAL_CPLUSPLUS )
#include <Windows4root.h>
//typedef UINT_PTR        SOCKET;
#endif
#include <ROMESQL.h>
#include <ROMEStr2DArray.h>
#include <sqlite3.h>

class ROMESQLite3 : public ROMESQL
{
protected:
   sqlite3*      db;
   sqlite3_stmt* stmt;
   ROMEStr2DArray result;
   int            numOfFields;
   int            numOfRows;
   int            currentRow;

public:
   ROMESQLite3();
   ~ROMESQLite3();
   bool  Connect(const char *server,const char *user,const char *passwd,const char *database,const char *port);
   bool  DisConnect();
   bool  MakeQuery(const char* query,bool store);
   bool  StoreResult();
   bool  NextRow();
   char* GetField(int fieldNumber);
   int   GetNumberOfRows(){ return numOfRows; }
   int   GetNumberOfFields(){ return numOfFields; }
   void  FreeResult();
   int   GetErrorCode(){ return sqlite3_errcode(db); }
   char* GetErrorMessage(){ return (char*) sqlite3_errmsg(db); }
};

#endif   // ROMESQLITE3_H
