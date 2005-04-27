/********************************************************************
  ROMESQLite.h, R. Sawada

  $Log$
  Revision 1.1  2005/04/27 10:30:45  sawada
  Added SQLite,SQLite3 support.


********************************************************************/
#ifndef ROMESQLITE_H
#define ROMESQLITE_H

#include <sqlite.h>
#include <ROMESQL.h>
#include <ROMEStrArray.h>
#include <ROMEStr2DArray.h>
const int fileOpenMode = 0;

class ROMESQLite : public ROMESQL
{
protected:
   char*          errmsg;
   int            errnum;
   sqlite*        db;
   sqlite_vm*     vm;
   ROMEStr2DArray result;
   int            numOfRows;
   int            numOfFields;
   int            currentRow;

public:
   ROMESQLite();
   virtual ~ROMESQLite();
   bool  Connect(const char *server,const char *user,const char *passwd,const char *database,const char *port);
   bool  DisConnect();
   bool  MakeQuery(const char* query,bool store);
   bool  StoreResult();
   bool  NextRow();
   char* GetField(int fieldNumber);
   int   GetNumberOfRows(){ return numOfRows; }
   int   GetNumberOfFields(){ return numOfFields; }
   void  FreeResult();
   int   GetErrorCode(){ return errnum; };
   char* GetErrorMessage(){ return errmsg; };
};

#endif   // ROMESQLITE_H
