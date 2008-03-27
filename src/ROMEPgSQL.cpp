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

//______________________________________________________________________________
ROMEPgSQL::ROMEPgSQL()
:ROMESQL()
,connection(0)
,result(0)
,fCurrentRow(0)
{
}

//______________________________________________________________________________
ROMEPgSQL::~ROMEPgSQL() {
   FreeResult();
   DisConnect();
}

//______________________________________________________________________________
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

//______________________________________________________________________________
Bool_t ROMEPgSQL::DisConnect()
{
   if (connection)
      PQfinish(connection);
   return true;
}

//______________________________________________________________________________
Bool_t ROMEPgSQL::Ping()
{
   result = PQexec(connection, "SELECT 1;");
   PQclear(result);

   if (PQstatus(connection) == CONNECTION_OK) {
      return kTRUE;
   }

   PQreset(connection);
   if (PQstatus(connection) == CONNECTION_OK) {
      return kTRUE;
   }

   return kFALSE;
}

//______________________________________________________________________________
Bool_t ROMEPgSQL::MakeQuery(const char* query, Bool_t /* store */)
{
   if (!Ping()) { // if this is bad for performance, last pinged time should be checked.
      ROMEPrint::Warning("program sleeps for 5s and tries again.\n");
      gSystem->Sleep(5000);
   }
   if (strlen(query) < 2048)
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

//______________________________________________________________________________
Bool_t ROMEPgSQL::StoreResult() {
   return true;
}

//______________________________________________________________________________
void ROMEPgSQL::FreeResult(){
   if (!result)
      return;
   PQclear(result);
   result      = 0;
   fCurrentRow = 0;
}

//______________________________________________________________________________
Int_t ROMEPgSQL::GetNumberOfRows() {
   if( !result ) {
      ROMEPrint::Error("GetNumberOfRows error : no query result.\n");
      return -1;
   }
   return static_cast<Int_t>(PQntuples(result));
}

//______________________________________________________________________________
Int_t ROMEPgSQL::GetNumberOfFields() {
   if (!result) {
      ROMEPrint::Error("GetFieldCount error : no query result.\n");
      return -1;
   }
   return PQnfields(result);
}

//______________________________________________________________________________
const char* ROMEPgSQL::GetField(Int_t fieldNumber) {
   if( fieldNumber < 0 || fieldNumber >= GetNumberOfFields() ) {
      ROMEPrint::Error("GetField error : field number out of bounds\n");
      return NULL;
   }
   return PQgetvalue(result, fCurrentRow, fieldNumber);
}

//______________________________________________________________________________
Bool_t ROMEPgSQL::NextRow() {
   if(fCurrentRow+1 >= GetNumberOfRows()){
      ROMEPrint::Print("NextRow error : You have tried nonexistent row.\n");
      return false;
   }
   fCurrentRow++;
   return true;
}

//______________________________________________________________________________
Int_t ROMEPgSQL::GetErrorCode() {
   return static_cast<Int_t>(PQresultStatus(result));
}

//______________________________________________________________________________
const char* ROMEPgSQL::GetErrorMessage() {
   return PQerrorMessage(connection);
}

Bool_t ROMEPgSQL::StartTransaction( const char* option ){
   TString sqlQuery = "START TRANSACTION ";
   sqlQuery += option;
   sqlQuery += ";";
   return MakeQuery(sqlQuery.Data(),false);
}

//______________________________________________________________________________
Bool_t ROMEPgSQL::CommitTransaction( const char* option ){
   TString sqlQuery = "COMMIT ";
   sqlQuery += option;
   sqlQuery += ";";
   return MakeQuery(sqlQuery.Data(),false);
}

//______________________________________________________________________________
Bool_t ROMEPgSQL::RollbackTransaction( const char* option ){
   TString sqlQuery = "ROLLBACK ";
   sqlQuery += option;
   sqlQuery += ";";
   return MakeQuery(sqlQuery.Data(),false);
}
