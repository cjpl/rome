// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMESQL                                                             //
//                                                                      //
//  Provides SQL data base access.
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <Riostream.h>
#include "ROMESQL.h"

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

char* ROMESQL::GetField(int fieldNumber)
{
   if (fieldNumber>=numberOfFields) return "ROMESQL : error";
   return (char*)fRows[fieldNumber].Data();
}

bool ROMESQL::NextField()
{
   MYSQL_ROW row;
   if (!(row = mysql_fetch_row(result))) {
      return false;
   }
   fRows = new TString[numberOfFields];
   for (int i=0;i<numberOfFields;i++) 
      fRows[i] = row[i];
   return true;
}

char* ROMESQL::ReadField(char *table,char* field,char* constraint)
{
   MYSQL_ROW row;
   char* defaultRow = "ROMESQL : invalid value";
   char sqlquery[128];
   sprintf(sqlquery,"select %s from %s where %s;",field,table,constraint);
   if (mysql_query(&mysql,sqlquery)) {
      cout << "Field '"<<field<<"' not found in table '"<<table<<"'. Error :" << mysql_error(&mysql) << endl;
      return "ROMESQL : error";
   }
   if (!(result = mysql_store_result(&mysql))) {
      cout << "No value found in Field '"<<field<<"'." << endl;
      return "ROMESQL : empty";
   }
   if ((numberOfFields = mysql_num_fields(result))==0) {
      cout << "No value found in Field '"<<field<<"'." << endl;
      return "ROMESQL : empty";
   }
   if (!(row = mysql_fetch_row(result))) {
      cout << "No value found in Field '"<<field<<"'." << endl;
      return "ROMESQL : empty";
   }
   fRows = new TString[numberOfFields];
   for (int i=0;i<numberOfFields;i++) {
      fRows[i] = row[i];
   }
   return row[0];
}

bool ROMESQL::MakeQuery(char* query)
{
   char sqlquery[128];
   sprintf(sqlquery,query);
   if (mysql_query(&mysql,sqlquery)) {
      cout << "Query error :" << mysql_error(&mysql) << endl;
      return false;
   }
   return true;
}

