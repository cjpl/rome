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
   numberOfRows = (int)mysql_num_rows(result);
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

int ROMESQL::DecodePath(char *path,char *start_id_extension,char **table_name,char **id_extension,int  &transition_depth,char *group_name,char *field_name)
{
  char sqlpath[1024];
  strcpy(sqlpath,path);
  char* pnext1 = sqlpath;
  char* pnext2;
  int   depth  = 0;
  int   maxdepth;
  transition_depth = -1;
  do{
    strcpy(table_name[depth],ROMEStatic::strtok_x(pnext1,"/",&pnext1));
    if(table_name[depth][0] == '('){
      transition_depth = depth;
      strcpy(group_name,&table_name[depth][1]);  
      group_name[strlen(group_name)-1]='\0'; //remove ')'
      continue;
    }
    pnext2 = table_name[depth];    
    if(strchr(pnext2,'|')){
      do{
	strcpy(table_name[depth],ROMEStatic::strtok_x(pnext2,"|",&pnext2));
	if(table_name[depth][0] == '('){
          transition_depth = depth;
	  strcpy(group_name,&table_name[depth][1]);  
	  group_name[strlen(group_name)-1]='\0'; //remove ')'
	  continue;
	}
	depth++;
	strcpy(id_extension[depth],"idx");
      }while(pnext2);
      depth--;
    }
    depth++;
    strcpy(id_extension[depth],"id");    
  }while(pnext1);
  
  maxdepth=depth-1;
  strcpy(id_extension[0],start_id_extension);
  //get field name
  pnext1=table_name[maxdepth];
  ROMEStatic::strtok_x(pnext1,".",&pnext1);
  strcpy(field_name,pnext1);

  return maxdepth;
}

void ROMESQL::ReadPathFields(char *path, int start_id, char *start_id_extension,int max){
  char sqlquery[2048];
  
  char from_phrase[4096]="";
  char where_phrase[4096]="";
  char order_phrase[4096]="";
  char limit_phrase[4096]="";
  char group_definition[3][4096];
  char table_name[32][128];
  char *ptable_name[32];
  char field_name[128];
  char entry[32][128];
  char id_extension[32][8];    // "id","idx"
  char *pid_extension[32];
  int  transition_depth;   // 
  int  nentry       = 0;
  int  maxdepth     = 0;
  int  depth        = 0;
  char group_name[128];

  char str[80];
  char* pnext1 = path;
  int  i;

  for(i=0;i<32;i++){
    ptable_name[i]=table_name[i];
    pid_extension[i]=id_extension[i];
  }
  maxdepth = DecodePath(path,start_id_extension
			     ,ptable_name,pid_extension,transition_depth
			     ,group_name,field_name);
  
  //--  start following relation.  --//
  sprintf(str,"%s.%s = %d",table_name[0],id_extension[0],start_id);    
  strcat(where_phrase,str);
  
  for(depth=0;depth<=maxdepth;depth++){
    
    //add table to FROM phrase
    if(transition_depth != depth){
      if(depth){
	      strcat(from_phrase,",");
      }
      strcat(from_phrase,table_name[depth]);
    }
    
    //add relation with next table to WHERE phrase    
    if(depth!=maxdepth && transition_depth != depth+1){
      strcat(where_phrase," and ");
      sprintf(str,"%s.%s_%s = %s.%s"
	      ,table_name[depth]  , table_name[depth+1]  , id_extension[depth+1]
	      ,table_name[depth+1], id_extension[depth+1]);    
      strcat(where_phrase,str);
    }

    //Read informations at connecting node (nonArray -> Array)
    if(transition_depth==depth+1){
      //get group definition
	   sprintf(sqlquery
		,"SELECT %s.%s_definition FROM %s WHERE %s;"
		,table_name[depth],group_name,from_phrase,where_phrase); 
//	cout<<sqlquery<<endl;	
         MakeQuery(sqlquery);
         this->NextRow();
         pnext1 = this->GetField(0);
//	cout<<"OK"<<endl;	
	//decode group definition
	strcpy(group_definition[0],ROMEStatic::strtok_x(pnext1,":",&pnext1)); //table list
	strcpy(group_definition[1],ROMEStatic::strtok_x(pnext1,":",&pnext1)); //condition
	strcpy(group_definition[2],pnext1);                       //order
	
	//decode menber list.    
	nentry=0;
	pnext1=group_definition[0];
	do{
	  strcpy(entry[nentry],ROMEStatic::strtok_x(pnext1," \t,:",&pnext1));
	  nentry++;
	}while(pnext1);
      
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
      if(transition_depth>=0){
	strcpy(order_phrase,group_definition[2]);
      }
    }//end of    "if(transition_depth==depth+1)"
  }//end of  "for(depth=0;depth<=maxdepth;depth++)"
  
  sprintf(sqlquery
	  ,"SELECT %s.%s FROM %s WHERE %s"
	  ,table_name[maxdepth] ,field_name
	  ,from_phrase          ,where_phrase
	  );
  if(transition_depth>=0){
    strcat(sqlquery," ORDER BY ");
    strcat(sqlquery,order_phrase);
  }
  
  sprintf(limit_phrase," LIMIT %d",max);
  strcat(sqlquery,limit_phrase);
  strcat(sqlquery,";");
  MakeQuery(sqlquery);

  return;
}

int ROMESQL::WritePathFields(char *path,int id,char **value,char *constraint,int number){
  char sqlpath[1024];
  char sqlconstraint[1024];
  strcpy(sqlpath,path);
  strcpy(sqlconstraint,constraint);
  
  char       sqlquery[2048];
  
  char where_phrase[4096]="";
  char table_name[32][64];
  char field_name[64];
  char* pnext1 = sqlpath;
  char constraint_element[64][64];
  int  nconstraint = 0;
  int  maxdepth     = 0;  
  int  i;
  char id_extension[32][8]; // "id","idx"
  char *ptable_name[32];
  char *pid_extension[32];
  char *start_id_extension="id";
  char group_name[128];
  int  transition_depth;

  for(i=0;i<32;i++){
    ptable_name[i]=table_name[i];
    pid_extension[i]=id_extension[i];
  }

  //--  decode SQL path  --//
  maxdepth = DecodePath(path,start_id_extension
			     ,ptable_name,pid_extension,transition_depth
			     ,group_name,field_name);
 
  //--decode  constraint  --//
  /*
    filedname1=value1,filedname2=value2
  */
  pnext1 = sqlconstraint;
  while(pnext1){
    strcpy(constraint_element[nconstraint],ROMEStatic::strtok_x(pnext1,",",&pnext1));
    nconstraint++;
  }    
  
  int j;
  //insert/update row
  for(i=0;i<number;i++){

    if(transition_depth>=0){
      sprintf(where_phrase,"id=%d AND idx=%d",id,i);
    }
    else{
      sprintf(where_phrase,"id=\"%d\"",id);
    }
    for(j=0;j<nconstraint;j++){
      strcat(where_phrase," AND ");
      strcat(where_phrase,constraint_element[j]);
    }
    sprintf(sqlquery
	    ,"SELECT %s FROM %s WHERE %s"
	    ,field_name,table_name[maxdepth] ,where_phrase
	    );
    strcat(sqlquery,";");
    MakeQuery(sqlquery);
    if(!this->GetNumberOfRows()){// if not exist
      if(transition_depth>=0){
      sprintf(sqlquery
	      ,"Insert INTO %s SET id=%d,idx=%d"
	      ,table_name[maxdepth] ,id,i
	      );
      }
      else{
      sprintf(sqlquery
	      ,"Insert INTO %s SET id=%d"
	      ,table_name[maxdepth] ,id
	      );
      }
      if(constraint){
	strcat(sqlquery,",");
	strcat(sqlquery,constraint);
      }
      strcat(sqlquery,";");
#ifndef READONLY
    MakeQuery(sqlquery);
#endif
    }
    sprintf(sqlquery
	    ,"UPDATE %s SET %s=\"%s\""
	    ,table_name[maxdepth] ,field_name,value[i]
	    );
    strcat(sqlquery," WHERE ");
    strcat(sqlquery,where_phrase);
    strcat(sqlquery,";");
#ifndef READONLY
    MakeQuery(sqlquery);
#endif
  }
  
  return 0;
}

/* old code

void ROMESQL::ReadPathFields(char *path, int start_id, char *start_id_extension)
{
  char sqlpath[1024];
  strcpy(sqlpath,path);
  
  char       sqlquery[2048];

  char from_phrase[4096]="";
  char where_phrase[4096]="";
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
         MakeQuery(sqlquery);
         this->NextRow();
         pnext1 = this->GetField(0);
         
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
	         MakeQuery(sqlquery);
	         this->NextRow();
	         pnext1 = this->GetField(0);
            
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

*/