#ifdef __cplusplus
#error "please compile with C compiler."
#endif

#include "ROMESQLDataBaseWrapper.h"
#include <stdio.h>

int main() {
   int int_value;
   const char* str_value;

   /* establish connection to a database */
   void *romedb = romesqldb_init("mydb", "sqlite3://./RDBDataBase.sqlite3");
   if (!romedb) {
      printf("database connection error\n");
   } else {

      /* read an integer */
      romesqldb_read(romedb, "/AAA[id=1]/BBB/bbb", 0, 0);
      int_value = romesqldb_get_int_value(romedb, 0, 0);
      printf("%d\n", int_value);

      /* read a string */
      romesqldb_read(romedb, "/AAA[id=1]/CCC[idx=0]/ccc", 0, 0);
      str_value = romesqldb_get_string_value(romedb, 0, 0);
      printf("%s\n", str_value);

      // disconnect and free memory of the working space
      romesqldb_disconnect(romedb);
   }

   return 0;
}
