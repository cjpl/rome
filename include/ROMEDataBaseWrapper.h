/********************************************************************
  ROMEDataBaseWrapper.h, R. Sawada

  $Id$

********************************************************************/
#ifndef ROMEDataBaseWrapper_H
#define ROMEDataBaseWrapper_H

#ifdef __cplusplus
extern      "C" {
#endif

   enum {
      kROMEDBWrapperDBTypeNone = 0,
      kROMEDBWrapperDBTypeSQL  = 1,
      kROMEDBWrapperDBTypeText = 2
   };

/* for C */
   void       *romedb_init(const char *name, int type, const char *connection);
   void        romedb_disconnect(void *romedb);
   int         romedb_read(void *romedb, const char *dataBasePath,
                           long long int runNumber, long long int eventNumber);
   int         romedb_write(void *romedb, const char *dataBasePath,
                            long long int runNumber, long long int eventNumber);
   int         romedb_get_int_value(void *romedb, int row, int column);
   double      romedb_get_double_value(void *romedb, int row, int column);
   float       romedb_get_float_value(void *romedb, int row, int column);
   const char *romedb_get_string_value(void *romedb, int row, int column);
   void        romedb_set_int_value(void *romedb, int row, int column, int val);
   void        romedb_set_double_value(void *romedb, int row, int column,
                                       double val);
   void        romedb_set_float_value(void *romedb, int row, int column,
                                      float val);
   void        romedb_set_string_value(void *romedb, int row, int column,
                                       const char *val);
   const char *romedb_get_type(void *romedb);
   const char *romedb_get_description(void *romedb);
   int         romedb_get_number_of_rows(void *romedb);
   int         romedb_next_row(void *romedb);
   int         romedb_get_number_of_fields(void *romedb);
   const char *romedb_get_field(void *romedb, int fieldNumber);

   void       *romesqldb_init(const char *name, const char *connection);
   void        romesqldb_disconnect(void *romedb);
   int         romesqldb_read(void *romedb, const char *dataBasePath,
                              long long int runNumber,
                              long long int eventNumber);
   int         romesqldb_write(void *romedb, const char *dataBasePath,
                               long long int runNumber,
                               long long int eventNumber);
   int         romesqldb_get_int_value(void *romedb, int row, int column);
   double      romesqldb_get_double_value(void *romedb, int row, int column);
   float       romesqldb_get_float_value(void *romedb, int row, int column);
   const char *romesqldb_get_string_value(void *romedb, int row, int column);
   void        romesqldb_set_int_value(void *romedb, int row, int column,
                                       int val);
   void        romesqldb_set_double_value(void *romedb, int row, int column,
                                          double val);
   void        romesqldb_set_float_value(void *romedb, int row, int column,
                                         float val);
   void        romesqldb_set_string_value(void *romedb, int row, int column,
                                          const char *val);
   const char *romesqldb_get_dbms_type(void *romedb);
   void        romesqldb_link_error(void *romedb);
   const char *romesqldb_get_type(void *romedb);
   const char *romesqldb_get_description(void *romedb);
   int         romesqldb_make_query(void *romedb, const char *query, int store);
   int         romesqldb_get_number_of_rows(void *romedb);
   int         romesqldb_next_row(void *romedb);
   int         romesqldb_get_number_of_fields(void *romedb);
   const char *romesqldb_get_field(void *romedb, int fieldNumber);
   void        romesqldb_free_reqult(void *romedb);
   int         romesqldb_get_error_code(void *romedb);
   const char *romesqldb_get_error_message(void *romedb);
   int         romesqldb_start_transaction(void *romedb, const char *option);
   int         romesqldb_commit_transaction(void *romedb, const char *option);
   int         romesqldb_rollback_transaction(void *romedb, const char *option);
   void        romesqldb_print(void *romedb, const char *);

   void       *rometextdb_init(const char *name, const char *connection);
   void        rometextdb_disconnect(void *romedb);
   int         rometextdb_read(void *romedb, const char *dataBasePath,
                               long long int runNumber,
                               long long int eventNumber);
   int         rometextdb_write(void *romedb, const char *dataBasePath,
                                long long int runNumber,
                                long long int eventNumber);
   int         rometextdb_get_int_value(void *romedb, int row, int column);
   double      rometextdb_get_double_value(void *romedb, int row, int column);
   float       rometextdb_get_float_value(void *romedb, int row, int column);
   const char *rometextdb_get_string_value(void *romedb, int row, int column);
   void        rometextdb_set_int_value(void *romedb, int row, int column,
                                        int val);
   void        rometextdb_set_double_value(void *romedb, int row, int column,
                                           double val);
   void        rometextdb_set_float_value(void *romedb, int row, int column,
                                          float val);
   void        rometextdb_set_string_value(void *romedb, int row, int column,
                                           const char *val);
   const char *rometextdb_get_type(void *romedb);
   const char *rometextdb_get_description(void *romedb);
   int         rometextdb_get_number_of_rows(void *romedb);
   int         rometextdb_next_row(void *romedb);
   int         rometextdb_get_number_of_fields(void *romedb);
   const char *rometextdb_get_field(void *romedb, int fieldNumber);

/* for Fortran */
   void        romedb_init__(char *name, int *type, char *connection,
                             long long *ret, const int namelen,
                             const int connection_len);
   void        romedb_disconnect__(long long *romedb);
   void        romedb_read__(long long *romedb, char *dataBasePath,
                             long long int *runNumber,
                             long long int *eventNumber, int *ret,
                             const int dataBasePath_len);
   void        romedb_write__(long long *romedb, char *dataBasePath,
                              long long int *runNumber,
                              long long int *eventNumber, int *ret,
                              const int dataBasePath_len);
   void        romedb_get_int_value__(long long *romedb, int *row, int *column,
                                      int *ret);
   void        romedb_get_double_value__(long long *romedb, int *row,
                                         int *column, double *ret);
   void        romedb_get_float_value__(long long *romedb, int *row,
                                        int *column, float *ret);
   void        romedb_get_string_value__(long long *romedb, int *row,
                                         int *column, char *ret,
                                         const int reg_len);
   void        romedb_set_int_value__(long long *romedb, int *row, int *column,
                                      int *val);
   void        romedb_set_double_value__(long long *romedb, int *row,
                                         int *column, double *val);
   void        romedb_set_float_value__(long long *romedb, int *row,
                                        int *column, float *val);
   void        romedb_set_string_value__(long long *romedb, int *row,
                                         int *column, char *val, const int val_len);
   void        romedb_get_type__(long long *romedb, char *ret,
                                 const int ret_len);
   void        romedb_get_description__(long long *romedb, char *ret,
                                        const int ret_len);
   void        romedb_get_number_of_rows__(long long *romedb, int *ret);
   void        romedb_next_row__(long long *romedb, int *ret);
   void        romedb_get_number_of_fields__(long long *romedb, int *ret);
   void        romedb_get_field__(long long *romedb, int *fieldNumber,
                                  char *ret, const int ret_len);

   void        romesqldb_init__(char *name, char *connection, long long *ret,
                                const int name_len, const int connection_len);
   void        romesqldb_disconnect__(long long *romedb);
   void        romesqldb_read__(long long *romedb, char *dataBasePath,
                                long long int *runNumber,
                                long long int *eventNumber, int *ret,
                                const int dataBasePath_len);
   void        romesqldb_write__(long long *romedb, char *dataBasePath,
                                 long long int *runNumber,
                                 long long int *eventNumber, int *ret,
                                 const int dataBasePath_len);
   void        romesqldb_get_int_value__(long long *romedb, int *row,
                                         int *column, int *ret);
   void        romesqldb_get_double_value__(long long *romedb, int *row,
                                            int *column, double *ret);
   void        romesqldb_get_float_value__(long long *romedb, int *row,
                                           int *column, float *ret);
   void        romesqldb_get_string_value__(long long *romedb, int *row,
                                            int *column, char *ret,
                                            const int ret_len);
   void        romesqldb_set_int_value__(long long *romedb, int *row,
                                         int *column, int *val);
   void        romesqldb_set_double_value__(long long *romedb, int *row,
                                            int *column, double *val);
   void        romesqldb_set_float_value__(long long *romedb, int *row,
                                           int *column, float *val);
   void        romesqldb_set_string_value__(long long *romedb, int *row,
                                            int *column, char *val,
                                            const int val_len);
   void        romesqldb_get_dbms_type__(long long *romedb, char *ret,
                                         const int ret_len);
   void        romesqldb_link_error__(long long *romedb);
   void        romesqldb_get_type__(long long *romedb, char *ret,
                                    const int ret_len);
   void        romesqldb_get_description__(long long *romedb, char *ret,
                                           const int ret_len);
   void        romesqldb_make_query__(long long *romedb, char *query,
                                      int *store, int *ret,
                                      const int query_len);
   void        romesqldb_get_number_of_rows__(long long *romedb, int *ret);
   void        romesqldb_next_row__(long long *romedb, int *ret);
   void        romesqldb_get_number_of_fields__(long long *romedb, int *ret);
   void        romesqldb_get_field__(long long *romedb, int *fieldNumber,
                                     char *ret, const int ret_len);
   void        romesqldb_free_reqult__(long long *romedb);
   void        romesqldb_get_error_code__(long long *romedb, int *ret);
   void        romesqldb_get_error_message__(long long *romedb, char *ret,
                                             const int ret_len);
   void        romesqldb_start_transaction__(long long *romedb, char *option,
                                             int *ret, const int option_len);
   void        romesqldb_commit_transaction__(long long *romedb, char *option,
                                              int *ret, const int option_len);
   void        romesqldb_rollback_transaction__(long long *romedb, char *option,
                                                int *ret, const int option_len);
   void        romesqldb_print__(long long *romedb, char *opt,
                                 const int opt_len);

   void        rometextdb_init__(char *name, char *connection, long long *ret,
                                 const int namelen, const int connection_len);
   void        rometextdb_disconnect__(long long *romedb);
   void        rometextdb_read__(long long *romedb, char *dataBasePath,
                                 long long int *runNumber,
                                 long long int *eventNumber, int *ret,
                                 const int dataBasePath_len);
   void        rometextdb_write__(long long *romedb, char *dataBasePath,
                                  long long int *runNumber,
                                  long long int *eventNumber, int *ret,
                                  const int dataBasePath_len);
   void        rometextdb_get_int_value__(long long *romedb, int *row,
                                          int *column, int *ret);
   void        rometextdb_get_double_value__(long long *romedb, int *row,
                                             int *column, double *ret);
   void        rometextdb_get_float_value__(long long *romedb, int *row,
                                            int *column, float *ret);
   void        rometextdb_get_string_value__(long long *romedb, int *row,
                                             int *column, char *ret,
                                             const int reg_len);
   void        rometextdb_set_int_value__(long long *romedb, int *row,
                                          int *column, int *val);
   void        rometextdb_set_double_value__(long long *romedb, int *row,
                                             int *column, double *val);
   void        rometextdb_set_float_value__(long long *romedb, int *row,
                                            int *column, float *val);
   void        rometextdb_set_string_value__(long long *romedb, int *row,
                                             int *column, char *val, const int val_len);
   void        rometextdb_get_type__(long long *romedb, char *ret,
                                     const int ret_len);
   void        rometextdb_get_description__(long long *romedb, char *ret,
                                            const int ret_len);
   void        rometextdb_get_number_of_rows__(long long *romedb, int *ret);
   void        rometextdb_next_row__(long long *romedb, int *ret);
   void        rometextdb_get_number_of_fields__(long long *romedb, int *ret);
   void        rometextdb_get_field__(long long *romedb, int *fieldNumber,
                                      char *ret, const int ret_len);

   void        romedb_init_(char *name, int *type, char *connection,
                            long long *ret, const int namelen,
                            const int connection_len);
   void        romedb_disconnect_(long long *romedb);
   void        romedb_read_(long long *romedb, char *dataBasePath,
                            long long int *runNumber,
                            long long int *eventNumber, int *ret,
                            const int dataBasePath_len);
   void        romedb_write_(long long *romedb, char *dataBasePath,
                             long long int *runNumber,
                             long long int *eventNumber, int *ret,
                             const int dataBasePath_len);
   void        romedb_get_int_value_(long long *romedb, int *row, int *column,
                                     int *ret);
   void        romedb_get_double_value_(long long *romedb, int *row,
                                        int *column, double *ret);
   void        romedb_get_float_value_(long long *romedb, int *row,
                                       int *column, float *ret);
   void        romedb_get_string_value_(long long *romedb, int *row,
                                        int *column, char *ret,
                                        const int reg_len);
   void        romedb_set_int_value_(long long *romedb, int *row, int *column,
                                     int *val);
   void        romedb_set_double_value_(long long *romedb, int *row,
                                        int *column, double *val);
   void        romedb_set_float_value_(long long *romedb, int *row, int *column,
                                       float *val);
   void        romedb_set_string_value_(long long *romedb, int *row,
                                        int *column, char *val);
   void        romedb_get_type_(long long *romedb, char *ret,
                                const int ret_len);
   void        romedb_get_description_(long long *romedb, char *ret,
                                       const int ret_len);
   void        romedb_get_number_of_rows_(long long *romedb, int *ret);
   void        romedb_next_row_(long long *romedb, int *ret);
   void        romedb_get_number_of_fields_(long long *romedb, int *ret);
   void        romedb_get_field_(long long *romedb, int *fieldNumber, char *ret,
                                 const int ret_len);

   void        romesqldb_init_(char *name, char *connection, long long *ret,
                               const int name_len, const int connection_len);
   void        romesqldb_disconnect_(long long *romedb);
   void        romesqldb_read_(long long *romedb, char *dataBasePath,
                               long long int *runNumber,
                               long long int *eventNumber, int *ret,
                               const int dataBasePath_len);
   void        romesqldb_write_(long long *romedb, char *dataBasePath,
                                long long int *runNumber,
                                long long int *eventNumber, int *ret,
                                const int dataBasePath_len);
   void        romesqldb_get_int_value_(long long *romedb, int *row,
                                        int *column, int *ret);
   void        romesqldb_get_double_value_(long long *romedb, int *row,
                                           int *column, double *ret);
   void        romesqldb_get_float_value_(long long *romedb, int *row,
                                          int *column, float *ret);
   void        romesqldb_get_string_value_(long long *romedb, int *row,
                                           int *column, char *ret,
                                           const int ret_len);
   void        romesqldb_set_int_value_(long long *romedb, int *row,
                                        int *column, int *val);
   void        romesqldb_set_double_value_(long long *romedb, int *row,
                                           int *column, double *val);
   void        romesqldb_set_float_value_(long long *romedb, int *row,
                                          int *column, float *val);
   void        romesqldb_set_string_value_(long long *romedb, int *row,
                                           int *column, char *val,
                                           const int val_len);
   void        romesqldb_get_dbms_type_(long long *romedb, char *ret,
                                        const int ret_len);
   void        romesqldb_link_error_(long long *romedb);
   void        romesqldb_get_type_(long long *romedb, char *ret,
                                   const int ret_len);
   void        romesqldb_get_description_(long long *romedb, char *ret,
                                          const int ret_len);
   void        romesqldb_make_query_(long long *romedb, char *query, int *store,
                                     int *ret, const int query_len);
   void        romesqldb_get_number_of_rows_(long long *romedb, int *ret);
   void        romesqldb_next_row_(long long *romedb, int *ret);
   void        romesqldb_get_number_of_fields_(long long *romedb, int *ret);
   void        romesqldb_get_field_(long long *romedb, int *fieldNumber,
                                    char *ret, const int ret_len);
   void        romesqldb_free_reqult_(long long *romedb);
   void        romesqldb_get_error_code_(long long *romedb, int *ret);
   void        romesqldb_get_error_message_(long long *romedb, char *ret,
                                            const int ret_len);
   void        romesqldb_start_transaction_(long long *romedb, char *option,
                                            int *ret, const int option_len);
   void        romesqldb_commit_transaction_(long long *romedb, char *option,
                                             int *ret, const int option_len);
   void        romesqldb_rollback_transaction_(long long *romedb, char *option,
                                               int *ret, const int option_len);
   void        romesqldb_print_(long long *romedb, char *opt,
                                const int opt_len);

   void        rometextdb_init_(char *name, char *connection, long long *ret,
                                const int namelen, const int connection_len);
   void        rometextdb_disconnect_(long long *romedb);
   void        rometextdb_read_(long long *romedb, char *dataBasePath,
                                long long int *runNumber,
                                long long int *eventNumber, int *ret,
                                const int dataBasePath_len);
   void        rometextdb_write_(long long *romedb, char *dataBasePath,
                                 long long int *runNumber,
                                 long long int *eventNumber, int *ret,
                                 const int dataBasePath_len);
   void        rometextdb_get_int_value_(long long *romedb, int *row,
                                         int *column, int *ret);
   void        rometextdb_get_double_value_(long long *romedb, int *row,
                                            int *column, double *ret);
   void        rometextdb_get_float_value_(long long *romedb, int *row,
                                           int *column, float *ret);
   void        rometextdb_get_string_value_(long long *romedb, int *row,
                                            int *column, char *ret,
                                            const int reg_len);
   void        rometextdb_set_int_value_(long long *romedb, int *row,
                                         int *column, int *val);
   void        rometextdb_set_double_value_(long long *romedb, int *row,
                                            int *column, double *val);
   void        rometextdb_set_float_value_(long long *romedb, int *row,
                                           int *column, float *val);
   void        rometextdb_set_string_value_(long long *romedb, int *row,
                                            int *column, char *val);
   void        rometextdb_get_type_(long long *romedb, char *ret,
                                    const int ret_len);
   void        rometextdb_get_description_(long long *romedb, char *ret,
                                           const int ret_len);
   void        rometextdb_get_number_of_rows_(long long *romedb, int *ret);
   void        rometextdb_next_row_(long long *romedb, int *ret);
   void        rometextdb_get_number_of_fields_(long long *romedb, int *ret);
   void        rometextdb_get_field_(long long *romedb, int *fieldNumber,
                                     char *ret, const int ret_len);

   void        _romedb_init_(char *name, int *type, char *connection,
                             long long *ret, const int namelen,
                             const int connection_len);
   void        _romedb_disconnect_(long long *romedb);
   void        _romedb_read_(long long *romedb, char *dataBasePath,
                             long long int *runNumber,
                             long long int *eventNumber, int *ret,
                             const int dataBasePath_len);
   void        _romedb_write_(long long *romedb, char *dataBasePath,
                              long long int *runNumber,
                              long long int *eventNumber, int *ret,
                              const int dataBasePath_len);
   void        _romedb_get_int_value_(long long *romedb, int *row, int *column,
                                      int *ret);
   void        _romedb_get_double_value_(long long *romedb, int *row,
                                         int *column, double *ret);
   void        _romedb_get_float_value_(long long *romedb, int *row,
                                        int *column, float *ret);
   void        _romedb_get_string_value_(long long *romedb, int *row,
                                         int *column, char *ret,
                                         const int reg_len);
   void        _romedb_set_int_value_(long long *romedb, int *row, int *column,
                                      int *val);
   void        _romedb_set_double_value_(long long *romedb, int *row,
                                         int *column, double *val);
   void        _romedb_set_float_value_(long long *romedb, int *row,
                                        int *column, float *val);
   void        _romedb_set_string_value_(long long *romedb, int *row,
                                         int *column, char *val);
   void        _romedb_get_type_(long long *romedb, char *ret,
                                 const int ret_len);
   void        _romedb_get_description_(long long *romedb, char *ret,
                                        const int ret_len);
   void        _romedb_get_number_of_rows_(long long *romedb, int *ret);
   void        _romedb_next_row_(long long *romedb, int *ret);
   void        _romedb_get_number_of_fields_(long long *romedb, int *ret);
   void        _romedb_get_field_(long long *romedb, int *fieldNumber,
                                  char *ret, const int ret_len);

   void        _romesqldb_init_(char *name, char *connection, long long *ret,
                                const int name_len, const int connection_len);
   void        _romesqldb_disconnect_(long long *romedb);
   void        _romesqldb_read_(long long *romedb, char *dataBasePath,
                                long long int *runNumber,
                                long long int *eventNumber, int *ret,
                                const int dataBasePath_len);
   void        _romesqldb_write_(long long *romedb, char *dataBasePath,
                                 long long int *runNumber,
                                 long long int *eventNumber, int *ret,
                                 const int dataBasePath_len);
   void        _romesqldb_get_int_value_(long long *romedb, int *row,
                                         int *column, int *ret);
   void        _romesqldb_get_double_value_(long long *romedb, int *row,
                                            int *column, double *ret);
   void        _romesqldb_get_float_value_(long long *romedb, int *row,
                                           int *column, float *ret);
   void        _romesqldb_get_string_value_(long long *romedb, int *row,
                                            int *column, char *ret,
                                            const int ret_len);
   void        _romesqldb_set_int_value_(long long *romedb, int *row,
                                         int *column, int *val);
   void        _romesqldb_set_double_value_(long long *romedb, int *row,
                                            int *column, double *val);
   void        _romesqldb_set_float_value_(long long *romedb, int *row,
                                           int *column, float *val);
   void        _romesqldb_set_string_value_(long long *romedb, int *row,
                                            int *column, char *val,
                                            const int val_len);
   void        _romesqldb_get_dbms_type_(long long *romedb, char *ret,
                                         const int ret_len);
   void        _romesqldb_link_error_(long long *romedb);
   void        _romesqldb_get_type_(long long *romedb, char *ret,
                                    const int ret_len);
   void        _romesqldb_get_description_(long long *romedb, char *ret,
                                           const int ret_len);
   void        _romesqldb_make_query_(long long *romedb, char *query,
                                      int *store, int *ret,
                                      const int query_len);
   void        _romesqldb_get_number_of_rows_(long long *romedb, int *ret);
   void        _romesqldb_next_row_(long long *romedb, int *ret);
   void        _romesqldb_get_number_of_fields_(long long *romedb, int *ret);
   void        _romesqldb_get_field_(long long *romedb, int *fieldNumber,
                                     char *ret, const int ret_len);
   void        _romesqldb_free_reqult_(long long *romedb);
   void        _romesqldb_get_error_code_(long long *romedb, int *ret);
   void        _romesqldb_get_error_message_(long long *romedb, char *ret,
                                             const int ret_len);
   void        _romesqldb_start_transaction_(long long *romedb, char *option,
                                             int *ret, const int option_len);
   void        _romesqldb_commit_transaction_(long long *romedb, char *option,
                                              int *ret, const int option_len);
   void        _romesqldb_rollback_transaction_(long long *romedb, char *option,
                                                int *ret, const int option_len);
   void        _romesqldb_print_(long long *romedb, char *opt,
                                 const int opt_len);

   void        _rometextdb_init_(char *name, char *connection, long long *ret,
                                 const int namelen, const int connection_len);
   void        _rometextdb_disconnect_(long long *romedb);
   void        _rometextdb_read_(long long *romedb, char *dataBasePath,
                                 long long int *runNumber,
                                 long long int *eventNumber, int *ret,
                                 const int dataBasePath_len);
   void        _rometextdb_write_(long long *romedb, char *dataBasePath,
                                  long long int *runNumber,
                                  long long int *eventNumber, int *ret,
                                  const int dataBasePath_len);
   void        _rometextdb_get_int_value_(long long *romedb, int *row,
                                          int *column, int *ret);
   void        _rometextdb_get_double_value_(long long *romedb, int *row,
                                             int *column, double *ret);
   void        _rometextdb_get_float_value_(long long *romedb, int *row,
                                            int *column, float *ret);
   void        _rometextdb_get_string_value_(long long *romedb, int *row,
                                             int *column, char *ret,
                                             const int reg_len);
   void        _rometextdb_set_int_value_(long long *romedb, int *row,
                                          int *column, int *val);
   void        _rometextdb_set_double_value_(long long *romedb, int *row,
                                             int *column, double *val);
   void        _rometextdb_set_float_value_(long long *romedb, int *row,
                                            int *column, float *val);
   void        _rometextdb_set_string_value_(long long *romedb, int *row,
                                             int *column, char *val);
   void        _rometextdb_get_type_(long long *romedb, char *ret,
                                     const int ret_len);
   void        _rometextdb_get_description_(long long *romedb, char *ret,
                                            const int ret_len);
   void        _rometextdb_get_number_of_rows_(long long *romedb, int *ret);
   void        _rometextdb_next_row_(long long *romedb, int *ret);
   void        _rometextdb_get_number_of_fields_(long long *romedb, int *ret);
   void        _rometextdb_get_field_(long long *romedb, int *fieldNumber,
                                      char *ret, const int ret_len);
#ifdef __cplusplus
}
#endif
#endif                          // ROMEDataBaseWrapper_H
