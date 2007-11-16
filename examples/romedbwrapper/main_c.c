#ifdef __cplusplus
#error "please compile with C compiler."
#endif

#include "ROMEDataBaseWrapper.h"
#include <stdio.h>

int main() {
   int int_value;
   const char* str_value;
   int irow, icol;
   void *sqldb;
   void *textdb;

   /***** SQL *****/
   /* establish connection to a database */
   sqldb = romedb_init("mysqldb", kROMEDBWrapperDBTypeSQL, "sqlite3://./RDBDataBase.sqlite3");
   if (!sqldb) {
      printf("database connection error\n");
   } else {

      /* read an integer */
      romedb_read(sqldb, "/AAA[id=1]/BBB/bbb", 0, 0);
      int_value = romedb_get_int_value(sqldb, 0, 0);
      printf("%d\n", int_value);

      /* read a string */
      romedb_read(sqldb, "/AAA[id=1]/CCC[idx=0]/ccc", 0, 0);
      str_value = romedb_get_string_value(sqldb, 0, 0);
      printf("%s\n", str_value);

      /* read array */
      romedb_read(sqldb, "/III[id=0](0,2)/JJJ_idx;III/idx(0,2)", 0, 0);
      for (irow = 0; irow < 3; irow++) {
         int_value = romedb_get_int_value(sqldb, irow, 0);
         printf("%d\n", int_value);
      }

      /* disconnect and free memory of the working space */
      romedb_disconnect(sqldb);
   }

   /***** Text *****/
   /* establish connection to a database */
   textdb = romedb_init("mytextdb", kROMEDBWrapperDBTypeText, "./");
   if (!textdb) {
      printf("database connection error\n");
   } else {

      /* read an integer */
      romedb_read(textdb, "text.db/val1", 0, 0);
      int_value = romedb_get_int_value(textdb, 0, 0);
      printf("%d\n", int_value);

      /* read a string */
      romedb_read(textdb, "text.db/val2", 0, 0);
      str_value = romedb_get_string_value(textdb, 0, 0);
      printf("%s\n", str_value);

      /* read array */
      romedb_read(textdb, "text.db/val3", 0, 0);
      for (irow = 0; irow < 10; irow++) {
         for (icol = 0; icol < 2; icol++) {
            int_value = romedb_get_int_value(textdb, irow, icol);
            printf("%d ", int_value);
         }
         printf("\n");
      }

      /* disconnect and free memory of the working space */
      romedb_disconnect(textdb);
   }

   return 0;
}
