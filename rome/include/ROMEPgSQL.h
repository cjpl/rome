/********************************************************************
  ROMEPgSQL.h, R. Sawada

  $Id:$

********************************************************************/
#ifndef ROMEPGSQL_H
#define ROMEPGSQL_H

#include <ROMESQL.h>
#if defined ( R__VISUAL_CPLUSPLUS )
#   include <Windows4root.h>
//typedef UINT_PTR        SOCKET;
#endif
//#include <postgres.h>
#include <libpq-fe.h>

class ROMEPgSQL : public ROMESQL
{
protected:
   PGconn     *connection;
   PGresult   *result;
   int        fCurrentRow;

public:
   ROMEPgSQL();
   virtual ~ROMEPgSQL();
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

};

#endif   // ROMEPGSQL_H
