// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMEODBOnlineDataBase
//
//  Access to the Midas ODB parameters online.
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////

#if defined ( HAVE_MIDAS ) && !defined ( __MAKECINT__ )
#   include "midas.h"
#endif
#include "ROMEPath.h"
#include "ROMEAnalyzer.h"
#include "ROMEODBOnlineDataBase.h"
#include "ROMEiostream.h"

ClassImp(ROMEODBOnlineDataBase)

ROMEODBOnlineDataBase::ROMEODBOnlineDataBase() {
}

ROMEODBOnlineDataBase::~ROMEODBOnlineDataBase() {
}

Bool_t ROMEODBOnlineDataBase::Init(const char* name,const char* /*path*/,const char* /*connection*/) {
   fName = name;
   return true;
}

Bool_t ROMEODBOnlineDataBase::Read(ROMEStr2DArray *values,const char *dataBasePath,Long64_t runNumber,Long64_t eventNumber)
{
#if defined ( HAVE_MIDAS )
   if (gROME->isOnline()) {
      int i,iv;
      HNDLE hKey;
      char name[500];
      int type,num_values,item_size;
      char buffer[5000];
      int buffersize = sizeof(buffer);
      char value[500];
      ROMEString odbPath;

      ROMEPath *path = new ROMEPath();
      // decode path
      if (!path->Decode(dataBasePath,runNumber,eventNumber)) {
         ROMEPrint::Error("\nPath decode error : %s\n", dataBasePath);
         return 0;
      }
      for (i=0;i<path->GetNumberOfTables();i++) {
         odbPath += "/";
         odbPath += path->GetTableNameAt(i);
      }
      odbPath += "/";
      odbPath += path->GetFieldName();

      if (db_find_key(gROME->GetMidasOnlineDataBase(), 0, (char*)(odbPath.Data()), &hKey)!= CM_SUCCESS) {
         ROMEPrint::Error("\nCannot read online database\n");
         return false;
      }
      if (db_get_key_info(gROME->GetMidasOnlineDataBase(), hKey,name,sizeof(name), &type, &num_values,&item_size)!= CM_SUCCESS) {
         ROMEPrint::Error("\nCannot read online database\n");
         return false;
      }
      if (db_get_data(gROME->GetMidasOnlineDataBase(),hKey,buffer,&buffersize,type)!= CM_SUCCESS) {
         ROMEPrint::Error("\nCannot read online database\n");
         return false;
      }
      iv = 0;
      for (i=path->GetFieldIndexAt(0);i<path->GetFieldIndexAt(1);i+=path->GetFieldIndexAt(2)) {
         if (db_sprintf(value, buffer, buffersize, i, type)!= CM_SUCCESS) {
            ROMEPrint::Error("\nCannot read online database\n");
            return false;
         }
         values->SetAt(value,iv,0);
         iv++;
      }
   }
#endif // HAVE_MIDAS
   return true;
   WarningSuppression(values);
   WarningSuppression(dataBasePath);
   WarningSuppression(runNumber);
   WarningSuppression(eventNumber);
}

Bool_t ROMEODBOnlineDataBase::Write(ROMEStr2DArray* values,const char * dataBasePath,Long64_t runNumber,Long64_t eventNumber)
{
#if defined ( HAVE_MIDAS )
   if (gROME->isOnline()) {
      int i,iv;
      HNDLE hKey;
      char name[500];
      int type,num_values,item_size;
      char buffer[5000];
      int buffersize = sizeof(buffer);
      ROMEString odbPath;

      ROMEPath *path = new ROMEPath();
      // decode path
      if (!path->Decode(dataBasePath,runNumber,eventNumber)) {
         ROMEPrint::Error("\nPath decode error : %s\n", dataBasePath);
         return 0;
      }
      for (i=0;i<path->GetNumberOfTables();i++) {
         odbPath += "/";
         odbPath += path->GetTableNameAt(i);
      }
      odbPath += "/";
      odbPath += path->GetFieldName();

      if (db_find_key(gROME->GetMidasOnlineDataBase(), 0, (char*)(odbPath.Data()), &hKey)!= CM_SUCCESS) {
         ROMEPrint::Error("\nCannot write to online database\n");
         return false;
      }
      if (db_get_key_info(gROME->GetMidasOnlineDataBase(), hKey,name,sizeof(name), &type, &num_values,&item_size)!= CM_SUCCESS) {
         ROMEPrint::Error("\nCannot write to online database\n");
         return false;
      }
      iv = 0;
      for (i=path->GetFieldIndexAt(0);i<path->GetFieldIndexAt(1);i+=path->GetFieldIndexAt(2)) {
         if (db_sscanf((char*)(values->At(iv,0).Data()),buffer,&buffersize,0,type)!= CM_SUCCESS) {
            ROMEPrint::Error("\nCannot write to online database\n");
            return false;
         }
         if (db_set_data_index(gROME->GetMidasOnlineDataBase(), hKey,buffer,buffersize,i,type)!= CM_SUCCESS) {
            ROMEPrint::Error("\nCannot write to online database\n");
            return false;
         }
         iv++;
      }
   }
#endif // HAVE_MIDAS
   return true;
   WarningSuppression(values);
   WarningSuppression(dataBasePath);
   WarningSuppression(runNumber);
   WarningSuppression(eventNumber);
}
