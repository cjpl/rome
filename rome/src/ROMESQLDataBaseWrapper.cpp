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
   ROMESQLDataBase * db;
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

void romesqldb_init__(char *name, char *connection, int *ret)
{
   void *a = romesqldb_init(name, connection);
   *ret = reinterpret_cast<int>(a);
}

void romesqldb_disconnect__(int *romedb)
{
   romesqldb_disconnect(reinterpret_cast<void*>(*romedb));
}

void romesqldb_read__(int *romedb, char *dataBasePath, long long int *runNumber,
                      long long int *eventNumber, int *ret)
{
   *ret = romesqldb_read(reinterpret_cast<void*>(*romedb), dataBasePath, *runNumber, *eventNumber);
}

void romesqldb_write__(int *romedb, char *dataBasePath, long long int *runNumber,
                       long long int *eventNumber, int *ret)
{
   *ret = romesqldb_write(reinterpret_cast<void*>(*romedb), dataBasePath, *runNumber, *eventNumber);
}

void romesqldb_get_int_value__(int *romedb, int *column, int *row, int *ret)
{
   *ret = romesqldb_get_int_value(reinterpret_cast<void*>(*romedb), *column, *row);
}

void romesqldb_get_float_value__(int *romedb, int *column, int *row, float *ret)
{
   *ret = romesqldb_get_double_value(reinterpret_cast<void*>(*romedb), *column, *row);
}

void romesqldb_get_double_value__(int *romedb, int *column, int *row, double *ret)
{
   *ret = romesqldb_get_double_value(reinterpret_cast<void*>(*romedb), *column, *row);
}

void romesqldb_get_string_value__(int *romedb, int *column, int *row, char *ret, const int ret_len)
{
   const char *p = romesqldb_get_string_value(reinterpret_cast<void*>(*romedb), *column, *row);
   if (strlen(p) <= static_cast<unsigned int>(ret_len)) {
      strcpy(ret, p);
      cstr2fstr(ret, ret_len);
   }
}

void romesqldb_set_int_value__(int *romedb, int *column, int *row, int *val)
{
   romesqldb_set_int_value(reinterpret_cast<void*>(*romedb), *column, *row, *val);
}

void romesqldb_set_double_value__(int *romedb, int *column, int *row, double *val)
{
   romesqldb_set_double_value(reinterpret_cast<void*>(*romedb), *column, *row, *val);
}

void romesqldb_set_string_value__(int *romedb, int *column, int *row, char *val)
{
   romesqldb_set_string_value(reinterpret_cast<void*>(*romedb), *column, *row, val);
}

void romesqldb_get_dbms_type__(int *romedb, char *ret, const int ret_len)
{
   const char * p = romesqldb_get_dbms_type(reinterpret_cast<void*>(*romedb));
   if (strlen(p) <= static_cast<unsigned int>(ret_len)) {
      strcpy(ret, p);
      cstr2fstr(ret, ret_len);
   }
}

void romesqldb_link_error__(int *romedb)
{
   romesqldb_link_error(reinterpret_cast<void*>(*romedb));
}

void romesqldb_get_type__(int *romedb, char *ret, const int ret_len)
{
   const char *p = romesqldb_get_type(reinterpret_cast<void*>(*romedb));
   if (strlen(p) <= static_cast<unsigned int>(ret_len)) {
      strcpy(ret, p);
      cstr2fstr(ret, ret_len);
   }
}

void romesqldb_get_description__(int *romedb, char *ret, const int ret_len)
{
   const char * p = romesqldb_get_description(reinterpret_cast<void*>(*romedb));
   if (strlen(p) <= static_cast<unsigned int>(ret_len)) {
      strcpy(ret, p);
      cstr2fstr(ret, ret_len);
   }
}

void romesqldb_make_query__(int *romedb, char *query, int *store, int *ret)
{
   *ret = romesqldb_make_query(reinterpret_cast<void*>(*romedb), query, *store);
}

void romesqldb_get_number_of_rows__(int *romedb, int *ret)
{
   *ret = romesqldb_get_number_of_rows(reinterpret_cast<void*>(*romedb));
}

void romesqldb_next_row__(int *romedb, int *ret)
{
   *ret = romesqldb_next_row(reinterpret_cast<void*>(*romedb));
}

void romesqldb_get_number_of_fields__(int *romedb, int *ret)
{
   *ret = romesqldb_get_number_of_fields(reinterpret_cast<void*>(*romedb));
}

void romesqldb_get_field__(int *romedb, int *fieldNumber, char *ret, const int ret_len)
{
   const char *p = romesqldb_get_field(reinterpret_cast<void*>(*romedb), *fieldNumber); 
   if (strlen(p) <= static_cast<unsigned int>(ret_len)) {
      strcpy(ret, p);
      cstr2fstr(ret, ret_len);
   }
}

void romesqldb_free_reqult__(int *romedb)
{
   romesqldb_free_reqult(reinterpret_cast<void*>(*romedb));
}

void romesqldb_get_error_code__(int *romedb, int *ret)
{
   *ret = romesqldb_get_error_code(reinterpret_cast<void*>(*romedb));
}

void romesqldb_get_error_message__(int *romedb, char *ret, const int ret_len)
{
   const char *p = romesqldb_get_error_message(reinterpret_cast<void*>(*romedb));
   if (strlen(p) <= static_cast<unsigned int>(ret_len)) {
      strcpy(ret, p);
      cstr2fstr(ret, ret_len);
   }
}

void romesqldb_start_transaction__(int *romedb, char *option, int *ret)
{
   *ret = romesqldb_start_transaction(reinterpret_cast<void*>(*romedb), option);
}

void romesqldb_commit_transaction__(int *romedb, char *option, int *ret)
{
   *ret = romesqldb_commit_transaction(reinterpret_cast<void*>(*romedb), option);
}

void romesqldb_rollback_transaction__(int *romedb, char *option, int *ret)
{
   *ret = romesqldb_rollback_transaction(reinterpret_cast<void*>(*romedb), option);
}

void romesqldb_print__(int *romedb, char *opt)
{
   romesqldb_print(reinterpret_cast<void*>(*romedb), opt);
}

#ifdef __cplusplus
}
#endif
