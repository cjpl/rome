/********************************************************************
  ROMEMySQL.h, M. Schneebeli PSI

  $Log$
  Revision 1.1  2005/04/27 10:30:44  sawada
  Added SQLite,SQLite3 support.

  Revision 1.10  2005/02/21 21:29:07  sawada

  Changed OS specifying macros
  Support for DEC,Ultrix,FreeBSD,Solaris

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
#ifndef ROMEMYSQL_H
#define ROMEMYSQL_H

#include <ROMESQL.h>
#if defined ( R__VISUAL_CPLUSPLUS )
#include <Windows4root.h>
//typedef UINT_PTR        SOCKET;
#endif
#include <mysql.h>

class ROMEMySQL : public ROMESQL
{
protected:
   MYSQL      mysql;
   MYSQL_RES* result;
   MYSQL_ROW  row;

public:
   ROMEMySQL();
   virtual ~ROMEMySQL();
   bool  Connect(const char *server,const char *user,const char *passwd,const char *database,const char *port);
   bool  DisConnect();
   bool  MakeQuery(const char* query,bool store);
   bool  StoreResult();
   bool  NextRow();
   char* GetField(int fieldNumber);
   int   GetNumberOfRows();
   int   GetNumberOfFields();
   void  FreeResult();
   int   GetErrorCode();
   char* GetErrorMessage();

   bool  DataSeek(my_ulonglong offset);
};

#endif   // ROMEMYSQL_H
