// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMESQLDataBase
//
//  SQLDataBase access.
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////
#include <RConfig.h>
#include <TObjString.h>
#if defined ( R__UNIX )
#   include <unistd.h>
#endif
#if defined ( HAVE_MYSQL )
#   include "ROMEMySQL.h"
#endif
#if defined ( HAVE_PGSQL )
#   include "ROMEPgSQL.h"
#endif
#if defined ( HAVE_SQLITE )
#   include "ROMESQLite.h"
#endif
#if defined ( HAVE_SQLITE3 )
#   include "ROMESQLite3.h"
#endif
#include "ROMEPath.h"
#include "ROMESQLDataBase.h"

const char RSQLDB_STR[] = "RomeWasNotBuiltInADay";
const int RSQLDB_STR_LEN = strlen(RSQLDB_STR);
const char *const kRunNumberReplace = "R_UN_NUMBE_R";
const char *const kEventNumberReplace = "E_VENT_NUMBE_R";

ClassImp(ROMESQLDataBase)

namespace
{
inline Bool_t InRange(Int_t value, Int_t b1, Int_t b2) {
   return TMath::Min(b1, b2) <= value && value <= TMath::Max(b1, b2);
}
}

//______________________________________________________________________________
ROMESQLDataBase::ROMESQLDataBase()
: ROMEDataBase()
, fSelectFieldList("")
, fInsertFieldList("")
, fSetFieldList("")
, fFromPhrase("")
, fWherePhrase("")
, fAdInsertFields("")
, fAdInsertValues("")
, fSQL(0)
, fDBMSType("")
, fCurrentCache(0)
, fLastRunNumber(0)
, fLastEventNumber(0)
{
   Int_t i;
   for (i = 0; i < kNumberOfReadCache; i++) {
      fPathCache[i] = 0;
   }
   sprintf(fLastRunNumberString, "0");
   sprintf(fLastEventNumberString, "0");
}

//______________________________________________________________________________
ROMESQLDataBase::~ROMESQLDataBase()
{
   Int_t i;
   for (i = 0; i < kNumberOfReadCache; i++) {
      SafeDelete(fPathCache[i]);
   }
}

//______________________________________________________________________________
void ROMESQLDataBase::ResetPhrase()
{
   fSelectFieldList.Resize(0);
   fInsertFieldList.Resize(0);
   fSetFieldList.Resize(0);
   fFromPhrase.Resize(0);
   fWherePhrase.Resize(0);
   fAdInsertFields.Resize(0);
   fAdInsertValues.Resize(0);
}

//______________________________________________________________________________
Bool_t ROMESQLDataBase::DecodeDBConstraint(const char *currentTableName, const char *nextTableName,
                                           const char *dbConstraint,
                                           Long64_t runNumber, Long64_t eventNumber, const char *currentIdName,
                                           const char *currentIdxName)
{
   ROMEString value = dbConstraint;
   ROMEPath *dbpath = new ROMEPath();
   int is1, ie1, is2, ie2, is3, ie3;
   int itmp;
   ROMEString temp = nextTableName;
   temp += "=\"";
   ROMEString val = currentTableName;
   val += ".";
   ROMEString pathString;
   ROMEString newpathString;
   ROMEString tname;

   // constraint is a path
   if ((is1 = value.Index(temp, temp.Length(), 0, TString::kIgnoreCase)) != -1) {
      if ((ie1 = value.Index("\"", 1, is1 + temp.Length(), TString::kIgnoreCase)) != -1) {
         value = value(is1 + temp.Length(), ie1 - is1 - temp.Length());
         if (value(value.Length() - 1) != '/') {
            value += "/";
         }
         temp.Remove(temp.Length() - 2, 2);
         while ((is1 = value.Index("(@@", 3, 0, TString::kIgnoreCase)) != -1) {
            if ((ie1 = value.Index(")", 1, is1 + 3, TString::kIgnoreCase)) == -1) {
               ie1 = value.Length();
            } else {
               value.Remove(ie1, 1);
            }
            pathString = value(is1 + 3, ie1 - is1 - 3);
            if (pathString.Contains("_")) {
               //"[id=(@@AAA_id)]" -> "[id=currentTableName.AAA_id]"
               value.Remove(is1, 3);
               value.Insert(is1, val);
            } else {
               //[(@@id)] -> [id=currentTableName.AAA_id]
               ie2 = 0;
               is3 = 1;
               ie3 = value.Length();
               while ((is2 = value.Index("/", 1, ie2, TString::kIgnoreCase)) != -1) {
                  if (is2 > is1) {
                     break;
                  }
                  is3 = is2 + 1;
                  ie2 = value.Length();
                  if ((itmp = value.Index("/", 1, is3, TString::kIgnoreCase)) != -1) {
                     ie2 = TMath::Min(ie2, itmp + 1);
                  }
                  if ((itmp = value.Index("{", 1, is3, TString::kIgnoreCase)) != -1) {
                     ie2 = TMath::Min(ie2, itmp + 1);
                  }
                  if ((itmp = value.Index("(", 1, is3, TString::kIgnoreCase)) != -1) {
                     ie2 = TMath::Min(ie2, itmp + 1);
                  }
                  if ((itmp = value.Index("[", 1, is3, TString::kIgnoreCase)) != -1) {
                     ie2 = TMath::Min(ie2, itmp + 1);
                  }
                  ie3 = ie2 - 1;
               }
               tname = value(is3, ie3 - is3);
               newpathString = pathString + "=" + currentTableName + "." + tname + "_" + pathString;
               value.Remove(is1, ie1 - is1);
               value.Insert(is1, newpathString);
            }
         }
         if (!dbpath->Decode(value, runNumber, eventNumber)) {
            delete dbpath;
            return false;
         }
         if (!MakePhrase(dbpath, runNumber, eventNumber)) {
            delete dbpath;
            return false;
         }
         //add relation
         if (strlen(dbpath->GetTableIDNameAt(dbpath->GetNumberOfTables() - 1))) {
            if (fWherePhrase.Length()) {
               fWherePhrase += " AND ";
            }
            fWherePhrase += temp + "." + "id" /*dbpath->GetTableIDNameAt(dbpath->GetNumberOfTables()-1) */  +
                  "=" + dbpath->GetTableNameAt(dbpath->GetNumberOfTables() - 1) + "." + temp + "_" +
                  dbpath->GetTableIDNameAt(dbpath->GetNumberOfTables() - 1);
         }
         if (strlen(dbpath->GetTableIDXNameAt(dbpath->GetNumberOfTables() - 1))) {
            if (fWherePhrase.Length()) {
               fWherePhrase += " AND ";
            }
            fWherePhrase += temp + "." + "idx" /* dbpath->GetTableIDXNameAt(dbpath->GetNumberOfTables()-1) */  +
                  "=" + dbpath->GetTableNameAt(dbpath->GetNumberOfTables() - 1) + "." + temp +
                  "_" + dbpath->GetTableIDXNameAt(dbpath->GetNumberOfTables() - 1);
         }
      }
      //add relation to the first @constraint table
      if (strlen(currentIdName)) {
         if (fWherePhrase.Length()) {
            fWherePhrase += " AND ";
         }
         fWherePhrase += dbpath->GetTableNameAt(0);
         fWherePhrase += ".";
         fWherePhrase += "id" /* dbpath->GetTableIDNameAt(dbpath->GetNumberOfTables()-1) */ ;
         fWherePhrase += "=";
         fWherePhrase += currentTableName;
         fWherePhrase += ".";
         fWherePhrase += dbpath->GetTableNameAt(0);
         fWherePhrase += "_";
         fWherePhrase += currentIdName;
      }
      if (strlen(currentIdxName)) {
         if (fWherePhrase.Length()) {
            fWherePhrase += " AND ";
         }
         fWherePhrase += dbpath->GetTableNameAt(0);
         fWherePhrase += ".";
         fWherePhrase += "idx" /* dbpath->GetTableIDXNameAt(dbpath->GetNumberOfTables()-1) */ ;
         fWherePhrase += "=";
         fWherePhrase += currentTableName;
         fWherePhrase += ".";
         fWherePhrase += dbpath->GetTableNameAt(0);
         fWherePhrase += "_";
         fWherePhrase += currentIdxName;
      }
   } else {
      ROMEPrint::Warning("Warning: DB constraint was not found for %s\n", nextTableName);
      delete dbpath;
      return false;
   }

   delete dbpath;
   return true;
}

//______________________________________________________________________________
Bool_t ROMESQLDataBase::MakePhrase(ROMEPath * path, Long64_t runNumber, Long64_t eventNumber)
{
   ROMEString sqlQuery;
   ROMEString sqlResult;
   ROMEString temp;
   ROMEString separator;
   int iTable = 0;
   int iConstraint;
   int iField;
   //field list
   if (!fSelectFieldList.Length()) {
      separator = "";
      for (iField = path->GetFieldIndexAt(0);
           !path->IsFieldArray() || InRange(iField, path->GetFieldIndexAt(0), path->GetFieldIndexAt(1));
           iField += path->GetFieldIndexAt(2)) {
         temp = path->GetFieldName();
         if (path->IsFieldArray()) {
            temp += "__";
            temp += iField;
         }
         fSelectFieldList += separator + path->GetTableNameAt(path->GetNumberOfTables() - 1) + "." + temp;
         fInsertFieldList += separator + temp;
         fSetFieldList += separator + temp + RSQLDB_STR;
         separator = ",";
         if (!path->IsFieldArray()) {
            break;
         }
      }
   }

   ROMEPrint::Debug("\n******************************************************************************\n");
   for (iTable = 0; iTable < path->GetNumberOfTables(); iTable++) {
      ROMEPrint::Debug("Table\t: %s\n", path->GetTableNameAt(iTable));
   }
   ROMEPrint::Debug("Field\t: %s\n\n", path->GetFieldName());
   ROMEPrint::Debug("following relations...\n");
   // start following relation.
   ROMEString tmp1, tmp2;
   for (iTable = 0; iTable < path->GetNumberOfTables(); iTable++) {
      ROMEPrint::Debug("Level-%d: %s\n", iTable, path->GetTableNameAt(iTable));
      //add table to FROM phrase
      tmp1 = ",";
      tmp1 += path->GetTableNameAt(iTable);
      tmp2 = ",";
      tmp2 += path->GetTableNameAt(iTable);
      tmp2 += ",";
      if (fFromPhrase != path->GetTableNameAt(iTable) && !fFromPhrase.EndsWith(tmp1) &&
          !fFromPhrase.Contains(tmp2)) {
         if (fFromPhrase.Length()) {
            fFromPhrase += ",";
         }
         fFromPhrase += path->GetTableNameAt(iTable);
      }

      //add constraint
      if (strlen(path->GetTableConstraintAt(iTable))) {
         if (!path->DecodeConstraint(path->GetTableConstraintAt(iTable))) {
            return false;
         }
         for (iConstraint = 0; iConstraint < path->GetNumberOfConstraints(); iConstraint++) {
            if (fAdInsertFields.Length()) {
               separator = ",";
            } else {
               separator = "";
            }
            fAdInsertFields += separator + path->GetConstraintFieldAt(iConstraint);
            fAdInsertValues += separator + path->GetConstraintValueAt(iConstraint);
            if (fWherePhrase.Length()) {
               fWherePhrase += " AND ";
            }
            fWherePhrase += path->GetTableNameAt(iTable);
            fWherePhrase += ".";
            fWherePhrase += path->GetConstraintFieldAt(iConstraint);
            fWherePhrase += "=";
            fWherePhrase += path->GetConstraintValueAt(iConstraint);
         }
      }

      //add dbconstraint
      if (strlen(path->GetTableDBConstraintAt(iTable))) {
         //read dbconstraint
         sqlQuery = "SELECT ";
         sqlQuery += path->GetTableNameAt(iTable);
         sqlQuery += ".";
         sqlQuery += path->GetTableDBConstraintAt(iTable);
         sqlQuery += " FROM ";
         sqlQuery += fFromPhrase;
         sqlQuery += " WHERE ";
         sqlQuery += fWherePhrase;
         sqlQuery += " LIMIT 1;";

         ROMEString tmpString;
         tmpString.SetFormatted(R_LLD, eventNumber);
         sqlQuery.ReplaceAll(kEventNumberReplace, tmpString);
         // replace # with the current run number
         tmpString.SetFormatted(R_LLD, runNumber);
         sqlQuery.ReplaceAll(kRunNumberReplace, tmpString);

         if (!fSQL->MakeQuery(sqlQuery.Data(), true)) {
            ROMEPrint::Error("Wrong path for data base constraint : %s\n", path->GetTableDBConstraintAt(iTable));
#if 0
            fSQL->FreeResult();
#endif
            return false;
         }
         if (!fSQL->NextRow()) {
            ROMEPrint::Error("Database constraint '%s' was not found\n", path->GetTableDBConstraintAt(iTable));
            fSQL->FreeResult();
            return false;
         }
         temp = fSQL->GetField(0);
         if (!DecodeDBConstraint(path->GetTableNameAt(iTable), path->GetTableNameAt(iTable + 1), temp.Data(),
                                 runNumber, eventNumber, path->GetTableIDNameAt(iTable),
                                 path->GetTableIDXNameAt(iTable))) {
            fSQL->FreeResult();
            return false;
         }
         fSQL->FreeResult();
      }

      //add relation to the next table
      if (iTable < path->GetNumberOfTables() - 1 && !strlen(path->GetTableDBConstraintAt(iTable))) {
         if (strlen(path->GetTableIDNameAt(iTable))) {
            if (fWherePhrase.Length()) {
               fWherePhrase += " AND ";
            }
            fWherePhrase += path->GetTableNameAt(iTable + 1);
            fWherePhrase += ".";
            fWherePhrase += "id" /* path->GetTableIDNameAt(iTable) */ ;
            fWherePhrase += "=";
            fWherePhrase += path->GetTableNameAt(iTable);
            fWherePhrase += ".";
            fWherePhrase += path->GetTableNameAt(iTable + 1);
            fWherePhrase += "_";
            fWherePhrase += path->GetTableIDNameAt(iTable);
         }
         if (strlen(path->GetTableIDXNameAt(iTable))) {
            if (fWherePhrase.Length()) {
               fWherePhrase += " AND ";
            }
            fWherePhrase += path->GetTableNameAt(iTable + 1);
            fWherePhrase += ".";
            fWherePhrase += "idx" /*path->GetTableIDXNameAt(iTable) */ ;
            fWherePhrase += "=";
            fWherePhrase += path->GetTableNameAt(iTable);
            fWherePhrase += ".";
            fWherePhrase += path->GetTableNameAt(iTable + 1);
            fWherePhrase += "_";
            fWherePhrase += path->GetTableIDXNameAt(iTable);
         }
      }
      ROMEPrint::Debug("\t  ... OK\n");
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMESQLDataBase::Init(const char *name, const char * /*dataBase */ , const char *connection)
{
   ROMEString path = connection;
   ROMEString server;
   ROMEString user;
   ROMEString passwd;
   ROMEString database;
   ROMEString port;
   ROMEString prompt;
   int is, ie;
   int istart;

   fName = name;

   path.StripSpaces();

   //decode dataBasePath
   if ((istart = path.Index("://", 3, 0, TString::kIgnoreCase)) == -1) {
      ROMEPrint::Error("Wrong path for SQL database : %s\n", path.Data());
      ROMEPrint::Error("Path should look like,\n");
      ROMEPrint::Error("mysql://username:password@servername:port/database\n");
      ROMEPrint::Error("postgresql://username:password@servername:port/database\n");
      ROMEPrint::Error("sqlite://filename\n");
      ROMEPrint::Error("sqlite3://filename\n");
      return false;
   }
   fDBMSType = path(0, istart);
   fDBMSType.ToLower();

   istart += 3;
   if (fDBMSType == "mysql" || fDBMSType == "postgresql") {
      //search for user
      is = istart;
      if ((ie = path.Index("@", 1, is, TString::kIgnoreCase)) == -1) {
         user = "";
         passwd = "";
      } else {
         istart = ie + 1;
         user = path(is, ie - is);
         //search for passwd
         if ((ie = user.Index(":", 1, 0, TString::kIgnoreCase)) == -1) {
            passwd = "";
         } else {
            passwd = user(ie + 1, user.Length() - ie - 1);
            user.Remove(ie, user.Length() - ie);
         }
      }
      //search for server
      is = istart;
      if ((ie = path.Index("/", 1, is, TString::kIgnoreCase)) == -1) {
         database = "";
      } else {
         istart = ie + 1;
         server = path(is, ie - is);
         //search for port
         if ((ie = server.Index(":", 1, 0, TString::kIgnoreCase)) == -1) {
            port = "0";
         } else {
            port = server(ie + 1, server.Length() - ie - 1);
            server.Remove(ie, server.Length() - ie);
         }
         //search for database
         database = path(istart, path.Length());
      }

      if (passwd.Length() == 1 && passwd(0) == '?') {
#if defined ( R__UNIX )
         prompt = user + "@" + server + "'s password: ";
         passwd = getpass(prompt.Data());
#endif
         ROMEPrint::Debug("******  SQL Connection  ******\n");
         ROMEPrint::Debug("server   : %s\n", server.Data());
         ROMEPrint::Debug("user     : %s\n", user.Data());
         ROMEPrint::Debug("passwd   : %s\n", passwd.Data());
         ROMEPrint::Debug("database : %s\n", database.Data());
         ROMEPrint::Debug("port     : %s\n", port.Data());
      }
   } else if (fDBMSType == "sqlite" || fDBMSType == "sqlite3") {
      server = path(istart, path.Length());
      user = "";
      passwd = "";
      database = "";
      port = "";
      ROMEPrint::Debug("******  SQL Connection  ******\n");
      ROMEPrint::Debug("filename  : %s\n", server.Data());
   } else {
      ROMEPrint::Error("Unknown DBMS type %s\n", fDBMSType.Data());
   }

   if (fDBMSType == "mysql") {
#if defined ( HAVE_MYSQL )
      fSQL = new ROMEMySQL();
#else
      LinkError();
      return false;
#endif
   } else if (fDBMSType == "postgresql") {
#if defined ( HAVE_PGSQL )
      fSQL = new ROMEPgSQL();
#else
      LinkError();
      return false;
#endif
   } else if (fDBMSType == "sqlite") {
#if defined ( HAVE_SQLITE )
      fSQL = new ROMESQLite();
#else
      LinkError();
      return false;
#endif
   } else if (fDBMSType == "sqlite3") {
#if defined ( HAVE_SQLITE3 )
      fSQL = new ROMESQLite3();
#else
      LinkError();
      return false;
#endif
   } else {
      ROMEPrint::Error("Error: DBMS \"%s\" is not supported\n", fDBMSType.Data());
      return false;
   }

   this->ResetPhrase();

   if (!fSQL->Connect(server.Data(), user.Data(), passwd.Data(), database.Data(), port.Data())) {
      return false;
   }

   return true;
}

//______________________________________________________________________________
Bool_t ROMESQLDataBase::Read(ROMEStr2DArray * values, const char *dataBasePath, Long64_t runNumber,
                             Long64_t eventNumber)
{
   int iField, iOrder;
   int iLastOrder = 0;
   int iArray, jArray;
   int iCount;
   bool keepCursor = false;
   ROMEPath *path = 0;
   ROMEString fieldName;
   ROMEString sqlQuery;
   ROMEString orderField;
   Bool_t cacheFound = kFALSE;

   // check cache
   for (iCount = 0; iCount < kNumberOfReadCache; iCount++) {
      if (fDBPathCache[iCount] == dataBasePath) {
         cacheFound = kTRUE;
         sqlQuery = fQueryCache[iCount];
         path = fPathCache[iCount];
         break;
      }
   }

   if (!cacheFound) {
      // store cache
      fDBPathCache[fCurrentCache] = dataBasePath;
      delete fPathCache[fCurrentCache];
      path = fPathCache[fCurrentCache] = new ROMEPath();

      ROMEString pathString = dataBasePath;
      pathString.ReplaceAll("##", kEventNumberReplace);
      pathString.ReplaceAll("#", kRunNumberReplace);

      if (!path->Decode(pathString.Data(), runNumber, eventNumber)) {
         ROMEPrint::Error("Path decode error : %s\n", dataBasePath);
         return false;
      }

      this->ResetPhrase();
      if (!MakePhrase(path, runNumber, eventNumber)) {
         ROMEPrint::Error("Invalid input for database read.\n");
         return false;
      }
      if (!fFromPhrase.Contains(path->GetOrderTableName())) {
         ROMEPrint::Error("Invalid path for database read.\n");
         ROMEPrint::Error("order tabele '%s' should be in path\n", path->GetOrderTableName());
         return false;
      }
      if (path->IsOrderArray()) {
         orderField += strlen(path->GetOrderTableName())? path->GetOrderTableName() :
               path->GetTableNameAt(path->GetNumberOfTables() - 1);
         orderField += ".";
         orderField += strlen(path->GetOrderFieldName())? path->GetOrderFieldName() : "idx";
      }

      sqlQuery = "SELECT ";
      sqlQuery += fSelectFieldList;
      if (path->IsOrderArray()) {
         sqlQuery += "," + orderField;
      }
      sqlQuery += " FROM " + fFromPhrase;
      if (fWherePhrase.Length()) {
         sqlQuery += " WHERE " + fWherePhrase;
      }
      if (orderField.Length()) {
         if (sqlQuery.Contains("WHERE")) {
            sqlQuery += " AND ";
         } else {
            sqlQuery += " WHERE ";
         }
         sqlQuery += "(" + orderField + " BETWEEN ";
         sqlQuery += TMath::Min(path->GetOrderIndexAt(0), path->GetOrderIndexAt(1));
         sqlQuery += " AND ";
         sqlQuery += TMath::Max(path->GetOrderIndexAt(0), path->GetOrderIndexAt(1));
         sqlQuery += ")";
         if (path->GetOrderIndexAt(2) != 1) {
            sqlQuery += " AND ((" + orderField + "-";
            sqlQuery += path->GetOrderIndexAt(0);
            sqlQuery += ") % ";
            sqlQuery += path->GetOrderIndexAt(2);
            sqlQuery += ")=0 ";
         }
         sqlQuery += " ORDER BY " + orderField + " ";
         if (path->GetOrderIndexAt(2) < 0) {
            sqlQuery += " DESC ";
         }
      }
      sqlQuery += ";";
      // store cache
      fQueryCache[fCurrentCache] = sqlQuery;
      fCurrentCache = (fCurrentCache + 1) % kNumberOfReadCache;
   }

   // replace ## with the current run number
   if (fLastRunNumber != runNumber) {
      sprintf(fLastRunNumberString, R_LLD, runNumber);
      fLastRunNumber = runNumber;
   }
   sqlQuery.ReplaceAll(kRunNumberReplace, fLastRunNumberString);

   if (fLastEventNumber != eventNumber) {
      sprintf(fLastEventNumberString, R_LLD, eventNumber);
      fLastEventNumber = eventNumber;
   }
   sqlQuery.ReplaceAll(kEventNumberReplace, fLastEventNumberString);

   if (!fSQL->MakeQuery(sqlQuery.Data(), true)) {
      ROMEPrint::Error("Invalid input for database read.\n");
#if 0
      fSQL->FreeResult();
#endif
      return false;
   }
   if (!fSQL->GetNumberOfRows()) {
      ROMEPrint::Warning("Warning: %s.%s was not found. Default value will be used.\n",
                         path->GetTableNameAt(path->GetNumberOfTables() - 1), path->GetFieldName());
      fSQL->FreeResult();
      return !fTreatWarningAsError;
   }

   const Int_t orderIndex[3] = { path->GetOrderIndexAt(0), path->GetOrderIndexAt(1), path->GetOrderIndexAt(2) };
   const Int_t fieldIndex[3] = { path->GetFieldIndexAt(0), path->GetFieldIndexAt(1), path->GetFieldIndexAt(2) };

   for (iOrder = orderIndex[0], iArray = 0; !path->IsOrderArray() || InRange(iOrder, orderIndex[0], orderIndex[1]);
        iOrder += orderIndex[2], iArray++) {
      if (!keepCursor) {
         if (!fSQL->NextRow()) {
            ROMEPrint::Warning("Warning: some records were not found in %s\n",
                               path->GetTableNameAt(path->GetNumberOfTables() - 1));
            fSQL->FreeResult();
            return true;
         }
      }
      keepCursor = false;

      if (path->IsOrderArray()) {
         // check number of records which have the same order number.
         iCount = 0;
         while (TMath::Sign(atoi(fSQL->GetField(fSQL->GetNumberOfFields() - 1)), orderIndex[2])
                < TMath::Sign(iOrder, orderIndex[2])) {
            if (!fSQL->NextRow()) {
               ROMEPrint::Warning("Warning: some records were not found in %s\n",
                                  path->GetTableNameAt(path->GetNumberOfTables() - 1));
               fSQL->FreeResult();
               return true;
            }
            iCount++;
         }
         if (iCount) {
            ROMEPrint::Warning("Warning: %s  has %d records which satisfy %s.%s=%d\n",
                               path->GetTableNameAt(path->GetNumberOfTables() - 1), iCount + 1,
                               path->GetOrderTableName(), path->GetOrderFieldName(), iLastOrder);
         }

         // check if the record exists
         if (TMath::Sign(iOrder, orderIndex[2])
             < TMath::Sign(atoi(fSQL->GetField(fSQL->GetNumberOfFields() - 1)), orderIndex[2])) {
            ROMEPrint::Warning("Warning: %s.%s(%s.%s=%d) was not found. Default value will be used.\n",
                               path->GetTableNameAt(path->GetNumberOfTables() - 1), path->GetFieldName()
                               , path->GetOrderTableName(), path->GetOrderFieldName(), iOrder);
            keepCursor = true;
            continue;
         }
      }

      // Fill result
      for (iField = fieldIndex[0], jArray = 0; !path->IsFieldArray() || InRange(iField, fieldIndex[0], fieldIndex[1]);
           iField += fieldIndex[2], jArray++) {
         values->SetAt(fSQL->GetField(jArray), iArray, jArray);
         iLastOrder = iOrder;
         if (!path->IsFieldArray()) {
            break;
         }
      }
      if (!path->IsOrderArray()) {
         break;
      }
   }

   fSQL->FreeResult();
   return true;
}

//______________________________________________________________________________
Bool_t ROMESQLDataBase::Write(ROMEStr2DArray * values, const char *dataBasePath, Long64_t runNumber,
                              Long64_t eventNumber)
{
   int iField;
   int iOrder;
   ROMEPath *path = new ROMEPath();
   ROMEString fieldName;
   ROMEString sqlQuery;
   ROMEString temp;
   ROMEString separator;
   int iArray, jArray;
   int istart;
   bool exist;

   if (!path->Decode(dataBasePath, runNumber, eventNumber)) {
      ROMEPrint::Error("Path decode error : %s\n", dataBasePath);
      delete path;
      return false;
   }
//   path->Print();
   if (path->GetNumberOfTables() != 1) {
      ROMEPrint::Error("Wrong data base path : %s\n", dataBasePath);
      delete path;
      return false;
   }
   if (strstr(path->GetTableConstraintAt(path->GetNumberOfTables() - 1), "!=")) {
      ROMEPrint::Error(" \"!=\" can not be used for default values.\n");
      delete path;
      return false;
   }

   this->ResetPhrase();
   if (!MakePhrase(path, runNumber, eventNumber)) {
      ROMEPrint::Error("Invalid input for database write.\n");
      delete path;
      return false;
   }

#if defined ( USE_TRANSACTION )
   fSQL->StartTransaction("");
#endif
   for (iOrder = path->GetOrderIndexAt(0), iArray = 0;
        !path->IsOrderArray() || InRange(iOrder, path->GetOrderIndexAt(0), path->GetOrderIndexAt(1));
        iOrder += path->GetOrderIndexAt(2), iArray++) {
      //check if the row exists
      sqlQuery = "SELECT ";
      sqlQuery += fSelectFieldList;
      sqlQuery += " FROM ";
      sqlQuery += fFromPhrase;
      if (fWherePhrase.Length()) {
         sqlQuery += " WHERE " + fWherePhrase;
      }
      if (path->IsOrderArray()) {
         if (!sqlQuery.Contains("WHERE")) {
            sqlQuery += " WHERE ";
         } else {
            sqlQuery += " AND ";
         }
         sqlQuery += path->GetTableNameAt(path->GetNumberOfTables() - 1);
         sqlQuery += ".";
         sqlQuery += strlen(path->GetTableIDXNameAt(path->GetNumberOfTables() - 1)) ?
               path->GetTableIDXNameAt(path->GetNumberOfTables() - 1) : "idx";
         sqlQuery += "='";
         sqlQuery += iOrder;
         sqlQuery += "'";
      }
      sqlQuery += " LIMIT 1;";
      if (!fSQL->MakeQuery(sqlQuery.Data(), true)) {
         ROMEPrint::Error("Invalid input for database write.\n");
#if 0
         fSQL->FreeResult();
#endif
         delete path;
#if defined ( USE_TRANSACTION )
         fSQL->RollbackTransaction("");
#endif
         return false;
      }
      exist = fSQL->GetNumberOfRows() != 0;
      fSQL->FreeResult();

      if (!exist) {             // insert new record
         sqlQuery = "INSERT INTO ";
         sqlQuery += fFromPhrase;
         sqlQuery += " ( ";
         sqlQuery += fInsertFieldList;
         if (path->IsOrderArray()) {
            sqlQuery += "," +
                  strlen(path->GetTableIDXNameAt(path->GetNumberOfTables() - 1)) ?
                  path->GetTableIDXNameAt(path->GetNumberOfTables() - 1) : "idx";
         }
         if (fAdInsertFields.Length()) {
            sqlQuery += "," + fAdInsertFields;
         }
         sqlQuery += " ) VALUES ( ";
         separator = "";
         for (iField = path->GetFieldIndexAt(0), jArray = 0;
              !path->IsFieldArray() || InRange(iField, path->GetFieldIndexAt(0), path->GetFieldIndexAt(1));
              iField += path->GetFieldIndexAt(2), jArray++) {
            sqlQuery += separator + "'" + values->At(iArray, jArray) + "'";
            separator = ",";
            if (!path->IsFieldArray()) {
               break;
            }
         }
         if (path->IsOrderArray()) {
            sqlQuery += ",'";
            sqlQuery += iOrder;
            sqlQuery += "'";
         }
         if (fAdInsertValues.Length()) {
            sqlQuery += "," + fAdInsertValues;
         }
         sqlQuery += " ) ";
         sqlQuery += ";";
      } else {                    //update existing record
         sqlQuery = "UPDATE ";
         sqlQuery += fFromPhrase;
         sqlQuery += " SET ";
         sqlQuery += fSetFieldList;
         for (iField = path->GetFieldIndexAt(0), jArray = 0, istart = 0;
              !path->IsFieldArray() || InRange(iField, path->GetFieldIndexAt(0), path->GetFieldIndexAt(1));
              iField += path->GetFieldIndexAt(2), jArray++) {
            if ((istart = sqlQuery.Index(RSQLDB_STR, RSQLDB_STR_LEN, istart, TString::kIgnoreCase)) != -1) {
               temp = "='" + values->At(iArray, jArray) + "'";
               sqlQuery.Remove(istart, RSQLDB_STR_LEN);
               sqlQuery.Insert(istart, temp);
            }
            if (!path->IsFieldArray()) {
               break;
            }
         }
         if (fWherePhrase.Length()) {
            sqlQuery += " WHERE " + fWherePhrase;
         }
         if (path->IsOrderArray()) {
            if (sqlQuery.Contains("WHERE")) {
               sqlQuery += " AND ";
            } else {
               sqlQuery += " WHERE ";
            }
            sqlQuery += path->GetTableNameAt(path->GetNumberOfTables() - 1);
            sqlQuery += ".";
            sqlQuery += strlen(path->GetTableIDXNameAt(path->GetNumberOfTables() - 1)) ?
                  path->GetTableIDXNameAt(path->GetNumberOfTables() - 1) : "idx";
            sqlQuery += " ='";
            sqlQuery += iOrder;
            sqlQuery += "'";
         }
      }

      ROMEPrint::Debug("ROMESQLDataBase::Write  : %s\n", sqlQuery.Data());
      if (!fSQL->MakeQuery(sqlQuery.Data(), false)) {
         ROMEPrint::Error("Invalid input for database write.\n");
         delete path;
#if defined ( USE_TRANSACTION )
         fSQL->RollbackTransaction("");
#endif
         return false;
      }
      if (!path->IsOrderArray()) {
         break;
      }
   }
#if defined ( USE_TRANSACTION )
   fSQL->CommitTransaction("");
#endif

   delete path;
   return true;
}

//______________________________________________________________________________
void ROMESQLDataBase::Print(Option_t *) const
{
   ROMEString temp = fSelectFieldList;
   temp.ReplaceAll(RSQLDB_STR, "");
   cout<< "******************************************************************************" << endl
       << "   FIELD : " << temp << endl
       << "   FROM  : " << fFromPhrase << endl
       << "   WHERE : " << fWherePhrase << endl
       << "******************************************************************************" << endl << endl;
}
