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

#include "ROMESQLDataBase.h"

const char RSQLDB_STR[]="RomeWasNotBuiltInADay";
const int  RSQLDB_STR_LEN = strlen(RSQLDB_STR);
const char* const kRunNumberReplace = "R_UN_NUMBE_R";
const char* const kEventNumberReplace = "E_VENT_NUMBE_R";

ClassImp(ROMESQLDataBase)

ROMESQLDataBase::ROMESQLDataBase() {
   fSQL = NULL;
   Int_t i;
   for (i = 0; i < kNumberOfReadCache; i++)
      fPathCache[i] = 0;
   fCurrentCache = 0;
   fLastRunNumber = 0;
   fLastEventNumber = 0;
   sprintf(fLastRunNumberString, "0");
   sprintf(fLastEventNumberString, "0");
}

ROMESQLDataBase::~ROMESQLDataBase() {
   if (fSQL)
      delete fSQL;
}

void ROMESQLDataBase:: ResetPhrase() {
   fSelectFieldList.Resize(0);
   fInsertFieldList.Resize(0);
   fSetFieldList.Resize(0);
   fFromPhrase.Resize(0);
   fWherePhrase.Resize(0);
   fAdInsertFields.Resize(0);
   fAdInsertValues.Resize(0);
}

Bool_t ROMESQLDataBase:: DecodeDBConstraint(const char* currentTableName,const char* nextTableName,const char* dbConstraint,Long64_t runNumber,Long64_t eventNumber,const char* currentIdName,const char* currentIdxName) {
   ROMEString value = dbConstraint;
   ROMEPath *dbpath = new ROMEPath();
   int is1,ie1,is2,ie2,is3,ie3;
   int itmp;
   ROMEString temp  =  nextTableName;
   temp += "=\"";
   ROMEString val = currentTableName;
   val += ".";
   ROMEString pathString;
   ROMEString newpathString;
   ROMEString tname;

   // constraint is a path
   if ((is1=value.Index(temp,temp.Length(),0,TString::kIgnoreCase))!=-1) {
      if ((ie1=value.Index("\"",1,is1+temp.Length(),TString::kIgnoreCase))!=-1) {
         value = value(is1+temp.Length(),ie1-is1-temp.Length());
         if (value(value.Length()-1)!='/')
            value += "/";
         temp.Remove(temp.Length()-2,2);
         while ((is1=value.Index("(@@",3,0,TString::kIgnoreCase))!=-1) {
            if ((ie1=value.Index(")",1,is1+3,TString::kIgnoreCase))==-1)
               ie1 = value.Length();
            else
               value.Remove(ie1,1);
            pathString = value(is1+3,ie1-is1-3);
            if (pathString.Contains ("_")) {
               //"[id=(@@AAA_id)]" -> "[id=currentTableName.AAA_id]"
               value.Remove(is1,3);
               value.Insert(is1,val);
            }
            else{
               //[(@@id)] -> [id=currentTableName.AAA_id]
               ie2=0;
               is3=1;
               ie3=value.Length();
               while ((is2=value.Index("/",1,ie2,TString::kIgnoreCase))!=-1) {
                  if (is2>is1)
                     break;
                  is3 = is2+1;
                  ie2 = value.Length();
                  if ((itmp=value.Index("/",1,is3,TString::kIgnoreCase))!=-1)
                     ie2 = TMath::Min(ie2,itmp+1);
                  if ((itmp=value.Index("{",1,is3,TString::kIgnoreCase))!=-1)
                     ie2 = TMath::Min(ie2,itmp+1);
                  if ((itmp=value.Index("(",1,is3,TString::kIgnoreCase))!=-1)
                     ie2 = TMath::Min(ie2,itmp+1);
                  if ((itmp=value.Index("[",1,is3,TString::kIgnoreCase))!=-1)
                     ie2 = TMath::Min(ie2,itmp+1);
                  ie3=ie2-1;
               }
               tname = value(is3,ie3-is3);
               newpathString.SetFormatted("%s=%s.%s_%s"
                                             ,pathString.Data(),currentTableName
                                             ,tname.Data()
                                             ,pathString.Data());
               value.Remove(is1,ie1-is1);
               value.Insert(is1,newpathString);
            }
         }
         if (!dbpath->Decode(value,runNumber,eventNumber)) {
            delete dbpath;
            return false;
         }
         if (!MakePhrase(dbpath,runNumber,eventNumber)) {
            delete dbpath;
            return false;
         }
         //add relation
         if (strlen(dbpath->GetTableIDNameAt(dbpath->GetNumberOfTables()-1))) {
            if (fWherePhrase.Length())
               fWherePhrase += " AND ";
            fWherePhrase.AppendFormatted("%s.%s=%s.%s_%s"
                                         ,temp.Data()
                                         ,"id"//dbpath->GetTableIDNameAt(dbpath->GetNumberOfTables()-1)
                                         , dbpath->GetTableNameAt(dbpath->GetNumberOfTables()-1)
                                         ,temp.Data()
                                         ,dbpath->GetTableIDNameAt(dbpath->GetNumberOfTables()-1)
               );
         }
         if (strlen(dbpath->GetTableIDXNameAt(dbpath->GetNumberOfTables()-1))) {
            if (fWherePhrase.Length())
               fWherePhrase += " AND ";
            fWherePhrase.AppendFormatted("%s.%s=%s.%s_%s"
                                         ,temp.Data()
                                         ,"idx"//dbpath->GetTableIDXNameAt(dbpath->GetNumberOfTables()-1)
                                         , dbpath->GetTableNameAt(dbpath->GetNumberOfTables()-1)
                                         ,temp.Data()
                                         ,dbpath->GetTableIDXNameAt(dbpath->GetNumberOfTables()-1)
               );
         }
      }
      //add relation to the first @constraint table
      if (strlen(currentIdName)) {
         if (fWherePhrase.Length())
            fWherePhrase += " AND ";
         fWherePhrase.AppendFormatted("%s.%s=%s.%s_%s"
                                      ,dbpath->GetTableNameAt(0)
                                      ,"id"//dbpath->GetTableIDNameAt(dbpath->GetNumberOfTables()-1)
                                      ,currentTableName
                                      ,dbpath->GetTableNameAt(0)
                                      ,currentIdName
            );
      }
      if (strlen(currentIdxName)) {
         if (fWherePhrase.Length())
            fWherePhrase += " AND ";
         fWherePhrase.AppendFormatted("%s.%s=%s.%s_%s"
                                      ,dbpath->GetTableNameAt(0)
                                      ,"idx"//dbpath->GetTableIDXNameAt(dbpath->GetNumberOfTables()-1)
                                      ,currentTableName
                                      ,dbpath->GetTableNameAt(0)
                                      ,currentIdxName
            );
      }
   }
   else{
      cout << "Warning: DB constraint was not found for "
           <<nextTableName<<endl;
      delete dbpath;
      return false;
   }

   delete dbpath;
   return true;
}

Bool_t ROMESQLDataBase:: MakePhrase(ROMEPath* path,Long64_t runNumber,Long64_t eventNumber) {
   ROMEString sqlQuery;
   ROMEString sqlResult;
   ROMEString temp;
   ROMEString separator;
   int        iTable = 0;
   int        iConstraint;
   int        iField;
   //field list
   if (!fSelectFieldList.Length()) {
      separator = "";
      for (iField=path->GetFieldIndexAt(0)
             ;!path->IsFieldArray() || InRange(iField,path->GetFieldIndexAt(0),path->GetFieldIndexAt(1))
             ;iField+=path->GetFieldIndexAt(2)) {
         temp = path->GetFieldName();
         if (path->IsFieldArray())
            temp.AppendFormatted("__%d",iField);
         fSelectFieldList.AppendFormatted("%s%s.%s",separator.Data(),path->GetTableNameAt(path->GetNumberOfTables()-1),temp.Data());
         fInsertFieldList.AppendFormatted("%s%s",separator.Data(),temp.Data());
         fSetFieldList.AppendFormatted("%s%s%s",separator.Data(),temp.Data(),RSQLDB_STR);
         separator = ",";
         if (!path->IsFieldArray())
            break;
      }
   }

#if defined ( SQLDEBUG )
   cout<<endl<<"******************************************************************************"<<endl;
   for (iTable=0;iTable<path->GetNumberOfTables();iTable++) {
      cout<<"Table\t: "<<path->GetTableNameAt(iTable)<<endl;
   }
   cout<<"Field\t: "<<path->GetFieldName()<<endl<<endl;
   cout<<"following relations..."<<endl;
#endif
   // start following relation.
   for (iTable=0;iTable<path->GetNumberOfTables();iTable++) {
#if defined ( SQLDEBUG )
      cout<<"Level-"<<iTable<<": "<<path->GetTableNameAt(iTable);
#endif
      //add table to FROM phrase
      if (!fFromPhrase.Contains(path->GetTableNameAt(iTable))) {
         if (fFromPhrase.Length())
            fFromPhrase += ",";
         fFromPhrase.AppendFormatted("%s",path->GetTableNameAt(iTable));
      }

      //add constraint
      if (strlen(path->GetTableConstraintAt(iTable))) {
         if (!path->DecodeConstraint(path->GetTableConstraintAt(iTable))) {
            return false;
         }
         for (iConstraint=0;iConstraint<path->GetNumberOfConstraints();iConstraint++) {
            if (fAdInsertFields.Length())
               separator = ",";
            else
               separator = "";
            fAdInsertFields.AppendFormatted("%s%s",separator.Data(),path->GetConstraintFieldAt(iConstraint));
            fAdInsertValues.AppendFormatted("%s%s",separator.Data(),path->GetConstraintValueAt(iConstraint));
            if (fWherePhrase.Length())
               fWherePhrase += " AND ";
            fWherePhrase.AppendFormatted("%s.%s=%s",path->GetTableNameAt(iTable)
                                         ,path->GetConstraintFieldAt(iConstraint)
                                         ,path->GetConstraintValueAt(iConstraint));
         }
      }

      //add dbconstraint
      if (strlen(path->GetTableDBConstraintAt(iTable))) {
         //read dbconstraint
         sqlQuery = "SELECT ";
         sqlQuery.AppendFormatted("%s.%s", path->GetTableNameAt(iTable),path->GetTableDBConstraintAt(iTable));
         sqlQuery += " FROM ";
         sqlQuery += fFromPhrase;
         sqlQuery += " WHERE ";
         sqlQuery += fWherePhrase;
         sqlQuery += " LIMIT 1;";

         ROMEString tmpString;
#if defined ( R__VISUAL_CPLUSPLUS )
         tmpString.SetFormatted("%I64d",eventNumber);
#else
         tmpString.SetFormatted("%lld",eventNumber);
#endif
         sqlQuery.ReplaceAll(kEventNumberReplace, tmpString);
         // replace # with the current run number
#if defined ( R__VISUAL_CPLUSPLUS )
         tmpString.SetFormatted("%I64d",runNumber);
#else
         tmpString.SetFormatted("%lld",runNumber);
#endif
         sqlQuery.ReplaceAll(kRunNumberReplace, tmpString);

         if (!fSQL->MakeQuery((char*)sqlQuery.Data(),true)) {
            cout << "Wrong path for data base constraint : " << path->GetTableDBConstraintAt(iTable) << endl;
            fSQL->FreeResult();
            return false;
         }
         if (!fSQL->NextRow()) {
            cout << "Database constraint ("<<path->GetTableDBConstraintAt(iTable)<<") was not found"<< endl;
            fSQL->FreeResult();
            return false;
         }
         temp = fSQL->GetField(0);
         if (!DecodeDBConstraint(path->GetTableNameAt(iTable),path->GetTableNameAt(iTable+1),temp.Data(),runNumber,eventNumber
                                ,path->GetTableIDNameAt(iTable),path->GetTableIDXNameAt(iTable))) {
            fSQL->FreeResult();
            return false;
         }
         fSQL->FreeResult();
      }

      //add relation to the next table
      if (iTable<path->GetNumberOfTables()-1 && !strlen(path->GetTableDBConstraintAt(iTable))) {
         if (strlen(path->GetTableIDNameAt(iTable))) {
            if (fWherePhrase.Length())
               fWherePhrase += " AND ";
            fWherePhrase.AppendFormatted("%s.%s=%s.%s_%s"
                                         ,path->GetTableNameAt(iTable+1)
                                         ,"id"//path->GetTableIDNameAt(iTable)
                                         , path->GetTableNameAt(iTable)
                                         ,path->GetTableNameAt(iTable+1)
                                         ,path->GetTableIDNameAt(iTable)
               );
         }
         if (strlen(path->GetTableIDXNameAt(iTable))) {
            if (fWherePhrase.Length())
               fWherePhrase += " AND ";
            fWherePhrase.AppendFormatted("%s.%s=%s.%s_%s"
                                         ,path->GetTableNameAt(iTable+1)
                                         ,"idx"//path->GetTableIDXNameAt(iTable)
                                         , path->GetTableNameAt(iTable)
                                         ,path->GetTableNameAt(iTable+1)
                                         ,path->GetTableIDXNameAt(iTable)
               );
         }
      }
#if defined ( SQLDEBUG )
      cout<<"\t  ... OK"<<endl;
#endif
   }
   return true;
}

Bool_t ROMESQLDataBase::Init(const char* name,const char* dataBase,const char* connection) {
   ROMEString path = connection;
   ROMEString server;
   ROMEString user;
   ROMEString passwd;
   ROMEString database;
   ROMEString port;
   ROMEString prompt;
   int is,ie;
   int istart;

   fName = name;

   path.StripSpaces();

   //decode dataBasePath
   if ((istart=path.Index("://",3,0,TString::kIgnoreCase))==-1) {
      cout << "Wrong path for SQL database : " << path << endl
           << "Path should look like," << endl
           << "mysql://username:password@servername:port/database" << endl
           << "postgresql://username:password@servername:port/database" << endl
           << "sqlite://filename" << endl
           << "sqlite3://filename" << endl;
      return false;
   }
   fDBMSType = path(0,istart);
   fDBMSType.ToLower();

   istart += 3;
   //search for user
   is = istart;
   if ((ie=path.Index("@",1,is,TString::kIgnoreCase))==-1) {
      user = "";
      passwd = "";
   }
   else {
      istart = ie+1;
      user = path(is,ie-is);
      //search for passwd
      if ((ie=user.Index(":",1,0,TString::kIgnoreCase))==-1) {
         passwd = "";
      }
      else{
         passwd = user(ie+1,user.Length()-ie-1);
         user.Remove(ie,user.Length()-ie);
      }
   }
   //search for server
   is = istart;
   if ((ie=path.Index("/",1,is,TString::kIgnoreCase))==-1) {
      database = "";
   }
   else {
      istart = ie+1;
      server = path(is,ie-is);
      //search for port
      if ((ie=server.Index(":",1,0,TString::kIgnoreCase))==-1) {
         port = "0";
      }
      else{
         port = server(ie+1,server.Length()-ie-1);
         server.Remove(ie,server.Length()-ie);
      }
      //search for database
      database = path(istart,path.Length());
   }

   if (passwd.Length()==1 && passwd(0) == '?') {
#if defined ( R__UNIX )
      prompt.SetFormatted("%s@%s's password: ",user.Data(),server.Data());
      passwd = getpass(prompt.Data());
#endif
// please implement similar function for windows
   }
#if defined ( SQLDEBUG )
   cout<<"******  SQL Connection  ******"<<endl
       <<"server   : "<<server<<endl
       <<"user     : "<<user<<endl
       <<"passwd   : "<<passwd<<endl
       <<"database : "<<database<<endl
       <<"port     : "<<port<<endl;
#endif

   if ( fDBMSType == "mysql" ) {
#if defined ( HAVE_MYSQL )
      fSQL = new ROMEMySQL();
#else
      LinkError();
      return false;
#endif
   }
   else if ( fDBMSType == "postgresql" ) {
#if defined ( HAVE_PGSQL )
      fSQL = new ROMEPgSQL();
#else
      LinkError();
      return false;
#endif
   }
   else if ( fDBMSType == "sqlite" ) {
#if defined ( HAVE_SQLITE )
      fSQL = new ROMESQLite();
#else
      LinkError();
      return false;
#endif
   }
   else if ( fDBMSType == "sqlite3" ) {
#if defined ( HAVE_SQLITE3 )
      fSQL = new ROMESQLite3();
#else
      LinkError();
      return false;
#endif
   }
   else{
      cout<<"Error: DBMS \""<<fDBMSType<<"\" is not supported"<<endl;
      return false;
   }

   this->ResetPhrase();

   fSQL->Connect(server.Data(),user.Data(),passwd.Data(),database.Data(),port.Data());

   return true;
}

Bool_t ROMESQLDataBase::Read(ROMEStr2DArray *values,const char *dataBasePath,Long64_t runNumber,Long64_t eventNumber) {
   int iField,iOrder;
   int iLastOrder=0;
   int iArray,jArray;
   int iCount;
   bool keepCursor=false;
   ROMEPath *path;
   ROMEString fieldName;
   ROMEString sqlQuery;
   ROMEString orderField;
   Bool_t cacheFound = kFALSE;

   // check cache
   for (iCount = 0; iCount < kNumberOfReadCache; iCount++) {
      if ( fDBPathCache[iCount] == dataBasePath ) {
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

      if (!path->Decode(pathString.Data(),runNumber,eventNumber)) {
         cout << "Path decode error : " << dataBasePath << endl;
         return false;
      }

      this->ResetPhrase();
      if (!MakePhrase(path,runNumber,eventNumber)) {
         cout<<"Invalid input for database read."<<endl;
         return false;
      }
      if (!fFromPhrase.Contains(path->GetOrderTableName())) {
         cout<<"Invalid path for database read."<<endl
             <<"order tabele("<<path->GetOrderTableName()<<") should be in path"<<endl;
         return false;
      }
      if (path->IsOrderArray())
         orderField.AppendFormatted("%s.%s",
                                    strlen(path->GetOrderTableName())
                                    ? path->GetOrderTableName() : path->GetTableNameAt(path->GetNumberOfTables()-1),
                                    strlen(path->GetOrderFieldName())
                                    ? path->GetOrderFieldName() : "idx");

      sqlQuery = "SELECT ";
      sqlQuery += fSelectFieldList;
      if (path->IsOrderArray())
         sqlQuery.AppendFormatted(",%s",orderField.Data());
      sqlQuery.AppendFormatted(" FROM %s",fFromPhrase.Data());
      if (fWherePhrase.Length())
         sqlQuery.AppendFormatted(" WHERE %s",fWherePhrase.Data());
      if (orderField.Length()) {
         if (sqlQuery.Contains("WHERE"))
            sqlQuery += " AND ";
         else
            sqlQuery += " WHERE ";
         sqlQuery.AppendFormatted("(%s BETWEEN %d AND %d)"
                                  ,orderField.Data()
                                  ,TMath::Min(path->GetOrderIndexAt(0),path->GetOrderIndexAt(1))
                                  ,TMath::Max(path->GetOrderIndexAt(0),path->GetOrderIndexAt(1)));
         if (path->GetOrderIndexAt(2)!=1) {
            sqlQuery.AppendFormatted(" AND ((%s-%d) %% %d)=0 "
                                     ,orderField.Data(),path->GetOrderIndexAt(0),path->GetOrderIndexAt(2));
         }
         sqlQuery.AppendFormatted(" ORDER BY %s ",orderField.Data());
         if (path->GetOrderIndexAt(2)<0)
            sqlQuery += " DESC ";
      }
      sqlQuery += ";";
      // store cache
      fQueryCache[fCurrentCache] = sqlQuery;
      fCurrentCache = (fCurrentCache+1) % kNumberOfReadCache;
   }

   // replace ## with the current run number
   if (fLastRunNumber != runNumber) {
#if defined ( R__VISUAL_CPLUSPLUS )
      sprintf(fLastRunNumberString, "%I64d", runNumber);
#else
      sprintf(fLastRunNumberString, "%lld", runNumber);
#endif
      fLastRunNumber = runNumber;
   }
   sqlQuery.ReplaceAll(kRunNumberReplace, fLastRunNumberString);

   if (fLastEventNumber != eventNumber) {
#if defined ( R__VISUAL_CPLUSPLUS )
      sprintf(fLastEventNumberString, "%I64d", eventNumber);
#else
      sprintf(fLastEventNumberString, "%lld", eventNumber);
#endif
      fLastEventNumber = eventNumber;
   }
   sqlQuery.ReplaceAll(kEventNumberReplace, fLastEventNumberString);

   if (!fSQL->MakeQuery((char*)sqlQuery.Data(),true)) {
      cout<<"Invalid input for database read."<<endl;
      fSQL->FreeResult();
      return false;
   }
   if (!fSQL->GetNumberOfRows()) {
      cout << "Warning: "<<path->GetTableNameAt(path->GetNumberOfTables()-1)<<"."<<path->GetFieldName();
      cout<<" was not found. Default value will be used."<<endl;
      fSQL->FreeResult();
      return true;
   }

   const Int_t orderIndex[3] = {path->GetOrderIndexAt(0), path->GetOrderIndexAt(1), path->GetOrderIndexAt(2)};
   const Int_t fieldIndex[3] = {path->GetFieldIndexAt(0), path->GetFieldIndexAt(1), path->GetFieldIndexAt(2)};

   for (iOrder=orderIndex[0],iArray=0
           ;!path->IsOrderArray() || InRange(iOrder,orderIndex[0],orderIndex[1])
           ;iOrder+=orderIndex[2],iArray++) {
      if (!keepCursor) {
         if (!fSQL->NextRow()) {
            cout << "Warning: some records were not found in "<<path->GetTableNameAt(path->GetNumberOfTables()-1)<<endl;
            fSQL->FreeResult();
            return true;
         }
      }
      keepCursor = false;

      if (path->IsOrderArray()) {
         // check number of records which have the same order number.
         iCount = 0;
         while (TMath::Sign(atoi(fSQL->GetField(fSQL->GetNumberOfFields()-1)), orderIndex[2])
                < TMath::Sign(iOrder, orderIndex[2])) {
            if (!fSQL->NextRow()) {
               cout << "Warning: some records were not found in "<<path->GetTableNameAt(path->GetNumberOfTables()-1)<<endl;
               fSQL->FreeResult();
               return true;
            }
            iCount++ ;
         }
         if (iCount)
            cout << "Warning: "<<path->GetTableNameAt(path->GetNumberOfTables()-1)<<" has "<<iCount+1
                 <<" records which satisfy "<<path->GetOrderTableName()<<"."<<path->GetOrderFieldName()<<"="<<iLastOrder<<endl;

         // check if the record exists
         if (TMath::Sign(iOrder,orderIndex[2])
             < TMath::Sign(atoi(fSQL->GetField(fSQL->GetNumberOfFields()-1)),orderIndex[2])) {
            cout << "Warning: "
                 <<path->GetTableNameAt(path->GetNumberOfTables()-1)<<"."<<path->GetFieldName()
                 <<"("<<path->GetOrderTableName()<<"."<<path->GetOrderFieldName()<<"="<<iOrder<<")"
                 <<" was not found. Default value will be used."<<endl;
            keepCursor = true;
            continue;
         }
      }

      // Fill result
      for (iField=fieldIndex[0],jArray=0
             ;!path->IsFieldArray() || InRange(iField,fieldIndex[0],fieldIndex[1])
             ;iField+=fieldIndex[2],jArray++) {
         values->SetAt(fSQL->GetField(jArray),iArray,jArray);
         iLastOrder = iOrder;
         if (!path->IsFieldArray())
            break;
      }
      if (!path->IsOrderArray())
         break;
   }

   fSQL->FreeResult();
   return true;
}

Bool_t ROMESQLDataBase::Write(ROMEStr2DArray* values,const char *dataBasePath,Long64_t runNumber,Long64_t eventNumber) {
   int iField;
   int iOrder;
   ROMEPath *path = new ROMEPath();
   ROMEString fieldName;
   ROMEString sqlQuery;
   ROMEString temp;
   ROMEString separator;
   int iArray,jArray;
   int istart;
   bool exist;

   if (!path->Decode(dataBasePath,runNumber,eventNumber)) {
      cout << "Path decode error : " << dataBasePath << endl;
      delete path;
      return false;
   }
//   path->Print();
   if (path->GetNumberOfTables()!=1) {
      cout << "Wrong data base path : " << dataBasePath << endl;
      delete path;
      return false;
   }
   if (strstr(path->GetTableConstraintAt(path->GetNumberOfTables()-1),"!=")) {
      cout << " \"!=\" can not be used for default values." << endl;
      delete path;
      return false;
   }

   this->ResetPhrase();
   if (!MakePhrase(path,runNumber,eventNumber)) {
      cout<<"Invalid input for database write."<<endl;
      delete path;
      return false;
   }

#if defined ( USE_TRANSACTION )
   fSQL->StartTransaction("");
#endif
   for (iOrder=path->GetOrderIndexAt(0),iArray=0
          ;!path->IsOrderArray() || InRange(iOrder,path->GetOrderIndexAt(0),path->GetOrderIndexAt(1))
          ;iOrder+=path->GetOrderIndexAt(2),iArray++) {
      //check if the row exists
      sqlQuery = "SELECT ";
      sqlQuery += fSelectFieldList;
      sqlQuery += " FROM ";
      sqlQuery += fFromPhrase;
      if (fWherePhrase.Length())
         sqlQuery.AppendFormatted(" WHERE %s",fWherePhrase.Data());
      if (path->IsOrderArray()) {
         if (!sqlQuery.Contains("WHERE"))
            sqlQuery += " WHERE ";
         else
            sqlQuery += " AND ";
         sqlQuery.AppendFormatted("%s.%s='%d'",path->GetTableNameAt(path->GetNumberOfTables()-1),
                                  strlen(path->GetTableIDXNameAt(path->GetNumberOfTables()-1))
                                  ? path->GetTableIDXNameAt(path->GetNumberOfTables()-1) : "idx"
                                  ,iOrder);
      }
      sqlQuery += " LIMIT 1;";
      if (!fSQL->MakeQuery((char*)sqlQuery.Data(),true)) {
         cout << "Invalid input for database write."<< endl;
         fSQL->FreeResult();
         delete path;
#if defined ( USE_TRANSACTION )
         fSQL->RollbackTransaction("");
#endif
         return false;
      }
      exist = fSQL->GetNumberOfRows()!=0;
      fSQL->FreeResult();

      if (!exist) { // insert new record
         sqlQuery = "INSERT INTO ";
         sqlQuery += fFromPhrase;
         sqlQuery += " ( ";
         sqlQuery += fInsertFieldList;
         if (path->IsOrderArray()) {
            sqlQuery.AppendFormatted(",%s",
                                     strlen(path->GetTableIDXNameAt(path->GetNumberOfTables()-1))
                                     ? path->GetTableIDXNameAt(path->GetNumberOfTables()-1) : "idx");
         }
         if (fAdInsertFields.Length())
            sqlQuery.AppendFormatted(",%s",fAdInsertFields.Data());
         sqlQuery += " ) VALUES ( ";
         separator = "";
         for (iField=path->GetFieldIndexAt(0),jArray=0
                ;!path->IsFieldArray() || InRange(iField,path->GetFieldIndexAt(0),path->GetFieldIndexAt(1))
                ;iField+=path->GetFieldIndexAt(2),jArray++) {
            sqlQuery.AppendFormatted("%s'%s'",separator.Data(),values->At(iArray,jArray).Data());
            separator = ",";
            if (!path->IsFieldArray())
               break;
         }
         if (path->IsOrderArray()) {
            sqlQuery.AppendFormatted(",'%d'",iOrder);
         }
         if (fAdInsertValues.Length())
            sqlQuery.AppendFormatted(",%s",fAdInsertValues.Data());
         sqlQuery += " ) ";
         sqlQuery += ";";
      }

      else{ //update existing record
         sqlQuery = "UPDATE ";
         sqlQuery += fFromPhrase;
         sqlQuery += " SET ";
         sqlQuery += fSetFieldList;
         for (iField=path->GetFieldIndexAt(0),jArray=0,istart=0
                ;!path->IsFieldArray() || InRange(iField,path->GetFieldIndexAt(0),path->GetFieldIndexAt(1))
                ;iField+=path->GetFieldIndexAt(2),jArray++) {
            if ((istart=sqlQuery.Index(RSQLDB_STR,RSQLDB_STR_LEN,istart,TString::kIgnoreCase))!=-1) {
               temp.SetFormatted("='%s'",values->At(iArray,jArray).Data());
               sqlQuery.Remove(istart,RSQLDB_STR_LEN);
               sqlQuery.Insert(istart,temp);
            }
            if (!path->IsFieldArray())
               break;
         }
         if (fWherePhrase.Length())
            sqlQuery.AppendFormatted(" WHERE %s",fWherePhrase.Data());
         if (path->IsOrderArray()) {
            if (sqlQuery.Contains("WHERE"))
               sqlQuery += " AND ";
            else
               sqlQuery += " WHERE ";
            sqlQuery.AppendFormatted("%s.%s ='%d'",path->GetTableNameAt(path->GetNumberOfTables()-1),
                                     strlen(path->GetTableIDXNameAt(path->GetNumberOfTables()-1))
                                     ? path->GetTableIDXNameAt(path->GetNumberOfTables()-1) : "idx"
                                     ,iOrder);
         }
      }

#if defined ( SQLDEBUG )
      cout<<"ROMESQLDataBase::Write  : "<<sqlQuery<<endl;
#else
      if (!fSQL->MakeQuery((char*)sqlQuery.Data(),false)) {
         cout<<"Invalid input for database write."<<endl;
         delete path;
#if defined ( USE_TRANSACTION )
         fSQL->RollbackTransaction("");
#endif
         return false;
      }
#endif
      if (!path->IsOrderArray())
         break;
   }
#if defined ( USE_TRANSACTION )
   fSQL->CommitTransaction("");
#endif

   delete path;
   return true;
}

void ROMESQLDataBase::Print(Option_t *) {
   ROMEString temp = fSelectFieldList;
   temp.ReplaceAll(RSQLDB_STR,"");
   cout << "******************************************************************************"<<endl
        << "   FIELD : " << temp         << endl
        << "   FROM  : " << fFromPhrase  << endl
        << "   WHERE : " << fWherePhrase << endl
        << "******************************************************************************"<<endl
        << endl;
}

