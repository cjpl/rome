// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMEXMLDataBase
//
//  XMLDataBase access.
//
//  $Log$
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

bool ROMEXMLDataBase::Read(ROMEStr2DArray *values,const char *dataBasePath)
{
   int i,j,k,ii,istart,iend,index;
   int iValue=0,jValue=0;
   ROMEString value;
   ROMEString val;
   ROMEString id;
   ROMEString file;
   ROMEString xmlPath;
   ROMEPath *path = new ROMEPath();
   bool handleArray = false;
   ROMEString basePath;
   ROMEXML *xml;
   ROMEStr2DArray pointerArray;
   ROMEStr2DArray fieldArray;
   char *cstop;
   int orderTableIndex = -1;
   int nArrayTables = -1;

   // decode path
   if (!path->Decode(dataBasePath)) {
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
      xmlPath = "//";
      xmlPath += fDataBaseName;
      xmlPath += "/";
      xmlPath += path->GetTableNameAt(i);
      // add constraint
      if (strlen(path->GetTableConstraintAt(i))>0) {
         xmlPath += "[";
         xmlPath += path->GetTableConstraintAt(i);
         xmlPath += "]";
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
               while ((is=value.Index("(@@",3,0,TString::kIgnoreCase))!=-1) {
                  if ((ie=value.Index(")",1,is+3,TString::kIgnoreCase))==-1)
                     ie = value.Length();
                  val = value(is+3,ie-is-3);
                  xmlPath = "//";
                  xmlPath += fDataBaseName;
                  xmlPath += "/";
                  xmlPath += path->GetTableNameAt(i);
                  xmlPath += "/";
                  xmlPath += val;
                  if (!xml->GetPathValue(xmlPath,val)) {
                     cout << "\nWrong path for data base value in a data base constraint : " << xmlPath.Data() << endl;
                     delete xml;
                     delete path;
                     return false;
                  }
                  value.Remove(is,ie-is+1);
                  value.Insert(is,val);
               }
               newDataBasePath.InsertFormatted(istart,(char*)value.Data());
               // decode new path
               delete xml;
               delete path;
               return Read(values,newDataBasePath.Data());
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
                  xmlPath.AppendFormatted("-%d",j);
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
                        fieldArray.SetAt(value.At(j),j,idxValue);
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
                  xmlPath.AppendFormatted("-%d",j);
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
                  values->SetAt(fieldArray.At(j,index),ii-path->GetOrderIndexAt(0),j=path->GetFieldIndexAt(0));
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

bool ROMEXMLDataBase::Write(ROMEStr2DArray* values,const char *dataBasePath)
{
   int i,j,ii,istart,iend,istep;
   int iValue=0,jValue=0;
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
   ROMEStrArray *array;
   ROMEPath *path = new ROMEPath();

   // decode path
   if (!path->Decode(dataBasePath)) {
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
            fieldName.AppendFormatted("-%d",j);
            xmlFieldPath.AppendFormatted("-%d",j);
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
