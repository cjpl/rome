// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMEMySQL                                                             //
//                                                                      //
//  Provides MySQL data base access.
//                                                                      //
//  $Id$
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include "ROMEiostream.h"
#include <TString.h>
#include "ROMEMySQL.h"

ClassImp(ROMEMySQL)

ROMEMySQL::ROMEMySQL() {
   mysql_init(&mysql);
}

ROMEMySQL::~ROMEMySQL() {
   DisConnect();
}

Bool_t ROMEMySQL::Connect(const char *server,const char *user,const char *passwd,const char *database,const char *port)
{
   if (!mysql_real_connect(&mysql, server, user, passwd, database,atoi(port), NULL, 0)) {
      ROMEPrint::Error("Could not connect to the data base '%s' : Error: %s\n", database, GetErrorMessage());
      return false;
   }
   return true;
}

Bool_t ROMEMySQL::DisConnect()
{
   mysql_close(&mysql);
   return true;
}

Bool_t ROMEMySQL::MakeQuery(const char* query, Bool_t store)
{
   if (strlen(query) < 2048)
      ROMEPrint::Debug("ROMEMySQL::MakeQuery : %s\n", query);
   if (mysql_query(&mysql,query)) {
      ROMEPrint::Error("%s\n", query);
      ROMEPrint::Error("Query error : %s\n", GetErrorMessage());
      return false;
   }
   if (store && !StoreResult()) {
      ROMEPrint::Error("%s\n", query);
      ROMEPrint::Error("Query error : %s\n", GetErrorMessage());
      return false;
   }
   return true;
}

Bool_t ROMEMySQL::StoreResult() {
   if(!(result = mysql_store_result(&mysql)))
      return false;
   return true;
}

void ROMEMySQL::FreeResult(){
   mysql_free_result(result);
}

Int_t ROMEMySQL::GetNumberOfRows() {
   if( !result ) {
      ROMEPrint::Error("GetNumberOfRows error : no query result.\n");
      return -1;
   }
   return (int)mysql_num_rows(result);
}

Int_t ROMEMySQL::GetNumberOfFields() {
   if( !this->row ) {
      ROMEPrint::Error("GetFieldCount error : no query result.\n");
      return -1;
   }
   return mysql_num_fields(result);
}

Bool_t ROMEMySQL::DataSeek(my_ulonglong offset) {
   if( !result ) {
      ROMEPrint::Error("DataSeek error : no query result");
      return false;
   }
   if( GetNumberOfRows() <= (int)offset ) {
      ROMEPrint::Error("DataSeek error : offset is larger than number of results");
      return false;
   }
   mysql_data_seek(result,offset);
   if( !(row = mysql_fetch_row(result)) ) {
      ROMEPrint::Error("DataSeek error : %s\n", GetErrorMessage());
      return false;
   }
   return true;
}

char* ROMEMySQL::GetField(Int_t fieldNumber) {
   if( !row ) {
      ROMEPrint::Error("GetField error : no query result.\n");
      return NULL;
   }
   if( fieldNumber < 0 || fieldNumber >= GetNumberOfFields() ) {
      ROMEPrint::Error("GetField error : field number out of bounds\n");
      return NULL;
   }
   return this->row[fieldNumber];
}

Bool_t ROMEMySQL::NextRow() {
   if( !result ) {
      ROMEPrint::Error("NextRow error : no query result.\n");
      return false;
   }
   if( !(row = mysql_fetch_row(result)) ) {
      ROMEPrint::Error("NextRow error : %s\n", GetErrorMessage());
      return false;
   }
   return true;
}

Int_t ROMEMySQL::GetErrorCode() {
   return mysql_errno(&mysql);
}

char* ROMEMySQL::GetErrorMessage() {
   return (char*) mysql_error(&mysql);
}

Bool_t ROMEMySQL::StartTransaction( const char* option ){
   TString sqlQuery = "START TRANSACTION ";
   sqlQuery += option;
   sqlQuery += ";";
   return MakeQuery(sqlQuery.Data(),false);

}

Bool_t ROMEMySQL::CommitTransaction( const char* option ){
   TString sqlQuery = "COMMIT ";
   sqlQuery += option;
   sqlQuery += ";";
   return MakeQuery(sqlQuery.Data(),false);
}

Bool_t ROMEMySQL::RollbackTransaction( const char* option ){
   TString sqlQuery = "ROLLBACK ";
   sqlQuery += option;
   sqlQuery += ";";
   return MakeQuery(sqlQuery.Data(),false);
}
