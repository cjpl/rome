// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMEXMLDataBase
//
//  XMLDataBase access.
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

#include <ROMEAnalyzer.h>
#include <ROMEXMLDataBase.h>

ROMEXMLDataBase::ROMEXMLDataBase() {
   xmlDB = NULL;
   xmlRunTable = new ROMEXML();
}

ROMEXMLDataBase::~ROMEXMLDataBase() {
}

bool ROMEXMLDataBase::Init(const char* path,const char* runTableName) {
   ROMEString runTable = path;
   runTable += runTableName;
   filePath = path;
   RunTableName = runTableName;
   if (!xmlRunTable->OpenFileForPath(runTable)) { 
      cout << "\nFailed to load xml database : '" << runTable << "'" << endl;
      cout << "Do you like the framework to generate a new xml database ([y]/n) ? ";
      char answer = 0;
      while (answer==0) {
         while (gROME->ss_kbhit()) {
            answer = gROME->ss_getchar(0);
         }
      }
      if (answer!='n') {
         ROMEXML *xmlNew = new ROMEXML();
         if (!xmlNew->OpenFileForWrite(runTable))
            return false;
         xmlNew->StartElement("RunTable");
         xmlNew->EndDocument();
         delete xmlNew;
         if (!xmlRunTable->OpenFileForPath(runTable)) {
            cout << "\nThe framework could not generated a new xml database." << endl;
            return false;
         }
         cout << "\nThe framework generated a new xml database." << endl;
      }
      else
         return false; 
   }
   return true;
}

void ROMEXMLDataBase::Read(TObjArray *values,const char *pathI,int runNumber,int numberOfFolders,int numberOfFields)
{
   char *cstop;
   char *name;
   int type;
   ROMEString value;
   ROMEString dbFile;
   ROMEString path = pathI;

   ROMEString xmlPath = "//";
   xmlPath += path(0,path.First('/')+1);
   xmlPath.AppendFormatted("Run_%0*d",5,runNumber);
   ROMEString attr = path;
   ROMEString folderName = path(path.Last('/')+1,path.Last('.')-(path.Last('/')+1));
   ROMEString fieldName = path(path.Last('.')+1,path.Length()-(path.Last('.')+1));

   if (xmlRunTable->GetPathAttribute(xmlPath,folderName+"File",dbFile)) {;
      ROMEString file = filePath+dbFile;
      xmlDB = new ROMEXML();
      if (!xmlDB->OpenFileForRead(file.Data())) { 
         cout << "Failed to load database : '" << file.Data() << "'" << endl;
         return;
      }
      while (xmlDB->NextLine()) {
         type = xmlDB->GetType();
         name = xmlDB->GetName();
         if (type == 1 && !strcmp(name,folderName)) {
            xmlDB->GetAttribute("Number",value,"0");
            int num = strtol(value.Data(),&cstop,10);
            while (xmlDB->NextLine()) {
               type = xmlDB->GetType();
               name = xmlDB->GetName();
               if (type == 1 && !strcmp(name,fieldName.Data())) {
                  if (xmlDB->GetValue(value)) {
                     values->AddLast(new TObjString(value.Data()));
                  };
               };
               if (type == 15 && !strcmp(name,folderName))
                  break;
            }
         }
         if (type == 15 && !strcmp(name,folderName+"s"))
            break;
      }
      delete xmlDB;
   }
}
void ROMEXMLDataBase::Write(const char *path,TObjArray* values)
{
}
/*
        fSQL->WritePathFields("RunCatalog/LPConf/(LPCalibPedestal)/LPCalibPedestal.mean0"
			      ,this->GetCurrentRunNumber(),pfield,"version=0",fLPCalibPedestalObjects->GetEntries());
      ROMEString name;
      ROMEString value;
      ROMEString dbFile;
      ROMEString filename;
      ROMEString path;
      ROMEString runNumberString;
      int n=0,i;
      ROMEXML *xml = new ROMEXML();
      this->GetCurrentRunNumberString(runNumberString);
      filename.SetFormatted("%s/RunTable.xml",this->GetInputDir());
      if (!xml->OpenFileForPath(filename.Data()))
         return;
      path.SetFormatted("//RunTable/Run_%s",runNumberString.Data());
      if (!xml->ExistPath(path.Data())) {
         name.SetFormatted("Run_%s",runNumberString.Data());
         if (!xml->HasPathChildren("//RunTable")) {
            xml->NewPathChildElement("//RunTable",name.Data(),NULL);
         }
         else {
            bool exist = true;
            n = this->GetCurrentRunNumber();
            while (!xml->ExistPath(path.Data())) {
               if (n==0) {
                  exist = false;
                  break;
               }
               path.SetFormatted("//RunTable/Run_%05d",n--);
            }
            if (exist) {
               xml->NewPathNextElement(path.Data(),name.Data(),NULL);
            }
            else {
               xml->NewPathPrevElement(path.Data(),name.Data(),NULL);
            }
         }
         path.SetFormatted("//RunTable/Run_%s",runNumberString.Data());
         dbFile = "dbCMCalib";
         dbFile.Append(runNumberString.Data());
         dbFile.Append("_0.xml");
         xml->NewPathAttribute(path.Data(),"CMCalibFile",dbFile.Data());
      }
      else {
         if (xml->GetPathAttribute(path,"CMCalibFile",dbFile)) {
            NextFile(dbFile,dbFile);
            xml->ReplacePathAttributeValue(path.Data(),"CMCalibFile",dbFile.Data());
         }
         else {
            dbFile = "dbCMCalib";
            dbFile.Append(runNumberString.Data());
            dbFile.Append("_0.xml");
            xml->NewPathAttribute(path.Data(),"CMCalibFile",dbFile.Data());
         }
      }
      xml->WritePathFile(filename.Data());
      filename.SetFormatted("%s/%s",this->GetDataBaseDir(),dbFile.Data());
      if (!xml->OpenFileForWrite(filename.Data()))
         return;
      xml->StartElement("CMCalibs");
      for (i=0;i<fCMCalibObjects->GetEntries();i++) {
         xml->StartElement("CMCalib");
         value.SetFormatted("%d",i);
         xml->WriteAttribute("Number",value.Data());
         value.SetFormatted("%f",((MEGCMCalib*)fCMCalibObjects->At(i))->GetADCPedestal());
         xml->WriteElement("ADCPedestal",value.Data());
         value.SetFormatted("%f",((MEGCMCalib*)fCMCalibObjects->At(i))->GetADCGain());
         xml->WriteElement("ADCGain",value.Data());
         value.SetFormatted("%f",((MEGCMCalib*)fCMCalibObjects->At(i))->GetTDCOffset());
         xml->WriteElement("TDCOffset",value.Data());
         value.SetFormatted("%f",((MEGCMCalib*)fCMCalibObjects->At(i))->GetTDCXScale());
         xml->WriteElement("TDCXScale",value.Data());
         xml->EndElement();
      }
      xml->EndDocument();
      delete xml;
*/
