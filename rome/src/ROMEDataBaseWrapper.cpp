// Author: Ryu Sawada
//////////////////////////////////////////////////////////////////////////
//
//  ROMEDataBaseWrapper
//
//  DataBase access.
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////

#include "ROMEDataBaseWrapper.h"
#include "ROMESQLDataBase.h"
#include "ROMETextDataBase.h"
#include "ROMEStr2DArray.h"
#include <string.h>

class ROMEDataBaseWorkSpace {
public:
   Int_t             type;
   ROMESQLDataBase  *sqldb;
   ROMEStr2DArray   *sqlvalues;

   ROMETextDataBase *textdb;
   ROMEStr2DArray   *textvalues;

public:
   ROMEDataBaseWorkSpace()
   :type(kROMEDBWrapperDBTypeNone)
   ,sqldb(0)
   ,sqlvalues(0)
   ,textdb(0)
   ,textvalues(0)
   {
   }

   ~ROMEDataBaseWorkSpace() {
      SafeDelete(sqldb);
      SafeDelete(sqlvalues);
      SafeDelete(textdb);
      SafeDelete(textvalues);
   }
};

// For C

void *romedb_init(const char *name, int type, const char *connection)
{
   switch (type) {
   case kROMEDBWrapperDBTypeSQL:
      return romesqldb_init(name, connection);
   case kROMEDBWrapperDBTypeText:
      return rometextdb_init(name, connection);
   default:
      break;
   }
   return 0;
}

void romedb_disconnect(void *romedb)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   SafeDelete(work);
}

int romedb_read(void *romedb, const char *dataBasePath, long long int runNumber, long long int eventNumber)
{
   if (!romedb) {
      return 0;
   }
   switch (static_cast<ROMEDataBaseWorkSpace*>(romedb)->type) {
   case kROMEDBWrapperDBTypeSQL:
      return romesqldb_read(romedb, dataBasePath, runNumber, eventNumber);
   case kROMEDBWrapperDBTypeText:
      return rometextdb_read(romedb, dataBasePath, runNumber, eventNumber);
   default:
      break;
   }
   return 0;
}

int romedb_write(void *romedb, const char *dataBasePath, long long int runNumber, long long int eventNumber)
{
   if (!romedb) {
      return 0;
   }
   switch (static_cast<ROMEDataBaseWorkSpace*>(romedb)->type) {
   case kROMEDBWrapperDBTypeSQL:
      return romesqldb_write(romedb, dataBasePath, runNumber, eventNumber);
   case kROMEDBWrapperDBTypeText:
      return rometextdb_write(romedb, dataBasePath, runNumber, eventNumber);
   default:
      break;
   }
   return 0;
}

int romedb_get_int_value(void *romedb, int row, int column)
{
   if (!romedb) {
      return 0;
   }
   switch (static_cast<ROMEDataBaseWorkSpace*>(romedb)->type) {
   case kROMEDBWrapperDBTypeSQL:
      return romesqldb_get_int_value(romedb, row, column);
   case kROMEDBWrapperDBTypeText:
      return rometextdb_get_int_value(romedb, row, column);
   default:
      break;
   }
   return 0;
}

double romedb_get_double_value(void *romedb, int row, int column)
{
   if (!romedb) {
      return 0;
   }
   switch (static_cast<ROMEDataBaseWorkSpace*>(romedb)->type) {
   case kROMEDBWrapperDBTypeSQL:
      return romesqldb_get_double_value(romedb, row, column);
   case kROMEDBWrapperDBTypeText:
      return rometextdb_get_double_value(romedb, row, column);
   default:
      break;
   }
   return 0;
}

float romedb_get_float_value(void *romedb, int row, int column)
{
   if (!romedb) {
      return 0;
   }
   switch (static_cast<ROMEDataBaseWorkSpace*>(romedb)->type) {
   case kROMEDBWrapperDBTypeSQL:
      return romesqldb_get_float_value(romedb, row, column);
   case kROMEDBWrapperDBTypeText:
      return rometextdb_get_float_value(romedb, row, column);
   default:
      break;
   }
   return 0;
}

const char *romedb_get_string_value(void *romedb, int row, int column)
{
   if (!romedb) {
      return 0;
   }
   switch (static_cast<ROMEDataBaseWorkSpace*>(romedb)->type) {
   case kROMEDBWrapperDBTypeSQL:
      return romesqldb_get_string_value(romedb, row, column);
   case kROMEDBWrapperDBTypeText:
      return rometextdb_get_string_value(romedb, row, column);
   default:
      break;
   }
   return 0;
}

void romedb_set_int_value(void *romedb, int row, int column, int val)
{
   if (!romedb) {
      return;
   }
   switch (static_cast<ROMEDataBaseWorkSpace*>(romedb)->type) {
   case kROMEDBWrapperDBTypeSQL:
      romesqldb_set_int_value(romedb, row, column, val);
      break;
   case kROMEDBWrapperDBTypeText:
      rometextdb_set_int_value(romedb, row, column, val);
      break;
   default:
      break;
   }
}

void romedb_set_double_value(void *romedb, int row, int column, double val)
{
   if (!romedb) {
      return;
   }
   switch (static_cast<ROMEDataBaseWorkSpace*>(romedb)->type) {
   case kROMEDBWrapperDBTypeSQL:
      romesqldb_set_double_value(romedb, row, column, val);
      break;
   case kROMEDBWrapperDBTypeText:
      rometextdb_set_double_value(romedb, row, column, val);
      break;
   default:
      break;
   }
}

void romedb_set_float_value(void *romedb, int row, int column, float val)
{
   if (!romedb) {
      return;
   }
   switch (static_cast<ROMEDataBaseWorkSpace*>(romedb)->type) {
   case kROMEDBWrapperDBTypeSQL:
      romesqldb_set_float_value(romedb, row, column, val);
      break;
   case kROMEDBWrapperDBTypeText:
      rometextdb_set_float_value(romedb, row, column, val);
      break;
   default:
      break;
   }
}

void romedb_set_string_value(void *romedb, int row, int column, const char *val)
{
   if (!romedb) {
      return;
   }
   switch (static_cast<ROMEDataBaseWorkSpace*>(romedb)->type) {
   case kROMEDBWrapperDBTypeSQL:
      romesqldb_set_string_value(romedb, row, column, val);
      break;
   case kROMEDBWrapperDBTypeText:
      rometextdb_set_string_value(romedb, row, column, val);
      break;
   default:
      break;
   }
}

const char *romedb_get_type(void *romedb)
{
   if (!romedb) {
      return 0;
   }
   switch (static_cast<ROMEDataBaseWorkSpace*>(romedb)->type) {
   case kROMEDBWrapperDBTypeSQL:
      return romesqldb_get_type(romedb);
   case kROMEDBWrapperDBTypeText:
      return rometextdb_get_type(romedb);
   default:
      break;
   }
   return 0;
}

const char *romedb_get_description(void *romedb)
{
   if (!romedb) {
      return 0;
   }
   switch (static_cast<ROMEDataBaseWorkSpace*>(romedb)->type) {
   case kROMEDBWrapperDBTypeSQL:
      return romesqldb_get_description(romedb);
   case kROMEDBWrapperDBTypeText:
      return rometextdb_get_description(romedb);
   default:
      break;
   }
   return 0;
}


void *romesqldb_init(const char *name, const char *connection)
{
   ROMEDataBaseWorkSpace *work = new ROMEDataBaseWorkSpace();
   work->type = kROMEDBWrapperDBTypeSQL;
   work->sqldb = new ROMESQLDataBase();
   work->sqlvalues = new ROMEStr2DArray(1, 1);
   if (!work->sqldb->Init(name, 0, connection)) {
      return 0;
   }
   return work;
}

void romesqldb_disconnect(void *romedb)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   SafeDelete(work);
}

int romesqldb_read(void *romedb, const char *dataBasePath, long long int runNumber, long long int eventNumber)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeSQL) {
      return 0;
   }
   return work->sqldb->Read(work->sqlvalues, dataBasePath, runNumber, eventNumber);
}

int romesqldb_write(void *romedb, const char *dataBasePath, long long int runNumber, long long int eventNumber)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeSQL) {
      return 0;
   }
   return work->sqldb->Write(work->sqlvalues, dataBasePath, runNumber, eventNumber);
}

int romesqldb_get_int_value(void *romedb, int row, int column)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeSQL) {
      return 0;
   }
   char *cstop;
   return strtol(work->sqlvalues->At(row, column).Data(), &cstop, 10);
}

double romesqldb_get_double_value(void *romedb, int row, int column)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeSQL) {
      return 0;
   }
   char *cstop;
   return strtod(work->sqlvalues->At(row, column).Data(), &cstop);
}

float romesqldb_get_float_value(void *romedb, int row, int column)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeSQL) {
      return 0;
   }
   char *cstop;
   return strtof(work->sqlvalues->At(row, column).Data(), &cstop);
}

const char *romesqldb_get_string_value(void *romedb, int row, int column)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeSQL) {
      return 0;
   }
   return work->sqlvalues->At2(row, column).Data();
}

void romesqldb_set_int_value(void *romedb, int row, int column, int val)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeSQL) {
      return;
   }
   ROMEString str;
   str.SetFormatted("%d", val);
   work->sqlvalues->SetAt(str, row, column);
}

void romesqldb_set_double_value(void *romedb, int row, int column, double val)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeSQL) {
      return;
   }
   ROMEString str;
   str.SetFormatted("%.16g", val);
   work->sqlvalues->SetAt(str, row, column);
}

void romesqldb_set_float_value(void *romedb, int row, int column, float val)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeSQL) {
      return;
   }
   ROMEString str;
   str.SetFormatted("%.8g", val);
   work->sqlvalues->SetAt(str, row, column);
}

void romesqldb_set_string_value(void *romedb, int row, int column, const char *val)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeSQL) {
      return;
   }
   ROMEString str = val;
   work->sqlvalues->SetAt(str, row, column);
}

const char *romesqldb_get_dbms_type(void *romedb)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeSQL) {
      return 0;
   }
   return work->sqldb->GetDBMSType();
}

void romesqldb_link_error(void *romedb)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeSQL) {
      return;
   }
   work->sqldb->LinkError();
}

const char *romesqldb_get_type(void *romedb)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeSQL) {
      return 0;
   }
   return work->sqldb->GetType();
}

const char *romesqldb_get_description(void *romedb)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeSQL) {
      return 0;
   }
   return work->sqldb->GetDescription();
}

int romesqldb_make_query(void *romedb, const char *query, int store)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeSQL) {
      return 0;
   }
   return work->sqldb->MakeQuery(query, store);
}

int romesqldb_get_number_of_rows(void *romedb)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeSQL) {
      return 0;
   }
   return work->sqldb->GetNumberOfRows();
}

int romesqldb_next_row(void *romedb)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeSQL) {
      return 0;
   }
   return work->sqldb->NextRow();
}

int romesqldb_get_number_of_fields(void *romedb)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeSQL) {
      return 0;
   }
   return work->sqldb->GetNumberOfFields();
}

const char *romesqldb_get_field(void *romedb, int fieldNumber)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeSQL) {
      return 0;
   }
   return work->sqldb->GetField(fieldNumber);
}

void romesqldb_free_reqult(void *romedb)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeSQL) {
      return;
   }
   work->sqldb->FreeResult();
}

int romesqldb_get_error_code(void *romedb)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeSQL) {
      return 0;
   }
   return work->sqldb->GetErrorCode();
}

const char *romesqldb_get_error_message(void *romedb)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeSQL) {
      return 0;
   }
   return work->sqldb->GetErrorMessage();
}

int romesqldb_start_transaction(void *romedb, const char *option)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeSQL) {
      return 0;
   }
   return work->sqldb->StartTransaction(option);
}

int romesqldb_commit_transaction(void *romedb, const char *option)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeSQL) {
      return 0;
   }
   return work->sqldb->CommitTransaction(option);
}

int romesqldb_rollback_transaction(void *romedb, const char *option)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeSQL) {
      return 0;
   }
   return work->sqldb->RollbackTransaction(option);
}

void romesqldb_print(void *romedb, const char *option)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeSQL) {
      return;
   }
   work->sqldb->Print(option);
}

void *rometextdb_init(const char *name, const char *connection)
{
   ROMEDataBaseWorkSpace *work = new ROMEDataBaseWorkSpace();
   work->type = kROMEDBWrapperDBTypeText;
   work->textdb = new ROMETextDataBase();
   work->textvalues = new ROMEStr2DArray(1, 1);
   if (!work->textdb->Init(name, connection, 0)) {
      return 0;
   }
   return work;
}

void rometextdb_disconnect(void *romedb)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   SafeDelete(work);
}

int rometextdb_read(void *romedb, const char *dataBasePath, long long int runNumber, long long int eventNumber)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeText) {
      return 0;
   }
   return work->textdb->Read(work->textvalues, dataBasePath, runNumber, eventNumber);
}

int rometextdb_write(void *romedb, const char *dataBasePath, long long int runNumber, long long int eventNumber)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeText) {
      return 0;
   }
   return work->textdb->Write(work->textvalues, dataBasePath, runNumber, eventNumber);
}

int rometextdb_get_int_value(void *romedb, int row, int column)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeText) {
      return 0;
   }
   char *cstop;
   return strtol(work->textvalues->At(row, column).Data(), &cstop, 10);
}

double rometextdb_get_double_value(void *romedb, int row, int column)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeText) {
      return 0;
   }
   char *cstop;
   return strtod(work->textvalues->At(row, column).Data(), &cstop);
}

float rometextdb_get_float_value(void *romedb, int row, int column)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeText) {
      return 0;
   }
   char *cstop;
   return strtof(work->textvalues->At(row, column).Data(), &cstop);
}

const char *rometextdb_get_string_value(void *romedb, int row, int column)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeText) {
      return 0;
   }
   return work->textvalues->At2(row, column).Data();
}

void rometextdb_set_int_value(void *romedb, int row, int column, int val)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeText) {
      return;
   }
   ROMEString str;
   str.SetFormatted("%d", val);
   work->textvalues->SetAt(str, row, column);
}

void rometextdb_set_double_value(void *romedb, int row, int column, double val)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeText) {
      return;
   }
   ROMEString str;
   str.SetFormatted("%.16g", val);
   work->textvalues->SetAt(str, row, column);
}

void rometextdb_set_float_value(void *romedb, int row, int column, float val)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeText) {
      return;
   }
   ROMEString str;
   str.SetFormatted("%.8g", val);
   work->textvalues->SetAt(str, row, column);
}

void rometextdb_set_string_value(void *romedb, int row, int column, const char *val)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeText) {
      return;
   }
   ROMEString str = val;
   work->textvalues->SetAt(str, row, column);
}

const char *rometextdb_get_type(void *romedb)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeText) {
      return 0;
   }
   return work->textdb->GetType();
}

const char *rometextdb_get_description(void *romedb)
{
   ROMEDataBaseWorkSpace *work = static_cast<ROMEDataBaseWorkSpace*>(romedb);
   if (!work || work->type != kROMEDBWrapperDBTypeText) {
      return 0;
   }
   return work->textdb->GetDescription();
}


/* for Fortran */
static void cstr2fstr(char *ret, const int ret_len)
{
   int len = strlen(ret);
   memset(ret + len, ' ', ret_len - len);
}

static void fstr2cstr(char *ret, const int ret_len)
{
   int i;

   if (ret[ret_len - 1] != ' ') {
      return;
   }
   for (i = ret_len - 2; ret[i] == ' ' && i >= 0; i--) {;}
   ret[i + 1] = '\0';
}


void romedb_init__(char *name, int *type, char *connection, long long *ret, const int name_len,
                   const int connection_len)
{
   switch (*type) {
   case kROMEDBWrapperDBTypeSQL:
      romesqldb_init__(name, connection, ret, name_len, connection_len);
      break;
   case kROMEDBWrapperDBTypeText:
      rometextdb_init__(name, connection, ret, name_len, connection_len);
      break;
   default:
      *ret = 0;
      break;
   }
}

void romedb_disconnect__(long long *romedb)
{
   romesqldb_disconnect(reinterpret_cast<void*>(*romedb));
}

void romedb_read__(long long *romedb, char *dataBasePath, long long int *runNumber,
                   long long int *eventNumber, int *ret, const int dataBasePath_len)
{
   if (!romedb || !(*romedb)) {
      *ret = 0;
      return;
   }
   switch (reinterpret_cast<ROMEDataBaseWorkSpace*>(*romedb)->type) {
   case kROMEDBWrapperDBTypeSQL:
      romesqldb_read__(romedb, dataBasePath, runNumber, eventNumber, ret, dataBasePath_len);
      break;
   case kROMEDBWrapperDBTypeText:
      rometextdb_read__(romedb, dataBasePath, runNumber, eventNumber, ret, dataBasePath_len);
      break;
   default:
      *ret = 0;
      break;
   }
}

void romedb_write__(long long *romedb, char *dataBasePath, long long int *runNumber,
                    long long int *eventNumber, int *ret, const int dataBasePath_len)
{
   if (!romedb || !(*romedb)) {
      *ret = 0;
      return;
   }
   switch (reinterpret_cast<ROMEDataBaseWorkSpace*>(*romedb)->type) {
   case kROMEDBWrapperDBTypeSQL:
      romesqldb_write__(romedb, dataBasePath, runNumber, eventNumber, ret, dataBasePath_len);
      break;
   case kROMEDBWrapperDBTypeText:
      rometextdb_write__(romedb, dataBasePath, runNumber, eventNumber, ret, dataBasePath_len);
      break;
   default:
      *ret = 0;
      break;
   }
}

void romedb_get_int_value__(long long *romedb, int *row, int *column, int *ret)
{
   if (!romedb || !(*romedb)) {
      *ret = 0;
      return;
   }
   switch (reinterpret_cast<ROMEDataBaseWorkSpace*>(*romedb)->type) {
   case kROMEDBWrapperDBTypeSQL:
      romesqldb_get_int_value__(romedb, row, column, ret);
      break;
   case kROMEDBWrapperDBTypeText:
      rometextdb_get_int_value__(romedb, row, column, ret);
      break;
   default:
      *ret = 0;
      break;
   }
}

void romedb_get_double_value__(long long *romedb, int *row, int *column, double *ret)
{
   if (!romedb || !(*romedb)) {
      *ret = 0;
      return;
   }
   switch (reinterpret_cast<ROMEDataBaseWorkSpace*>(*romedb)->type) {
   case kROMEDBWrapperDBTypeSQL:
      romesqldb_get_double_value__(romedb, row, column, ret);
      break;
   case kROMEDBWrapperDBTypeText:
      rometextdb_get_double_value__(romedb, row, column, ret);
      break;
   default:
      *ret = 0;
      break;
   }
}

void romedb_get_float_value__(long long *romedb, int *row, int *column, float *ret)
{
   if (!romedb || !(*romedb)) {
      *ret = 0;
      return;
   }
   switch (reinterpret_cast<ROMEDataBaseWorkSpace*>(*romedb)->type) {
   case kROMEDBWrapperDBTypeSQL:
      romesqldb_get_float_value__(romedb, row, column, ret);
      break;
   case kROMEDBWrapperDBTypeText:
      rometextdb_get_float_value__(romedb, row, column, ret);
      break;
   default:
      *ret = 0;
      break;
   }
}

void romedb_get_string_value__(long long *romedb, int *row, int *column, char *ret, const int ret_len)
{
   if (!romedb || !(*romedb)) {
      if (ret_len > 0) {
         strcpy(ret, "");
         cstr2fstr(ret, ret_len);
      }
      return;
   }
   switch (reinterpret_cast<ROMEDataBaseWorkSpace*>(*romedb)->type) {
   case kROMEDBWrapperDBTypeSQL:
      romesqldb_get_string_value__(romedb, row, column, ret, ret_len);
      break;
   case kROMEDBWrapperDBTypeText:
      rometextdb_get_string_value__(romedb, row, column, ret, ret_len);
      break;
   default:
      if (ret_len > 0) {
         strcpy(ret, "");
         cstr2fstr(ret, ret_len);
      }
      break;
   }
}

void romedb_set_int_value__(long long *romedb, int *row, int *column, int *val)
{
   if (!romedb || !(*romedb)) {
      return;
   }
   switch (reinterpret_cast<ROMEDataBaseWorkSpace*>(*romedb)->type) {
   case kROMEDBWrapperDBTypeSQL:
      romesqldb_set_int_value__(romedb, row, column, val);
      break;
   case kROMEDBWrapperDBTypeText:
      rometextdb_set_int_value__(romedb, row, column, val);
      break;
   default:
      break;
   }
}

void romedb_set_double_value__(long long *romedb, int *row, int *column, double *val)
{
   if (!romedb || !(*romedb)) {
      return;
   }
   switch (reinterpret_cast<ROMEDataBaseWorkSpace*>(*romedb)->type) {
   case kROMEDBWrapperDBTypeSQL:
      romesqldb_set_double_value__(romedb, row, column, val);
      break;
   case kROMEDBWrapperDBTypeText:
      rometextdb_set_double_value__(romedb, row, column, val);
      break;
   default:
      break;
   }
}

void romedb_set_float_value__(long long *romedb, int *row, int *column, float *val)
{
   if (!romedb || !(*romedb)) {
      return;
   }
   switch (reinterpret_cast<ROMEDataBaseWorkSpace*>(*romedb)->type) {
   case kROMEDBWrapperDBTypeSQL:
      romesqldb_set_float_value__(romedb, row, column, val);
      break;
   case kROMEDBWrapperDBTypeText:
      rometextdb_set_float_value__(romedb, row, column, val);
      break;
   default:
      break;
   }
}

void romedb_set_string_value__(long long *romedb, int *row, int *column, char *val, const int val_len)
{
   if (!romedb || !(*romedb)) {
      return;
   }
   switch (reinterpret_cast<ROMEDataBaseWorkSpace*>(*romedb)->type) {
   case kROMEDBWrapperDBTypeSQL:
      romesqldb_set_string_value__(romedb, row, column, val, val_len);
      break;
   case kROMEDBWrapperDBTypeText:
      rometextdb_set_string_value__(romedb, row, column, val, val_len);
      break;
   default:
      break;
   }
}

void romedb_get_type__(long long *romedb, char *ret, const int ret_len)
{
   if (!romedb || !(*romedb)) {
      if (ret_len > 0) {
         strcpy(ret, "");
         cstr2fstr(ret, ret_len);
      }
      return;
   }
   switch (reinterpret_cast<ROMEDataBaseWorkSpace*>(*romedb)->type) {
   case kROMEDBWrapperDBTypeSQL:
      romesqldb_get_type__(romedb,  ret, ret_len);
      break;
   case kROMEDBWrapperDBTypeText:
      rometextdb_get_type__(romedb, ret, ret_len);
      break;
   default:
      if (ret_len > 0) {
         strcpy(ret, "");
         cstr2fstr(ret, ret_len);
      }
      break;
   }
}

void romedb_get_description__(long long *romedb, char *ret, const int ret_len)
{
   if (!romedb || !(*romedb)) {
      if (ret_len > 0) {
         strcpy(ret, "");
         cstr2fstr(ret, ret_len);
      }
      return;
   }
   switch (reinterpret_cast<ROMEDataBaseWorkSpace*>(*romedb)->type) {
   case kROMEDBWrapperDBTypeSQL:
      romesqldb_get_description__(romedb,  ret, ret_len);
      break;
   case kROMEDBWrapperDBTypeText:
      rometextdb_get_description__(romedb, ret, ret_len);
      break;
   default:
      if (ret_len > 0) {
         strcpy(ret, "");
         cstr2fstr(ret, ret_len);
      }
      break;
   }
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

void romesqldb_get_int_value__(long long *romedb, int *row, int *column, int *ret)
{
   *ret = romesqldb_get_int_value(reinterpret_cast<void*>(*romedb), *row - 1, *column - 1);
}

void romesqldb_get_double_value__(long long *romedb, int *row, int *column, double *ret)
{
   *ret = romesqldb_get_double_value(reinterpret_cast<void*>(*romedb), *row - 1, *column - 1);
}

void romesqldb_get_float_value__(long long *romedb, int *row, int *column, float *ret)
{
   *ret = romesqldb_get_float_value(reinterpret_cast<void*>(*romedb), *row - 1, *column - 1);
}

void romesqldb_get_string_value__(long long *romedb, int *row, int *column, char *ret, const int ret_len)
{
   const char *p = romesqldb_get_string_value(reinterpret_cast<void*>(*romedb), *row - 1, *column - 1);
   if (!p) {
      if (ret_len > 0) {
         strcpy(ret, "");
         cstr2fstr(ret, ret_len);
      }
      return;
   }
   if (strlen(p) <= static_cast<unsigned int>(ret_len)) {
      strcpy(ret, p);
      cstr2fstr(ret, ret_len);
   }
}

void romesqldb_set_int_value__(long long *romedb, int *row, int *column, int *val)
{
   romesqldb_set_int_value(reinterpret_cast<void*>(*romedb), *row - 1, *column - 1, *val);
}

void romesqldb_set_double_value__(long long *romedb, int *row, int *column, double *val)
{
   romesqldb_set_double_value(reinterpret_cast<void*>(*romedb), *row - 1, *column - 1, *val);
}

void romesqldb_set_float_value__(long long *romedb, int *row, int *column, float *val)
{
   romesqldb_set_float_value(reinterpret_cast<void*>(*romedb), *row - 1, *column - 1, *val);
}

void romesqldb_set_string_value__(long long *romedb, int *row, int *column, char *val, const int val_len)
{
   char *val_tmp = new char[val_len + 1];
   memcpy(val_tmp, val, val_len);
   val_tmp[val_len] = '\0';
   fstr2cstr(val_tmp, val_len);

   romesqldb_set_string_value(reinterpret_cast<void*>(*romedb), *row - 1, *column - 1, val_tmp);

   delete [] val_tmp;
}

void romesqldb_get_dbms_type__(long long *romedb, char *ret, const int ret_len)
{
   const char *p = romesqldb_get_dbms_type(reinterpret_cast<void*>(*romedb));
   if (!p) {
      if (ret_len > 0) {
         strcpy(ret, "");
         cstr2fstr(ret, ret_len);
      }
      return;
   }
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
   if (!p) {
      if (ret_len > 0) {
         strcpy(ret, "");
         cstr2fstr(ret, ret_len);
      }
      return;
   }
   if (strlen(p) <= static_cast<unsigned int>(ret_len)) {
      strcpy(ret, p);
      cstr2fstr(ret, ret_len);
   }
}

void romesqldb_get_description__(long long *romedb, char *ret, const int ret_len)
{
   const char *p = romesqldb_get_description(reinterpret_cast<void*>(*romedb));
   if (!p) {
      if (ret_len > 0) {
         strcpy(ret, "");
         cstr2fstr(ret, ret_len);
      }
      return;
   }
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
   const char *p = romesqldb_get_field(reinterpret_cast<void*>(*romedb), *fieldNumber - 1);
   if (!p) {
      if (ret_len > 0) {
         strcpy(ret, "");
         cstr2fstr(ret, ret_len);
      }
      return;
   }
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
   if (!p) {
      if (ret_len > 0) {
         strcpy(ret, "");
         cstr2fstr(ret, ret_len);
      }
      return;
   }
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

void rometextdb_init__(char *name, char *connection, long long *ret, const int name_len, const int connection_len)
{
   char *name_tmp = new char[name_len + 1];
   memcpy(name_tmp, name, name_len);
   name_tmp[name_len] = '\0';
   fstr2cstr(name_tmp, name_len);

   char *connection_tmp = new char[connection_len + 1];
   memcpy(connection_tmp, connection, connection_len);
   connection_tmp[connection_len] = '\0';
   fstr2cstr(connection_tmp, connection_len);

   *ret = reinterpret_cast<long long>(rometextdb_init(name_tmp, connection_tmp));

   delete [] name_tmp;
   delete [] connection_tmp;
}

void rometextdb_disconnect__(long long *romedb)
{
   rometextdb_disconnect(reinterpret_cast<void*>(*romedb));
}

void rometextdb_read__(long long *romedb, char *dataBasePath, long long int *runNumber,
                       long long int *eventNumber, int *ret, const int dataBasePath_len)
{
   char *dataBasePath_tmp = new char[dataBasePath_len + 1];
   memcpy(dataBasePath_tmp, dataBasePath, dataBasePath_len);
   dataBasePath_tmp[dataBasePath_len] = '\0';
   fstr2cstr(dataBasePath_tmp, dataBasePath_len);

   *ret = rometextdb_read(reinterpret_cast<void*>(*romedb), dataBasePath_tmp, *runNumber, *eventNumber);

   delete [] dataBasePath_tmp;
}

void rometextdb_write__(long long *romedb, char *dataBasePath, long long int *runNumber,
                        long long int *eventNumber, int *ret, const int dataBasePath_len)
{
   char *dataBasePath_tmp = new char[dataBasePath_len + 1];
   memcpy(dataBasePath_tmp, dataBasePath, dataBasePath_len);
   dataBasePath_tmp[dataBasePath_len] = '\0';
   fstr2cstr(dataBasePath_tmp, dataBasePath_len);

   *ret = rometextdb_write(reinterpret_cast<void*>(*romedb), dataBasePath_tmp, *runNumber, *eventNumber);

   delete [] dataBasePath_tmp;
}

void rometextdb_get_int_value__(long long *romedb, int *row, int *column, int *ret)
{
   *ret = rometextdb_get_int_value(reinterpret_cast<void*>(*romedb), *row - 1, *column - 1);
}

void rometextdb_get_double_value__(long long *romedb, int *row, int *column, double *ret)
{
   *ret = rometextdb_get_double_value(reinterpret_cast<void*>(*romedb), *row - 1, *column - 1);
}

void rometextdb_get_float_value__(long long *romedb, int *row, int *column, float *ret)
{
   *ret = rometextdb_get_float_value(reinterpret_cast<void*>(*romedb), *row - 1, *column - 1);
}

void rometextdb_get_string_value__(long long *romedb, int *row, int *column, char *ret, const int ret_len)
{
   const char *p = rometextdb_get_string_value(reinterpret_cast<void*>(*romedb), *row - 1, *column - 1);
   if (!p) {
      if (ret_len > 0) {
         strcpy(ret, "");
         cstr2fstr(ret, ret_len);
      }
      return;
   }
   if (strlen(p) <= static_cast<unsigned int>(ret_len)) {
      strcpy(ret, p);
      cstr2fstr(ret, ret_len);
   }
}

void rometextdb_set_int_value__(long long *romedb, int *row, int *column, int *val)
{
   rometextdb_set_int_value(reinterpret_cast<void*>(*romedb), *row - 1, *column - 1, *val);
}

void rometextdb_set_double_value__(long long *romedb, int *row, int *column, double *val)
{
   rometextdb_set_double_value(reinterpret_cast<void*>(*romedb), *row - 1, *column - 1, *val);
}

void rometextdb_set_float_value__(long long *romedb, int *row, int *column, float *val)
{
   rometextdb_set_float_value(reinterpret_cast<void*>(*romedb), *row - 1, *column - 1, *val);
}

void rometextdb_set_string_value__(long long *romedb, int *row, int *column, char *val, const int val_len)
{
   char *val_tmp = new char[val_len + 1];
   memcpy(val_tmp, val, val_len);
   val_tmp[val_len] = '\0';
   fstr2cstr(val_tmp, val_len);

   rometextdb_set_string_value(reinterpret_cast<void*>(*romedb), *row - 1, *column - 1, val_tmp);

   delete [] val_tmp;
}

void rometextdb_get_type__(long long *romedb, char *ret, const int ret_len)
{
   const char *p = rometextdb_get_type(reinterpret_cast<void*>(*romedb));
   if (!p) {
      if (ret_len > 0) {
         strcpy(ret, "");
         cstr2fstr(ret, ret_len);
      }
      return;
   }
   if (strlen(p) <= static_cast<unsigned int>(ret_len)) {
      strcpy(ret, p);
      cstr2fstr(ret, ret_len);
   }
}

void rometextdb_get_description__(long long *romedb, char *ret, const int ret_len)
{
   const char *p = rometextdb_get_description(reinterpret_cast<void*>(*romedb));
   if (!p) {
      if (ret_len > 0) {
         strcpy(ret, "");
         cstr2fstr(ret, ret_len);
      }
      return;
   }
   if (strlen(p) <= static_cast<unsigned int>(ret_len)) {
      strcpy(ret, p);
      cstr2fstr(ret, ret_len);
   }
}


void romedb_init_(char *name, int *type, char *connection, long long *ret, const int name_len, const int connection_len)
{
   romedb_init__(name, type, connection, ret, name_len, connection_len);
}

void romedb_disconnect_(long long *romedb)
{
   romedb_disconnect__(romedb);
}

void romedb_read_(long long *romedb, char *dataBasePath, long long int *runNumber,
                  long long int *eventNumber, int *ret, const int dataBasePath_len)
{
   romedb_read__(romedb, dataBasePath, runNumber, eventNumber, ret, dataBasePath_len);
}

void romedb_write_(long long *romedb, char *dataBasePath, long long int *runNumber,
                   long long int *eventNumber, int *ret, const int dataBasePath_len)
{
   romedb_write__(romedb, dataBasePath, runNumber, eventNumber, ret, dataBasePath_len);
}

void romedb_get_int_value_(long long *romedb, int *row, int *column, int *ret)
{
   romedb_get_int_value__(romedb, row, column, ret);
}

void romedb_get_double_value_(long long *romedb, int *row, int *column, double *ret)
{
   romedb_get_double_value__(romedb, row, column, ret);
}

void romedb_get_float_value_(long long *romedb, int *row, int *column, float *ret)
{
   romedb_get_float_value__(romedb, row, column, ret);
}

void romedb_get_string_value_(long long *romedb, int *row, int *column, char *ret, const int ret_len)
{
   romedb_get_string_value__(romedb, row, column, ret, ret_len);
}

void romedb_set_int_value_(long long *romedb, int *row, int *column, int *val)
{
   romedb_set_int_value__(romedb, row, column, val);
}

void romedb_set_double_value_(long long *romedb, int *row, int *column, double *val)
{
   romedb_set_double_value__(romedb, row, column, val);
}

void romedb_set_float_value_(long long *romedb, int *row, int *column, float *val)
{
   romedb_set_float_value__(romedb, row, column, val);
}

void romedb_set_string_value_(long long *romedb, int *row, int *column, char *val, const int val_len)
{
   romedb_set_string_value__(romedb, row, column, val, val_len);
}

void romedb_get_type_(long long *romedb, char *ret, const int ret_len)
{
   romedb_get_type__(romedb, ret, ret_len);
}

void romedb_get_description_(long long *romedb, char *ret, const int ret_len)
{
   romedb_get_description__(romedb, ret, ret_len);
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

void romesqldb_get_int_value_(long long *romedb, int *row, int *column, int *ret)
{
   romesqldb_get_int_value__(romedb, row, column, ret);
}

void romesqldb_get_double_value_(long long *romedb, int *row, int *column, double *ret)
{
   romesqldb_get_double_value__(romedb, row, column, ret);
}

void romesqldb_get_float_value_(long long *romedb, int *row, int *column, float *ret)
{
   romesqldb_get_float_value__(romedb, row, column, ret);
}

void romesqldb_get_string_value_(long long *romedb, int *row, int *column, char *ret, const int ret_len)
{
   romesqldb_get_string_value__(romedb, row, column, ret, ret_len);
}

void romesqldb_set_int_value_(long long *romedb, int *row, int *column, int *val)
{
   romesqldb_set_int_value__(romedb, row, column, val);
}

void romesqldb_set_double_value_(long long *romedb, int *row, int *column, double *val)
{
   romesqldb_set_double_value__(romedb, row, column, val);
}

void romesqldb_set_float_value_(long long *romedb, int *row, int *column, float *val)
{
   romesqldb_set_float_value__(romedb, row, column, val);
}

void romesqldb_set_string_value_(long long *romedb, int *row, int *column, char *val, const int val_len)
{
   romesqldb_set_string_value__(romedb, row, column, val, val_len);
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

void rometextdb_init_(char *name, char *connection, long long *ret, const int name_len, const int connection_len)
{
   rometextdb_init__(name, connection, ret, name_len, connection_len);
}

void rometextdb_disconnect_(long long *romedb)
{
   rometextdb_disconnect__(romedb);
}

void rometextdb_read_(long long *romedb, char *dataBasePath, long long int *runNumber,
                      long long int *eventNumber, int *ret, const int dataBasePath_len)
{
   rometextdb_read__(romedb, dataBasePath, runNumber, eventNumber, ret, dataBasePath_len);
}

void rometextdb_write_(long long *romedb, char *dataBasePath, long long int *runNumber,
                       long long int *eventNumber, int *ret, const int dataBasePath_len)
{
   rometextdb_write__(romedb, dataBasePath, runNumber, eventNumber, ret, dataBasePath_len);
}

void rometextdb_get_int_value_(long long *romedb, int *row, int *column, int *ret)
{
   rometextdb_get_int_value__(romedb, row, column, ret);
}

void rometextdb_get_double_value_(long long *romedb, int *row, int *column, double *ret)
{
   rometextdb_get_double_value__(romedb, row, column, ret);
}

void rometextdb_get_float_value_(long long *romedb, int *row, int *column, float *ret)
{
   rometextdb_get_float_value__(romedb, row, column, ret);
}

void rometextdb_get_string_value_(long long *romedb, int *row, int *column, char *ret, const int ret_len)
{
   rometextdb_get_string_value__(romedb, row, column, ret, ret_len);
}

void rometextdb_set_int_value_(long long *romedb, int *row, int *column, int *val)
{
   rometextdb_set_int_value__(romedb, row, column, val);
}

void rometextdb_set_double_value_(long long *romedb, int *row, int *column, double *val)
{
   rometextdb_set_double_value__(romedb, row, column, val);
}

void rometextdb_set_float_value_(long long *romedb, int *row, int *column, float *val)
{
   rometextdb_set_float_value__(romedb, row, column, val);
}

void rometextdb_set_string_value_(long long *romedb, int *row, int *column, char *val, const int val_len)
{
   rometextdb_set_string_value__(romedb, row, column, val, val_len);
}

void rometextdb_get_type_(long long *romedb, char *ret, const int ret_len)
{
   rometextdb_get_type__(romedb, ret, ret_len);
}

void rometextdb_get_description_(long long *romedb, char *ret, const int ret_len)
{
   rometextdb_get_description__(romedb, ret, ret_len);
}


void _romedb_init_(char *name, int *type, char *connection, long long *ret, const int name_len, const int connection_len)
{
   romedb_init__(name, type, connection, ret, name_len, connection_len);
}

void _romedb_disconnect_(long long *romedb)
{
   romedb_disconnect__(romedb);
}

void _romedb_read_(long long *romedb, char *dataBasePath, long long int *runNumber,
                   long long int *eventNumber, int *ret, const int dataBasePath_len)
{
   romedb_read__(romedb, dataBasePath, runNumber, eventNumber, ret, dataBasePath_len);
}

void _romedb_write_(long long *romedb, char *dataBasePath, long long int *runNumber,
                    long long int *eventNumber, int *ret, const int dataBasePath_len)
{
   romedb_write__(romedb, dataBasePath, runNumber, eventNumber, ret, dataBasePath_len);
}

void _romedb_get_int_value_(long long *romedb, int *row, int *column, int *ret)
{
   romedb_get_int_value__(romedb, row, column, ret);
}

void _romedb_get_double_value_(long long *romedb, int *row, int *column, double *ret)
{
   romedb_get_double_value__(romedb, row, column, ret);
}

void _romedb_get_float_value_(long long *romedb, int *row, int *column, float *ret)
{
   romedb_get_float_value__(romedb, row, column, ret);
}

void _romedb_get_string_value_(long long *romedb, int *row, int *column, char *ret, const int ret_len)
{
   romedb_get_string_value__(romedb, row, column, ret, ret_len);
}

void _romedb_set_int_value_(long long *romedb, int *row, int *column, int *val)
{
   romedb_set_int_value__(romedb, row, column, val);
}

void _romedb_set_double_value_(long long *romedb, int *row, int *column, double *val)
{
   romedb_set_double_value__(romedb, row, column, val);
}

void _romedb_set_float_value_(long long *romedb, int *row, int *column, float *val)
{
   romedb_set_float_value__(romedb, row, column, val);
}

void _romedb_set_string_value_(long long *romedb, int *row, int *column, char *val, const int val_len)
{
   romedb_set_string_value__(romedb, row, column, val, val_len);
}

void _romedb_get_type_(long long *romedb, char *ret, const int ret_len)
{
   romedb_get_type__(romedb, ret, ret_len);
}

void _romedb_get_description_(long long *romedb, char *ret, const int ret_len)
{
   romedb_get_description__(romedb, ret, ret_len);
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

void _romesqldb_get_int_value_(long long *romedb, int *row, int *column, int *ret)
{
   romesqldb_get_int_value__(romedb, row, column, ret);
}

void _romesqldb_get_double_value_(long long *romedb, int *row, int *column, double *ret)
{
   romesqldb_get_double_value__(romedb, row, column, ret);
}

void _romesqldb_get_float_value_(long long *romedb, int *row, int *column, float *ret)
{
   romesqldb_get_float_value__(romedb, row, column, ret);
}

void _romesqldb_get_string_value_(long long *romedb, int *row, int *column, char *ret, const int ret_len)
{
   romesqldb_get_string_value__(romedb, row, column, ret, ret_len);
}

void _romesqldb_set_int_value_(long long *romedb, int *row, int *column, int *val)
{
   romesqldb_set_int_value__(romedb, row, column, val);
}

void _romesqldb_set_double_value_(long long *romedb, int *row, int *column, double *val)
{
   romesqldb_set_double_value__(romedb, row, column, val);
}

void _romesqldb_set_float_value_(long long *romedb, int *row, int *column, float *val)
{
   romesqldb_set_float_value__(romedb, row, column, val);
}

void _romesqldb_set_string_value_(long long *romedb, int *row, int *column, char *val, const int val_len)
{
   romesqldb_set_string_value__(romedb, row, column, val, val_len);
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

void _rometextdb_init_(char *name, char *connection, long long *ret, const int name_len, const int connection_len)
{
   rometextdb_init__(name, connection, ret, name_len, connection_len);
}

void _rometextdb_disconnect_(long long *romedb)
{
   rometextdb_disconnect__(romedb);
}

void _rometextdb_read_(long long *romedb, char *dataBasePath, long long int *runNumber,
                       long long int *eventNumber, int *ret, const int dataBasePath_len)
{
   rometextdb_read__(romedb, dataBasePath, runNumber, eventNumber, ret, dataBasePath_len);
}

void _rometextdb_write_(long long *romedb, char *dataBasePath, long long int *runNumber,
                        long long int *eventNumber, int *ret, const int dataBasePath_len)
{
   rometextdb_write__(romedb, dataBasePath, runNumber, eventNumber, ret, dataBasePath_len);
}

void _rometextdb_get_int_value_(long long *romedb, int *row, int *column, int *ret)
{
   rometextdb_get_int_value__(romedb, row, column, ret);
}

void _rometextdb_get_double_value_(long long *romedb, int *row, int *column, double *ret)
{
   rometextdb_get_double_value__(romedb, row, column, ret);
}

void _rometextdb_get_float_value_(long long *romedb, int *row, int *column, float *ret)
{
   rometextdb_get_float_value__(romedb, row, column, ret);
}

void _rometextdb_get_string_value_(long long *romedb, int *row, int *column, char *ret, const int ret_len)
{
   rometextdb_get_string_value__(romedb, row, column, ret, ret_len);
}

void _rometextdb_set_int_value_(long long *romedb, int *row, int *column, int *val)
{
   rometextdb_set_int_value__(romedb, row, column, val);
}

void _rometextdb_set_double_value_(long long *romedb, int *row, int *column, double *val)
{
   rometextdb_set_double_value__(romedb, row, column, val);
}

void _rometextdb_set_float_value_(long long *romedb, int *row, int *column, float *val)
{
   rometextdb_set_float_value__(romedb, row, column, val);
}

void _rometextdb_set_string_value_(long long *romedb, int *row, int *column, char *val, const int val_len)
{
   rometextdb_set_string_value__(romedb, row, column, val, val_len);
}

void _rometextdb_get_type_(long long *romedb, char *ret, const int ret_len)
{
   rometextdb_get_type__(romedb, ret, ret_len);
}

void _rometextdb_get_description_(long long *romedb, char *ret, const int ret_len)
{
   rometextdb_get_description__(romedb, ret, ret_len);
}
