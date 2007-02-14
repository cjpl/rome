/********************************************************************
  ROMESQLDataBaseWrapper.h, R. Sawada

  $Id$

********************************************************************/
#ifndef ROMESQLDataBaseWrapper_H
#define ROMESQLDataBaseWrapper_H

#ifdef __cplusplus
extern "C" {
#endif

/* for C */
   void       *romesqldb_init(const char *name, const char *connection);
   void        romesqldb_disconnect(void *romedb);
   int         romesqldb_read(void *romedb, const char *dataBasePath, long long int runNumber, long long int eventNumber);
   int         romesqldb_write(void *romedb, const char *dataBasePath, long long int runNumber, long long int eventNumber);

   int         romesqldb_get_int_value(void *romedb, int column, int row);
   double      romesqldb_get_double_value(void *romedb, int column, int row);
   const char *romesqldb_get_string_value(void *romedb, int column, int row);

   void        romesqldb_set_int_value(void *romedb, int column, int row, int val);
   void        romesqldb_set_double_value(void *romedb, int column, int row, double val);
   void        romesqldb_set_string_value(void *romedb, int column, int row, const char *val);

   char       *romesqldb_get_dbms_type(void *romedb);
   void        romesqldb_link_error(void *romedb);
   const char *romesqldb_get_type(void *romedb);
   const char *romesqldb_get_description(void *romedb);
   int         romesqldb_make_query(void *romedb, const char *query, int store);
   int         romesqldb_get_number_of_rows(void *romedb);
   int         romesqldb_next_row(void *romedb);
   int         romesqldb_get_number_of_fields(void *romedb);
   char       *romesqldb_get_field(void *romedb, int fieldNumber);
   void        romesqldb_free_reqult(void *romedb);
   int         romesqldb_get_error_code(void *romedb);
   char       *romesqldb_get_error_message(void *romedb);
   int         romesqldb_start_transaction(void *romedb, const char *option);
   int         romesqldb_commit_transaction(void *romedb, const char *option);
   int         romesqldb_rollback_transaction(void *romedb, const char *option);
   void        romesqldb_print(void *romedb, const char *);

/* for Fortran */
   void        romesqldb_init__(char *name, char *connection, int *ret);
   void        romesqldb_disconnect__(int *romedb);
   void        romesqldb_read__(int *romedb, char *dataBasePath, long long int *runNumber,
                                long long int *eventNumber, int *ret);
   void        romesqldb_write__(int *romedb, char *dataBasePath, long long int *runNumber,
                                 long long int *eventNumber, int *ret);

   void        romesqldb_get_int_value__(int *romedb, int *column, int *row, int *ret);
   void        romesqldb_get_float_value__(int *romedb, int *column, int *row, float *ret);
   void        romesqldb_get_double_value__(int *romedb, int *column, int *row, double *ret);
   void        romesqldb_get_string_value__(int *romedb, int *column, int *row, char *ret, const int ret_len);

   void        romesqldb_set_int_value__(int *romedb, int *column, int *row, int *val);
   void        romesqldb_set_double_value__(int *romedb, int *column, int *row, double *val);
   void        romesqldb_set_string_value__(int *romedb, int *column, int *row, char *val);

   void        romesqldb_get_dbms_type__(int *romedb, char *ret, const int ret_len);
   void        romesqldb_link_error__(int *romedb);
   void        romesqldb_get_type__(int *romedb, char *ret, const int ret_len);
   void        romesqldb_get_description__(int *romedb, char *ret, const int ret_len);
   void        romesqldb_make_query__(int *romedb, char *query, int *store, int *ret);
   void        romesqldb_get_number_of_rows__(int *romedb, int *ret);
   void        romesqldb_next_row__(int *romedb, int *ret);
   void        romesqldb_get_number_of_fields__(int *romedb, int *ret);
   void        romesqldb_get_field__(int *romedb, int *fieldNumber, char *ret, const int ret_len);
   void        romesqldb_free_reqult__(int *romedb);
   void        romesqldb_get_error_code__(int *romedb, int *ret);
   void        romesqldb_get_error_message__(int *romedb, char *ret, const int ret_len);
   void        romesqldb_start_transaction__(int *romedb, char *option, int *ret);
   void        romesqldb_commit_transaction__(int *romedb, char *option, int *ret);
   void        romesqldb_rollback_transaction__(int *romedb, char *option, int *ret);
   void        romesqldb_print__(int *romedb, char *opt);

#ifdef __cplusplus
}
#endif
#endif                          // ROMESQLDataBaseWrapper_H
