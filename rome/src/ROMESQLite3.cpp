// Author: Ryu Sawada
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMESQLite3                                                           //
//                                                                      //
//  Provides SQL data base access.
//                                                                      //
//  $Log$
//  Revision 1.4  2005/04/28 20:26:39  sawada
//  Added transaction during writing database.
//  This is disabled by default because it is not well tested.
//  These codes are in "#ifdef USE_TRANSACTION #endif".
//  Probably, database access will be accelerated with transaction.
//
//  Revision 1.3  2005/04/28 10:01:45  sawada
//  PostgreSQL support.
//
//  Revision 1.2  2005/04/27 18:34:08  sawada
//  small bug fix.
//
//  Revision 1.1  2005/04/27 10:30:45  sawada
//  Added SQLite,SQLite3 support.
//
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <ROMESQLite3.h>

ROMESQLite3::ROMESQLite3() {
   db   = NULL;
   stmt = NULL;
   FreeResult();
}

ROMESQLite3::~ROMESQLite3() {
   DisConnect();
}

bool ROMESQLite3::Connect(const char *server,const char *user,const char *passwd,const char *database,const char *port)
{
   TString filename = server;
   if(strlen(database) && strlen(server)){
      filename += "/";
      filename += database;
   }
   sqlite3_open(filename.Data(), &db);
   if( GetErrorCode() != SQLITE_OK ){
      cout<<"ROMESQLite3:"<<GetErrorMessage()<<endl;
      return false;
   }
   return true;
}

bool ROMESQLite3::DisConnect()
{
   if (db)
      sqlite3_close(db);
   return true;
}

bool ROMESQLite3::MakeQuery(const char* query, bool store)
{
#ifdef SQLDEBUG
   cout<<endl<<"ROMESQLite3::MakeQuery : "<<query<<endl;
#endif
   if(store){
      sqlite3_prepare(db, query, 0, &stmt, NULL);
      if(GetErrorCode() != SQLITE_OK){
         cout<<query<<endl;
         cout << "Query error :" << GetErrorMessage() << endl;
         return false;
      }
      if(!StoreResult()){
         cout << "Query error :" << query << endl;
         return false;
      }
   }
   else if(sqlite3_exec(db, query, NULL, NULL, NULL) != SQLITE_OK){
      cout << query <<endl;
      cout << "Query error :" << GetErrorMessage() << endl;
      return false;
   }
   return true;
}

bool ROMESQLite3::StoreResult()
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
      };
   }
   numOfFields = i;
   numOfRows   = j;
   currentRow  = -1;
   return true;
}

bool ROMESQLite3::NextRow() {
   if(currentRow+1 >= GetNumberOfRows()){
      cout << "NextRow error : You have tried nonexistent row." << endl;
      return false;
   }
   currentRow++;
   return true;
}


char* ROMESQLite3::GetField(int fieldNumber) {
   if( fieldNumber < 0 || fieldNumber >= GetNumberOfFields() ) {
      cout << "GetField error : field number out of bounds" << endl;
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

bool ROMESQLite3::StartTransaction( const char* option ){
   TString sqlQuery = "BEGIN ";
   sqlQuery += option;
   sqlQuery += ";";
   return MakeQuery(sqlQuery.Data(),false);
}

bool ROMESQLite3::CommitTransaction( const char* option ){
   TString sqlQuery = "COMMIT ";
   sqlQuery += option;
   sqlQuery += ";";
   return MakeQuery(sqlQuery.Data(),false);
}

bool ROMESQLite3::RollbackTransaction( const char* option ){
   TString sqlQuery = "ROLLBACK ";
   sqlQuery += option;
   sqlQuery += ";";
   return MakeQuery(sqlQuery.Data(),false);
}
