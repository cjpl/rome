/********************************************************************
  ROMESQL.h, M. Schneebeli PSI

  $Log$
  Revision 1.9  2004/11/19 16:26:24  sawada
  speed up with reading order array at once.

  Revision 1.8  2004/11/18 15:23:23  sawada

  Modify handling the order of array.
  Enable inverse order.
  Enable to send sql query from user tasks.

  Revision 1.7  2004/11/16 12:11:06  sawada
  SQL Init,Read

  Revision 1.6  2004/10/05 07:52:44  schneebeli_m
  dyn. Folders, TRef Objects, XML format changed, ROMEStatic removed

  Revision 1.5  2004/09/30 13:08:21  schneebeli_m
  ...

  Revision 1.4  2004/09/25 01:34:48  schneebeli_m
  implemented FW dependent EventLoop and DataBase classes

********************************************************************/
#ifndef ROMESQL_H
#define ROMESQL_H

#include <TString.h>
#if defined ( _MSC_VER )
#include <Windows4root.h>
//typedef UINT_PTR        SOCKET;
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
   ~ROMESQL();
   bool Connect(const char *server,const char *user,const char *passwd,const char *database,const char *port);
   bool DisConnect();
   bool CreateDataBase(char* database);
   bool DeleteDataBase(char* database);
   bool CreateTable(char* table,char* fields);
   bool DeleteTable(char* table);
   bool ReadField(char *table,char* field,char* constraint);
   bool InsertRow(char *table,char* fields,char* values);
   bool DeleteRow(char *table,char* constraint);
   bool ReplaceField(char *table,char* field,char* value,char* constraint);
   bool ExistField(char *table,char* field);
   bool MakeQuery(char* query,bool store);
   int  GetNumberOfRows();
   bool NextRow();
   bool DataSeek(my_ulonglong offset);
   int  GetNumberOfFields();
   char* GetField(int fieldNumber);
   void FreeResult(){mysql_free_result(result);}
};

#endif   // ROMESQL_H
