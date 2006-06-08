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
      cout<<"Could not connect to the data base '"<<database<<"' : Error: "<<GetErrorMessage()<<endl;
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
#if defined( SQLDEBUG )
   cout<<endl<<"ROMEMySQL::MakeQuery : "<<query<<endl;
#endif
   if (mysql_query(&mysql,query)) {
      cout << query <<endl;
      cout << "Query error :" << GetErrorMessage() << endl;
      return false;
   }
   if (store && !StoreResult()) {
      cout << query <<endl;
      cout << "Query error :" << GetErrorMessage() << endl;
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
      cout << "GetNumberOfRows error : no query result." << endl;
      return -1;
   }
   return (int)mysql_num_rows(result);
}

Int_t ROMEMySQL::GetNumberOfFields() {
   if( !this->row ) {
      cout << "GetFieldCount error : no query result." << endl;
      return -1;
   }
   return mysql_num_fields(result);
}

Bool_t ROMEMySQL::DataSeek(my_ulonglong offset) {
   if( !result ) {
      cout << "DataSeek error : no query result" << endl;
      return false;
   }
   if( GetNumberOfRows() <= (int)offset ) {
      cout << "DataSeek error : offset is larger than number of results"<<endl;
      return false;
   }
   mysql_data_seek(result,offset);
   if( !(row = mysql_fetch_row(result)) ) {
      cout << "DataSeek error :" << GetErrorMessage() << endl;
      return false;
   }
   return true;
}

char* ROMEMySQL::GetField(Int_t fieldNumber) {
   if( !row ) {
      cout << "GetField error : no query result." << endl;
      return NULL;
   }
   if( fieldNumber < 0 || fieldNumber >= GetNumberOfFields() ) {
      cout << "GetField error : field number out of bounds" << endl;
      return NULL;
   }
   return this->row[fieldNumber];
}

Bool_t ROMEMySQL::NextRow() {
   if( !result ) {
      cout << "NextRow error : no query result." << endl;
      return false;
   }
   if( !(row = mysql_fetch_row(result)) ) {
      cout << "NextRow error :" << GetErrorMessage() << endl;
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
