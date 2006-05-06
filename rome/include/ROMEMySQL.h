/********************************************************************
  ROMEMySQL.h, M. Schneebeli PSI

  $Id$

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
   MYSQL_RES *result;
   MYSQL_ROW  row;

public:
   ROMEMySQL();
   virtual ~ROMEMySQL();
   Bool_t  Connect(const char *server,const char *user,const char *passwd,const char *database,const char *port);
   Bool_t  DisConnect();
   Bool_t  MakeQuery(const char* query,Bool_t store);
   Bool_t  StoreResult();
   Bool_t  NextRow();
   char   *GetField(Int_t fieldNumber);
   Int_t   GetNumberOfRows();
   Int_t   GetNumberOfFields();
   void    FreeResult();
   Int_t   GetErrorCode();
   char   *GetErrorMessage();
   Bool_t  StartTransaction( const char* option );
   Bool_t  CommitTransaction( const char* option );
   Bool_t  RollbackTransaction( const char* option );

   bool  DataSeek(my_ulonglong offset);

   ClassDef(ROMEMySQL, 0) // Interface to MySQL database
};

#endif   // ROMEMYSQL_H
