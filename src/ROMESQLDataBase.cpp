// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMESQLDataBase
//
//  SQLDataBase access.
//
//  $Log$
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

#include <ROMESQLDataBase.h>
//      buffer.AppendFormatted("   return fSQL->Connect(\"pc4466.psi.ch\",\"rome\",\"rome\",\"%s\");\n",shortCut.Data());
ROMESQLDataBase::ROMESQLDataBase() {
   fSQL = new ROMESQL();
}

ROMESQLDataBase::~ROMESQLDataBase() {
}

bool ROMESQLDataBase::Init(const char* path,const char* runTableName) {
   return true;
}

int ROMESQLDataBase::DecodePath(char *path,char *start_id_extension,char **table_name,char **id_extension,int  &transition_depth,char *group_name,char *field_name)
{
  char sqlpath[1024];
  strcpy(sqlpath,path);
  char* pnext1 = sqlpath;
  char* pnext2;
  int   depth  = 0;
  int   maxdepth;
  transition_depth = -1;
  do{
    strcpy(table_name[depth],strtok_x(pnext1,"/",&pnext1));
    if(table_name[depth][0] == '('){
      transition_depth = depth;
      strcpy(group_name,&table_name[depth][1]);  
      group_name[strlen(group_name)-1]='\0'; //remove ')'
      continue;
    }
    pnext2 = table_name[depth];    
    if(strchr(pnext2,'|')){
      do{
	strcpy(table_name[depth],strtok_x(pnext2,"|",&pnext2));
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
  strtok_x(pnext1,".",&pnext1);
  strcpy(field_name,pnext1);

  return maxdepth;
}

void ROMESQLDataBase::Read(TObjArray *values,const char *path, int start_id,int max,int numberOfFields){
  char sqlquery[2048];
  
  char *start_id_extension="id";


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
  char* pnext1 = (char*)path;
  int  i;

  for(i=0;i<32;i++){
    ptable_name[i]=table_name[i];
    pid_extension[i]=id_extension[i];
  }
  maxdepth = DecodePath((char*)path,start_id_extension
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
         fSQL->MakeQuery(sqlquery);
         fSQL->NextRow();
         pnext1 = fSQL->GetField(0);
//	cout<<"OK"<<endl;	
	//decode group definition
	strcpy(group_definition[0],strtok_x(pnext1,":",&pnext1)); //table list
	strcpy(group_definition[1],strtok_x(pnext1,":",&pnext1)); //condition
	strcpy(group_definition[2],pnext1);                       //order
	
	//decode menber list.    
	nentry=0;
	pnext1=group_definition[0];
	do{
	  strcpy(entry[nentry],strtok_x(pnext1," \t,:",&pnext1));
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
  fSQL->MakeQuery(sqlquery);

  char *value;
  for(i=0;i<fSQL->GetNumberOfRows();i++){
     fSQL->NextRow();
     value = fSQL->GetField(0);
     values->AddLast(new TObjString(value));
  }
  return;
}

void ROMESQLDataBase::Write(const char *path,TObjArray* values) {}
/*int ROMESQLDataBase::Write(char *path,int id,char **value,char *constraint,int number){
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
  // filedname1=value1,filedname2=value2

  pnext1 = sqlconstraint;
  while(pnext1){
    strcpy(constraint_element[nconstraint],strtok_x(pnext1,",",&pnext1));
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
*/
char* ROMESQLDataBase::strtok_x(char *str,char* sub,char** tail) {
   int l = strlen(str);
   char* start = strtok(str,sub);
   *tail = str+strlen(str)+1;
   if (*tail>str+l) *tail = NULL;
   return start;
}
