// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMEMySQL                                                             //
//                                                                      //
//  Provides MySQL data base access.
//                                                                      //
//  $Log$
//  Revision 1.1  2005/04/27 10:30:45  sawada
//  Added SQLite,SQLite3 support.
//
//  Revision 1.14  2005/03/13 08:43:14  sawada
//  removed or comment out unused variables.
//  made virtual destructor of ROMEConfig and ROMEDataBase.
//
//  Revision 1.13  2004/11/19 16:26:24  sawada
//  speed up with reading order array at once.
//
//  Revision 1.12  2004/11/18 15:23:23  sawada
//
//  Modify handling the order of array.
//  Enable inverse order.
//  Enable to send sql query from user tasks.
//
//  Revision 1.11  2004/11/16 18:53:44  sawada
//  SQL Write
//
//  Revision 1.10  2004/11/16 12:11:06  sawada
//  SQL Init,Read
//
//  Revision 1.9  2004/10/05 07:52:44  schneebeli_m
//  dyn. Folders, TRef Objects, XML format changed, ROMEStatic removed
//
//  Revision 1.8  2004/09/30 13:08:21  schneebeli_m
//  ...
//
//  Revision 1.7  2004/09/25 01:34:48  schneebeli_m
//  implemented FW dependent EventLoop and DataBase classes
//
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <ROMEMySQL.h>

ROMEMySQL::ROMEMySQL() {
   mysql_init(&mysql);
}

ROMEMySQL::~ROMEMySQL() {
   DisConnect();
}

bool ROMEMySQL::Connect(const char *server,const char *user,const char *passwd,const char *database,const char *port)
{
   if (!mysql_real_connect(&mysql, server, user, passwd, database,atoi(port), NULL, 0)) {
      cout<<"Could not connect to the data base '"<<database<<"' : Error: "<<GetErrorMessage()<<endl;
      return false;
   }
   return true;
}

bool ROMEMySQL::DisConnect()
{
   mysql_close(&mysql);
   return true;
}

bool ROMEMySQL::MakeQuery(const char* query, bool store)
{
#ifdef SQLDEBUG
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

bool ROMEMySQL::StoreResult() {
   if(!(result = mysql_store_result(&mysql)))
      return false;
   return true;
}

void ROMEMySQL::FreeResult(){
   mysql_free_result(result);
}

int ROMEMySQL::GetNumberOfRows() {
   if( !result ) {
      cout << "NextRow error :" << GetErrorMessage() << endl;
      return -1;
   }
   return (int)mysql_num_rows(result);
}

int ROMEMySQL::GetNumberOfFields() {
   if( !this->row ) {
      cout << "GetFieldCount error : row is empty" << endl;
      return -1;
   }
   return mysql_num_fields(result);
}

bool ROMEMySQL::DataSeek(my_ulonglong offset) {
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
      cout << "NextRow error :" << GetErrorMessage() << endl;
      return false;
   }
   return true;
}

char* ROMEMySQL::GetField(int fieldNumber) {
   if( !row ) {
      cout << "GetField error :" << GetErrorMessage() << endl;
      return NULL;
   }
   if( fieldNumber < 0 || fieldNumber >= GetNumberOfFields() ) {
      cout << "GetField error : field number out of bounds" << endl;
      return NULL;
   }
   return this->row[fieldNumber];
}

bool ROMEMySQL::NextRow() {
   if( !result ) {
      cout << "NextRow error : no query result" << endl;
      return false;
   }
   if( !(row = mysql_fetch_row(result)) ) {
      cout << "NextRow error :" << GetErrorMessage() << endl;
      return false;
   }
   return true;
}

int ROMEMySQL::GetErrorCode() {
   return mysql_errno(&mysql);
}

char* ROMEMySQL::GetErrorMessage() {
   return (char*) mysql_error(&mysql);
}
