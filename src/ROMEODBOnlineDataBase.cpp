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

//______________________________________________________________________________
ROMEODBOnlineDataBase::ROMEODBOnlineDataBase()
:ROMEDataBase()
,fODBBufferSize(5000)
,fODBBuffer(new char[fODBBufferSize])
{
}

//______________________________________________________________________________
ROMEODBOnlineDataBase::~ROMEODBOnlineDataBase() {
   SafeDeleteArray(fODBBuffer);
}

//______________________________________________________________________________
Bool_t ROMEODBOnlineDataBase::Init(const char* name, const char* /*path*/, const char* /*connection*/) {
   fName = name;
   return true;
}

//______________________________________________________________________________
Bool_t ROMEODBOnlineDataBase::Read(ROMEStr2DArray *values, const char *dataBasePath, Long64_t runNumber, Long64_t eventNumber)
{
#if defined ( HAVE_MIDAS )
   if (gROME->isOnline()) {
      int i, iv;
      HNDLE hKey;
      char name[500];
      int type, num_values, item_size;
      int buffersize = fODBBufferSize;
      char value[500];
      ROMEString odbPath;
      ROMEString errMsg;

      ROMEPath *path = new ROMEPath();

      try {
         // decode path
         if (!path->Decode(dataBasePath, runNumber, eventNumber)) {
            errMsg.SetFormatted("Path decode error : %s", dataBasePath);
            throw errMsg.Data();
         }
         for (i = 0; i < path->GetNumberOfTables(); i++) {
            odbPath += "/";
            odbPath += path->GetTableNameAt(i);
         }
         odbPath += "/";
         odbPath += path->GetFieldName();
         if (db_find_key(gROME->GetMidasOnlineDataBase(), 0, const_cast<char*>(odbPath.Data()), &hKey) != CM_SUCCESS) {
            errMsg.SetFormatted("Cannot read online database\n");
            throw errMsg.Data();
         }
         if (db_get_key_info(gROME->GetMidasOnlineDataBase(), hKey, name, sizeof(name), &type, &num_values, &item_size) !=
             CM_SUCCESS) {
            errMsg.SetFormatted("Cannot read online database");
            throw errMsg.Data();
         }
         if (num_values * item_size > fODBBufferSize) {
            SetODBBufferSize(num_values * item_size);
         }
         if (db_get_data(gROME->GetMidasOnlineDataBase(), hKey, fODBBuffer, &buffersize, type) != CM_SUCCESS) {
            errMsg.SetFormatted("Cannot read online database");
            throw errMsg.Data();
         }
         if (path->GetFieldIndexAt(1) == -1) {
            // single field
            if (db_sprintf(value, fODBBuffer, buffersize, 0, type) != CM_SUCCESS) {
               errMsg.SetFormatted("Cannot read online database");
               throw errMsg.Data();
            }
            values->SetAt(value, 0, 0);
         } else {
            // array field
            iv = 0;
            for (i = path->GetFieldIndexAt(0); i < path->GetFieldIndexAt(1); i += path->GetFieldIndexAt(2)) {
               if (db_sprintf(value, fODBBuffer, buffersize, i, type) != CM_SUCCESS) {
                  errMsg.SetFormatted("Cannot read online database");
                  throw errMsg.Data();
               }
               values->SetAt(value, iv, 0);
               iv++;
            }
         }
      }
      catch(const char* err) {
         if (err) {
            ROMEPrint::Error("\n%s\n", err);
         }
         delete path;
         return kFALSE;
      }
      delete path;
   }
#endif // HAVE_MIDAS
   return true;
   WarningSuppression(values);
   WarningSuppression(dataBasePath);
   WarningSuppression(runNumber);
   WarningSuppression(eventNumber);
}

//______________________________________________________________________________
Bool_t ROMEODBOnlineDataBase::Write(ROMEStr2DArray* values, const char * dataBasePath, Long64_t runNumber, Long64_t eventNumber)
{
#if defined ( HAVE_MIDAS )
   if (gROME->isOnline()) {
      int i, iv;
      HNDLE hKey;
      char name[500];
      int type, num_values, item_size;
      int buffersize = fODBBufferSize;
      ROMEString odbPath;
      ROMEString errMsg;

      ROMEPath *path = new ROMEPath();
      try {
         // decode path
         if (!path->Decode(dataBasePath, runNumber, eventNumber)) {
            errMsg.SetFormatted("\nPath decode error : %s\n", dataBasePath);
            throw errMsg.Data();
         }
         for (i = 0;i < path->GetNumberOfTables(); i++) {
            odbPath += "/";
            odbPath += path->GetTableNameAt(i);
         }
         odbPath += "/";
         odbPath += path->GetFieldName();

         if (db_find_key(gROME->GetMidasOnlineDataBase(), 0, const_cast<char*>(odbPath.Data()), &hKey) != CM_SUCCESS) {
            errMsg.SetFormatted("\nCannot write to online database\n");
            throw errMsg.Data();
         }
         if (db_get_key_info(gROME->GetMidasOnlineDataBase(), hKey, name, sizeof(name), &type, &num_values, &item_size) != CM_SUCCESS) {
            errMsg.SetFormatted("\nCannot write to online database\n");
            throw errMsg.Data();
         }
         if (num_values * item_size > fODBBufferSize) {
            SetODBBufferSize(num_values * item_size);
         }
         if (path->GetFieldIndexAt(1) == -1) {
            // single field
            if (db_sscanf(const_cast<char*>(values->At(0, 0).Data()), fODBBuffer, &buffersize, 0, type) != CM_SUCCESS) {
               errMsg.SetFormatted("\nCannot write to online database\n");
               throw errMsg.Data();
            }
            if (db_set_data(gROME->GetMidasOnlineDataBase(), hKey, fODBBuffer, buffersize, 1, type) != CM_SUCCESS) {
               errMsg.SetFormatted("\nCannot write to online database\n");
               throw errMsg.Data();
            }
         } else {
            // array field
            iv = 0;
            for (i = path->GetFieldIndexAt(0); i < path->GetFieldIndexAt(1); i += path->GetFieldIndexAt(2)) {
               if (db_sscanf(const_cast<char*>(values->At(iv, 0).Data()), fODBBuffer, &buffersize, 0, type) != CM_SUCCESS) {
                  errMsg.SetFormatted("\nCannot write to online database\n");
                  throw errMsg.Data();
               }
               if (db_set_data_index(gROME->GetMidasOnlineDataBase(), hKey, fODBBuffer, buffersize, i, type) != CM_SUCCESS) {
                  errMsg.SetFormatted("\nCannot write to online database\n");
                  throw errMsg.Data();
               }
               iv++;
            }
         }
      }
      catch(const char* err) {
         if (err) {
            ROMEPrint::Error("\n%s\n", err);
         }
         delete path;
         return kFALSE;
      }
      delete path;
   }
#endif // HAVE_MIDAS
   return true;
   WarningSuppression(values);
   WarningSuppression(dataBasePath);
   WarningSuppression(runNumber);
   WarningSuppression(eventNumber);
}

//______________________________________________________________________________
void ROMEODBOnlineDataBase::SetODBBufferSize(Int_t size)
{
   if (size > 0) {
      fODBBufferSize = size;
      SafeDeleteArray(fODBBuffer);
      fODBBuffer = new char[fODBBufferSize];
   }
}
