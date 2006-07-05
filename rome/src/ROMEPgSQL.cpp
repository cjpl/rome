// Author: Ryu Sawada
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMEPgSQL                                                             //
//                                                                      //
//  Provides PostgreSQL data base access.
//                                                                      //
//  $Id$
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include "ROMEiostream.h"
#include <TString.h>
#include "ROMEPgSQL.h"

ClassImp(ROMEPgSQL)

ROMEPgSQL::ROMEPgSQL() {
   connection = 0;
   result = 0;
}

ROMEPgSQL::~ROMEPgSQL() {
   DisConnect();
   SafeDelete(connection);
   SafeDelete(result);
}

Bool_t ROMEPgSQL::Connect(const char *server,const char *user,const char *passwd,const char *database,const char *port)
{
   if(!strcmp(port,"0"))
      port = NULL;
   connection = PQsetdbLogin(server, port, NULL, NULL, database, user, passwd);
   if (PQstatus(connection) == CONNECTION_BAD) {
      ROMEPrint::Error("Could not connect to the data base '%s' : Error: %s\n", database, GetErrorMessage());
      return false;
   }
   return true;
}

Bool_t ROMEPgSQL::DisConnect()
{
   if (connection)
      PQfinish(connection);
   return true;
}

Bool_t ROMEPgSQL::MakeQuery(const char* query, Bool_t store)
{
   ROMEPrint::Debug("ROMEPgSQL::MakeQuery : %s\n", query);
   result = PQexec(connection, query);
   if ((PQresultStatus(result) != PGRES_COMMAND_OK) &&
       (PQresultStatus(result) != PGRES_TUPLES_OK)) {
      ROMEPrint::Error("%s\n", query);
      ROMEPrint::Error("Query error : %s\n", GetErrorMessage());
      PQclear(result);
      return false;
   }
   fCurrentRow = -1;
   return true;
}

Bool_t ROMEPgSQL::StoreResult() {
   return true;
}

void ROMEPgSQL::FreeResult(){
   if (!result)
      return;
   PQclear(result);
   result      = 0;
   fCurrentRow = 0;
}

Int_t ROMEPgSQL::GetNumberOfRows() {
   if( !result ) {
      ROMEPrint::Error("GetNumberOfRows error : no query result.\n");
      return -1;
   }
   return (int)PQntuples(result);
}

Int_t ROMEPgSQL::GetNumberOfFields() {
   if (!result) {
      ROMEPrint::Error("GetFieldCount error : no query result.\n");
      return -1;
   }
   return PQnfields(result);
}

char* ROMEPgSQL::GetField(Int_t fieldNumber) {
   if( fieldNumber < 0 || fieldNumber >= GetNumberOfFields() ) {
      ROMEPrint::Error("GetField error : field number out of bounds\n");
      return NULL;
   }
   return PQgetvalue(result, fCurrentRow, fieldNumber);
}

Bool_t ROMEPgSQL::NextRow() {
   if(fCurrentRow+1 >= GetNumberOfRows()){
      ROMEPrint::Print("NextRow error : You have tried nonexistent row.\n");
      return false;
   }
   fCurrentRow++;
   return true;
}

Int_t ROMEPgSQL::GetErrorCode() {
   return (int) PQresultStatus(result);
}

char* ROMEPgSQL::GetErrorMessage() {
   return (char*) PQerrorMessage(connection);
}

Bool_t ROMEPgSQL::StartTransaction( const char* option ){
   TString sqlQuery = "START TRANSACTION ";
   sqlQuery += option;
   sqlQuery += ";";
   return MakeQuery(sqlQuery.Data(),false);
}

Bool_t ROMEPgSQL::CommitTransaction( const char* option ){
   TString sqlQuery = "COMMIT ";
   sqlQuery += option;
   sqlQuery += ";";
   return MakeQuery(sqlQuery.Data(),false);
}

Bool_t ROMEPgSQL::RollbackTransaction( const char* option ){
   TString sqlQuery = "ROLLBACK ";
   sqlQuery += option;
   sqlQuery += ";";
   return MakeQuery(sqlQuery.Data(),false);
}
