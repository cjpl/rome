#ifndef ROMESQL_H
#define ROMESQL_H

#include <TString.h>
#if defined ( _MSC_VER )
#include <winsock.h>
#endif
#include <mysql.h>

class ROMESQL
{
protected:
   MYSQL mysql;
   MYSQL_RES *result;
   int numberOfFields;
   MYSQL_ROW row;
   int numberOfRows;
public:
   ROMESQL();
   bool Connect(char *server,char *user,char *passwd,char *database);
   bool CreateDataBase(char* database);
   bool DeleteDataBase(char* database);
   bool CreateTable(char* table,char* fields);
   bool DeleteTable(char* table);
   bool ReadField(char *table,char* field,char* constraint);
   void ReadPathFields(char *path, int start_id, char *start_id_extension,int max=1000);
   int  WritePathFields(char *path,int id,char **value,char *constraint,int number);
   bool InsertRow(char *table,char* fields,char* values);
   bool DeleteRow(char *table,char* constraint);
   bool ReplaceField(char *table,char* field,char* value,char* constraint);
   bool ExistField(char *table,char* field);
   bool MakeQuery(char* query);
   int GetNumberOfRows();
   bool NextRow();
   int GetNumberOfFields();
   char* GetField(int fieldNumber);
private:
   int DecodePath(char *path,char *start_id_extension,char **table_name,char **id_extension,int  &transition_depth,char *group_name,char *field_name);
};

#endif   // ROMESQL_H
