// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMESQL                                                             //
//                                                                      //
//  Provides SQL data base access.
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <Riostream.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <string.h>
#include <ROMESQL.h>
#include <ROMEStatic.h>

ROMESQL::ROMESQL() {
   mysql_init(&mysql);
}

bool ROMESQL::Connect(char *server,char *user,char *passwd,char *database) 
{
   if (!mysql_real_connect(&mysql, server, user, passwd,database,0,NULL,0)) {
      printf("Could not connect to the data base '%s' : Error: %s\n",database,mysql_error(&mysql));
      return false;
   }
   return true;
}

bool ROMESQL::CreateDataBase(char* database)
{
   char sqlquery[128];
   sprintf(sqlquery,"CREATE DATABASE %s;",database);
   if (mysql_query(&mysql,sqlquery)) {
      cout << "Could not create data base '"<<database<<"'. Error :" << mysql_error(&mysql) << endl;
      return false;
   }
   return true;
}

bool ROMESQL::DeleteDataBase(char* database)
{
   char sqlquery[128];
   sprintf(sqlquery,"DROP DATABASE IF EXISTS %s;",database);
   if (mysql_query(&mysql,sqlquery)) {
      cout << "Could not delete data base '"<<database<<"'. Error :" << mysql_error(&mysql) << endl;
      return false;
   }
   return true;
}

bool ROMESQL::CreateTable(char* table,char* fields)
{
   char sqlquery[128];
   sprintf(sqlquery,"CREATE TABLE %s (%s);",table,fields);
   if (mysql_query(&mysql,sqlquery)) {
      cout << "Could not create table '"<<table<<"'. Error :" << mysql_error(&mysql) << endl;
      return false;
   }
   return true;
}

bool ROMESQL::DeleteTable(char* table)
{
   char sqlquery[128];
   sprintf(sqlquery,"DROP TABLE IF EXISTS %s;",table);
   if (mysql_query(&mysql,sqlquery)) {
      cout << "Could not delete table '"<<table<<"'. Error :" << mysql_error(&mysql) << endl;
      return false;
   }
   return true;
}

bool ROMESQL::InsertRow(char *table,char* fields,char* values)
{
   char sqlquery[128000];
   sprintf(sqlquery,"INSERT INTO %s (%s) VALUES (%s);",table,fields,values);
   if (mysql_query(&mysql,sqlquery)) {
      cout << "Could not insert row into table '"<<table<<"'. Error :" << mysql_error(&mysql) << endl;
      return false;
   }
   return true;
}

bool ROMESQL::DeleteRow(char *table,char* constraint)
{
   char sqlquery[128000];
   sprintf(sqlquery,"DELETE FROM %s WHERE %s;",table,constraint);
   if (mysql_query(&mysql,sqlquery)) {
      cout << "Could not delete row from table '"<<table<<"'. Error :" << mysql_error(&mysql) << endl;
      return false;
   }
   return true;
}

bool ROMESQL::ReplaceField(char *table,char* field,char* value,char* constraint)
{
   char sqlquery[128];
   sprintf(sqlquery,"UPDATE %s SET %s=%s WHERE %s;",table,field,value,constraint);
   if (mysql_query(&mysql,sqlquery)) {
      cout << "Could not replace field '"<<field<<"' in table '"<<table<<"'. Error :" << mysql_error(&mysql) << endl;
      return false;
   }
   return true;
}

bool ROMESQL::ExistField(char *table,char* field)
{
   char sqlquery[128];
   sprintf(sqlquery,"select %s from %s;",field,table);
   if (mysql_query(&mysql,sqlquery)) {
      return false;
   }
   mysql_store_result(&mysql);
   return true;
}

bool ROMESQL::ReadField(char *table,char* field,char* constraint)
{
   char* defaultRow = "ROMESQL : invalid value";
   char sqlquery[128];
   sprintf(sqlquery,"select %s from %s where %s;",field,table,constraint);
   if (mysql_query(&mysql,sqlquery)) {
      cout << "Field '"<<field<<"' not found in table '"<<table<<"'. Error :" << mysql_error(&mysql) << endl;
      return false;
   }
   if (!(result = mysql_store_result(&mysql))) {
      cout << "No value found in Field '"<<field<<"'." << endl;
      return false;
   }
   if ((numberOfFields = mysql_num_fields(result))==0) {
      cout << "No value found in Field '"<<field<<"'." << endl;
      return false;
   }
   if (!(row = mysql_fetch_row(result))) {
      cout << "No value found in Field '"<<field<<"'." << endl;
      return false;
   }
   return true;
}

bool ROMESQL::MakeQuery(char* query)
{
   if (mysql_query(&mysql,query)) {
      cout << "Query error :" << mysql_error(&mysql) << endl;
      return false;
   }
   if (!(result = mysql_store_result(&mysql))) {
      cout << "Query error :" << mysql_error(&mysql) << endl;
      return false;
   }
   this->numberOfRows = -1;
   return true;
}

void ROMESQL::ReadPathFields(char *path, int start_id, char *start_id_extension)
{
  char sqlpath[1024];
  strcpy(sqlpath,path);
  
  char       sqlquery[2048];

  char from_phrase[1024]="";
  char where_phrase[1024]="";
  char order_phrase[1024]="";
  char group_definition[3][1024];
  char table_name[32][128];
  char field_name[128];
  char group_name[32][128];
  char entry[32][128];
  char id_extension[32][8]; // "id","idx"
  int  relation_type[32];   // 0: goto nonArray    1: goto Array
  int  isArray      = 0;     // number of transition from nonArray to Array, it is limited to 1.
  int  ngroup       = 0;
  int  nentry       = 0;
  int  group_number =-1;
  int  maxdepth     = 0;
  int  depth        = 0;

  char str[80];
  char* pnext1 = sqlpath;
  char* pnext2;
  int  i;

  memset(relation_type,0,sizeof(relation_type));

  //--  decode SQL path  --//
   do{
      strcpy(table_name[depth],ROMEStatic::strtok_x(pnext1,"/",&pnext1));
      if(table_name[depth][0] == '%'){
         relation_type[depth] = 1;
         strcpy(table_name[depth],&table_name[depth][1]);  
      }
      strcpy(id_extension[depth],"id");    

      pnext2 = table_name[depth];    
      while(strchr(pnext2,'|') != NULL){
         depth++;
         strcpy(id_extension[depth],"idx");
         ROMEStatic::strtok_x(pnext2,"|",&pnext2);
         strcpy(table_name[depth],ROMEStatic::strtok_x(pnext2,"|",&pnext2));
         if(table_name[depth][0] == '%'){
            relation_type[depth] = 1;
            strcpy(table_name[depth],&table_name[depth][1]);  
         }
      }
      depth++;
   }while(pnext1);
   maxdepth=depth-1;
   strcpy(id_extension[0],start_id_extension);
   //get field name
   pnext1=table_name[maxdepth];
   pnext1 = strstr(pnext1,".")+1;
   strtok(table_name[maxdepth],".");
   strcpy(field_name,pnext1);
    
   //--  start following relation.  --//
   sprintf(str,"%s.%s = %d",table_name[0],id_extension[0],start_id);    
   strcat(where_phrase,str);
  
   for(depth=0;depth<=maxdepth;depth++){    
      //add table to FROM phrase
      if(!relation_type[depth]){
         if(depth){
	         strcat(from_phrase,",");
         }
         strcat(from_phrase,table_name[depth]);
      }
    
      //add relation with next table to WHERE phrase    
      if(depth!=maxdepth && !relation_type[depth+1]){
         strcat(where_phrase," and ");
         sprintf(str,"%s.%s_%s = %s.%s"
	         ,table_name[depth]  , table_name[depth+1]  , id_extension[depth+1]
	         ,table_name[depth+1], id_extension[depth+1]);    
         strcat(where_phrase,str);
      }
    
      //Read informations at connecting node (nonArray -> Array)
      if(relation_type[depth+1]==1){
         isArray++;
         //get group
         sprintf(sqlquery
	         ,"SELECT %s.groups FROM %s WHERE %s;"
	         ,table_name[depth],from_phrase,where_phrase
	         );
         cout<<"6 : "<<sqlquery<<endl;
         MakeQuery(sqlquery);
         this->NextRow();
         pnext1 = this->GetField(1);
         
         //decode group name list
         ngroup=0;
         do{
	         strcpy(group_name[ngroup],ROMEStatic::strtok_x(pnext1," \t,:",&pnext1));
	         ngroup++;
         }while(pnext1);
      
         //get group definition
         for(i=0;i<ngroup;i++){
	         if(group_number!=-1){break;}
	         sprintf(sqlquery
		         ,"SELECT %s.%s_definition FROM %s WHERE %s;"
		         ,table_name[depth],group_name[i],from_phrase,where_phrase); 
               cout<<"7 : "<<sqlquery<<endl;
	         MakeQuery(sqlquery);
	         this->NextRow();
	         pnext1 = this->GetField(1);
            
	         //decode group definition
	         strcpy(group_definition[0],ROMEStatic::strtok_x(pnext1,":",&pnext1)); //table list
	         strcpy(group_definition[1],ROMEStatic::strtok_x(pnext1,":",&pnext1)); //condition
	         strcpy(group_definition[2],pnext1);                       //order
	
	         //decode menber list.    
	         nentry=0;
	         pnext1=group_definition[0];
	         do{
	            strcpy(entry[nentry],ROMEStatic::strtok_x(pnext1," \t,:",&pnext1));
	            if(!strcmp(entry[nentry],table_name[depth+1])){group_number=i;}
	            nentry++;
	         }while(pnext1);
         }
      
         //make FROM phrase
         for(i=0;i<nentry;i++){
	         strcat(from_phrase,",");
	         strcat(from_phrase,entry[i]);
         };
      
         //make WHERE phrase
         strcat(where_phrase," and ");
         strcat(where_phrase,group_definition[1]);
      
         for(i=0;i<nentry;i++){
	         sprintf(str," and %s.%s_%s = %s.%s"
		         ,table_name[depth],entry[i],id_extension[depth+1],entry[i],id_extension[depth+1]);
	         strcat(where_phrase,str);
         };
      
         //make ORDER BY phase
         if(isArray){
	         strcpy(order_phrase,group_definition[2]);
         }
      }//end of    "if(relation_type[depth+1]==1)"
   }//end of  "for(depth=0;depth<=maxdepth;depth++)"
  
   sprintf(sqlquery
	   ,"SELECT %s.%s FROM %s WHERE %s"
	   ,table_name[maxdepth] ,field_name
	   ,from_phrase          ,where_phrase
	   );
   if(isArray){
      strcat(sqlquery," ORDER BY ");
      strcat(sqlquery,order_phrase);
   }
     
   strcat(sqlquery,";");
   this->MakeQuery(sqlquery);

   return;
}

bool ROMESQL::NextRow() {
   if( !result ) {
      cout << "NextRow error : no query result" << endl;
      return false;
   }
   row = mysql_fetch_row(result);
   if( !row ) {
      cout << "NextRow error :" << mysql_error(&mysql) << endl;
      return false;
   }
   return true;
}

int ROMESQL::GetNumberOfRows() {
   if( !result ) {
      cout << "NextRow error :" << mysql_error(&mysql) << endl;
      return -1;
   }
   if( numberOfRows > -1 ) {
      return numberOfRows;
   }
   numberOfRows = mysql_num_rows(result);
   return numberOfRows;
}

int ROMESQL::GetNumberOfFields() {
   if( !this->row ) {
      cout << "GetFieldCount error : row is empty" << endl;
      return -1;
   }
   return mysql_num_fields(result);
}

char* ROMESQL::GetField(int fieldNumber) {
   if( !row ) {
      cout << "GetField error :" << mysql_error(&mysql) << endl;
      return NULL;
   }
   if( fieldNumber < 0 || fieldNumber >= GetNumberOfFields() ) {
      cout << "GetField error : field number out of bounds" << endl;
      return NULL;
   }
   return this->row[fieldNumber];
}
