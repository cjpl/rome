// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMEXMLDataBase
//
//  XMLDataBase access.
//
//  $Log$
//  Revision 1.19  2005/04/14 07:56:46  schneebeli_m
//  Implemented odb database (offline)
//
//  Revision 1.18  2005/04/07 08:27:36  schneebeli_m
//  config bug, db bug
//
//  Revision 1.17  2005/04/05 15:03:05  schneebeli_m
//  linux
//
//  Revision 1.16  2005/04/05 14:54:52  schneebeli_m
//  Database write & _exit on linux
//
//  Revision 1.15  2005/04/01 14:56:24  schneebeli_m
//  Histo moved, multiple databases, db-paths moved, InputDataFormat->DAQSystem, GetMidas() to access banks, User DAQ
//
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

#include <ROMEXMLDataBase.h>

ROMEXMLDataBase::ROMEXMLDataBase() {
   xml = new ROMEXML();
}

ROMEXMLDataBase::~ROMEXMLDataBase() {
   delete xml;
}

bool ROMEXMLDataBase::Init(const char* name,const char* path,const char* connection) {
   fDirectoryPath = path;
   fDataBaseName = connection;
   fName = name;
   return true;
}


int  ROMEXMLDataBase::SearchTable(ROMEPath *path,ROMEStr2DArray *values,const char* dataBasePath,int runNumber,bool write) {
   int i,j,k,ii,index;
   ROMEString value;
   ROMEString val;
   ROMEString id;
   ROMEString xmlPath;
   bool handleArray = false;
   ROMEString basePath;
   char *cstop;
   int nArrayTables = -1;
   ROMEString ConstraintPath;
   ROMEString ConstraintTable;
   ROMEString ConstraintField;
   ROMEStr2DArray ConstraintValue(1,1);
   ROMEStrArray valueArr;

   fOrderTableIndex = -1;
   fPointerArray.RemoveAll();
   
   // decode path
   if (!path->Decode(dataBasePath,runNumber)) {
      cout << "\nPath decode error : " << dataBasePath << endl;
      return 0;
   }
//   path->Print();


   // search through the tables
   for (i=0;i<path->GetNumberOfTables();i++) {
      valueArr.RemoveAll();
      // open file
      fFileName = fDirectoryPath;
      fFileName += "/";
      fFileName += path->GetTableNameAt(i);
      fFileName += ".xml";
      if (!xml->OpenFileForPath(fFileName.Data())) { 
         cout << "\nFailed to load xml database file : '" << fFileName.Data() << "'" << endl;
         return 0;
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
            return 0;
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
         if ((istart=dbPath.Index("[@",2,0,TString::kExact))==-1) {
            cout << "\nData base constraint statment not found : " << dataBasePath << endl;
            return 0;
         }
         if ((iend=dbPath.Index("]",1,istart,TString::kExact))==-1) {
            cout << "\nData base constraint statment not closed : " << dataBasePath << endl;
            return 0;
         }
         newDataBasePath = dbPath(0,istart);
         newDataBasePath += dbPath(iend+1,dbPath.Length()-iend-1);

         // read data base constraint
         xmlPath += path->GetTableDBConstraintAt(i);
         if (!xml->GetPathValue(xmlPath,value)) {
            cout << "\nWrong path for data base constraint : " << xmlPath.Data() << endl;
            return 0;
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
               if (write) {
                  if (Write(values,newDataBasePath.Data(),runNumber))
                     return -1;
               }
               else {
                  if (Read(values,newDataBasePath.Data(),runNumber))
                     return -1;
               }
               return 0;
            }
         }
         // constraint is not a path
         xmlPath = basePath;
         xmlPath += "[";
         xmlPath += value;
         xmlPath += "]";
      }
      // next table
      fXMLBase = xmlPath;
      id.Resize(0);
      if (strlen(path->GetTableIDNameAt(i))>0&&i<path->GetNumberOfTables()-1) {
         xmlPath += path->GetTableNameAt(i+1);
         xmlPath += "_";
         xmlPath += path->GetTableIDNameAt(i);
         if (!xml->GetPathValue(xmlPath,id)) {
            cout << "\nWrong data base path : " << xmlPath.Data() << endl;
            return 0;
         }
      }
      if (path->IsOrderArray()&&!strcmp(path->GetTableNameAt(i),path->GetOrderTableName())) {
         handleArray = true;
         fOrderTableIndex = i;
      }
      if (handleArray) {
         nArrayTables++;
         fIDX.RemoveAll();
         xmlPath = fXMLBase;
         if (!strcmp(path->GetTableNameAt(i),path->GetOrderTableName()))
            xmlPath += path->GetOrderFieldName();
         else
            xmlPath += path->GetTableIDXNameAt(i-1);
         if (!xml->GetPathValues(xmlPath,&fIDX)) {
            cout << "\nWrong data base path : " << xmlPath.Data() << endl;
            return 0;
         }
         xmlPath = fXMLBase;
         // Table pointers
         if (i<path->GetNumberOfTables()-1) {
            xmlPath += path->GetTableNameAt(i+1);
            xmlPath += "_";
            if (!strcmp(path->GetTableNameAt(i),path->GetOrderTableName()))
               xmlPath += path->GetOrderFieldName();
            else
               xmlPath += path->GetTableIDXNameAt(i-1);
            if (!xml->GetPathValues(xmlPath,&valueArr)) {
               cout << "\nWrong data base path : " << xmlPath.Data() << endl;
               return 0;
            }
            if (valueArr.GetEntriesFast()!=fIDX.GetEntriesFast()) {
               cout << "\nInvalid Table : " << path->GetTableNameAt(i) << endl;
               return 0;
            }
            for (j=0;j<valueArr.GetEntriesFast();j++) {
               int idxValue = strtol(fIDX.At(j).Data(),&cstop,10);
               if (idxValue>=0)
                  fPointerArray.SetAt(valueArr.At(j),nArrayTables,idxValue);
            }
         }
         else {
            // Field : Array Folder
            return 2;
         }
      }
      else {
         if (i==path->GetNumberOfTables()-1) {
            // Field : Single Folder
            return 1;
         }
      }
   }
   return 2;
}

bool ROMEXMLDataBase::Read(ROMEStr2DArray *values,const char *dataBasePath,int runNumber)
{
   int i,ii,j,k,index;
   char *cstop;

   ROMEStr2DArray fieldArray;
   ROMEStrArray valueArr;
   ROMEString xmlPath;
   ROMEPath *path = new ROMEPath();

   int retValue = SearchTable(path,values,dataBasePath,runNumber,false);

   if (retValue==0) {
      delete path;
      return false;
   }
   if (retValue==-1) {
      delete path;
      return true;
   }

   // handle folder array
   if (retValue==2) {
      if (path->IsFieldArray()) {
         for (j=path->GetFieldIndexAt(0);j<=path->GetFieldIndexAt(1);j=j+path->GetFieldIndexAt(2)) {
            xmlPath = fXMLBase;
            xmlPath += path->GetFieldName();
            xmlPath.AppendFormatted("__%d",j);
            if (!xml->GetPathValues(xmlPath,&valueArr)) {
               cout << "\nWrong data base path : " << xmlPath.Data() << endl;
               delete path;
               return false;
            }
            if (valueArr.GetEntriesFast()!=fIDX.GetEntriesFast()) {
               cout << "\nInvalid Table : " << path->GetTableNameAt(path->GetNumberOfTables()-1) << endl;
               delete path;
               return false;
            }
            for (k=0;k<valueArr.GetEntriesFast();k++) {
               int idxValue = strtol(fIDX.At(k).Data(),&cstop,10);
               if (idxValue>=0)
                  fieldArray.SetAt(valueArr.At(k),j,idxValue);
            }
         }
      }
      else {
         xmlPath = fXMLBase;
         xmlPath += path->GetFieldName();
         if (!xml->GetPathValues(xmlPath,&valueArr)) {
            cout << "\nWrong data base path : " << xmlPath.Data() << endl;
            delete path;
            return false;
         }
         if (valueArr.GetEntriesFast()!=fIDX.GetEntriesFast()) {
            cout << "\nInvalid Table : " << path->GetTableNameAt(path->GetNumberOfTables()-1) << endl;
            delete path;
            return false;
         }
         for (j=0;j<valueArr.GetEntriesFast();j++) {
            int idxValue = strtol(fIDX.At(j).Data(),&cstop,10);
            if (idxValue>=0)
               fieldArray.SetAt(valueArr.At(j),0,idxValue);
         }
      }
      for (ii=path->GetOrderIndexAt(0);ii<=path->GetOrderIndexAt(1);ii=ii+path->GetOrderIndexAt(2)) {
         index = ii;
         for (i=fOrderTableIndex;i<path->GetNumberOfTables();i++) {
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
               index = strtol(fPointerArray.At(i-fOrderTableIndex,index).Data(),&cstop,10);
            }
         }
      }
   }
   // handle single folder
   else {
      if (path->IsFieldArray()) {
         for (j=path->GetFieldIndexAt(0);j<=path->GetFieldIndexAt(1);j=j+path->GetFieldIndexAt(2)) {
            xmlPath = fXMLBase;
            xmlPath += path->GetFieldName();
            xmlPath.AppendFormatted("__%d",j);
            if (!xml->GetPathValues(xmlPath,&valueArr)) {
               cout << "\nWrong data base path : " << xmlPath.Data() << endl;
               delete path;
               return false;
            }
            values->SetAt(valueArr.At(0),0,j-path->GetFieldIndexAt(0));
         }
      }
      else {
         xmlPath = fXMLBase;
         xmlPath += path->GetFieldName();
         if (!xml->GetPathValues(xmlPath,&valueArr)) {
            cout << "\nWrong data base path : " << xmlPath.Data() << endl;
            delete path;
            return false;
         }
         values->SetAt(valueArr.At(0),0,0);
      }
   }
   delete path;
   return true; 
}

bool ROMEXMLDataBase::WriteValue(ROMEXML *xml,ROMEPath *path,ROMEString& basePath,ROMEString& value,int arrayIndex)
{
   int i;
   ROMEString xmlPath;
   ROMEString fieldName = path->GetFieldName();
   if (arrayIndex>-1)
      fieldName.AppendFormatted("__%d",arrayIndex);

   xmlPath = basePath;
   xmlPath += fieldName;
   if (xml->ExistPath(xmlPath)) {
      // replace
      if (!xml->ReplacePathValue(xmlPath,value)) {
         cout << "\nWrong data base path : " << xmlPath.Data() << endl;
         delete path;
         return false;
      }
   }
   else {
      // new
      basePath = basePath(1,basePath.Length()-1);
      if (xml->ExistPath("/"+basePath)) {
         if (!xml->NewPathChildElement(basePath,fieldName,value)) {
            cout << "\nWrong data base path : " << ("/"+basePath).Data() << endl;
            delete path;
            return false;
         }
      }
      else {
         xmlPath.SetFormatted("/%s/%s",fDataBaseName.Data(),path->GetTableNameAt(0));
         int num = xml->NumberOfOccurrenceOfPath(xmlPath.Data())+1;
         if (!xml->NewPathLastElement(xmlPath.Data(),path->GetTableNameAt(0),NULL)) {
            cout << "\nWrong data base path : " << xmlPath.Data() << endl;
            delete path;
            return false;
         }
         xmlPath.AppendFormatted("[%d]",num);
         if (!xml->NewPathChildElement(xmlPath.Data(),fieldName,value)) {
            cout << "\nWrong data base path : " << xmlPath.Data() << endl;
            delete path;
            return false;
         }
         for (i=0;i<path->GetNumberOfConstraints();i++) {
            if (!xml->NewPathChildElement(xmlPath.Data(),path->GetConstraintFieldAt(i),path->GetConstraintValueAt(i))) {
               cout << "\nWrong data base path : " << xmlPath.Data() << endl;
               delete path;
               return false;
            }
         }
      }
   }
   return true;
}
bool ROMEXMLDataBase::Write(ROMEStr2DArray* values,const char *dataBasePath,int runNumber)
{
   int i,ii,j,k,index;
   char *cstop;

   ROMEStr2DArray fieldArray;
   ROMEStrArray valueArr;
   ROMEString xmlPath;
   ROMEPath *path = new ROMEPath();
   ROMEString vTemp;

   int retValue = SearchTable(path,values,dataBasePath,runNumber,true);

   if (retValue==0) {
      delete path;
      return false;
   }
   if (retValue==-1) {
      delete path;
      return true;
   }

   // handle folder array
   if (retValue==2) {
      if (path->IsFieldArray()) {
/*         for (j=path->GetFieldIndexAt(0);j<=path->GetFieldIndexAt(1);j=j+path->GetFieldIndexAt(2)) {
            xmlPath = fXMLBase;
            xmlPath += path->GetFieldName();
            xmlPath.AppendFormatted("__%d",j);
            if (!xml->GetPathValues(xmlPath,&valueArr)) {
               cout << "\nWrong data base path : " << xmlPath.Data() << endl;
               delete path;
               return false;
            }
            if (valueArr.GetEntriesFast()!=fIDX.GetEntriesFast()) {
               cout << "\nInvalid Table : " << path->GetTableNameAt(path->GetNumberOfTables()-1) << endl;
               delete path;
               return false;
            }
            for (k=0;k<valueArr.GetEntriesFast();k++) {
               int idxValue = strtol(fIDX.At(k).Data(),&cstop,10);
               if (idxValue>=0)
                  fieldArray.SetAt(valueArr.At(k),j,idxValue);
            }
         }*/
      }
      else {
         for (i=0;i<fIDX.GetEntriesFast();i++) {
            cout << i << endl;
            xmlPath = fXMLBase;
            xmlPath.InsertFormatted(xmlPath.Length()-1,"[idx=%d]",strtol(fIDX.At(i).Data(),&cstop,10));
            xmlPath += path->GetFieldName();
            path->Decode(xmlPath.Data(),0);
            vTemp = values->At(i,0);
            if (!WriteValue(xml,path,fXMLBase,vTemp,-1)) {
               delete path;
               return false;
            }
         }
      }
   }
   // handle single folder
   else {
      if (path->IsFieldArray()) {
         for (j=path->GetFieldIndexAt(0);j<=path->GetFieldIndexAt(1);j=j+path->GetFieldIndexAt(2)) {
            vTemp = values->At(0,j);
            if (!WriteValue(xml,path,fXMLBase,vTemp,j)) {
               delete path;
               return false;
            }
         }
      }
      else {
         vTemp = values->At(0,0);
         if (!WriteValue(xml,path,fXMLBase,vTemp,-1)) {
            delete path;
            return false;
         }
      }
   }
//   xml->WritePathFile(fFileName.Data());
   xml->WritePathFile("C:/Data/analysis/MEG/DataBase/x.xml");
   delete path;
   return true; 
}

/*

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
*/
