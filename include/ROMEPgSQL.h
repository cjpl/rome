/********************************************************************
  ROMEPgSQL.h, R. Sawada

  $Id$

********************************************************************/
#ifndef ROMEPGSQL_H
#define ROMEPGSQL_H

#include "ROMESQL.h"
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
   Int_t       fCurrentRow;

public:
   ROMEPgSQL();
   virtual ~ROMEPgSQL();
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

   ClassDef(ROMEPgSQL, 0) // Interface to PostgreSQL database
};

#endif   // ROMEPGSQL_H
