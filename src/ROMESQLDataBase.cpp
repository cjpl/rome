// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMESQLDataBase
//
//  SQLDataBase access.
//
//  $Log$
//  Revision 1.28  2005/04/27 10:30:45  sawada
//  Added SQLite,SQLite3 support.
//
//  Revision 1.27  2005/04/23 21:54:21  sawada
//  remove () around @@ constraint.
//
//  Revision 1.26  2005/04/01 14:56:23  schneebeli_m
//  Histo moved, multiple databases, db-paths moved, InputDataFormat->DAQSystem, GetMidas() to access banks, User DAQ
//
//  Revision 1.25  2005/03/03 19:24:32  sawada
//  compatibility with SQL and XML database.
//
//  Revision 1.24  2005/03/01 14:34:24  sawada
//  compatibility of SQL and XML database of @constraint.
//  bug fix of loop counter.
//  field separator of field array was changed from $ to __.
//
//  Revision 1.23  2005/02/21 21:29:07  sawada
//
//  Changed OS specifying macros
//  Support for DEC,Ultrix,FreeBSD,Solaris
//
//  Revision 1.22  2005/01/27 16:21:06  schneebeli_m
//  print method & no gROME in path
//
//  Revision 1.21  2005/01/14 08:29:53  sawada
//  bug fix. removed FreeResult from Write().
//
//  Revision 1.20  2004/12/08 09:40:15  sawada
//  added unistd.h for getpass. and small change
//
//  Revision 1.19  2004/12/06 16:03:02  sawada
//  code cleanup (tab -> space)
//
//  Revision 1.18  2004/12/02 17:46:43  sawada
//  Macintosh port
//
//  Revision 1.17  2004/11/21 00:10:41  sawada
//  error handling
//
//  Revision 1.16  2004/11/19 22:37:14  sawada
//  bug fix
//
//  Revision 1.15  2004/11/19 16:26:24  sawada
//  speed up with reading order array at once.
//
//  Revision 1.14  2004/11/18 15:23:23  sawada
//
//  Modify handling the order of array.
//  Enable inverse order.
//  Enable to send sql query from user tasks.
//
//  Revision 1.13  2004/11/17 18:50:30  sawada
//  bug fix in handling database constraint
//
//  Revision 1.12  2004/11/17 13:27:17  sawada
//  bug fix
//
//  Revision 1.11  2004/11/17 10:36:27  sawada
//  argument for Init
//
//  Revision 1.9  2004/11/16 21:59:45  sawada
//  read/write field array at once.
//
//  Revision 1.8  2004/11/16 18:53:44  sawada
//  SQL Write
//
//  Revision 1.7  2004/11/16 16:14:01  schneebeli_m
//  implemented task hierarchy
//
//  Revision 1.6  2004/11/16 14:27:21  sawada
//  small modification
//
//  Revision 1.5  2004/11/16 12:11:06  sawada
//  SQL Init,Read
//
//  Revision 1.4  2004/11/11 12:55:28  schneebeli_m
//  Implemented XML database with new path rules
//
//  Revision 1.3  2004/10/05 07:52:44  schneebeli_m
//  dyn. Folders, TRef Objects, XML format changed, ROMEStatic removed
//
//  Revision 1.2  2004/09/30 13:08:21  schneebeli_m
//  ...
//
//  Revision 1.1  2004/09/27 18:26:05  schneebeli_m
//  new database classes
//
//
//////////////////////////////////////////////////////////////////////////
#include <RConfig.h>
#include <TObjString.h>
#if defined( R__UNIX )
#include <unistd.h>
#endif

#include <ROMESQLDataBase.h>

const char RSQLDB_STR[]="RomeWasNotBuiltInADay";
const int  RSQLDB_STR_LEN = strlen(RSQLDB_STR);

ROMESQLDataBase::ROMESQLDataBase() {
   fSQL = NULL;
}

ROMESQLDataBase::~ROMESQLDataBase() {
   if(fSQL)
      delete fSQL;
}

void ROMESQLDataBase:: ResetPhrase(){
   fFieldList.Resize(0);
   fFromPhrase.Resize(0);
   fWherePhrase.Resize(0);
}

bool ROMESQLDataBase:: DecodeDBConstraint(const char* currentTableName,const char* nextTableName,const char* dbConstraint,int runNumber,const char* currentIdName,const char* currentIdxName){
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
         if(value(value.Length()-1)!='/')
            value += "/";
         temp.Remove(temp.Length()-2,2);
         while ((is1=value.Index("(@@",3,0,TString::kIgnoreCase))!=-1) {
            if ((ie1=value.Index(")",1,is1+3,TString::kIgnoreCase))==-1)
               ie1 = value.Length();
            else
               value.Remove(ie1,1);
            pathString = value(is1+3,ie1-is1-3);
            if(pathString.Contains ("_")){
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
                  if(is2>is1)
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
         if(!dbpath->Decode(value,runNumber)){
            delete dbpath;
            return false;
         }
         if(!MakePhrase(dbpath,runNumber)){
            delete dbpath;
            return false;
         }
         //add relation
         if(strlen(dbpath->GetTableIDNameAt(dbpath->GetNumberOfTables()-1))){
            if(fWherePhrase.Length())
               fWherePhrase += " AND ";
            fWherePhrase.AppendFormatted("%s.%s=%s.%s_%s"
                                         ,temp.Data()
                                         ,"id"//dbpath->GetTableIDNameAt(dbpath->GetNumberOfTables()-1)
                                         , dbpath->GetTableNameAt(dbpath->GetNumberOfTables()-1)
                                         ,temp.Data()
                                         ,dbpath->GetTableIDNameAt(dbpath->GetNumberOfTables()-1)
               );
         }
         if(strlen(dbpath->GetTableIDXNameAt(dbpath->GetNumberOfTables()-1))){
            if(fWherePhrase.Length())
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
      if(strlen(currentIdName)){
         if(fWherePhrase.Length())
            fWherePhrase += " AND ";
         fWherePhrase.AppendFormatted("%s.%s=%s.%s_%s"
                                      ,dbpath->GetTableNameAt(0)
                                      ,"id"//dbpath->GetTableIDNameAt(dbpath->GetNumberOfTables()-1)
                                      ,currentTableName
                                      ,dbpath->GetTableNameAt(0)
                                      ,currentIdName
            );
      }
      if(strlen(currentIdxName)){
         if(fWherePhrase.Length())
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

bool ROMESQLDataBase:: MakePhrase(ROMEPath* path,int runNumber){
   ROMEString sqlQuery;
   ROMEString sqlResult;
   ROMEString temp;
   int        iTable = 0;   
   int        iConstraint;
   int        iField;
   //field list 
   if(!fFieldList.Length()){      
      for(iField=path->GetFieldIndexAt(0)
             ;!path->IsFieldArray() || InRange(iField,path->GetFieldIndexAt(0),path->GetFieldIndexAt(1))
             ;iField+=path->GetFieldIndexAt(2)){
         temp = path->GetFieldName();
         if(path->IsFieldArray())
            temp.AppendFormatted("__%d",iField);     
         fFieldList.AppendFormatted("%s.%s%s,",path->GetTableNameAt(path->GetNumberOfTables()-1)
                                    ,temp.Data(),RSQLDB_STR);
         if(!path->IsFieldArray())
            break;
      }
      fFieldList.Remove(fFieldList.Length()-1,1);
   }
   
#if defined ( SQLDEBUG )
   cout<<endl<<"******************************************************************************"<<endl;
   for(iTable=0;iTable<path->GetNumberOfTables();iTable++){
      cout<<"Table\t: "<<path->GetTableNameAt(iTable)<<endl;
   }
   cout<<"Field\t: "<<path->GetFieldName()<<endl<<endl;
   cout<<"following relations..."<<endl;
#endif
   
   // start following relation.
   for(iTable=0;iTable<path->GetNumberOfTables();iTable++){
#if defined ( SQLDEBUG )
      cout<<"Level-"<<iTable<<": "<<path->GetTableNameAt(iTable);
#endif
      //add table to FROM phrase
      if(!fFromPhrase.Contains(path->GetTableNameAt(iTable))){
         if(fFromPhrase.Length())
            fFromPhrase += ",";
         fFromPhrase.AppendFormatted("%s",path->GetTableNameAt(iTable));
      }
      
      //add constraint
      if(strlen(path->GetTableConstraintAt(iTable))){
         if (!path->DecodeConstraint(path->GetTableConstraintAt(iTable))) {
            return false;
         }
         for(iConstraint=0;iConstraint<path->GetNumberOfConstraints();iConstraint++){
            if(fWherePhrase.Length())
               fWherePhrase += " AND ";
            fWherePhrase.AppendFormatted("%s.%s=%s",path->GetTableNameAt(iTable)
                                         ,path->GetConstraintFieldAt(iConstraint)
                                         ,path->GetConstraintValueAt(iConstraint));
         }
      }

      //add dbconstraint
      if(strlen(path->GetTableDBConstraintAt(iTable))){
         //read dbconstraint
         sqlQuery = "SELECT ";
         sqlQuery.AppendFormatted("%s.%s", path->GetTableNameAt(iTable),path->GetTableDBConstraintAt(iTable));
         sqlQuery += " FROM ";
         sqlQuery += fFromPhrase;
         sqlQuery += " WHERE ";
         sqlQuery += fWherePhrase;
         sqlQuery += " LIMIT 1;";
         if(!fSQL->MakeQuery((char*)sqlQuery.Data(),true)){
            cout << "Wrong path for data base constraint : " << path->GetTableDBConstraintAt(iTable) << endl;            
            fSQL->FreeResult();
            return false;
         }
         if(!fSQL->NextRow()){
            cout << "Database constraint ("<<path->GetTableDBConstraintAt(iTable)<<") was not found"<< endl;
            fSQL->FreeResult();
            return false;
         }
         temp = fSQL->GetField(0);
         if(!DecodeDBConstraint(path->GetTableNameAt(iTable),path->GetTableNameAt(iTable+1),temp.Data(),runNumber
                                ,path->GetTableIDNameAt(iTable),path->GetTableIDXNameAt(iTable))){
            fSQL->FreeResult();
            return false;
         }
         fSQL->FreeResult();
      }
      
      //add relation to the next table
      if(iTable<path->GetNumberOfTables()-1 && !strlen(path->GetTableDBConstraintAt(iTable))){
         if(strlen(path->GetTableIDNameAt(iTable))){
            if(fWherePhrase.Length())
               fWherePhrase += " AND ";
            fWherePhrase.AppendFormatted("%s.%s=%s.%s_%s"
                                         ,path->GetTableNameAt(iTable+1)
                                         ,"id"//path->GetTableIDNameAt(iTable)
                                         , path->GetTableNameAt(iTable)
                                         ,path->GetTableNameAt(iTable+1)
                                         ,path->GetTableIDNameAt(iTable)
               );
         }
         if(strlen(path->GetTableIDXNameAt(iTable))){
            if(fWherePhrase.Length())
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

bool ROMESQLDataBase::Init(const char* name,const char* dataBase,const char* connection) {
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

   //decode dataBasePath
   if ((istart=path.Index("://",3,0,TString::kIgnoreCase))==-1) {
      cout << "Wrong path for SQL database : " << path << endl
           << "Path should look like," << endl
           << "mysql://username:password@servername:port/database" << endl
           << "sqlite://filename" << endl
           << "sqlite3://filename" << endl;
      return false;
   }
   fDBMSType = path(0,istart);
   fDBMSType.ToLower();
   fDBMSType.ReplaceAll(" ","");

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
   
   if(passwd.Length()==1 && passwd(0) == '?'){
#if defined( R__UNIX )
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

   if ( fDBMSType == "mysql" ){
#if defined ( HAVE_MYSQL )
      fSQL = new ROMEMySQL();
#else
      LinkError();
      return false;
#endif
   }
   else if ( fDBMSType == "sqlite" ){
#if defined ( HAVE_SQLITE )
      fSQL = new ROMESQLite();
#else
      LinkError();
      return false;
#endif
   }
   else if ( fDBMSType == "sqlite3" ){
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

bool ROMESQLDataBase::Read(ROMEStr2DArray *values,const char *dataBasePath,int runNumber){
   int iField,iOrder;
   int iLastOrder=0;
   int iArray,jArray;
   int iCount;
   bool keepCursor=false;
   ROMEPath *path = new ROMEPath();
   ROMEString fieldName;
   ROMEString sqlQuery;
   ROMEString orderField;
   
   if (!path->Decode(dataBasePath,runNumber)) {
      cout << "Path decode error : " << dataBasePath << endl;
      delete path;
      return false;
   }
   
   this->ResetPhrase();
   if(!MakePhrase(path,runNumber)){
      cout<<"Invalid input for database read."<<endl;
      delete path;
      return false;
   }
   if(!fFromPhrase.Contains(path->GetOrderTableName())){
      cout<<"Invalid path for database read."<<endl
          <<"order tabele("<<path->GetOrderTableName()<<") should be in path"<<endl;
      delete path;
      return false;
   }
   if(path->IsOrderArray())
      orderField.AppendFormatted("%s.%s",
                                 strlen(path->GetOrderTableName())
                                 ? path->GetOrderTableName() : path->GetTableNameAt(path->GetNumberOfTables()-1),
                                 strlen(path->GetOrderFieldName())
                                 ? path->GetOrderFieldName() : "idx");
   
   sqlQuery = "SELECT ";
   sqlQuery += fFieldList;
   sqlQuery.ReplaceAll(RSQLDB_STR,"");
   if(path->IsOrderArray())
      sqlQuery.AppendFormatted(",%s",orderField.Data());
   sqlQuery.AppendFormatted(" FROM %s",fFromPhrase.Data());
   if(fWherePhrase.Length())
      sqlQuery.AppendFormatted(" WHERE %s",fWherePhrase.Data());
   if(orderField.Length()){
      if(sqlQuery.Contains("WHERE"))
         sqlQuery += " AND ";
      else
         sqlQuery += " WHERE ";
      sqlQuery.AppendFormatted("(%s BETWEEN %d AND %d)"
                               ,orderField.Data()
                               ,TMath::Min(path->GetOrderIndexAt(0),path->GetOrderIndexAt(1))
                               ,TMath::Max(path->GetOrderIndexAt(0),path->GetOrderIndexAt(1)));
      if(path->GetOrderIndexAt(2)!=1){
         if(fDBMSType == "mysql")
            sqlQuery.AppendFormatted(" AND MOD(%s-%d,%d)=0 "
                                     ,orderField.Data(),path->GetOrderIndexAt(0),path->GetOrderIndexAt(2));
         if(fDBMSType == "sqlite")
            sqlQuery.AppendFormatted(" AND ((%s-%d)%%%d)=0 "
                                     ,orderField.Data(),path->GetOrderIndexAt(0),path->GetOrderIndexAt(2));
         if(fDBMSType == "sqlite3")
            sqlQuery.AppendFormatted(" AND ((%s-%d)%%%d)=0 "
                                     ,orderField.Data(),path->GetOrderIndexAt(0),path->GetOrderIndexAt(2));
      }
      sqlQuery.AppendFormatted(" ORDER BY %s ",orderField.Data());
      if(path->GetOrderIndexAt(2)<0)
         sqlQuery += " DESC ";
   }
   sqlQuery += ";";
   
   if(!fSQL->MakeQuery((char*)sqlQuery.Data(),true)){
      cout<<"Invalid input for database read."<<endl;
      fSQL->FreeResult();
      delete path;
      return false;
   }
   if(!fSQL->GetNumberOfRows()){
      cout << "Warning: "<<path->GetTableNameAt(path->GetNumberOfTables()-1)<<"."<<path->GetFieldName();
      cout<<" was not found. Default value will be used."<<endl;
      fSQL->FreeResult();
      delete path;
      return true;
   }
   
   for(iOrder=path->GetOrderIndexAt(0),iArray=0
          ;!path->IsOrderArray() || InRange(iOrder,path->GetOrderIndexAt(0),path->GetOrderIndexAt(1))
          ;iOrder+=path->GetOrderIndexAt(2),iArray++){
      if(!keepCursor){
         if(!fSQL->NextRow()){
            cout << "Warning: some records were not found in "<<path->GetTableNameAt(path->GetNumberOfTables()-1)<<endl;
            fSQL->FreeResult();
            delete path;
            return true;
         }
      }
      keepCursor = false;
      
      if(path->IsOrderArray()){
         // check number of records which have the same order number.
         iCount = 0;
         while(TMath::Sign(atoi(fSQL->GetField(fSQL->GetNumberOfFields()-1)),path->GetOrderIndexAt(2))
               < TMath::Sign(iOrder,path->GetOrderIndexAt(2))){
            if(!fSQL->NextRow()){
               cout << "Warning: some records were not found in "<<path->GetTableNameAt(path->GetNumberOfTables()-1)<<endl;
               fSQL->FreeResult();
               delete path;
               return true;
            }
            iCount++ ;
         }
         if(iCount)
            cout << "Warning: "<<path->GetTableNameAt(path->GetNumberOfTables()-1)<<" has "<<iCount+1
                 <<" records which satisfy "<<path->GetOrderTableName()<<"."<<path->GetOrderFieldName()<<"="<<iLastOrder<<endl;
         
         // check if the record exists
         if(TMath::Sign(iOrder,path->GetOrderIndexAt(2))
            < TMath::Sign(atoi(fSQL->GetField(fSQL->GetNumberOfFields()-1)),path->GetOrderIndexAt(2))){
            cout << "Warning: "
                 <<path->GetTableNameAt(path->GetNumberOfTables()-1)<<"."<<path->GetFieldName()
                 <<"("<<path->GetOrderTableName()<<"."<<path->GetOrderFieldName()<<"="<<iOrder<<")"
                 <<" was not found. Default value will be used."<<endl;
            keepCursor = true;
            continue;
         }
      }
      for(iField=path->GetFieldIndexAt(0),jArray=0
             ;!path->IsFieldArray() || InRange(iField,path->GetFieldIndexAt(0),path->GetFieldIndexAt(1))
             ;iField+=path->GetFieldIndexAt(2),jArray++){
         values->SetAt(fSQL->GetField(jArray),iArray,jArray);
         iLastOrder = iOrder;
         if(!path->IsFieldArray())
            break;
      }            
      if(!path->IsOrderArray())
         break;
   }
   
   fSQL->FreeResult();      
   delete path;
   return true;
}

bool ROMESQLDataBase::Write(ROMEStr2DArray* values,const char *dataBasePath,int runNumber) {
   int iField;
   int iOrder;
   ROMEPath *path = new ROMEPath();
   ROMEString fieldName;
   ROMEString sqlQuery;
   ROMEString setPhrase;
   ROMEString temp;
   int iArray,jArray;
   int istart;
   bool exist;
   
   if (!path->Decode(dataBasePath,runNumber)) {
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
   if(strstr(path->GetTableConstraintAt(path->GetNumberOfTables()-1),"!=")){
      cout << " \"!=\" can not be used for default values." << endl;
      delete path;
      return false;
   }
   
   this->ResetPhrase();
   if(!MakePhrase(path,runNumber)){
      cout<<"Invalid input for database write."<<endl;
      delete path;
      return false;
   }
   setPhrase = fWherePhrase;
   setPhrase.ReplaceAll(" AND ",",");
   setPhrase.ReplaceAll(" and ",",");
   setPhrase.ReplaceAll(" And ",",");        
      
   for(iOrder=path->GetOrderIndexAt(0),iArray=0
          ;!path->IsOrderArray() || InRange(iOrder,path->GetOrderIndexAt(0),path->GetOrderIndexAt(1))
          ;iOrder+=path->GetOrderIndexAt(2),iArray++){
      //check if the row exists
      sqlQuery = "SELECT ";
      sqlQuery += fFieldList;
      sqlQuery.ReplaceAll(RSQLDB_STR,"");
      sqlQuery += " FROM ";
      sqlQuery += fFromPhrase;
      if(fWherePhrase.Length())
         sqlQuery.AppendFormatted(" WHERE %s",fWherePhrase.Data());
      if(path->IsOrderArray()){
         if(!sqlQuery.Contains("WHERE"))
            sqlQuery += " WHERE ";
         else
            sqlQuery += " AND ";
         sqlQuery.AppendFormatted("%s.%s='%d'",path->GetTableNameAt(path->GetNumberOfTables()-1),
                                  strlen(path->GetTableIDXNameAt(path->GetNumberOfTables()-1)) 
                                  ? path->GetTableIDXNameAt(path->GetNumberOfTables()-1) : "idx"
                                  ,iOrder);
      }
      sqlQuery += " LIMIT 1;";
      if(!fSQL->MakeQuery((char*)sqlQuery.Data(),true)){
         cout << "Invalid input for database write."<< endl;
         fSQL->FreeResult();
         delete path;
         return false;
      }      
      exist = fSQL->GetNumberOfRows()!=0;
      fSQL->FreeResult();
      
      if(!exist){ // insert new record
         sqlQuery = "INSERT INTO ";
         sqlQuery += fFromPhrase;
         sqlQuery += " SET ";
         sqlQuery += fFieldList;
         for(iField=path->GetFieldIndexAt(0),jArray=0,istart=0
                ;!path->IsFieldArray() || InRange(iField,path->GetFieldIndexAt(0),path->GetFieldIndexAt(1))
                ;iField+=path->GetFieldIndexAt(2),jArray++){
            if ((istart=sqlQuery.Index(RSQLDB_STR,RSQLDB_STR_LEN,istart,TString::kIgnoreCase))!=-1) {
               temp.SetFormatted("='%s'",values->At(iArray,jArray).Data());
               sqlQuery.Remove(istart,RSQLDB_STR_LEN);
               sqlQuery.Insert(istart,temp);
            }
            if(!path->IsFieldArray())
               break;
         }         
         if(setPhrase.Length())
            sqlQuery.AppendFormatted(",%s",setPhrase.Data());
         if(path->IsOrderArray()){
            sqlQuery.AppendFormatted(",%s.%s ='%d'",path->GetTableNameAt(path->GetNumberOfTables()-1),
                                     strlen(path->GetTableIDXNameAt(path->GetNumberOfTables()-1)) 
                                     ? path->GetTableIDXNameAt(path->GetNumberOfTables()-1) : "idx"
                                     ,iOrder);
         }
         sqlQuery += ";";
      }

      else{ //update existing record
         sqlQuery = "UPDATE ";
         sqlQuery += fFromPhrase;
         sqlQuery += " SET ";
         sqlQuery += fFieldList;         
         for(iField=path->GetFieldIndexAt(0),jArray=0,istart=0
                ;!path->IsFieldArray() || InRange(iField,path->GetFieldIndexAt(0),path->GetFieldIndexAt(1))
                ;iField+=path->GetFieldIndexAt(2),jArray++){
            if ((istart=sqlQuery.Index(RSQLDB_STR,RSQLDB_STR_LEN,istart,TString::kIgnoreCase))!=-1) {
               temp.SetFormatted("='%s'",values->At(iArray,jArray).Data());
               sqlQuery.Remove(istart,RSQLDB_STR_LEN);
               sqlQuery.Insert(istart,temp);
            }
            if(!path->IsFieldArray())
               break;
         }
         if(fWherePhrase.Length())
            sqlQuery.AppendFormatted(" WHERE %s",fWherePhrase.Data());
         if(path->IsOrderArray()){
            if(sqlQuery.Contains("WHERE"))
               sqlQuery += " AND ";
            else
               sqlQuery += " WHERE ";
            sqlQuery.AppendFormatted("%s.%s ='%d'",path->GetTableNameAt(path->GetNumberOfTables()-1),
                                     strlen(path->GetTableIDXNameAt(path->GetNumberOfTables()-1)) 
                                     ? path->GetTableIDXNameAt(path->GetNumberOfTables()-1) : "idx"
                                     ,iOrder);
         }
         sqlQuery += " LIMIT 1;";
      }
      
#if defined ( SQLDEBUG )
      cout<<"ROMESQLDataBase::Write  : "<<sqlQuery<<endl;
#else
      if(!fSQL->MakeQuery((char*)sqlQuery.Data(),false)){
         cout<<"Invalid input for database write."<<endl;
         delete path;
         return false;
      }
#endif
      if(!path->IsOrderArray())
         break;
   }
   
   delete path;
   return true;
}

void ROMESQLDataBase::Print() {
   ROMEString temp = fFieldList;
   temp.ReplaceAll(RSQLDB_STR,"");
   cout << "******************************************************************************"<<endl
        << "   FIELD : " << temp         << endl
        << "   FROM  : " << fFromPhrase  << endl
        << "   WHERE : " << fWherePhrase << endl
        << "******************************************************************************"<<endl
        << endl;
}

