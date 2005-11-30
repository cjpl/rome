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
#include <ROMEPgSQL.h>

ROMEPgSQL::ROMEPgSQL() {
   connection = 0;
   result = 0;
}

ROMEPgSQL::~ROMEPgSQL() {
   DisConnect();
}

bool ROMEPgSQL::Connect(const char *server,const char *user,const char *passwd,const char *database,const char *port)
{
   if(!strcmp(port,"0"))
      port = NULL;
   connection = PQsetdbLogin(server, port, NULL, NULL, database, user, passwd);
   if (PQstatus(connection) == CONNECTION_BAD) {
      cout<<"Could not connect to the data base '"<<database<<"' : Error: "<<GetErrorMessage()<<endl;
      return false;
   }
   return true;
}

bool ROMEPgSQL::DisConnect()
{
   if (connection)
      PQfinish(connection);
   return true;
}

bool ROMEPgSQL::MakeQuery(const char* query, bool store)
{
#if defined( SQLDEBUG )
   cout<<endl<<"ROMEPgSQL::MakeQuery : "<<query<<endl;
#endif
   result = PQexec(connection, query);
   if ((PQresultStatus(result) != PGRES_COMMAND_OK) &&
       (PQresultStatus(result) != PGRES_TUPLES_OK)) {
      cout << query <<endl;
      cout << "Query error :" << GetErrorMessage() << endl;
      PQclear(result);
      return false;
   }
   fCurrentRow = -1;
   return true;
}

bool ROMEPgSQL::StoreResult() {
   return true;
}

void ROMEPgSQL::FreeResult(){
   if (!result)
      return;
   PQclear(result);
   result      = 0;
   fCurrentRow = 0;
}

int ROMEPgSQL::GetNumberOfRows() {
   if( !result ) {
      cout << "GetNumberOfRows error : no query result." << endl;
      return -1;
   }
   return (int)PQntuples(result);
}

int ROMEPgSQL::GetNumberOfFields() {
   if (!result) {
      cout << "GetFieldCount error : no query result." << endl;
      return -1;
   }
   return PQnfields(result);
}

char* ROMEPgSQL::GetField(int fieldNumber) {
   if( fieldNumber < 0 || fieldNumber >= GetNumberOfFields() ) {
      cout << "GetField error : field number out of bounds" << endl;
      return NULL;
   }
   return PQgetvalue(result, fCurrentRow, fieldNumber);
}

bool ROMEPgSQL::NextRow() {
   if(fCurrentRow+1 >= GetNumberOfRows()){
      cout << "NextRow error : You have tried nonexistent row." << endl;
      return false;
   }
   fCurrentRow++;
   return true;
}

int ROMEPgSQL::GetErrorCode() {
   return (int) PQresultStatus(result);
}

char* ROMEPgSQL::GetErrorMessage() {
   return (char*) PQerrorMessage(connection);
}

bool ROMEPgSQL::StartTransaction( const char* option ){
   TString sqlQuery = "START TRANSACTION ";
   sqlQuery += option;
   sqlQuery += ";";
   return MakeQuery(sqlQuery.Data(),false);
}

bool ROMEPgSQL::CommitTransaction( const char* option ){
   TString sqlQuery = "COMMIT ";
   sqlQuery += option;
   sqlQuery += ";";
   return MakeQuery(sqlQuery.Data(),false);
}

bool ROMEPgSQL::RollbackTransaction( const char* option ){
   TString sqlQuery = "ROLLBACK ";
   sqlQuery += option;
   sqlQuery += ";";
   return MakeQuery(sqlQuery.Data(),false);
}
