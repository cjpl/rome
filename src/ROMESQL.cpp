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
#include <Riostream.h>
#include "ROMEString.h"
#include "ROMESQL.h"

ClassImp(ROMESQL)

Bool_t ROMESQL::CreateDataBase(const char* database)
{
   ROMEString sqlquery;
   sqlquery.SetFormatted("CREATE DATABASE %s;",database);
   if (!MakeQuery(sqlquery,false)) {
      cout << "Could not create data base '"<<database<<"'. Error :" << GetErrorMessage() << endl;
      return false;
   }
   return true;
}

Bool_t ROMESQL::DeleteDataBase(const char* database)
{
   ROMEString sqlquery;
   sqlquery.SetFormatted("DROP DATABASE IF EXISTS %s;",database);
   if (!MakeQuery(sqlquery,false)) {
      cout << "Could not delete data base '"<<database<<"'. Error :" << GetErrorMessage() << endl;
      return false;
   }
   return true;
}

Bool_t ROMESQL::CreateTable(const char* table,const char* fields)
{
   ROMEString sqlquery;
   sqlquery.SetFormatted("CREATE TABLE %s (%s);",table,fields);
   if (!MakeQuery(sqlquery,false)) {
      cout << "Could not create table '"<<table<<"'. Error :" << GetErrorMessage() << endl;
      return false;
   }
   return true;
}

Bool_t ROMESQL::DeleteTable(const char* table)
{
   ROMEString sqlquery;
   sqlquery.SetFormatted("DROP TABLE IF EXISTS %s;",table);
   if (!MakeQuery(sqlquery,false)) {
      cout << "Could not delete table '"<<table<<"'. Error :" << GetErrorMessage() << endl;
      return false;
   }
   return true;
}

Bool_t ROMESQL::InsertRow(const char* table,const char* fields,const char* values)
{
   ROMEString sqlquery;
   sqlquery.SetFormatted("INSERT INTO %s (%s) VALUES (%s);",table,fields,values);
   if (!MakeQuery(sqlquery,false)) {
      cout << "Could not insert row into table '"<<table<<"'. Error :" << GetErrorMessage() << endl;
      return false;
   }
   return true;
}

Bool_t ROMESQL::DeleteRow(const char* table,const char* constraint)
{
   ROMEString sqlquery;
   sqlquery.SetFormatted("DELETE FROM %s WHERE %s;",table,constraint);
   if (!MakeQuery(sqlquery,false)) {
      cout << "Could not delete row from table '"<<table<<"'. Error :" << GetErrorMessage() << endl;
      return false;
   }
   return true;
}

Bool_t ROMESQL::ReplaceField(const char* table,const char* field,const char* value,const char* constraint)
{
   ROMEString sqlquery;
   sqlquery.SetFormatted("UPDATE %s SET %s=%s WHERE %s;",table,field,value,constraint);
   if (!MakeQuery(sqlquery,false)) {
      cout << "Could not replace field '"<<field<<"' in table '"<<table<<"'. Error :" << GetErrorMessage() << endl;
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
      cout << "Field '"<<field<<"' not found in table '"<<table<<"'. Error :" << GetErrorMessage() << endl;
      return false;
   }
   if (!StoreResult()) {
      cout << "No value found in Field '"<<field<<"'." << endl;
      return false;
   }
   if (!GetNumberOfFields()) {
      cout << "No value found in Field '"<<field<<"'." << endl;
      return false;
   }
   if (!NextRow()) {
      cout << "No value found in Field '"<<field<<"'." << endl;
      return false;
   }
   return true;
}
