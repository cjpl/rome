/********************************************************************
  ROMEMySQL.h, M. Schneebeli PSI

  $Id:$

********************************************************************/
#ifndef ROMEMYSQL_H
#define ROMEMYSQL_H

#include <ROMESQL.h>
#if defined ( R__VISUAL_CPLUSPLUS )
#   include <Windows4root.h>
//typedef UINT_PTR        SOCKET;
#endif
#include <mysql.h>

class ROMEMySQL : public ROMESQL
{
protected:
   MYSQL      mysql;
   MYSQL_RES* result;
   MYSQL_ROW  row;

public:
   ROMEMySQL();
   virtual ~ROMEMySQL();
   bool  Connect(const char *server,const char *user,const char *passwd,const char *database,const char *port);
   bool  DisConnect();
   bool  MakeQuery(const char* query,bool store);
   bool  StoreResult();
   bool  NextRow();
   char* GetField(int fieldNumber);
   int   GetNumberOfRows();
   int   GetNumberOfFields();
   void  FreeResult();
   int   GetErrorCode();
   char* GetErrorMessage();
   bool  StartTransaction( const char* option );
   bool  CommitTransaction( const char* option );
   bool  RollbackTransaction( const char* option );

   bool  DataSeek(my_ulonglong offset);
};

#endif   // ROMEMYSQL_H
