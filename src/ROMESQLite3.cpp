// Author: Ryu Sawada
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMESQLite3                                                           //
//                                                                      //
//  Provides SQL data base access.
//                                                                      //
//  $Id$
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include "ROMEiostream.h"
#include <TString.h>
#include "ROMESQLite3.h"

ClassImp(ROMESQLite3)

ROMESQLite3::ROMESQLite3() {
   db   = NULL;
   stmt = NULL;
   FreeResult();
}

ROMESQLite3::~ROMESQLite3() {
   DisConnect();
}

Bool_t ROMESQLite3::Connect(const char *server,const char * /*user*/,const char * /*passwd*/,const char *database,const char * /*port*/)
{
   TString filename = server;
   if(strlen(database) && strlen(server)){
      filename += "/";
      filename += database;
   }
   sqlite3_open(filename.Data(), &db);
   if( GetErrorCode() != SQLITE_OK ){
      ROMEPrint::Error("ROMESQLite3: %s\n", GetErrorMessage());
      return false;
   }
   return true;
}

Bool_t ROMESQLite3::DisConnect()
{
   if (db)
      sqlite3_close(db);
   return true;
}

Bool_t ROMESQLite3::MakeQuery(const char* query, Bool_t store)
{
   ROMEPrint::Debug("\nROMESQLite3::MakeQuery : %s\n", query);
   if(store){
      sqlite3_prepare(db, query, -1, &stmt, NULL);
      if(GetErrorCode() != SQLITE_OK){
         ROMEPrint::Error("%s\n", query);
         ROMEPrint::Error("Query error : %s\n", GetErrorMessage());
         return false;
      }
      if(!StoreResult()){
         ROMEPrint::Error("Query error : %s\n", GetErrorMessage());
         return false;
      }
   }
   else if(sqlite3_exec(db, query, NULL, NULL, NULL) != SQLITE_OK){
      ROMEPrint::Error("%s\n", query);
      ROMEPrint::Error("Query error : %s\n", GetErrorMessage());
      return false;
   }
   return true;
}

Bool_t ROMESQLite3::StoreResult()
{
   int i=0;
   int j=0;
   while(sqlite3_step(stmt) != SQLITE_DONE){
      switch (GetErrorCode()){
         case SQLITE_BUSY:
            usleep(10);
            continue;
            break;
         case SQLITE_ROW:
            for(i=0;i<sqlite3_column_count(stmt);i++)
               result.SetAt((char*)sqlite3_column_text(stmt,i),i,j);
            j++;
            break;
         default:
            return false;
      }
   }
   numOfFields = i;
   numOfRows   = j;
   currentRow  = -1;
   return true;
}

Bool_t ROMESQLite3::NextRow() {
   if(currentRow+1 >= GetNumberOfRows()){
      ROMEPrint::Error("NextRow error : You have tried nonexistent row.\n");
      return false;
   }
   currentRow++;
   return true;
}


char* ROMESQLite3::GetField(Int_t fieldNumber) {
   if( fieldNumber < 0 || fieldNumber >= GetNumberOfFields() ) {
      ROMEPrint::Error("GetField error : field number out of bounds\n");
      return NULL;
   }
   return (char*) result.At(fieldNumber,currentRow).Data();
}

void ROMESQLite3::FreeResult() {
   if(stmt){
      sqlite3_finalize(stmt);
   }
   numOfFields = 0;
   numOfRows   = 0;
   currentRow  = 0;
   result.RemoveAll();
}

Bool_t ROMESQLite3::StartTransaction( const char* option ){
   TString sqlQuery = "BEGIN ";
   sqlQuery += option;
   sqlQuery += ";";
   return MakeQuery(sqlQuery.Data(),false);
}

Bool_t ROMESQLite3::CommitTransaction( const char* option ){
   TString sqlQuery = "COMMIT ";
   sqlQuery += option;
   sqlQuery += ";";
   return MakeQuery(sqlQuery.Data(),false);
}

Bool_t ROMESQLite3::RollbackTransaction( const char* option ){
   TString sqlQuery = "ROLLBACK ";
   sqlQuery += option;
   sqlQuery += ";";
   return MakeQuery(sqlQuery.Data(),false);
}
