// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMESQLDataBase
//
//  SQLDataBase access.
//
//  $Log$
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
#include <ROMEString.h>
#include <TObjString.h>
#include <limits.h>
#ifdef __linux__
#include <unistd.h>
#endif

#include <ROMEAnalyzer.h>
#include <ROMESQLDataBase.h>
#include <ROMEPath.h>
#include <ROMEStrArray.h>
#include <ROMEPath.h>

ROMESQLDataBase::ROMESQLDataBase() {
   fSQL = new ROMESQL();
   this->ResetPhrase();
}

ROMESQLDataBase::~ROMESQLDataBase() {
   delete fSQL;
}

void ROMESQLDataBase:: ResetPhrase(){
   fFromPhrase.Resize(0);
   fWherePhrase.Resize(0);
   fOrderPhrase.Resize(0);
   fLimitPhrase.Resize(0);
}

bool ROMESQLDataBase:: MakePhrase(ROMEPath* path){
   ROMEString sqlQuery;
   ROMEString sqlResult;
   ROMEString temp;
   int        iTable = 0;   
   int        iConstraint;
   
   if(path->IsOrderArray()){
      //limit phrase
      fLimitPhrase.AppendFormatted("%d",path->GetOrderIndexAt(1)-path->GetOrderIndexAt(0)+1);
      
      //order phrase
      if(strlen(path->GetOrderTableName())>0 && strlen(path->GetOrderFieldName())>0)
	 fOrderPhrase.AppendFormatted("%s.%s",path->GetOrderTableName(),path->GetOrderFieldName());
   }
   else if(!fLimitPhrase.Length()){
      fLimitPhrase = "1";
   }

#ifdef SQLDEBUG
   cout<<endl<<"******************************************************************************"<<endl;
   for(iTable=0;iTable<path->GetNumberOfTables();iTable++){
      cout<<"Table\t: "<<path->GetTableNameAt(iTable)<<endl;
   }
   cout<<"Field\t: "<<path->GetFieldName()<<endl<<endl;
   cout<<"following relations..."<<endl;
#endif
   
   // start following relation.
   for(iTable=0;iTable<path->GetNumberOfTables();iTable++){
#ifdef SQLDEBUG
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
	    cout << "\nWrong path for data base constraint : " << path->GetTableDBConstraintAt(iTable) << endl;
            return false;
	 }
	 fSQL->NextRow();
	 ROMEString value =  fSQL->GetField(0);
	 
	 ROMEPath *dbpath = new ROMEPath();
	 int is1,ie1,is2,ie2,is3,ie3;
	 int itmp[5];
	 ROMEString temp  =  path->GetTableNameAt(iTable+1);
         temp += "=\"";
	 ROMEString val = path->GetTableNameAt(iTable);
	 val += ".";
	 ROMEString dbConstraint;
	 ROMEString newdbConstraint;
	 ROMEString tname;
         
         // constraint is a path
	 if ((is1=value.Index(temp,temp.Length(),0,TString::kIgnoreCase))!=-1) {
            if ((ie1=value.Index("\"",1,is1+temp.Length(),TString::kIgnoreCase))!=-1) {
               value = value(is1+temp.Length(),ie1-is1-temp.Length());
               while ((is1=value.Index("(@@",3,0,TString::kIgnoreCase))!=-1) {
                  if ((ie1=value.Index(")",1,is1+3,TString::kIgnoreCase))==-1)
                     ie1 = value.Length();
		  dbConstraint = value(is1+3,ie1-is1-3);
		  if(dbConstraint.Index("_",1,0,TString::kIgnoreCase)!=-1){
		     value.Remove(is1+1,2);
		     value.Insert(is1,val);
		  }
		  else{
		     ie2=0;
		     while ((is2=value.Index("/",1,ie2,TString::kIgnoreCase))!=-1) {
			if(is2>is1)
			   break;
			is3 = is2+1;
			itmp[0] = value.Length();
			if ((itmp[1]=value.Index("/",1,is3,TString::kIgnoreCase))==-1)
			   itmp[1] = INT_MAX;
			if ((itmp[2]=value.Index("{",1,is3,TString::kIgnoreCase))==-1)
			   itmp[2] = INT_MAX;
			if ((itmp[3]=value.Index("(",1,is3,TString::kIgnoreCase))==-1)
			   itmp[3] = INT_MAX;
			if ((itmp[4]=value.Index("[",1,is3,TString::kIgnoreCase))==-1)
			   itmp[4] = INT_MAX;
			ie2 = TMath::MinElement(5,itmp)+1;
			ie3=ie2-1;
		     }
		     tname = value(is3,ie3-is3);
		     newdbConstraint.Resize(0);
		     newdbConstraint.AppendFormatted("%s=%s.%s_%s"
						     ,dbConstraint.Data(),path->GetTableNameAt(iTable)
						     ,tname.Data()
						     ,dbConstraint.Data());
		     value.Remove(is1+1,ie1-is1-1);
		     value.Insert(is1+1,newdbConstraint);
		  }
               }
	       dbpath->Decode(value);
	       MakePhrase(dbpath);
            }
         }
	 delete dbpath;
      }
      
      //add relation to next table to WHERE phrase    
      if(iTable<path->GetNumberOfTables()-1){
	 if(strlen(path->GetTableIDNameAt(iTable))){
	    if(fWherePhrase.Length())
	       fWherePhrase += " AND ";
	    fWherePhrase.AppendFormatted("%s.%s_%s=%s.%s"
					 , path->GetTableNameAt(iTable)
					 ,path->GetTableNameAt(iTable+1)
					 ,path->GetTableIDNameAt(iTable)
					 ,path->GetTableNameAt(iTable+1)
					 ,path->GetTableIDNameAt(iTable));
	 }
	 if(strlen(path->GetTableIDXNameAt(iTable))){
	    if(fWherePhrase.Length())
	       fWherePhrase += " AND ";
	    fWherePhrase.AppendFormatted("%s.%s_%s=%s.%s"
					 , path->GetTableNameAt(iTable)
					 ,path->GetTableNameAt(iTable+1)
					 ,path->GetTableIDXNameAt(iTable)
					 ,path->GetTableNameAt(iTable+1)
					 ,path->GetTableIDXNameAt(iTable));
	 }
      }
#ifdef SQLDEBUG
      cout<<"\t  ... OK"<<endl;
#endif
   }
   return true;
}

bool ROMESQLDataBase::Init(const char* dataBasePath,const char* runTableName) {
   ROMEString path = dataBasePath;
   ROMEString server;
   ROMEString user;
   ROMEString passwd;
   ROMEString database;
   ROMEString port;
   ROMEString prompt;
   int is,ie;
   int istart;

   //decode dataBasePath
   if ((istart=path.Index("mysql://",8,0,TString::kIgnoreCase))==-1) {
      cout << "\nWrong path for mysql database : " << path << endl;
      return false;
   }
   istart+=8;
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
#ifdef __linux__
      prompt.AppendFormatted("%s@%s's password: ",user.Data(),server.Data());
      passwd = getpass(prompt.Data());
#endif
// please implement similar function for windows
   }
#ifdef SQLDEBUG
   cout<<"******  SQL Connection  ******"<<endl
       <<"server   : "<<server<<endl
       <<"user     : "<<user<<endl
       <<"passwd   : "<<passwd<<endl
       <<"database : "<<database<<endl
       <<"port     : "<<port<<endl;
#endif
   fSQL->Connect(server.Data(),user.Data(),passwd.Data(),database.Data(),port.Data());
   return true;
}

bool ROMESQLDataBase::Read(ROMEStr2DArray *values,const char *dataBasePath){
   int iField;
   int iArray;
   int iRow;
   ROMEPath *path = new ROMEPath();
   ROMEString fieldName;
   ROMEString sqlQuery;
   int i,j;

   if (!path->Decode(dataBasePath)) {
      cout << "\nPath decode error : " << dataBasePath << endl;
      delete path;
      return false;
   }
//   path->Print();
   
   this->ResetPhrase();
   this->MakePhrase(path);
   
   iField=path->GetFieldIndexAt(0);
   j=0;
   do{
      fieldName = path->GetFieldName();
      if(path->IsFieldArray()){
	 fieldName.AppendFormatted("$%d",iField);
      }
      
      sqlQuery = "SELECT ";
      sqlQuery.AppendFormatted("%s.%s", path->GetTableNameAt(path->GetNumberOfTables()-1),fieldName.Data());
      if(fFromPhrase.Length()){
	 sqlQuery += " FROM ";
	 sqlQuery += fFromPhrase;
      }
      if(fWherePhrase.Length()){
	 sqlQuery += " WHERE ";
	 sqlQuery += fWherePhrase;
      }
      if(fOrderPhrase.Length()){
	 sqlQuery += " ORDER BY ";
	 sqlQuery += fOrderPhrase;
      }
      if(fLimitPhrase.Length()){
	 sqlQuery += " LIMIT ";
	 sqlQuery += fLimitPhrase;
      }
      sqlQuery += ";";
      
      if(!fSQL->MakeQuery((char*)sqlQuery.Data(),true)){
	 return false;
      }

      fSQL->NextRow();
      for(iRow=0;iRow<path->GetOrderIndexAt(0);iRow++){
	 fSQL->NextRow();
      }
      iArray=path->GetOrderIndexAt(0);
      i=0;
      do{
	 if(iArray>=fSQL->GetNumberOfRows()){
	    cout << "\nWarning: There is missing data in SQL Database for "
	    << path->GetTableNameAt(path->GetNumberOfTables()-1)<<"."<<path->GetFieldName() << endl;
	    break;
	 }
	 if(i!=0){
	    for(iRow=0;iRow<path->GetOrderIndexAt(2);iRow++)
	       fSQL->NextRow();
	 }
	 values->SetAt(fSQL->GetField(0),i,j);
	 i++;
	 iArray+=path->GetOrderIndexAt(2);
      }while(iArray<=path->GetOrderIndexAt(1));
      
      iField+=path->GetFieldIndexAt(2);
      j++;
   }while(iField<=path->GetFieldIndexAt(1));
   
   return true;
}

bool ROMESQLDataBase::Write(ROMEStr2DArray* values,const char *dataBasePath) {
/*
   int iField;
   ROMEPath *path = new ROMEPath();
   ROMEString fieldName;
   ROMEString sqlQuery;
   if (!path->Decode(dataBasePath)) {
      cout << "\nPath decode error : " << dataBasePath << endl;
      delete path;
      return false;
   }
//   path->Print();
   this->ResetPhrase();
   this->MakePhrase(path);
   
   for (iField=path->GetFieldIndexAt(0);iField<=path->GetFieldIndexAt(1);iField=iField+path->GetFieldIndexAt(2)) {
      fieldName = path->GetFieldName();
      if(path->IsFieldArray()){
	 fieldName.AppendFormatted("$%d",iField);
      }
      
      //check if the row exists
      sqlQuery += "SELECT FROM ";
      sqlQuery += fFromPhrase;
      sqlQuery += " WHERE ";
      if(fWherePhrase.Length()){
	 sqlQuery += " AND ";
	 sqlQuery += fWherePhrase;
      }
      sqlQuery += " AND ";
      sqlQuery += " id =";
      sqlQuery.AppendFormatted("%d",path->GetOrderIndexAt(path->GetNumberOfTables()));// maybe this is wrong
      sqlQuery += " LIMIT 1;";
      fSQL->MakeQuery((char*)sqlQuery.Data(),true);
      fSQL->NextRow();      
      
      if(fSQL->GetNumberOfFields()){ // row exists
	 sqlQuery = "INSERT INTO ";
      }
      else{
	 sqlQuery = "UPDATE ";
      }
      sqlQuery += fFromPhrase;
      sqlQuery += " SET ";
      sqlQuery.AppendFormatted("%s.%s=%s", path->GetTableNameAt(path->GetNumberOfTables())
			       ,fieldName,values->At(iField,0).Data());
      sqlQuery += ", ";
      sqlQuery.AppendFormatted("%s", path->GetTableNameAt(path->GetNumberOfTables()));
//      sqlQuery.AppendFormatted(".id = %d",path->GetTableIndexAt(path->GetNumberOfTables()));
      if(path->IsFieldArray()){
	 sqlQuery += " ,";
	 sqlQuery.AppendFormatted("%s", path->GetTableNameAt(path->GetNumberOfTables()));
	 sqlQuery.AppendFormatted(".idx = %d",iField);
      }
      sqlQuery += " WHERE ";
      sqlQuery += fWherePhrase;
      sqlQuery += " ORDER BY ";
	 sqlQuery += fOrderPhrase;
	 sqlQuery += " LIMIT 1;";
   }
   fSQL->MakeQuery((char*)sqlQuery.Data(),false);

   return true;
*/
}

void ROMESQLDataBase::Print() {
   cout << "******************************************************************************"<<endl
	<< "   FROM  : " << fFromPhrase << endl
	<< "   WHERE : " << fWherePhrase << endl
	<< "   ORDER : " << fOrderPhrase << endl
	<< "   LIMIT : " << fLimitPhrase << endl
	<< "******************************************************************************"<<endl
	<< endl;
}
