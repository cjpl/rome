// Author: Ryu Sawada
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMESQL                                                              //
//                                                                      //
//  Provides SQL data base access.
//                                                                      //
//  $Id$
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include "ROMEiostream.h"
#include "ROMEString.h"
#include "ROMESQL.h"

ClassImp(ROMESQL)

Bool_t ROMESQL::CreateDataBase(const char* database)
{
   ROMEString sqlquery;
   sqlquery.SetFormatted("CREATE DATABASE %s;",database);
   if (!MakeQuery(sqlquery,false)) {
      ROMEPrint::Error("Could not create data base '%s'. Error : %s\n", database, GetErrorMessage());
      return false;
   }
   return true;
}

Bool_t ROMESQL::DeleteDataBase(const char* database)
{
   ROMEString sqlquery;
   sqlquery.SetFormatted("DROP DATABASE IF EXISTS %s;",database);
   if (!MakeQuery(sqlquery,false)) {
      ROMEPrint::Error("Could not delete data base '%s'. Error : %s\n", database, GetErrorMessage());
      return false;
   }
   return true;
}

Bool_t ROMESQL::CreateTable(const char* table,const char* fields)
{
   ROMEString sqlquery;
   sqlquery.SetFormatted("CREATE TABLE %s (%s);",table,fields);
   if (!MakeQuery(sqlquery,false)) {
      ROMEPrint::Error("Could not create table '%s'. Error : %s\n", table, GetErrorMessage());
      return false;
   }
   return true;
}

Bool_t ROMESQL::DeleteTable(const char* table)
{
   ROMEString sqlquery;
   sqlquery.SetFormatted("DROP TABLE IF EXISTS %s;",table);
   if (!MakeQuery(sqlquery,false)) {
      ROMEPrint::Error("Could not delete table '%s'. Error : %s\n", table, GetErrorMessage());
      return false;
   }
   return true;
}

Bool_t ROMESQL::InsertRow(const char* table,const char* fields,const char* values)
{
   ROMEString sqlquery;
   sqlquery.SetFormatted("INSERT INTO %s (%s) VALUES (%s);",table,fields,values);
   if (!MakeQuery(sqlquery,false)) {
      ROMEPrint::Error("Could not insert row into table '%s'. Error : %s\n", table, GetErrorMessage());
      return false;
   }
   return true;
}

Bool_t ROMESQL::DeleteRow(const char* table,const char* constraint)
{
   ROMEString sqlquery;
   sqlquery.SetFormatted("DELETE FROM %s WHERE %s;",table,constraint);
   if (!MakeQuery(sqlquery,false)) {
      ROMEPrint::Error("Could not delete row from table '%s'. Error : %s\n", table ,GetErrorMessage());
      return false;
   }
   return true;
}

Bool_t ROMESQL::ReplaceField(const char* table,const char* field,const char* value,const char* constraint)
{
   ROMEString sqlquery;
   sqlquery.SetFormatted("UPDATE %s SET %s=%s WHERE %s;",table,field,value,constraint);
   if (!MakeQuery(sqlquery,false)) {
      ROMEPrint::Error("Could not replace field '%s' in table '%s'. Error : %s\n", field, table, GetErrorMessage());
      return false;
   }
   return true;
}

Bool_t ROMESQL::ExistField(const char* table,const char* field)
{
   ROMEString sqlquery;
   sqlquery.SetFormatted("select %s from %s;",field,table);
   if (!MakeQuery(sqlquery,false)) {
      return false;
   }
   StoreResult();
   return true;
}

Bool_t ROMESQL::ReadField(const char* table,const char* field,const char* constraint)
{
   ROMEString sqlquery;
   sqlquery.SetFormatted("select %s from %s where %s;",field,table,constraint);
   if (!MakeQuery(sqlquery,false)) {
      ROMEPrint::Error("Field '%s' not found in table '%s'. Error : %s\n", field, table, GetErrorMessage());
      return false;
   }
   if (!StoreResult()) {
      ROMEPrint::Error("No value found in Field '%s'.\n", field);
      return false;
   }
   if (!GetNumberOfFields()) {
      ROMEPrint::Error("No value found in Field '%s'.\n", field);
      return false;
   }
   if (!NextRow()) {
      ROMEPrint::Error("No value found in Field '%s'.\n", field);
      return false;
   }
   return true;
}
