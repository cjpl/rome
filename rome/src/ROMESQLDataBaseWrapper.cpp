// Author: Ryu Sawada
//////////////////////////////////////////////////////////////////////////
//
//  ROMESQLDataBaseWrapper
//
//  SQLDataBase access.
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////

#include "ROMESQLDataBaseWrapper.h"
#include "ROMESQLDataBase.h"
#include "ROMEStr2DArray.h"
#include <string.h>

class ROMESQLDataBaseWorkSpace {
public:
   ROMESQLDataBase *db;
   ROMEStr2DArray *values;

public:
   ROMESQLDataBaseWorkSpace() {
      db = 0;
      values = 0;
   } ~ROMESQLDataBaseWorkSpace() {
      SafeDelete(db);
      SafeDelete(values);
   }
};

// For C
void *romesqldb_init(const char *name, const char *connection)
{
   ROMESQLDataBaseWorkSpace *work = new ROMESQLDataBaseWorkSpace();
   work->db = new ROMESQLDataBase();
   work->values = new ROMEStr2DArray(1, 1);
   if (!work->db->Init(name, 0, connection)) {
      return 0;
   }
   return work;
}

void romesqldb_disconnect(void *romedb)
{
   ROMESQLDataBaseWorkSpace *work = static_cast<ROMESQLDataBaseWorkSpace*>(romedb);
   SafeDelete(work);
}

int romesqldb_read(void *romedb, const char *dataBasePath, long long int runNumber, long long int eventNumber)
{
   ROMESQLDataBaseWorkSpace *work = static_cast<ROMESQLDataBaseWorkSpace*>(romedb);
   if (work == 0) {
      return 0;
   }
   return work->db->Read(work->values, dataBasePath, runNumber, eventNumber);
}

int romesqldb_write(void *romedb, const char *dataBasePath, long long int runNumber, long long int eventNumber)
{
   ROMESQLDataBaseWorkSpace *work = static_cast<ROMESQLDataBaseWorkSpace*>(romedb);
   if (work == 0) {
      return 0;
   }
   return work->db->Write(work->values, dataBasePath, runNumber, eventNumber);
}

int romesqldb_get_int_value(void *romedb, int column, int row)
{
   ROMESQLDataBaseWorkSpace *work = static_cast<ROMESQLDataBaseWorkSpace*>(romedb);
   if (work == 0) {
      return 0;
   }
   char *cstop;
   return strtol(work->values->At(column, row).Data(), &cstop, 10);
}

double romesqldb_get_double_value(void *romedb, int column, int row)
{
   ROMESQLDataBaseWorkSpace *work = static_cast<ROMESQLDataBaseWorkSpace*>(romedb);
   if (work == 0) {
      return 0;
   }
   char *cstop;
   return strtod(work->values->At(column, row).Data(), &cstop);
}

float romesqldb_get_float_value(void *romedb, int column, int row)
{
   ROMESQLDataBaseWorkSpace *work = static_cast<ROMESQLDataBaseWorkSpace*>(romedb);
   if (work == 0) {
      return 0;
   }
   char *cstop;
   return strtof(work->values->At(column, row).Data(), &cstop);
}

const char *romesqldb_get_string_value(void *romedb, int column, int row)
{
   ROMESQLDataBaseWorkSpace *work = static_cast<ROMESQLDataBaseWorkSpace*>(romedb);
   if (work == 0) {
      return 0;
   }
   return work->values->At(column, row).Data();
}

void romesqldb_set_int_value(void *romedb, int column, int row, int val)
{
   ROMESQLDataBaseWorkSpace *work = static_cast<ROMESQLDataBaseWorkSpace*>(romedb);
   if (work == 0) {
      return;
   }
   ROMEString str;
   str.SetFormatted("%d", val);
   work->values->SetAt(str, column, row);
}

void romesqldb_set_double_value(void *romedb, int column, int row, double val)
{
   ROMESQLDataBaseWorkSpace *work = static_cast<ROMESQLDataBaseWorkSpace*>(romedb);
   if (work == 0) {
      return;
   }
   ROMEString str;
   str.SetFormatted("%g", val);
   work->values->SetAt(str, column, row);
}

void romesqldb_set_float_value(void *romedb, int column, int row, float val)
{
   ROMESQLDataBaseWorkSpace *work = static_cast<ROMESQLDataBaseWorkSpace*>(romedb);
   if (work == 0) {
      return;
   }
   ROMEString str;
   str.SetFormatted("%g", val);
   work->values->SetAt(str, column, row);
}

void romesqldb_set_string_value(void *romedb, int column, int row, const char *val)
{
   ROMESQLDataBaseWorkSpace *work = static_cast<ROMESQLDataBaseWorkSpace*>(romedb);
   if (work == 0) {
      return;
   }
   ROMEString str = val;
   work->values->SetAt(str, column, row);
}

char *romesqldb_get_dbms_type(void *romedb)
{
   ROMESQLDataBaseWorkSpace *work = static_cast<ROMESQLDataBaseWorkSpace*>(romedb);
   if (!work) {
      return 0;
   }
   return work->db->GetDBMSType();
}

void romesqldb_link_error(void *romedb)
{
   ROMESQLDataBaseWorkSpace *work = static_cast<ROMESQLDataBaseWorkSpace*>(romedb);
   if (!work) {
      return;
   }
   work->db->LinkError();
}

const char *romesqldb_get_type(void *romedb)
{
   ROMESQLDataBaseWorkSpace *work = static_cast<ROMESQLDataBaseWorkSpace*>(romedb);
   if (!work) {
      return 0;
   }
   return work->db->GetType();
}

const char *romesqldb_get_description(void *romedb)
{
   ROMESQLDataBaseWorkSpace *work = static_cast<ROMESQLDataBaseWorkSpace*>(romedb);
   if (!work) {
      return 0;
   }
   return work->db->GetDescription();
}

int romesqldb_make_query(void *romedb, const char *query, int store)
{
   ROMESQLDataBaseWorkSpace *work = static_cast<ROMESQLDataBaseWorkSpace*>(romedb);
   if (!work) {
      return 0;
   }
   return work->db->MakeQuery(query, store);
}

int romesqldb_get_number_of_rows(void *romedb)
{
   ROMESQLDataBaseWorkSpace *work = static_cast<ROMESQLDataBaseWorkSpace*>(romedb);
   if (!work) {
      return 0;
   }
   return work->db->GetNumberOfRows();
}

int romesqldb_next_row(void *romedb)
{
   ROMESQLDataBaseWorkSpace *work = static_cast<ROMESQLDataBaseWorkSpace*>(romedb);
   if (!work) {
      return 0;
   }
   return work->db->NextRow();
}

int romesqldb_get_number_of_fields(void *romedb)
{
   ROMESQLDataBaseWorkSpace *work = static_cast<ROMESQLDataBaseWorkSpace*>(romedb);
   if (!work) {
      return 0;
   }
   return work->db->GetNumberOfFields();
}

char *romesqldb_get_field(void *romedb, int fieldNumber)
{
   ROMESQLDataBaseWorkSpace *work = static_cast<ROMESQLDataBaseWorkSpace*>(romedb);
   if (!work) {
      return 0;
   }
   return work->db->GetField(fieldNumber);
}

void romesqldb_free_reqult(void *romedb)
{
   ROMESQLDataBaseWorkSpace *work = static_cast<ROMESQLDataBaseWorkSpace*>(romedb);
   if (!work) {
      return;
   }
   work->db->FreeResult();
}

int romesqldb_get_error_code(void *romedb)
{
   ROMESQLDataBaseWorkSpace *work = static_cast<ROMESQLDataBaseWorkSpace*>(romedb);
   if (!work) {
      return 0;
   }
   return work->db->GetErrorCode();
}

char *romesqldb_get_error_message(void *romedb)
{
   ROMESQLDataBaseWorkSpace *work = static_cast<ROMESQLDataBaseWorkSpace*>(romedb);
   if (!work) {
      return 0;
   }
   return work->db->GetErrorMessage();
}

int romesqldb_start_transaction(void *romedb, const char *option)
{
   ROMESQLDataBaseWorkSpace *work = static_cast<ROMESQLDataBaseWorkSpace*>(romedb);
   if (!work) {
      return 0;
   }
   return work->db->StartTransaction(option);
}

int romesqldb_commit_transaction(void *romedb, const char *option)
{
   ROMESQLDataBaseWorkSpace *work = static_cast<ROMESQLDataBaseWorkSpace*>(romedb);
   if (!work) {
      return 0;
   }
   return work->db->CommitTransaction(option);
}

int romesqldb_rollback_transaction(void *romedb, const char *option)
{
   ROMESQLDataBaseWorkSpace *work = static_cast<ROMESQLDataBaseWorkSpace*>(romedb);
   if (!work) {
      return 0;
   }
   return work->db->RollbackTransaction(option);
}

void romesqldb_print(void *romedb, const char *option)
{
   ROMESQLDataBaseWorkSpace *work = static_cast<ROMESQLDataBaseWorkSpace*>(romedb);
   if (!work) {
      return;
   }
   work->db->Print(option);
}


/* for Fortran */
#ifdef __cplusplus
extern "C" {
#endif

static void cstr2fstr(char *ret, const int ret_len)
{
   int len = strlen(ret);
   memset(ret + len, 0, ret_len - len);
}

static void fstr2cstr(char *ret, const int ret_len)
{
   int i;

   if (ret[ret_len] != ' ') {
      return;
   }
   for (i = ret_len - 2; ret[i] == ' ' && i >= 0; i--) {;}
   ret[i + 1] = '\0';
}

void romesqldb_init__(char *name, char *connection, long long *ret, const int name_len, const int connection_len)
{
   char *name_tmp = new char[name_len + 1];
   memcpy(name_tmp, name, name_len);
   name_tmp[name_len] = '\0';
   fstr2cstr(name_tmp, name_len);

   char *connection_tmp = new char[connection_len + 1];
   memcpy(connection_tmp, connection, connection_len);
   connection_tmp[connection_len] = '\0';
   fstr2cstr(connection_tmp, connection_len);

   *ret = reinterpret_cast<long long>(romesqldb_init(name_tmp, connection_tmp));

   delete [] name_tmp;
   delete [] connection_tmp;
}

void romesqldb_disconnect__(long long *romedb)
{
   romesqldb_disconnect(reinterpret_cast<void*>(*romedb));
}

void romesqldb_read__(long long *romedb, char *dataBasePath, long long int *runNumber,
                      long long int *eventNumber, int *ret, const int dataBasePath_len)
{
   char *dataBasePath_tmp = new char[dataBasePath_len + 1];
   memcpy(dataBasePath_tmp, dataBasePath, dataBasePath_len);
   dataBasePath_tmp[dataBasePath_len] = '\0';
   fstr2cstr(dataBasePath_tmp, dataBasePath_len);

   *ret = romesqldb_read(reinterpret_cast<void*>(*romedb), dataBasePath_tmp, *runNumber, *eventNumber);

   delete [] dataBasePath_tmp;
}

void romesqldb_write__(long long *romedb, char *dataBasePath, long long int *runNumber,
                       long long int *eventNumber, int *ret, const int dataBasePath_len)
{
   char *dataBasePath_tmp = new char[dataBasePath_len + 1];
   memcpy(dataBasePath_tmp, dataBasePath, dataBasePath_len);
   dataBasePath_tmp[dataBasePath_len] = '\0';
   fstr2cstr(dataBasePath_tmp, dataBasePath_len);

   *ret = romesqldb_write(reinterpret_cast<void*>(*romedb), dataBasePath_tmp, *runNumber, *eventNumber);

   delete [] dataBasePath_tmp;
}

void romesqldb_get_int_value__(long long *romedb, int *column, int *row, int *ret)
{
   *ret = romesqldb_get_int_value(reinterpret_cast<void*>(*romedb), *column, *row);
}

void romesqldb_get_double_value__(long long *romedb, int *column, int *row, double *ret)
{
   *ret = romesqldb_get_double_value(reinterpret_cast<void*>(*romedb), *column, *row);
}

void romesqldb_get_float_value__(long long *romedb, int *column, int *row, float *ret)
{
   *ret = romesqldb_get_float_value(reinterpret_cast<void*>(*romedb), *column, *row);
}

void romesqldb_get_string_value__(long long *romedb, int *column, int *row, char *ret, const int ret_len)
{
   const char *p = romesqldb_get_string_value(reinterpret_cast<void*>(*romedb), *column, *row);
   if (strlen(p) <= static_cast<unsigned int>(ret_len)) {
      strcpy(ret, p);
      cstr2fstr(ret, ret_len);
   }
}

void romesqldb_set_int_value__(long long *romedb, int *column, int *row, int *val)
{
   romesqldb_set_int_value(reinterpret_cast<void*>(*romedb), *column, *row, *val);
}

void romesqldb_set_double_value__(long long *romedb, int *column, int *row, double *val)
{
   romesqldb_set_double_value(reinterpret_cast<void*>(*romedb), *column, *row, *val);
}

void romesqldb_set_float_value__(long long *romedb, int *column, int *row, float *val)
{
   romesqldb_set_float_value(reinterpret_cast<void*>(*romedb), *column, *row, *val);
}

void romesqldb_set_string_value__(long long *romedb, int *column, int *row, char *val, const int val_len)
{
   char *val_tmp = new char[val_len + 1];
   memcpy(val_tmp, val, val_len);
   val_tmp[val_len] = '\0';
   fstr2cstr(val_tmp, val_len);

   romesqldb_set_string_value(reinterpret_cast<void*>(*romedb), *column, *row, val_tmp);

   delete [] val_tmp;
}

void romesqldb_get_dbms_type__(long long *romedb, char *ret, const int ret_len)
{
   const char *p = romesqldb_get_dbms_type(reinterpret_cast<void*>(*romedb));
   if (strlen(p) <= static_cast<unsigned int>(ret_len)) {
      strcpy(ret, p);
      cstr2fstr(ret, ret_len);
   }
}

void romesqldb_link_error__(long long *romedb)
{
   romesqldb_link_error(reinterpret_cast<void*>(*romedb));
}

void romesqldb_get_type__(long long *romedb, char *ret, const int ret_len)
{
   const char *p = romesqldb_get_type(reinterpret_cast<void*>(*romedb));
   if (strlen(p) <= static_cast<unsigned int>(ret_len)) {
      strcpy(ret, p);
      cstr2fstr(ret, ret_len);
   }
}

void romesqldb_get_description__(long long *romedb, char *ret, const int ret_len)
{
   const char *p = romesqldb_get_description(reinterpret_cast<void*>(*romedb));
   if (strlen(p) <= static_cast<unsigned int>(ret_len)) {
      strcpy(ret, p);
      cstr2fstr(ret, ret_len);
   }
}

void romesqldb_make_query__(long long *romedb, char *query, int *store, int *ret, const int query_len)
{
   char *query_tmp = new char[query_len + 1];
   memcpy(query_tmp, query, query_len);
   query_tmp[query_len] = '\0';
   fstr2cstr(query_tmp, query_len);

   *ret = romesqldb_make_query(reinterpret_cast<void*>(*romedb), query_tmp, *store);

   delete [] query_tmp;
}

void romesqldb_get_number_of_rows__(long long *romedb, int *ret)
{
   *ret = romesqldb_get_number_of_rows(reinterpret_cast<void*>(*romedb));
}

void romesqldb_next_row__(long long *romedb, int *ret)
{
   *ret = romesqldb_next_row(reinterpret_cast<void*>(*romedb));
}

void romesqldb_get_number_of_fields__(long long *romedb, int *ret)
{
   *ret = romesqldb_get_number_of_fields(reinterpret_cast<void*>(*romedb));
}

void romesqldb_get_field__(long long *romedb, int *fieldNumber, char *ret, const int ret_len)
{
   const char *p = romesqldb_get_field(reinterpret_cast<void*>(*romedb), *fieldNumber); 
   if (strlen(p) <= static_cast<unsigned int>(ret_len)) {
      strcpy(ret, p);
      cstr2fstr(ret, ret_len);
   }
}

void romesqldb_free_reqult__(long long *romedb)
{
   romesqldb_free_reqult(reinterpret_cast<void*>(*romedb));
}

void romesqldb_get_error_code__(long long *romedb, int *ret)
{
   *ret = romesqldb_get_error_code(reinterpret_cast<void*>(*romedb));
}

void romesqldb_get_error_message__(long long *romedb, char *ret, const int ret_len)
{
   const char *p = romesqldb_get_error_message(reinterpret_cast<void*>(*romedb));
   if (strlen(p) <= static_cast<unsigned int>(ret_len)) {
      strcpy(ret, p);
      cstr2fstr(ret, ret_len);
   }
}

void romesqldb_start_transaction__(long long *romedb, char *option, int *ret, const int option_len)
{
   char *option_tmp = new char[option_len + 1];
   memcpy(option_tmp, option, option_len);
   option_tmp[option_len] = '\0';
   fstr2cstr(option_tmp, option_len);

   *ret = romesqldb_start_transaction(reinterpret_cast<void*>(*romedb), option_tmp);

   delete [] option_tmp;
}

void romesqldb_commit_transaction__(long long *romedb, char *option, int *ret, const int option_len)
{
   char *option_tmp = new char[option_len + 1];
   memcpy(option_tmp, option, option_len);
   option_tmp[option_len] = '\0';
   fstr2cstr(option_tmp, option_len);

   *ret = romesqldb_commit_transaction(reinterpret_cast<void*>(*romedb), option_tmp);

   delete [] option_tmp;
}

void romesqldb_rollback_transaction__(long long *romedb, char *option, int *ret, const int option_len)
{
   char *option_tmp = new char[option_len + 1];
   memcpy(option_tmp, option, option_len);
   option_tmp[option_len] = '\0';
   fstr2cstr(option_tmp, option_len);

   *ret = romesqldb_rollback_transaction(reinterpret_cast<void*>(*romedb), option_tmp);

   delete [] option_tmp;
}

void romesqldb_print__(long long *romedb, char *opt, const int opt_len)
{
   char *opt_tmp = new char[opt_len + 1];
   memcpy(opt_tmp, opt, opt_len);
   opt_tmp[opt_len] = '\0';
   fstr2cstr(opt_tmp, opt_len);

   romesqldb_print(reinterpret_cast<void*>(*romedb), opt_tmp);

   delete [] opt_tmp;
}


void romesqldb_init_(char *name, char *connection, long long *ret, const int name_len, const int connection_len)
{
   romesqldb_init__(name, connection, ret, name_len, connection_len);
}

void romesqldb_disconnect_(long long *romedb)
{
   romesqldb_disconnect__(romedb);
}

void romesqldb_read_(long long *romedb, char *dataBasePath, long long int *runNumber,
                      long long int *eventNumber, int *ret, const int dataBasePath_len)
{
   romesqldb_read__(romedb, dataBasePath, runNumber, eventNumber, ret, dataBasePath_len);
}

void romesqldb_write_(long long *romedb, char *dataBasePath, long long int *runNumber,
                       long long int *eventNumber, int *ret, const int dataBasePath_len)
{
   romesqldb_write__(romedb, dataBasePath, runNumber, eventNumber, ret, dataBasePath_len);
}

void romesqldb_get_int_value_(long long *romedb, int *column, int *row, int *ret)
{
   romesqldb_get_int_value__(romedb, column, row, ret);
}

void romesqldb_get_double_value_(long long *romedb, int *column, int *row, double *ret)
{
   romesqldb_get_double_value__(romedb, column, row, ret);
}

void romesqldb_get_float_value_(long long *romedb, int *column, int *row, float *ret)
{
   romesqldb_get_float_value__(romedb, column, row, ret);
}

void romesqldb_get_string_value_(long long *romedb, int *column, int *row, char *ret, const int ret_len)
{
   romesqldb_get_string_value__(romedb, column, row, ret, ret_len);
}

void romesqldb_set_int_value_(long long *romedb, int *column, int *row, int *val)
{
   romesqldb_set_int_value__(romedb, column, row, val);
}

void romesqldb_set_double_value_(long long *romedb, int *column, int *row, double *val)
{
   romesqldb_set_double_value__(romedb, column, row, val);
}

void romesqldb_set_float_value_(long long *romedb, int *column, int *row, float *val)
{
   romesqldb_set_float_value__(romedb, column, row, val);
}

void romesqldb_set_string_value_(long long *romedb, int *column, int *row, char *val, const int val_len)
{
   romesqldb_set_string_value__(romedb, column, row, val, val_len);
}

void romesqldb_get_dbms_type_(long long *romedb, char *ret, const int ret_len)
{
   romesqldb_get_dbms_type__(romedb, ret, ret_len);
}

void romesqldb_link_error_(long long *romedb)
{
   romesqldb_link_error__(romedb);
}

void romesqldb_get_type_(long long *romedb, char *ret, const int ret_len)
{
   romesqldb_get_type__(romedb, ret, ret_len);
}

void romesqldb_get_description_(long long *romedb, char *ret, const int ret_len)
{
   romesqldb_get_description__(romedb, ret, ret_len);
}

void romesqldb_make_query_(long long *romedb, char *query, int *store, int *ret, const int query_len)
{
   romesqldb_make_query__(romedb, query, store, ret, query_len);
}

void romesqldb_get_number_of_rows_(long long *romedb, int *ret)
{
   romesqldb_get_number_of_rows__(romedb, ret);
}

void romesqldb_next_row_(long long *romedb, int *ret)
{
   romesqldb_next_row__(romedb, ret);
}

void romesqldb_get_number_of_fields_(long long *romedb, int *ret)
{
   romesqldb_get_number_of_fields__(romedb, ret);
}

void romesqldb_get_field_(long long *romedb, int *fieldNumber, char *ret, const int ret_len)
{
   romesqldb_get_field__(romedb, fieldNumber, ret, ret_len);
}

void romesqldb_free_reqult_(long long *romedb)
{
   romesqldb_free_reqult__(romedb);
}

void romesqldb_get_error_code_(long long *romedb, int *ret)
{
   romesqldb_get_error_code__(romedb, ret);
}

void romesqldb_get_error_message_(long long *romedb, char *ret, const int ret_len)
{
   romesqldb_get_error_message__(romedb, ret, ret_len);
}

void romesqldb_start_transaction_(long long *romedb, char *option, int *ret, const int query_len)
{
   romesqldb_start_transaction__(romedb, option, ret, query_len);
}

void romesqldb_commit_transaction_(long long *romedb, char *option, int *ret, const int option_len)
{
   romesqldb_commit_transaction__(romedb, option, ret, option_len);
}

void romesqldb_rollback_transaction_(long long *romedb, char *option, int *ret, const int option_len)
{
   romesqldb_rollback_transaction__(romedb, option, ret, option_len);
}

void romesqldb_print_(long long *romedb, char *opt, const int opt_len)
{
   romesqldb_print__(romedb, opt, opt_len);
}


void _romesqldb_init_(char *name, char *connection, long long *ret, const int name_len, const int connection_len)
{
   romesqldb_init__(name, connection, ret, name_len, connection_len);
}

void _romesqldb_disconnect_(long long *romedb)
{
   romesqldb_disconnect__(romedb);
}

void _romesqldb_read_(long long *romedb, char *dataBasePath, long long int *runNumber,
                      long long int *eventNumber, int *ret, const int dataBasePath_len)
{
   romesqldb_read__(romedb, dataBasePath, runNumber, eventNumber, ret, dataBasePath_len);
}

void _romesqldb_write_(long long *romedb, char *dataBasePath, long long int *runNumber,
                       long long int *eventNumber, int *ret, const int dataBasePath_len)
{
   romesqldb_write__(romedb, dataBasePath, runNumber, eventNumber, ret, dataBasePath_len);
}

void _romesqldb_get_int_value_(long long *romedb, int *column, int *row, int *ret)
{
   romesqldb_get_int_value__(romedb, column, row, ret);
}

void _romesqldb_get_double_value_(long long *romedb, int *column, int *row, double *ret)
{
   romesqldb_get_double_value__(romedb, column, row, ret);
}

void _romesqldb_get_float_value_(long long *romedb, int *column, int *row, float *ret)
{
   romesqldb_get_float_value__(romedb, column, row, ret);
}

void _romesqldb_get_string_value_(long long *romedb, int *column, int *row, char *ret, const int ret_len)
{
   romesqldb_get_string_value__(romedb, column, row, ret, ret_len);
}

void _romesqldb_set_int_value_(long long *romedb, int *column, int *row, int *val)
{
   romesqldb_set_int_value__(romedb, column, row, val);
}

void _romesqldb_set_double_value_(long long *romedb, int *column, int *row, double *val)
{
   romesqldb_set_double_value__(romedb, column, row, val);
}

void _romesqldb_set_float_value_(long long *romedb, int *column, int *row, float *val)
{
   romesqldb_set_float_value__(romedb, column, row, val);
}

void _romesqldb_set_string_value_(long long *romedb, int *column, int *row, char *val, const int val_len)
{
   romesqldb_set_string_value__(romedb, column, row, val, val_len);
}

void _romesqldb_get_dbms_type_(long long *romedb, char *ret, const int ret_len)
{
   romesqldb_get_dbms_type__(romedb, ret, ret_len);
}

void _romesqldb_link_error_(long long *romedb)
{
   romesqldb_link_error__(romedb);
}

void _romesqldb_get_type_(long long *romedb, char *ret, const int ret_len)
{
   romesqldb_get_type__(romedb, ret, ret_len);
}

void _romesqldb_get_description_(long long *romedb, char *ret, const int ret_len)
{
   romesqldb_get_description__(romedb, ret, ret_len);
}

void _romesqldb_make_query_(long long *romedb, char *query, int *store, int *ret, const int query_len)
{
   romesqldb_make_query__(romedb, query, store, ret, query_len);
}

void _romesqldb_get_number_of_rows_(long long *romedb, int *ret)
{
   romesqldb_get_number_of_rows__(romedb, ret);
}

void _romesqldb_next_row_(long long *romedb, int *ret)
{
   romesqldb_next_row__(romedb, ret);
}

void _romesqldb_get_number_of_fields_(long long *romedb, int *ret)
{
   romesqldb_get_number_of_fields__(romedb, ret);
}

void _romesqldb_get_field_(long long *romedb, int *fieldNumber, char *ret, const int ret_len)
{
   romesqldb_get_field__(romedb, fieldNumber, ret, ret_len);
}

void _romesqldb_free_reqult_(long long *romedb)
{
   romesqldb_free_reqult__(romedb);
}

void _romesqldb_get_error_code_(long long *romedb, int *ret)
{
   romesqldb_get_error_code__(romedb, ret);
}

void _romesqldb_get_error_message_(long long *romedb, char *ret, const int ret_len)
{
   romesqldb_get_error_message__(romedb, ret, ret_len);
}

void _romesqldb_start_transaction_(long long *romedb, char *option, int *ret, const int option_len)
{
   romesqldb_start_transaction__(romedb, option, ret, option_len);
}

void _romesqldb_commit_transaction_(long long *romedb, char *option, int *ret, const int option_len)
{
   romesqldb_commit_transaction__(romedb, option, ret, option_len);
}

void _romesqldb_rollback_transaction_(long long *romedb, char *option, int *ret, const int option_len)
{
   romesqldb_rollback_transaction__(romedb, option, ret, option_len);
}

void _romesqldb_print_(long long *romedb, char *opt, const int opt_len)
{
   romesqldb_print__(romedb, opt, opt_len);
}

#ifdef __cplusplus
}
#endif
