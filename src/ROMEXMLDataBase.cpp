// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMEXMLDataBase
//
//  XMLDataBase access.
//
//  $Log$
//  Revision 1.14  2005/03/23 09:06:11  schneebeli_m
//  libxml replaced by mxml, Bool SP error
//
//  Revision 1.13  2005/03/21 17:29:47  schneebeli_m
//  minor changes
//
//  Revision 1.12  2005/03/17 15:44:52  schneebeli_m
//  GetAbsolutePath error removed
//
//  Revision 1.11  2005/03/13 08:43:14  sawada
//  removed or comment out unused variables.
//  made virtual destructor of ROMEConfig and ROMEDataBase.
//
//  Revision 1.10  2005/03/03 19:24:32  sawada
//  compatibility with SQL and XML database.
//
//  Revision 1.9  2005/03/01 14:34:23  sawada
//  compatibility of SQL and XML database of @constraint.
//  bug fix of loop counter.
//  field separator of field array was changed from $ to __.
//
//  Revision 1.8  2005/01/27 16:21:06  schneebeli_m
//  print method & no gROME in path
//
//  Revision 1.7  2004/11/16 16:14:01  schneebeli_m
//  implemented task hierarchy
//
//  Revision 1.6  2004/11/12 17:35:18  schneebeli_m
//  fast xml database
//
//  Revision 1.5  2004/11/11 13:17:37  schneebeli_m
//  detail
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

#include <ROMEAnalyzer.h>
#include <ROMEXMLDataBase.h>
#include <ROMEPath.h>
#include <ROMEStrArray.h>

ROMEXMLDataBase::ROMEXMLDataBase() {
}

ROMEXMLDataBase::~ROMEXMLDataBase() {
}

bool ROMEXMLDataBase::Init(const char* path,const char* connection) {
   fDirectoryPath = path;
   fDataBaseName = connection;
   return true;
}

bool ROMEXMLDataBase::Read(ROMEStr2DArray *values,const char *dataBasePath,int runNumber)
{
   int i,j,k,ii,index;
   ROMEString value;
   ROMEString val;
   ROMEString id;
   ROMEString file;
   ROMEString xmlPath;
   ROMEPath *path = new ROMEPath();
   bool handleArray = false;
   ROMEString basePath;
   ROMEXML *xml=NULL;
   ROMEStr2DArray pointerArray;
   ROMEStr2DArray fieldArray;
   char *cstop;
   int orderTableIndex = -1;
   int nArrayTables = -1;
   ROMEString ConstraintPath;
   ROMEString ConstraintTable;
   ROMEString ConstraintField;
   ROMEStr2DArray ConstraintValue(1,1);

   // decode path
   if (!path->Decode(dataBasePath,runNumber)) {
      cout << "\nPath decode error : " << dataBasePath << endl;
      delete path;
      return false;
   }
//   path->Print();


   // search through the tables
   for (i=0;i<path->GetNumberOfTables();i++) {
      // open file
      file = fDirectoryPath;
      file += "/";
      file += path->GetTableNameAt(i);
      file += ".xml";
      xml = new ROMEXML();
      if (!xml->OpenFileForPath(file.Data())) { 
         cout << "\nFailed to load xml database file : '" << file.Data() << "'" << endl;
         delete xml;
         delete path;
         return false;
      }
      // create path
      xmlPath = "/";
      xmlPath += fDataBaseName;
      xmlPath += "/";
      xmlPath += path->GetTableNameAt(i);
      // add constraint
      if(strlen(path->GetTableConstraintAt(i))){
         int itmp;
         if (!path->DecodeConstraint(path->GetTableConstraintAt(i))) {
            return false;
         }
         for(j=0;j<path->GetNumberOfConstraints();j++){
            xmlPath += "[";
            xmlPath += path->GetConstraintFieldAt(j);
            
            ConstraintTable = path->GetConstraintValueAt(j);
            if ((itmp=ConstraintTable.Index(".",1,0,TString::kExact))!=-1) {
               ConstraintField = ConstraintTable(itmp,ConstraintTable.Length());
               ConstraintTable = ConstraintTable(0,itmp);
               ConstraintTable.ReplaceAll(" ","");
               path->GetAbsolutePath(ConstraintPath,ConstraintTable);
               ConstraintPath += "/";
               ConstraintPath += ConstraintField(1,ConstraintField.Length());
               Read(&ConstraintValue,ConstraintPath.Data(),runNumber);
               xmlPath += "=";
               xmlPath += ConstraintValue.At(0,0).Data();
            }
            else{
               xmlPath += "=";
               xmlPath += path->GetConstraintValueAt(j);
            }
            xmlPath += "]";
         }
      }      
      if (i>0) {
         if (strlen(path->GetTableIDNameAt(i-1))>0) {
            xmlPath += "[";
            xmlPath += path->GetTableIDNameAt(i-1);
            xmlPath += "=";
            xmlPath += id.Data();
            xmlPath += "]";
         }
      }
      xmlPath += "/";
      // Data base constraint
      if (strlen(path->GetTableDBConstraintAt(i))>0) {
         basePath = xmlPath;
         int istart,iend;
         ROMEString newDataBasePath;
         ROMEString dbPath = dataBasePath;

         // build new path
         if ((istart=dbPath.Index("(@",2,0,TString::kExact))==-1) {
            cout << "\nData base constraint statment not found : " << dataBasePath << endl;
            delete xml;
            delete path;
            return false;
         }
         if ((iend=dbPath.Index(")",1,istart,TString::kExact))==-1) {
            cout << "\nData base constraint statment not closed : " << dataBasePath << endl;
            delete xml;
            delete path;
            return false;
         }
         newDataBasePath = dbPath(0,istart);
         newDataBasePath += dbPath(iend+1,dbPath.Length()-iend-1);

         // read data base constraint
         xmlPath += path->GetTableDBConstraintAt(i);
         if (!xml->GetPathValue(xmlPath,value)) {
            cout << "\nWrong path for data base constraint : " << xmlPath.Data() << endl;
            delete xml;
            delete path;
            return false;
         }
         ROMEString temp = path->GetTableNameAt(i+1);
         temp += "=\"";
         int is,ie;
         // constraint is a path
         if ((is=value.Index(temp,temp.Length(),0,TString::kIgnoreCase))!=-1) {
            if ((ie=value.Index("\"",1,is+temp.Length(),TString::kIgnoreCase))!=-1) {
               value = value(is+temp.Length(),ie-is-temp.Length());
               if(value(value.Length()-1)=='/')
                  value.Remove(value.Length()-1,value.Length());
               while ((is=value.Index("(@@",3,0,TString::kIgnoreCase))!=-1) {
                  if ((ie=value.Index(")",1,is+3,TString::kIgnoreCase))==-1)
                     ie = value.Length();
                  val = value(is+3,ie-is-3);
                  path->GetAbsolutePath(ConstraintPath,path->GetTableNameAt(i));
                  ConstraintPath += "/";
                  ConstraintPath += val;
                  Read(&ConstraintValue,ConstraintPath.Data(),runNumber);
                  value.Remove(is,ie-is+1);
                  value.Insert(is,ConstraintValue.At(0,0));           
               }
               newDataBasePath.InsertFormatted(istart,(char*)value.Data());
               // decode new path
               delete xml;
               delete path;
               return Read(values,newDataBasePath.Data(),runNumber);
            }
         }
         // constraint is not a path
         xmlPath = basePath;
         xmlPath += "[";
         xmlPath += value;
         xmlPath += "]";
      }
      // next table
      ROMEString xmlBase = xmlPath;
      id.Resize(0);
      if (strlen(path->GetTableIDNameAt(i))>0&&i<path->GetNumberOfTables()-1) {
         xmlPath += path->GetTableNameAt(i+1);
         xmlPath += "_";
         xmlPath += path->GetTableIDNameAt(i);
         if (!xml->GetPathValue(xmlPath,id)) {
            cout << "\nWrong data base path : " << xmlPath.Data() << endl;
            delete xml;
            delete path;
            return false;
         }
      }
      if (path->IsOrderArray()&&!strcmp(path->GetTableNameAt(i),path->GetOrderTableName())) {
         handleArray = true;
         orderTableIndex = i;
      }
      ROMEStrArray value;
      if (handleArray) {
         nArrayTables++;
         ROMEStrArray idx;
         xmlPath = xmlBase;
         if (!strcmp(path->GetTableNameAt(i),path->GetOrderTableName()))
            xmlPath += path->GetOrderFieldName();
         else
            xmlPath += path->GetTableIDXNameAt(i-1);
         if (!xml->GetPathValues(xmlPath,&idx)) {
            cout << "\nWrong data base path : " << xmlPath.Data() << endl;
            delete xml;
            delete path;
            return false;
         }
         xmlPath = xmlBase;
         // Table pointers
         if (i<path->GetNumberOfTables()-1) {
            xmlPath += path->GetTableNameAt(i+1);
            xmlPath += "_";
            if (!strcmp(path->GetTableNameAt(i),path->GetOrderTableName()))
               xmlPath += path->GetOrderFieldName();
            else
               xmlPath += path->GetTableIDXNameAt(i-1);
            if (!xml->GetPathValues(xmlPath,&value)) {
               cout << "\nWrong data base path : " << xmlPath.Data() << endl;
               delete xml;
               delete path;
               return false;
            }
            if (value.GetEntriesFast()!=idx.GetEntriesFast()) {
               cout << "\nInvalid Table : " << path->GetTableNameAt(i) << endl;
               delete xml;
               delete path;
               return false;
            }
            for (j=0;j<value.GetEntriesFast();j++) {
               int idxValue = strtol(idx.At(j).Data(),&cstop,10);
               if (idxValue>=0)
                  pointerArray.SetAt(value.At(j),nArrayTables,idxValue);
            }
         }
         // Field
         else {
            if (path->IsFieldArray()) {
               for (j=path->GetFieldIndexAt(0);j<=path->GetFieldIndexAt(1);j=j+path->GetFieldIndexAt(2)) {
                  xmlPath = xmlBase;
                  xmlPath += path->GetFieldName();
                  xmlPath.AppendFormatted("__%d",j);
                  if (!xml->GetPathValues(xmlPath,&value)) {
                     cout << "\nWrong data base path : " << xmlPath.Data() << endl;
                     delete xml;
                     delete path;
                     return false;
                  }
                  if (value.GetEntriesFast()!=idx.GetEntriesFast()) {
                     cout << "\nInvalid Table : " << path->GetTableNameAt(i) << endl;
                     delete xml;
                     delete path;
                     return false;
                  }
                  for (k=0;k<value.GetEntriesFast();k++) {
                     int idxValue = strtol(idx.At(k).Data(),&cstop,10);
                     if (idxValue>=0)
                        fieldArray.SetAt(value.At(k),j,idxValue);
                  }
               }
            }
            else {
               xmlPath += path->GetFieldName();
               if (!xml->GetPathValues(xmlPath,&value)) {
                  cout << "\nWrong data base path : " << xmlPath.Data() << endl;
                  delete xml;
                  delete path;
                  return false;
               }
               if (value.GetEntriesFast()!=idx.GetEntriesFast()) {
                  cout << "\nInvalid Table : " << path->GetTableNameAt(i) << endl;
                  delete xml;
                  delete path;
                  return false;
               }
               for (j=0;j<value.GetEntriesFast();j++) {
                  int idxValue = strtol(idx.At(j).Data(),&cstop,10);
                  if (idxValue>=0)
                     fieldArray.SetAt(value.At(j),0,idxValue);
               }
            }
         }
      }
      else {
         if (i==path->GetNumberOfTables()-1) {
            if (path->IsFieldArray()) {
               for (j=path->GetFieldIndexAt(0);j<=path->GetFieldIndexAt(1);j=j+path->GetFieldIndexAt(2)) {
                  xmlPath = xmlBase;
                  xmlPath += path->GetFieldName();
                  xmlPath.AppendFormatted("__%d",j);
                  if (!xml->GetPathValues(xmlPath,&value)) {
                     cout << "\nWrong data base path : " << xmlPath.Data() << endl;
                     delete xml;
                     delete path;
                     return false;
                  }
                  values->SetAt(value.At(0),0,j-path->GetFieldIndexAt(0));
               }
            }
            else {
               xmlPath += path->GetFieldName();
               if (!xml->GetPathValues(xmlPath,&value)) {
                  cout << "\nWrong data base path : " << xmlPath.Data() << endl;
                  delete xml;
                  delete path;
                  return false;
               }
               values->SetAt(value.At(0),0,0);
            }
            return true;
         }
      }
   }
   // handle array
   for (ii=path->GetOrderIndexAt(0);ii<=path->GetOrderIndexAt(1);ii=ii+path->GetOrderIndexAt(2)) {
      index = ii;
      for (i=orderTableIndex;i<path->GetNumberOfTables();i++) {
         // add field
         if (i==path->GetNumberOfTables()-1) {
            // field array
            if (path->IsFieldArray()) {
               for (j=path->GetFieldIndexAt(0);j<=path->GetFieldIndexAt(1);j=j+path->GetFieldIndexAt(2)) {
                  values->SetAt(fieldArray.At(j,index),ii-path->GetOrderIndexAt(0),j);
               }
            }
            // single field
            else {
               values->SetAt(fieldArray.At(0,index),ii-path->GetOrderIndexAt(0),0);
            }
         }
         else {
            index = strtol(pointerArray.At(i-orderTableIndex,index).Data(),&cstop,10); // todo
         }
      }
   }
   delete xml;
   delete path;
   return true; 
}

bool ROMEXMLDataBase::Write(ROMEStr2DArray* values,const char *dataBasePath,int runNumber)
{
   int i,j,ii,istart,iend;
   ROMEString value;
   ROMEString constraint;
   ROMEString id;
   ROMEString idx;
   ROMEString file;
   ROMEString xmlPath;
   ROMEString fieldName;
   ROMEString xmlFieldPath;
   ROMEString xmlBasePath;
   ROMEString xmlTablePath;
   ROMEPath *path = new ROMEPath();

   // decode path
   if (!path->Decode(dataBasePath,runNumber)) {
      cout << "\nPath decode error : " << dataBasePath << endl;
      delete path;
      return false;
   }
//   path->Print();

   if (path->GetNumberOfTables()!=1) {
      cout << "\nWrong data base path : " << dataBasePath << endl;
      delete path;
      return false;
   }

   file = fDirectoryPath;
   file += "/";
   file += path->GetTableNameAt(0);
   file += ".xml";
   ROMEXML *xml = new ROMEXML();
   if (!xml->OpenFileForPath(file.Data())) { 
      cout << "\nFailed to load xml database file : '" << file.Data() << "'" << endl;
      delete xml;
      delete path;
      return false;
   }

   xmlPath = "//";
   xmlPath += fDataBaseName;
   xmlPath += "/";
   xmlPath += path->GetTableNameAt(0);
   xmlTablePath = xmlPath;
   // add constraint
   if (strlen(path->GetTableConstraintAt(0))>0) {
      xmlPath += "[";
      xmlPath += path->GetTableConstraintAt(0);
      xmlPath += "]";
   }
   xmlBasePath = xmlPath;

   if (path->IsOrderArray()) {
      iend = path->GetOrderIndexAt(1);
   }
   else
      iend = 0;
   istart = path->GetOrderIndexAt(0);
   for (i=istart;i<=iend||iend==-1;i=i+path->GetOrderIndexAt(2)) {
      if (path->IsOrderArray()) {
         xmlPath = xmlBasePath;
         xmlPath += "[idx=";
         xmlPath.AppendFormatted("%d",i);
         xmlPath += "]";
      }
      xmlFieldPath = xmlPath;
      xmlFieldPath += "/";
      xmlFieldPath += path->GetFieldName();
      if (values==NULL) {
         cout << "\nInvalid input for database write." << endl;
         delete xml;
         delete path;
         return false;
      }
      if (path->IsFieldArray()) {
         for (j=path->GetFieldIndexAt(0);j<=path->GetFieldIndexAt(1);j=j+path->GetFieldIndexAt(2)) {
            if (values->At(i-istart,j)==NULL) {
               cout << "\nInvalid input for database write." << endl;
               delete xml;
               delete path;
               return false;
            }
            fieldName = path->GetFieldName();
            fieldName.AppendFormatted("__%d",j);
            xmlFieldPath.AppendFormatted("__%d",j);
            // new
            if (!xml->GetPathValue(xmlPath,value)) {
               xmlPath = xmlTablePath;
               int pos = xml->NewPathLastElement(xmlPath.Data(),path->GetTableNameAt(0),"");
               xmlPath.AppendFormatted("[position()=%d]",pos+1);
               if (path->IsOrderArray()) {
                  value.SetFormatted("%d",i);
                  xml->NewPathChildElement(xmlPath.Data(),"idx",value);
               }
               if (!path->DecodeConstraint(path->GetTableConstraintAt(0))) {
                  delete xml;
                  delete path;
                  return false;
               }
               for (ii=0;ii<path->GetNumberOfConstraints();ii++) {
                  xml->NewPathChildElement(xmlPath.Data(),path->GetConstraintFieldAt(ii),path->GetConstraintValueAt(ii));
               }
               xml->NewPathChildElement(xmlPath.Data(),fieldName.Data(),values->At(i-istart,j).Data());
            }
            // replace
            else {
               if (!xml->GetPathValue(xmlFieldPath,value)) {
                  xml->NewPathChildElement(xmlPath.Data(),fieldName.Data(),values->At(i-istart,j).Data());
               }
               else {
                  xml->ReplacePathValue(xmlFieldPath.Data(),values->At(i-istart,j).Data());
               }
            }
         }
      }
      else {
         if (values->At(i-istart,0)==NULL) {
            cout << "\nInvalid input for database write." << endl;
            delete xml;
            delete path;
            return false;
         }
         // new
         if (!xml->GetPathValue(xmlPath,value)) {
            xmlPath = xmlTablePath;
            int pos = xml->NewPathLastElement(xmlPath.Data(),path->GetTableNameAt(0),"");
            xmlPath.AppendFormatted("[position()=%d]",pos+1);
            if (path->IsOrderArray()) {
               value.SetFormatted("%d",i);
               xml->NewPathChildElement(xmlPath.Data(),"idx",value);
            }
            if (!path->DecodeConstraint(path->GetTableConstraintAt(0))) {
               delete xml;
               delete path;
               return false;
            }
            for (ii=0;ii<path->GetNumberOfConstraints();ii++) {
               xml->NewPathChildElement(xmlPath.Data(),path->GetConstraintFieldAt(ii),path->GetConstraintValueAt(ii));
            }
            xml->NewPathChildElement(xmlPath.Data(),path->GetFieldName(),values->At(i-istart,0).Data());
         }
         // replace
         else {
            if (!xml->GetPathValue(xmlFieldPath,value)) {
               xml->NewPathChildElement(xmlPath.Data(),path->GetFieldName(),values->At(i-istart,0).Data());
            }
            else {
               xml->ReplacePathValue(xmlFieldPath.Data(),values->At(i-istart,0).Data());
            }
         }
      }
   }
   xml->WritePathFile(file.Data());

   delete xml;
   delete path;

   return true;
}
