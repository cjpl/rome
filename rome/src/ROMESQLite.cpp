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
#include <ROMESQLite.h>

#if defined( R__VISUAL_CPLUSPLUS )
#   include <windows.h>
#endif

ROMESQLite::ROMESQLite() {
   db = NULL;
   vm = NULL;
   FreeResult();
}

ROMESQLite::~ROMESQLite() {
   DisConnect();
}

bool ROMESQLite::Connect(const char *server,const char *user,const char *passwd,const char *database,const char *port)
{
   TString filename = server;
   if(strlen(database) && strlen(server)){
      filename += "/";
      filename += database;
   }
   db = sqlite_open(filename.Data(), fileOpenMode, &errmsg);
   if(errmsg){
      cout<<"ROMESQLite:"<<GetErrorMessage()<<endl;
      DisConnect();
      return false;
   }
   return true;
}

bool ROMESQLite::DisConnect()
{
   if (db){
      sqlite_close(db);
      db = NULL;
   }
   return true;
}

bool ROMESQLite::MakeQuery(const char* query, bool store)
{
#if defined( SQLDEBUG )
   cout<<endl<<"ROMESQLite::MakeQuery : "<<query<<endl;
#endif
   if(store){
      if( (errnum = sqlite_compile(db, query, NULL, &vm, &errmsg)) != SQLITE_OK){
         cout<<query<<endl;
         cout << "Query error :" << GetErrorMessage() << endl;
         return false;
      }
      if(!StoreResult()){
         cout << "Query error :" << query << endl;
         return false;
      }
   }
   else if( (errnum = sqlite_exec(db, query, NULL, NULL, NULL)) != SQLITE_OK){
      cout << query <<endl;
      cout << "Query error :" << GetErrorMessage() << endl;
      numOfFields = 0;
      numOfRows   = 0;
      return false;
   }
   return true;
}

bool ROMESQLite::StoreResult()
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

bool ROMESQLite::NextRow() {
   if(currentRow+1 >= GetNumberOfRows()){
      cout << "NextRow error : You have tried nonexistent row." << endl;
      return false;
   }
   currentRow++;
   return true;
}

char* ROMESQLite::GetField(int fieldNumber) {
   if( fieldNumber < 0 || fieldNumber >= GetNumberOfFields() ) {
      cout << "GetField error : field number out of bounds" << endl;
      return NULL;
   }
   return (char*) result.At(fieldNumber,currentRow).Data();
}

void ROMESQLite::FreeResult() {
   if(vm){
      errnum = sqlite_finalize(vm, &errmsg);
      if(errmsg)
         cout<<GetErrorMessage()<<endl;
      vm = NULL;
   }
   numOfRows   = 0;
   numOfFields = 0;
   currentRow  = 0;
   result.RemoveAll();
}

bool ROMESQLite::StartTransaction( const char* option ){
   TString sqlQuery = "START TRANSACTION ";
   sqlQuery += option;
   sqlQuery += ";";
   return MakeQuery(sqlQuery.Data(),false);
}

bool ROMESQLite::CommitTransaction( const char* option ){
   TString sqlQuery = "COMMIT ";
   sqlQuery += option;
   sqlQuery += ";";
   return MakeQuery(sqlQuery.Data(),false);
}

bool ROMESQLite::RollbackTransaction( const char* option ){
   TString sqlQuery = "ROLLBACK ";
   sqlQuery += option;
   sqlQuery += ";";
   return MakeQuery(sqlQuery.Data(),false);
}
