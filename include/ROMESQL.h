#ifndef ROMESQL_H
#define ROMESQL_H

#include <TString.h>
#include <winsock.h>
#include <mysql.h>

class ROMESQL
{
protected:
   MYSQL mysql;
   TString *fRows;
   MYSQL_RES *result;
   int numberOfFields;
public:
   ROMESQL();
   bool Connect(char *server,char *user,char *passwd,char *database);
   bool CreateDataBase(char* database);
   bool DeleteDataBase(char* database);
   bool CreateTable(char* table,char* fields);
   bool DeleteTable(char* table);
   char* ReadField(char *table,char* field,char* constraint);
   char* GetField(int fieldNumber);
   bool NextField();
   bool InsertRow(char *table,char* fields,char* values);
   bool DeleteRow(char *table,char* constraint);
   bool ReplaceField(char *table,char* field,char* value,char* constraint);
   bool ExistField(char *table,char* field);
   bool MakeQuery(char* query);
};

#endif   // ROMESQL_H
