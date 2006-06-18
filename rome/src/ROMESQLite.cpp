// Author: Ryu Sawada
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMESQLite                                                           //
//                                                                      //
//  Provides SQL data base access.
//                                                                      //
//  $Id$
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include "ROMEiostream.h"
#include <TString.h>
#include "ROMESQLite.h"

#if defined( R__VISUAL_CPLUSPLUS )
#   include <windows.h>
#endif

ClassImp(ROMESQLite)

ROMESQLite::ROMESQLite() {
   db = NULL;
   vm = NULL;
   FreeResult();
}

ROMESQLite::~ROMESQLite() {
   DisConnect();
}

Bool_t ROMESQLite::Connect(const char *server,const char *user,const char *passwd,const char *database,const char *port)
{
   TString filename = server;
   if(strlen(database) && strlen(server)){
      filename += "/";
      filename += database;
   }
   db = sqlite_open(filename.Data(), fileOpenMode, &errmsg);
   if(errmsg){
      ROMEPrint::Error("ROMESQLite: %s\n", GetErrorMessage());
      DisConnect();
      return false;
   }
   return true;
}

Bool_t ROMESQLite::DisConnect()
{
   if (db){
      sqlite_close(db);
      db = NULL;
   }
   return true;
}

Bool_t ROMESQLite::MakeQuery(const char* query, Bool_t store)
{
   ROMEPrint::Debug("\nROMESQLite::MakeQuery : %s\n", query);
   if(store){
      if( (errnum = sqlite_compile(db, query, NULL, &vm, &errmsg)) != SQLITE_OK){
         ROMEPrint::Error("%s\n", query);
         ROMEPrint::Error("Query error : %s\n", GetErrorMessage());
         return false;
      }
      if(!StoreResult()){
         ROMEPrint::Error("Query error : %s\n", query);
         return false;
      }
   }
   else if( (errnum = sqlite_exec(db, query, NULL, NULL, NULL)) != SQLITE_OK){
      ROMEPrint::Error("%s\n", query);
      ROMEPrint::Error("Query error : %s\n", GetErrorMessage());
      numOfFields = 0;
      numOfRows   = 0;
      return false;
   }
   return true;
}

Bool_t ROMESQLite::StoreResult()
{
   int status;
   const char** values = NULL;
   const char** colNames = NULL;
   int nFields;
   int i=0;
   int j=0;
   while((status = sqlite_step(vm, &nFields, &values, &colNames)) != SQLITE_DONE){
      switch (status){
         case SQLITE_BUSY:
#if defined( R__VISUAL_CPLUSPLUS )
            Sleep(10);
#endif
#if defined( R__UNIX )
            usleep(10);
#endif
            continue;
            break;
         case SQLITE_ROW:
            for(i=0;i<nFields;i++)
               result.SetAt(values[i],i,j);
            j++;
            break;
         default:
            return false;
      };
   }
   numOfFields = i;
   numOfRows   = j;
   currentRow  = -1;
   return true;
}

Bool_t ROMESQLite::NextRow() {
   if(currentRow+1 >= GetNumberOfRows()){
      ROMEPrint::Error("NextRow error : You have tried nonexistent row.\n");
      return false;
   }
   currentRow++;
   return true;
}

char* ROMESQLite::GetField(Int_t fieldNumber) {
   if( fieldNumber < 0 || fieldNumber >= GetNumberOfFields() ) {
      ROMEPrint::Error("GetField error : field number out of bounds\n");
      return NULL;
   }
   return (char*) result.At(fieldNumber,currentRow).Data();
}

void ROMESQLite::FreeResult() {
   if(vm){
      errnum = sqlite_finalize(vm, &errmsg);
      if(errmsg)
         ROMEPrint::Error("%s\n", GetErrorMessage());
      vm = NULL;
   }
   numOfRows   = 0;
   numOfFields = 0;
   currentRow  = 0;
   result.RemoveAll();
}

Bool_t ROMESQLite::StartTransaction( const char* option ){
   TString sqlQuery = "START TRANSACTION ";
   sqlQuery += option;
   sqlQuery += ";";
   return MakeQuery(sqlQuery.Data(),false);
}

Bool_t ROMESQLite::CommitTransaction( const char* option ){
   TString sqlQuery = "COMMIT ";
   sqlQuery += option;
   sqlQuery += ";";
   return MakeQuery(sqlQuery.Data(),false);
}

Bool_t ROMESQLite::RollbackTransaction( const char* option ){
   TString sqlQuery = "ROLLBACK ";
   sqlQuery += option;
   sqlQuery += ";";
   return MakeQuery(sqlQuery.Data(),false);
}
