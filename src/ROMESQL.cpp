// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMESQL                                                             //
//                                                                      //
//  Provides SQL data base access.
//                                                                      //
//  $Log$
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
#include <Riostream.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <string.h>
#include <ROMESQL.h>

ROMESQL::ROMESQL() {
   mysql_init(&mysql);
}

bool ROMESQL::Connect(char *server,char *user,char *passwd,char *database) 
{
   if (!mysql_real_connect(&mysql, server, user, passwd,database,0,NULL,0)) {
      printf("Could not connect to the data base '%s' : Error: %s\n",database,mysql_error(&mysql));
      return false;
   }
   return true;
}

bool ROMESQL::CreateDataBase(char* database)
{
   char sqlquery[128];
   sprintf(sqlquery,"CREATE DATABASE %s;",database);
   if (mysql_query(&mysql,sqlquery)) {
      cout << "Could not create data base '"<<database<<"'. Error :" << mysql_error(&mysql) << endl;
      return false;
   }
   return true;
}

bool ROMESQL::DeleteDataBase(char* database)
{
   char sqlquery[128];
   sprintf(sqlquery,"DROP DATABASE IF EXISTS %s;",database);
   if (mysql_query(&mysql,sqlquery)) {
      cout << "Could not delete data base '"<<database<<"'. Error :" << mysql_error(&mysql) << endl;
      return false;
   }
   return true;
}

bool ROMESQL::CreateTable(char* table,char* fields)
{
   char sqlquery[128];
   sprintf(sqlquery,"CREATE TABLE %s (%s);",table,fields);
   if (mysql_query(&mysql,sqlquery)) {
      cout << "Could not create table '"<<table<<"'. Error :" << mysql_error(&mysql) << endl;
      return false;
   }
   return true;
}

bool ROMESQL::DeleteTable(char* table)
{
   char sqlquery[128];
   sprintf(sqlquery,"DROP TABLE IF EXISTS %s;",table);
   if (mysql_query(&mysql,sqlquery)) {
      cout << "Could not delete table '"<<table<<"'. Error :" << mysql_error(&mysql) << endl;
      return false;
   }
   return true;
}

bool ROMESQL::InsertRow(char *table,char* fields,char* values)
{
   char sqlquery[128000];
   sprintf(sqlquery,"INSERT INTO %s (%s) VALUES (%s);",table,fields,values);
   if (mysql_query(&mysql,sqlquery)) {
      cout << "Could not insert row into table '"<<table<<"'. Error :" << mysql_error(&mysql) << endl;
      return false;
   }
   return true;
}

bool ROMESQL::DeleteRow(char *table,char* constraint)
{
   char sqlquery[128000];
   sprintf(sqlquery,"DELETE FROM %s WHERE %s;",table,constraint);
   if (mysql_query(&mysql,sqlquery)) {
      cout << "Could not delete row from table '"<<table<<"'. Error :" << mysql_error(&mysql) << endl;
      return false;
   }
   return true;
}

bool ROMESQL::ReplaceField(char *table,char* field,char* value,char* constraint)
{
   char sqlquery[128];
   sprintf(sqlquery,"UPDATE %s SET %s=%s WHERE %s;",table,field,value,constraint);
   if (mysql_query(&mysql,sqlquery)) {
      cout << "Could not replace field '"<<field<<"' in table '"<<table<<"'. Error :" << mysql_error(&mysql) << endl;
      return false;
   }
   return true;
}

bool ROMESQL::ExistField(char *table,char* field)
{
   char sqlquery[128];
   sprintf(sqlquery,"select %s from %s;",field,table);
   if (mysql_query(&mysql,sqlquery)) {
      return false;
   }
   mysql_store_result(&mysql);
   return true;
}

bool ROMESQL::ReadField(char *table,char* field,char* constraint)
{
   char* defaultRow = "ROMESQL : invalid value";
   char sqlquery[128];
   sprintf(sqlquery,"select %s from %s where %s;",field,table,constraint);
   if (mysql_query(&mysql,sqlquery)) {
      cout << "Field '"<<field<<"' not found in table '"<<table<<"'. Error :" << mysql_error(&mysql) << endl;
      return false;
   }
   if (!(result = mysql_store_result(&mysql))) {
      cout << "No value found in Field '"<<field<<"'." << endl;
      return false;
   }
   if ((numberOfFields = mysql_num_fields(result))==0) {
      cout << "No value found in Field '"<<field<<"'." << endl;
      return false;
   }
   if (!(row = mysql_fetch_row(result))) {
      cout << "No value found in Field '"<<field<<"'." << endl;
      return false;
   }
   return true;
}

bool ROMESQL::MakeQuery(char* query)
{
   if (mysql_query(&mysql,query)) {
      cout << "Query error :" << mysql_error(&mysql) << endl;
      return false;
   }
   if (!(result = mysql_store_result(&mysql))) {
      cout << "Query error :" << mysql_error(&mysql) << endl;
      return false;
   }
   this->numberOfRows = -1;
   return true;
}

bool ROMESQL::NextRow() {
   if( !result ) {
      cout << "NextRow error : no query result" << endl;
      return false;
   }
   row = mysql_fetch_row(result);
   if( !row ) {
      cout << "NextRow error :" << mysql_error(&mysql) << endl;
      return false;
   }
   return true;
}

int ROMESQL::GetNumberOfRows() {
   if( !result ) {
      cout << "NextRow error :" << mysql_error(&mysql) << endl;
      return -1;
   }
   if( numberOfRows > -1 ) {
      return numberOfRows;
   }
   numberOfRows = (int)mysql_num_rows(result);
   return numberOfRows;
}

int ROMESQL::GetNumberOfFields() {
   if( !this->row ) {
      cout << "GetFieldCount error : row is empty" << endl;
      return -1;
   }
   return mysql_num_fields(result);
}

char* ROMESQL::GetField(int fieldNumber) {
   if( !row ) {
      cout << "GetField error :" << mysql_error(&mysql) << endl;
      return NULL;
   }
   if( fieldNumber < 0 || fieldNumber >= GetNumberOfFields() ) {
      cout << "GetField error : field number out of bounds" << endl;
      return NULL;
   }
   return this->row[fieldNumber];
}
