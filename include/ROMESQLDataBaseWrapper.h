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
   int         romesqldb_read(void *romedb, const char *dataBasePath,
                              long long int runNumber, long long int eventNumber);
   int         romesqldb_write(void *romedb, const char *dataBasePath,
                               long long int runNumber, long long int eventNumber);
   int         romesqldb_get_int_value(void *romedb, int column, int row);
   double      romesqldb_get_double_value(void *romedb, int column, int row);
   float       romesqldb_get_float_value(void *romedb, int column, int row);
   const char *romesqldb_get_string_value(void *romedb, int column, int row);
   void        romesqldb_set_int_value(void *romedb, int column, int row, int val);
   void        romesqldb_set_double_value(void *romedb, int column, int row, double val);
   void        romesqldb_set_float_value(void *romedb, int column, int row, float val);
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
   void        romesqldb_init__(char *name, char *connection, long long *ret,
                                const int name_len, const int connection_len);
   void        romesqldb_disconnect__(long long *romedb);
   void        romesqldb_read__(long long *romedb, char *dataBasePath, long long int *runNumber,
                                long long int *eventNumber, int *ret, const int dataBasePath_len);
   void        romesqldb_write__(long long *romedb, char *dataBasePath, long long int *runNumber,
                                 long long int *eventNumber, int *ret, const int dataBasePath_len);
   void        romesqldb_get_int_value__(long long *romedb, int *column, int *row, int *ret);
   void        romesqldb_get_double_value__(long long *romedb, int *column, int *row, double *ret);
   void        romesqldb_get_float_value__(long long *romedb, int *column, int *row, float *ret);
   void        romesqldb_get_string_value__(long long *romedb, int *column, int *row, char *ret, const int ret_len);
   void        romesqldb_set_int_value__(long long *romedb, int *column, int *row, int *val);
   void        romesqldb_set_double_value__(long long *romedb, int *column, int *row, double *val);
   void        romesqldb_set_float_value__(long long *romedb, int *column, int *row, float *val);
   void        romesqldb_set_string_value__(long long *romedb, int *column, int *row, char *val, const int val_len);
   void        romesqldb_get_dbms_type__(long long *romedb, char *ret, const int ret_len);
   void        romesqldb_link_error__(long long *romedb);
   void        romesqldb_get_type__(long long *romedb, char *ret, const int ret_len);
   void        romesqldb_get_description__(long long *romedb, char *ret, const int ret_len);
   void        romesqldb_make_query__(long long *romedb, char *query, int *store, int *ret, const int query_len);
   void        romesqldb_get_number_of_rows__(long long *romedb, int *ret);
   void        romesqldb_next_row__(long long *romedb, int *ret);
   void        romesqldb_get_number_of_fields__(long long *romedb, int *ret);
   void        romesqldb_get_field__(long long *romedb, int *fieldNumber, char *ret, const int ret_len);
   void        romesqldb_free_reqult__(long long *romedb);
   void        romesqldb_get_error_code__(long long *romedb, int *ret);
   void        romesqldb_get_error_message__(long long *romedb, char *ret, const int ret_len);
   void        romesqldb_start_transaction__(long long *romedb, char *option, int *ret, const int option_len);
   void        romesqldb_commit_transaction__(long long *romedb, char *option, int *ret, const int option_len);
   void        romesqldb_rollback_transaction__(long long *romedb, char *option, int *ret, const int option_len);
   void        romesqldb_print__(long long *romedb, char *opt, const int opt_len);

   void        romesqldb_init_(char *name, char *connection, long long *ret,
                               const int name_len, const int connection_len);
   void        romesqldb_disconnect_(long long *romedb);
   void        romesqldb_read_(long long *romedb, char *dataBasePath, long long int *runNumber,
                               long long int *eventNumber, int *ret, const int dataBasePath_len);
   void        romesqldb_write_(long long *romedb, char *dataBasePath, long long int *runNumber,
                                long long int *eventNumber, int *ret, const int dataBasePath_len);
   void        romesqldb_get_int_value_(long long *romedb, int *column, int *row, int *ret);
   void        romesqldb_get_double_value_(long long *romedb, int *column, int *row, double *ret);
   void        romesqldb_get_float_value_(long long *romedb, int *column, int *row, float *ret);
   void        romesqldb_get_string_value_(long long *romedb, int *column, int *row, char *ret, const int ret_len);
   void        romesqldb_set_int_value_(long long *romedb, int *column, int *row, int *val);
   void        romesqldb_set_double_value_(long long *romedb, int *column, int *row, double *val);
   void        romesqldb_set_float_value_(long long *romedb, int *column, int *row, float *val);
   void        romesqldb_set_string_value_(long long *romedb, int *column, int *row, char *val, const int val_len);
   void        romesqldb_get_dbms_type_(long long *romedb, char *ret, const int ret_len);
   void        romesqldb_link_error_(long long *romedb);
   void        romesqldb_get_type_(long long *romedb, char *ret, const int ret_len);
   void        romesqldb_get_description_(long long *romedb, char *ret, const int ret_len);
   void        romesqldb_make_query_(long long *romedb, char *query, int *store, int *ret, const int query_len);
   void        romesqldb_get_number_of_rows_(long long *romedb, int *ret);
   void        romesqldb_next_row_(long long *romedb, int *ret);
   void        romesqldb_get_number_of_fields_(long long *romedb, int *ret);
   void        romesqldb_get_field_(long long *romedb, int *fieldNumber, char *ret, const int ret_len);
   void        romesqldb_free_reqult_(long long *romedb);
   void        romesqldb_get_error_code_(long long *romedb, int *ret);
   void        romesqldb_get_error_message_(long long *romedb, char *ret, const int ret_len);
   void        romesqldb_start_transaction_(long long *romedb, char *option, int *ret, const int option_len);
   void        romesqldb_commit_transaction_(long long *romedb, char *option, int *ret, const int option_len);
   void        romesqldb_rollback_transaction_(long long *romedb, char *option, int *ret, const int option_len);
   void        romesqldb_print_(long long *romedb, char *opt, const int opt_len);

   void        _romesqldb_init_(char *name, char *connection, long long *ret,
                                const int name_len, const int connection_len);
   void        _romesqldb_disconnect_(long long *romedb);
   void        _romesqldb_read_(long long *romedb, char *dataBasePath, long long int *runNumber,
                                long long int *eventNumber, int *ret, const int dataBasePath_len);
   void        _romesqldb_write_(long long *romedb, char *dataBasePath, long long int *runNumber,
                                 long long int *eventNumber, int *ret, const int dataBasePath_len);
   void        _romesqldb_get_int_value_(long long *romedb, int *column, int *row, int *ret);
   void        _romesqldb_get_double_value_(long long *romedb, int *column, int *row, double *ret);
   void        _romesqldb_get_float_value_(long long *romedb, int *column, int *row, float *ret);
   void        _romesqldb_get_string_value_(long long *romedb, int *column, int *row, char *ret, const int ret_len);
   void        _romesqldb_set_int_value_(long long *romedb, int *column, int *row, int *val);
   void        _romesqldb_set_double_value_(long long *romedb, int *column, int *row, double *val);
   void        _romesqldb_set_float_value_(long long *romedb, int *column, int *row, float *val);
   void        _romesqldb_set_string_value_(long long *romedb, int *column, int *row, char *val, const int val_len);
   void        _romesqldb_get_dbms_type_(long long *romedb, char *ret, const int ret_len);
   void        _romesqldb_link_error_(long long *romedb);
   void        _romesqldb_get_type_(long long *romedb, char *ret, const int ret_len);
   void        _romesqldb_get_description_(long long *romedb, char *ret, const int ret_len);
   void        _romesqldb_make_query_(long long *romedb, char *query, int *store, int *ret, const int query_len);
   void        _romesqldb_get_number_of_rows_(long long *romedb, int *ret);
   void        _romesqldb_next_row_(long long *romedb, int *ret);
   void        _romesqldb_get_number_of_fields_(long long *romedb, int *ret);
   void        _romesqldb_get_field_(long long *romedb, int *fieldNumber, char *ret, const int ret_len);
   void        _romesqldb_free_reqult_(long long *romedb);
   void        _romesqldb_get_error_code_(long long *romedb, int *ret);
   void        _romesqldb_get_error_message_(long long *romedb, char *ret, const int ret_len);
   void        _romesqldb_start_transaction_(long long *romedb, char *option, int *ret, const int option_len);
   void        _romesqldb_commit_transaction_(long long *romedb, char *option, int *ret, const int option_len);
   void        _romesqldb_rollback_transaction_(long long *romedb, char *option, int *ret, const int option_len);
   void        _romesqldb_print_(long long *romedb, char *opt, const int opt_len);
#ifdef __cplusplus
}
#endif
#endif                          // ROMESQLDataBaseWrapper_H
