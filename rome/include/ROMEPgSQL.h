/********************************************************************
  ROMEPgSQL.h, R. Sawada

  $Log$
  Revision 1.3  2005/05/13 23:51:14  sawada
  code cleanup.

  Revision 1.2  2005/04/28 20:26:39  sawada
  Added transaction during writing database.
  This is disabled by default because it is not well tested.
  These codes are in "#ifdef USE_TRANSACTION #endif".
  Probably, database access will be accelerated with transaction.

  Revision 1.1  2005/04/27 17:34:45  sawada
  Added PostgreSQL class. This is not yet available in ROME since it is not tested.


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
